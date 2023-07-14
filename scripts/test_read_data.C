//
#define __CLING__ 1

#include <iomanip>
#include <iostream>

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
void print_dtc_registers(DTCLib::DTC* Dtc) {
  uint32_t res; 
  int      rc;

  mu2edev* dev = Dtc->GetDevice();

  rc = dev->read_register(0x9100,100,&res); printf("0x9100: DTC status       : 0x%08x\n",res); // expect: 0x40808404
  rc = dev->read_register(0x9138,100,&res); printf("0x9138: SERDES Reset Done: 0x%08x\n",res); // expect: 0xbfbfbfbf
  rc = dev->read_register(0x91a8,100,&res); printf("0x9158: time window      : 0x%08x\n",res); // expect: 
  rc = dev->read_register(0x91c8,100,&res); printf("0x91c8: debug packet type: 0x%08x\n",res); // expect: 0x00000000
}


//-----------------------------------------------------------------------------
void print_roc_registers(DTCLib::DTC* Dtc, DTCLib::DTC_Link_ID RocID, const char* Header) {
  uint16_t  roc_reg[100];
  printf("---------------------- %s print_roc_registers\n",Header);

  // roc_reg[11] = Dtc->ReadROCRegister(RocID,11,100);
  // printf("roc_reg[11] = 0x%04x\n",roc_reg[11]);
  // roc_reg[13] = Dtc->ReadROCRegister(RocID,13,100);
  // printf("roc_reg[13] = 0x%04x\n",roc_reg[13]);
  // roc_reg[14] = Dtc->ReadROCRegister(RocID,14,100);
  // printf("roc_reg[14] = 0x%04x\n",roc_reg[14]);

  printf("---------------------- END print_roc_registers\n");
}

//-----------------------------------------------------------------------------
// this implements Monica's bash DTC_Reset
//-----------------------------------------------------------------------------
void monica_dtc_reset(DTCLib::DTC* Dtc) {

  mu2edev* dev = Dtc->GetDevice();

  dev->write_register(0x9100,100,0x80000000);
  dev->write_register(0x9100,100,0x00008000);
  dev->write_register(0x9118,100,0xffff00ff);
  dev->write_register(0x9118,100,0x00000000);
}

//-----------------------------------------------------------------------------
void monica_digi_clear(DTCLib::DTC* dtc) {
//-----------------------------------------------------------------------------
//  Monica's digi_clear
//  this will proceed in 3 steps each for HV and CAL DIGIs:
// 1) pass TWI address and data toTWI controller (fiber is enabled by default)
// 2) write TWI INIT high
// 3) write TWI INIT low
//-----------------------------------------------------------------------------
// rocUtil write_register -l $LINK -a 28 -w 16 > /dev/null
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,28,0x10,false,1000); // 

  // Writing 0 & 1 to  address=16 for HV DIGIs ??? 
  // rocUtil write_register -l $LINK -a 27 -w  0 > /dev/null # write 0 
  // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null ## toggle INIT 
  // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,27,0x00,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x01,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x00,false,1000); // 


  // rocUtil write_register -l $LINK -a 27 -w  1 > /dev/null # write 1  
  // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null # toggle INIT
  // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,27,0x01,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x01,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x00,false,1000); // 

  // echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
  // rocUtil write_register -l $LINK -a 25 -w 16 > /dev/null
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,25,0x10,false,1000); // 

// rocUtil write_register -l $LINK -a 24 -w  0 > /dev/null # write 0
// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,24,0x00,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x01,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x00,false,1000); // 

// rocUtil write_register -l $LINK -a 24 -w  1 > /dev/null # write 1
// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,24,0x01,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x01,false,1000); // 
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x00,false,1000); // 
}

