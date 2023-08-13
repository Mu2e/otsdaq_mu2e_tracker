//
#define __CLING__ 1

#include <iomanip>
#include <iostream>

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"


int gSleepTimeDMA    =   200;  // [us]
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

    if (gSleepTimeDMA > 0) {
      // usleep(gSleepTimeDMA);
      std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDMA));
    }

    rc = dev->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x\n",res); // expect: 0x40808404
    rc = dev->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x\n",res); // expect: 0x00000000
  }
  dev->release_all(DTC_DMA_Engine_DAQ);
}


//-----------------------------------------------------------------------------
// test2: each time, CFO requests one event
// if defined, OutputFn is the name of the output raw file
// sleep time in ms
//-----------------------------------------------------------------------------
void test2_read_data(int NEvents, 
                     int SleepTimeDMA      =  200,  // [us]
                     int SleepTimeDTC      = 1000,  // [us]
                     int SleepTimeROC      = 4000, 
                     int SleepTimeROCReset = 4000, 
                     int ResetROC          =    1, 
                     const char* OutputFn  = nullptr) {

  std::ofstream outputStream;

  uint16_t  roc_reg[100];

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,0x1,"");
  // std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint32_t res; 
  int      rc;

  gSleepTimeDMA      = SleepTimeDMA;
  gSleepTimeDTC      = SleepTimeDTC;
  gSleepTimeROC      = SleepTimeROC;
  gSleepTimeROCReset = SleepTimeROCReset;

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

  // std::this_thread::sleep_for(std::chrono::milliseconds(100));

  int incrementTimestamp =    1;
  int heartbeatInterval  = 1000;
  int requestsAhead      =    1;
  int heartbeatsAfter    =   16;

  int tmo_ms             = 1500;
//-----------------------------------------------------------------------------
// for some reason, it seems critical that we reset device time after sending 
// requests for range. 
// definition of a timeout ?
//-----------------------------------------------------------------------------
  monica_digi_clear     (&dtc);
  monica_var_link_config(&dtc);
  dev->write_register(0x91a8,100,heartbeatInterval);

  for (int i=0; i<NEvents; i++) {
//-----------------------------------------------------------------------------
// reset and go back to 25.6 us
//-----------------------------------------------------------------------------
    if (ResetROC != 0) {
      monica_digi_clear     (&dtc);
      monica_var_link_config(&dtc);
      dev->write_register(0x91a8,100,heartbeatInterval);
    }
//-----------------------------------------------------------------------------
// emulate timing signals of the next event 
//-----------------------------------------------------------------------------
    int nev = 1;
    cfo.SendRequestsForRange(nev,DTCLib::DTC_EventWindowTag(uint64_t(i)),incrementTimestamp,heartbeatInterval,requestsAhead,heartbeatsAfter);
    std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

    dev->ResetDeviceTime();
    // cfo.SendRequestForTimestamp(DTCLib::DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

    // print_roc_registers(&dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

    size_t nbytes     (0);
    bool   timeout    (false);
    bool   readSuccess(false);

    // printf(" ----------------------------------------------------------------------- reading event %i\n",i);

    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, nbytes);
    //    int sts = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

    printf(" --- read event %6i readSuccess:%i timeout:%i nbytes: %5lu\n",i,readSuccess,timeout,nbytes);

    if (OutputFn) {
      if (readSuccess and (not timeout)) {
        char* ptr = (char*) buffer;
        outputStream.write(ptr+8 ,nbytes-8);
      }
    }

    // print_roc_registers(&dtc,DTCLib::DTC_Link_0,"002 [after readDTCBuffer]");

    DTCLib::Utilities::PrintBuffer(buffer, nbytes, 0);

    dev->read_release(DTC_DMA_Engine_DAQ, 1);

    if (gSleepTimeDMA > 0) {
      // usleep(gSleepTimeDMA);
      std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDMA));
    }

    rc = dev->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x\n",res); // expect: 0x40808404
    rc = dev->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x\n",res); // expect: 0x00000000
  }

  dev->release_all(DTC_DMA_Engine_DAQ);

  if (OutputFn) {
    outputStream.flush();
    outputStream.close();
  }
}
