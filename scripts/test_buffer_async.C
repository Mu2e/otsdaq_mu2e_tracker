//
#define __CLING__ 1

#include <iomanip>
#include <iostream>

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

// //-----------------------------------------------------------------------------
// mu2e_databuff_t* readDTCBuffer(mu2edev* device, bool& readSuccess, bool& timeout, size_t& nbytes) {
//   mu2e_databuff_t* buffer;
//   auto tmo_ms = 1500;
//   readSuccess = false;

//   nbytes = device->read_data(DTC_DMA_Engine_DAQ, reinterpret_cast<void**>(&buffer), tmo_ms);
  
//   if (nbytes > 0)    {
//     readSuccess = true;
//     void* readPtr = &buffer[0];
//     uint16_t bufSize = static_cast<uint16_t>(*static_cast<uint64_t*>(readPtr));
//     readPtr = static_cast<uint8_t*>(readPtr) + 8;
    
//     timeout = false;
//     if (nbytes > sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader) + 8) {
//       // Check for dead or cafe in first packet
//       readPtr = static_cast<uint8_t*>(readPtr) + sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader);
//       std::vector<size_t> wordsToCheck{ 1, 2, 3, 7, 8 };
//       for (auto& word : wordsToCheck) 	{
// 	uint16_t* wordPtr = static_cast<uint16_t*>(readPtr) + (word - 1);
// 	if (*wordPtr == 0xcafe || *wordPtr == 0xdead) 	  {
// 	  printf(" Buffer Timeout detected! word=%5lu data: 0x%04x\n",word, *wordPtr);
// 	  DTCLib::Utilities::PrintBuffer(readPtr, 16, 0, /*TLVL_TRACE*/4 + 3);
// 	  timeout = true;
// 	  break;
// 	}
//       }
//     }
//   }
//   return buffer;
// }

//-----------------------------------------------------------------------------
void test_buffer_async(int Link, int NEvents, const char* OutputFn = nullptr) {

  std::ofstream    outputStream;

  DTCLib::DTC_Link_ID roc_link;

  if      (Link == 0) roc_link = DTCLib::DTC_Link_0;
  else if (Link == 1) roc_link = DTCLib::DTC_Link_1;

  int link_mask = 0x1 << (4*Link);

  printf("ROC link mask: 0x%08x\n",link_mask);

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,link_mask,"");

  uint32_t res; 
  int      rc;

  if (OutputFn) {
    outputStream.open(OutputFn, std::ios::out | std::ios::app | std::ios::binary);
  }

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
// var_pattern_config
//-----------------------------------------------------------------------------
  dev->write_register(0x91a8,100,0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  dtc.WriteROCRegister(roc_link,14,     1,false,1000); // reset ROC
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  dtc.WriteROCRegister(roc_link, 8,0x0010,false,1000); // configure ROC to send patterns
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  dtc.WriteROCRegister(roc_link,30,     0,false,1000); // configure STATUS_BIT in MODE=0
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  rc = dev->read_register(0x9100,100,&res); printf("0x9100: DTC status       : 0x%08x\n",res); // expect: 0x40808404
  rc = dev->read_register(0x9138,100,&res); printf("0x9138: SERDES Reset Done: 0x%08x\n",res); // expect: 
  rc = dev->read_register(0x91a8,100,&res); printf("0x9158: time window      : 0x%08x\n",res); // expect: 
  rc = dev->read_register(0x91c8,100,&res); printf("0x91c8: debug packet type: 0x%08x\n",res); // expect: 0x00000000
//-----------------------------------------------------------------------------
// buffer_test
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

  int incrementTimestamp =   1;
  int cfodelay           = 200;
  int requestsAhead      =   0;
  int heartbeatsAfter    =  16;

  int tmo_ms             = 1500;
  int delay              = 200;

  cfo.SendRequestsForRange(NEvents,DTCLib::DTC_EventWindowTag(uint64_t(0)),incrementTimestamp,cfodelay,requestsAhead,heartbeatsAfter);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
//-----------------------------------------------------------------------------
// for some reason, it is critical that we reset device time after sending 
// requests for range. 
// definition of a timeout ?
//-----------------------------------------------------------------------------
  for (int i=0; i<NEvents+1; i++) {
    
    dev->ResetDeviceTime();

    // dtc->WriteROCRegister(DTCLib::DTC_Link_0,14,     1,false,1000); // reset ROC
    // dtc->WriteROCRegister(DTCLib::DTC_Link_0, 8,0x0010,false,1000); // configure ROC to send patterns
    // dtc->WriteROCRegister(DTCLib::DTC_Link_0,30,     0,false,1000); // configure STATUS_BIT in MODE=0

    size_t nbytes(0);
    bool   timeout(false);
    bool   readSuccess(false);

    printf(" ---- read event %i\n",i);

    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, nbytes);
    //    int nbytes = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,nbytes);

    if (OutputFn) {
      if (readSuccess and (not timeout)) {
	char* ptr = (char*) buffer;
	outputStream.write(ptr+8 ,nbytes-8);
      }
    }

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
