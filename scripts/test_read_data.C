//
#define __CLING__ 1

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"
//-----------------------------------------------------------------------------
mu2e_databuff_t* readDTCBuffer(mu2edev* device, bool& readSuccess, bool& timeout, size_t& sts) {
  mu2e_databuff_t* buffer;
  auto tmo_ms = 1500;
  readSuccess = false;

  sts = device->read_data(DTC_DMA_Engine_DAQ, reinterpret_cast<void**>(&buffer), tmo_ms);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  
  if (sts > 0) {
    readSuccess   = true;
    void* readPtr = &buffer[0];
    uint16_t bufSize = static_cast<uint16_t>(*static_cast<uint64_t*>(readPtr));
    readPtr = static_cast<uint8_t*>(readPtr) + 8;
    
    timeout = false;
    if (sts > sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader) + 8) {
      // Check for 'dead' or 'cafe' in first packet
      readPtr = static_cast<uint8_t*>(readPtr) + sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader);
      std::vector<size_t> wordsToCheck{ 1, 2, 3, 7, 8 };
      for (auto& word : wordsToCheck) 	{
	uint16_t* wordPtr = static_cast<uint16_t*>(readPtr) + (word - 1);
	if ((*wordPtr == 0xcafe) or (*wordPtr == 0xdead)) {
	  printf(" Buffer Timeout detected! word=%5lu data: 0x%04x\n",word, *wordPtr);
	  DTCLib::Utilities::PrintBuffer(readPtr, 16, 0, /*TLVL_TRACE*/4 + 3);
	  timeout = true;
	  break;
	}
      }
    }
  }
  return buffer;
}


//-----------------------------------------------------------------------------
void print_roc_registers(DTCLib::DTC* Dtc, DTCLib::DTC_Link_ID RocID, const char* Header) {
  uint16_t  roc_reg[100];
  printf("---------------------- %s print_roc_registers\n",Header);

  roc_reg[11] = Dtc->ReadROCRegister(RocID,11,100);
  printf("roc_reg[11] = 0x%04x\n",roc_reg[11]);
  roc_reg[13] = Dtc->ReadROCRegister(RocID,13,100);
  printf("roc_reg[13] = 0x%04x\n",roc_reg[13]);
  roc_reg[14] = Dtc->ReadROCRegister(RocID,14,100);
  printf("roc_reg[14] = 0x%04x\n",roc_reg[14]);

  printf("---------------------- END print_roc_registers\n");
}

//-----------------------------------------------------------------------------
void test_read_data(int NEvents) {

  uint16_t  roc_reg[100];

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,0x1,"");

  uint32_t res; 
  int      rc;

  mu2edev* dev = dtc.GetDevice();
//-----------------------------------------------------------------------------
// this implements bash DTC_Reset
//-----------------------------------------------------------------------------
  // dev->write_register(0x9100,100,0x80000000);
  // dev->write_register(0x9100,100,0x00008000);
  // dev->write_register(0x9118,100,0xffff00ff);
  // dev->write_register(0x9118,100,0x00000000);
  
  std::this_thread::sleep_for(std::chrono::milliseconds(500));
  
  rc = dev->read_register(0x9138,100,&res); printf("0x9138: SERDES Reset Done: 0x%08x\n",res); // expect: 
//-----------------------------------------------------------------------------
// here is Monica's var_link_config
//-----------------------------------------------------------------------------
  dev->write_register(0x91a8,100,0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  dtc.WriteROCRegister(DTCLib::DTC_Link_0,14,     1,false,1000); // reset ROC
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  dtc.WriteROCRegister(DTCLib::DTC_Link_0, 8,0x030f,false,1000); // configure ROC to read all 4 lanes
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
//-----------------------------------------------------------------------------
// back to 25.6 us
//-----------------------------------------------------------------------------
  dev->write_register(0x91a8,100,0x400);
//-----------------------------------------------------------------------------
// print the DTC registers
//-----------------------------------------------------------------------------
  rc = dev->read_register(0x9100,100,&res); printf("0x9100: DTC status       : 0x%08x\n",res); // expect: 0x40808404
  rc = dev->read_register(0x9138,100,&res); printf("0x9138: SERDES Reset Done: 0x%08x\n",res); // expect: 
  rc = dev->read_register(0x91a8,100,&res); printf("0x9158: time window      : 0x%08x\n",res); // expect: 
  rc = dev->read_register(0x91c8,100,&res); printf("0x91c8: debug packet type: 0x%08x\n",res); // expect: 0x00000000
//-----------------------------------------------------------------------------
// print the ROC registers 11,13,14 - previously used in the readout
//-----------------------------------------------------------------------------
  print_roc_registers(&dtc,DTCLib::DTC_Link_0,"000");
//-----------------------------------------------------------------------------
// now to reading the data
//-----------------------------------------------------------------------------
  dtc.SetSequenceNumberDisable(); 
  auto initTime = dev->GetDeviceTime();
  dev->ResetDeviceTime();
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  bool useCFOEmulator   = true;
  int  packetCount      = 0;
  bool stickyDebugType  = true;
  bool quiet            = false;
  bool asyncRR          = false; 
  bool forceNoDebug     = true;
  bool useCFODRP        = false;

  DTCLib::DTCSoftwareCFO cfo(&dtc,useCFOEmulator,packetCount,DTCLib::DTC_DebugType_SpecialSequence,stickyDebugType,quiet,asyncRR,forceNoDebug,useCFODRP);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  int incrementTimestamp=1;
  int cfodelay = 200;
  int requestsAhead=0;
  int heartbeatsAfter=16;

  int tmo_ms = 1500;
  int delay  = 500;

//  cfo.SendRequestsForRange(NEvents,DTCLib::DTC_EventWindowTag(uint64_t(0)),incrementTimestamp,cfodelay,requestsAhead,heartbeatsAfter);
//  std::this_thread::sleep_for(std::chrono::milliseconds(100));
//-----------------------------------------------------------------------------
// for some reason, it is critical that we reset device time after sending 
// requests for range. 
// definition of a timeout ?
//-----------------------------------------------------------------------------
  dev->ResetDeviceTime();

  for (int i=0; i<NEvents+1; i++) {
    cfo.SendRequestForTimestamp(DTCLib::DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    print_roc_registers(&dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

    size_t sts(0);
    bool   timeout(false);
    bool   readSuccess(false);

    printf(" ---- read event %i\n",i);

    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, sts);
    //    int sts = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

    printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,sts);

    print_roc_registers(&dtc,DTCLib::DTC_Link_0,"002 [after readDTCBuffer]");

    DTCLib::Utilities::PrintBuffer(buffer, sts, 10);

    dev->read_release(DTC_DMA_Engine_DAQ, 1);

    if (delay > 0) usleep(delay);

    rc = dev->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x\n",res); // expect: 0x40808404
    rc = dev->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x\n",res); // expect: 0x00000000
  }
  dev->release_all(DTC_DMA_Engine_DAQ);
}