//-----------------------------------------------------------------------------
void monica_var_link_config(DTCLib::DTC* dtc) {
  mu2edev* dev = dtc->GetDevice();

  dev->write_register(0x91a8,100,0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  dtc->WriteROCRegister(DTCLib::DTC_Link_0,14,     1,false,1000); // reset ROC
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  dtc->WriteROCRegister(DTCLib::DTC_Link_0, 8,0x030f,false,1000); // configure ROC to read all 4 lanes
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


//-----------------------------------------------------------------------------
// test1: each time, CFO requests N events, no CFO calls in the loop
//-----------------------------------------------------------------------------
void test1_read_data(int NEvents) {

  uint16_t  roc_reg[100];

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint32_t res; 
  int      rc;

  mu2edev* dev = dtc.GetDevice();

  //  monica_dtc_reset(&dtc);
  
  rc = dev->read_register(0x9138,100,&res); printf("0x9138: SERDES Reset Done: 0x%08x\n",res); // expect: 

  monica_digi_clear     (&dtc);
  monica_var_link_config(&dtc);
//-----------------------------------------------------------------------------
// back to 25.6 us
//-----------------------------------------------------------------------------
  dev->write_register(0x91a8,100,0x400);
//-----------------------------------------------------------------------------
// print the DTC registers
//-----------------------------------------------------------------------------
  print_dtc_registers(&dtc);
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

  DTCLib::DTCSoftwareCFO cfo(&dtc,useCFOEmulator,packetCount,
			     DTCLib::DTC_DebugType_SpecialSequence,
			     stickyDebugType,quiet,asyncRR,forceNoDebug,useCFODRP);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  int incrementTimestamp =    1;
  int cfodelay           = 1024;
  int requestsAhead      =    1;
  int heartbeatsAfter    =   16;

  int tmo_ms             = 1500;
  int delay              =  500;

  cfo.SendRequestsForRange(NEvents,DTCLib::DTC_EventWindowTag(uint64_t(0)),incrementTimestamp,cfodelay,requestsAhead,heartbeatsAfter);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
//-----------------------------------------------------------------------------
// for some reason, it is critical that we reset device time after sending 
// requests for range. 
// definition of a timeout ?
//-----------------------------------------------------------------------------
  dev->ResetDeviceTime();

  for (int i=0; i<NEvents+1; i++) {
    // cfo.SendRequestForTimestamp(DTCLib::DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    print_roc_registers(&dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

    size_t sts(0);
    bool   timeout(false);
    bool   readSuccess(false);

    printf("----------------------------------------------------------------------- read event %i\n",i);

    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, sts);
    //    int sts = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

    printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,sts);

    // print_roc_registers(&dtc,DTCLib::DTC_Link_0,"002 [after readDTCBuffer]");

    DTCLib::Utilities::PrintBuffer(buffer, sts, 0);

    dev->read_release(DTC_DMA_Engine_DAQ, 1);

    if (delay > 0) usleep(delay);

    rc = dev->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x\n",res); // expect: 0x40808404
    rc = dev->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x\n",res); // expect: 0x00000000
  }
  dev->release_all(DTC_DMA_Engine_DAQ);
}


//-----------------------------------------------------------------------------
// test2: each time, CFO requests one event
// if defined, OutputFn is the name of the output raw file
//-----------------------------------------------------------------------------
void test2_read_data(int NEvents, const char* OutputFn = nullptr) {

  std::ofstream outputStream;

  uint16_t  roc_reg[100];

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint32_t res; 
  int      rc;

  if (OutputFn) {
    outputStream.open(OutputFn, std::ios::out | std::ios::app | std::ios::binary);
  }

  mu2edev* dev = dtc.GetDevice();

  print_dtc_registers(&dtc);
//-----------------------------------------------------------------------------
// print the ROC registers 11,13,14 - previously used in the readout
//-----------------------------------------------------------------------------
//  print_roc_registers(&dtc,DTCLib::DTC_Link_0,"000");
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

  DTCLib::DTCSoftwareCFO cfo(&dtc,useCFOEmulator,packetCount,
			     DTCLib::DTC_DebugType_SpecialSequence,
			     stickyDebugType,quiet,asyncRR,forceNoDebug,useCFODRP);

  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  int incrementTimestamp =    1;
  int cfodelay           = 1024;
  int requestsAhead      =    1;
  int heartbeatsAfter    =   16;

  int tmo_ms = 1500;
  int delay  = 200;         // 500 was OK;
//-----------------------------------------------------------------------------
// for some reason, it seems critical that we reset device time after sending 
// requests for range. 
// definition of a timeout ?
//-----------------------------------------------------------------------------
  for (int i=0; i<NEvents; i++) {
    monica_digi_clear     (&dtc);
    monica_var_link_config(&dtc);
//-----------------------------------------------------------------------------
// back to 25.6 us
//-----------------------------------------------------------------------------
    dev->write_register(0x91a8,100,0x400);

    int nev = 1;
    cfo.SendRequestsForRange(nev,DTCLib::DTC_EventWindowTag(uint64_t(i)),incrementTimestamp,cfodelay,requestsAhead,heartbeatsAfter);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    dev->ResetDeviceTime();
    // cfo.SendRequestForTimestamp(DTCLib::DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    print_roc_registers(&dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

    size_t nbytes     (0);
    bool   timeout    (false);
    bool   readSuccess(false);

    printf(" ----------------------------------------------------------------------- reading event %i\n",i);

    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, nbytes);
    //    int sts = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

    printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,nbytes);

    if (OutputFn) {
      if (readSuccess and (not timeout)) {
	char* ptr = (char*) buffer;
	outputStream.write(ptr+8 ,nbytes-8);
      }
    }

    print_roc_registers(&dtc,DTCLib::DTC_Link_0,"002 [after readDTCBuffer]");

    DTCLib::Utilities::PrintBuffer(buffer, nbytes, 0);

    dev->read_release(DTC_DMA_Engine_DAQ, 1);

    if (delay > 0) usleep(delay);

    rc = dev->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x\n",res); // expect: 0x40808404
    rc = dev->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x\n",res); // expect: 0x00000000
  }

  dev->release_all(DTC_DMA_Engine_DAQ);

  if (OutputFn) {
    outputStream.flush();
    outputStream.close();
  }
}
