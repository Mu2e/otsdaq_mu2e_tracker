//
#define __CLING__ 1

#include <iomanip>
#include <iostream>

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"


using namespace DTCLib;

int gSleepTimeDMA    =   200;  // [us]
//-----------------------------------------------------------------------------
// test1: each time, CFO requests N events, no CFO calls in the loop
//-----------------------------------------------------------------------------
void test1_read_data(int Link, int NEvents) {

  uint16_t  roc_reg[100];

  auto link = DTCLib::DTC_Link_ID(Link);

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,0x1<<4*Link,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint32_t res; 
  int      rc;

  mu2edev* dev = dtc.GetDevice();

  //  monica_dtc_reset(&dtc);
  
  rc = dev->read_register(0x9138,100,&res); printf("0x9138: SERDES Reset Done: 0x%08x\n",res); // expect: 

  monica_digi_clear     (&dtc,link);
  monica_var_link_config(&dtc,link);
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
  print_roc_registers(&dtc,link,"000");
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

    print_roc_registers(&dtc,link,"001 [after cfo.SendRequestForTimestamp]");

    size_t sts(0);
    bool   timeout(false);
    bool   readSuccess(false);

    printf("----------------------------------------------------------------------- read event %i\n",i);

    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, sts);
    //    int sts = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

    printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,sts);

    // print_roc_registers(&dtc,link,"002 [after readDTCBuffer]");

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
// test2: read DTC only (DTC to read is defined by $env(DTCLIB_DTC)
// each time, CFO requests one event
// if defined, OutputFn is the name of the output raw file
// sleep times in us
//-----------------------------------------------------------------------------
void test2_read_data(int LinkMask                ,  // add, as the link can vary
                     int NEvents                 , 
                     int HBInterval        = 1000,
                     int SleepTimeDMA      =  200,  // [us]
                     int SleepTimeDTC      = 1000,  // [us]
                     int SleepTimeROC      = 4000, 
                     int SleepTimeROCReset = 4000, 
                     int ResetROC          =    1, 
                     int NevPrint          =   -1,
                     const char* OutputFn  = nullptr) {

  std::ofstream outputStream;

  uint16_t  roc_reg[100];

  std::vector<DTCLib::DTC_Link_ID> active_links;

  for (int i=0; i<6; i++) {
    int bit = (LinkMask >> 4*i) & 0x1;
    if (bit) {
      active_links.push_back(DTCLib::DTC_Link_ID(i));
    }
  }

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,LinkMask,"");
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
  int requestsAhead      =    1;
  int heartbeatsAfter    =   16;

  int tmo_ms             = 1500;
//-----------------------------------------------------------------------------
// for some reason, it seems critical that we reset device time after sending 
// requests for range. 
// definition of a timeout ?
//-----------------------------------------------------------------------------
  for (auto link : active_links) {
    monica_digi_clear     (&dtc,link);
    monica_var_link_config(&dtc,link);
  }

  // dev->write_register(0x91a8,100,HBInterval);

  for (int i=0; i<NEvents; i++) {
//-----------------------------------------------------------------------------
// reset and go back to 25.6 us
//-----------------------------------------------------------------------------
    if (ResetROC != 0) {
      for (auto link : active_links) {
        monica_digi_clear     (&dtc,link);
        // monica_var_link_config(&dtc,link);
      }
      dev->write_register(0x91a8,100,HBInterval);
    }
//-----------------------------------------------------------------------------
// emulate timing signals of the next event 
//-----------------------------------------------------------------------------
    int nev = 1;
    cfo.SendRequestsForRange(nev,DTCLib::DTC_EventWindowTag(uint64_t(i)),
                             incrementTimestamp,
                             HBInterval,requestsAhead,heartbeatsAfter);
    std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

    dev->ResetDeviceTime();
    // cfo.SendRequestForTimestamp(DTCLib::DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

    // print_roc_registers(&dtc,link,"001 [after cfo.SendRequestForTimestamp]");

    size_t nbytes     (0);
    bool   timeout    (false);
    bool   readSuccess(false);

    // printf(" ----------------------------------------------------------------------- reading event %i\n",i);

    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, nbytes);
    //    int sts = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

    if (OutputFn) {
      if (readSuccess and (not timeout)) {
        char* ptr = (char*) buffer;
        outputStream.write(ptr ,nbytes);
      }
    }

    // print_roc_registers(&dtc,link,"002 [after readDTCBuffer]");

    if (i < NevPrint) {
      printf(" --- read event %6i readSuccess:%i timeout:%i nbytes: %5lu\n",i,readSuccess,timeout,nbytes);
      DTCLib::Utilities::PrintBuffer(buffer, nbytes, 0);
    }

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

//-----------------------------------------------------------------------------
int readDTCRegisters(DTCLib::DTC* Dtc, uint16_t* reg, int nreg, uint32_t* f2d) {

  int      rc(0);
  // 
//-----------------------------------------------------------------------------
// first goes the version
//----------------------------------------------------------------------------------------
  f2d[0] = 1; 

  for (int i=0; i<nreg; i++) {
    f2d[2*i+1] = reg[i];
    printf(" --- reading DTC reg 0x%04x",reg[i]);
    try   { 
      rc   = Dtc->GetDevice()->read_register(reg[i],100,f2d+2*i+2); 
      std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDTC));   
      printf(" --- var: 0x%08x",f2d[2*i+2]);
    }
    catch (...) {
      printf("readDTC ERROR, register : 0x%04x\n",reg[i]);
      break;
    }
    printf("\n");
  }

  return rc;
}

//-----------------------------------------------------------------------------
// test3: 
// - each time, CFO requests one event
// - read SPI and the DTC registers
//
// if defined, OutputFn is the name of the output raw file
// sleep time in ms
//-----------------------------------------------------------------------------
void test3_read_data(int Link,             
                     int NEvents, 
                     int SleepTimeDMA      =  200,  // [us]
                     int SleepTimeDTC      = 1000,  // [us]
                     int SleepTimeROC      = 4000, 
                     int SleepTimeROCReset = 4000, 
                     int ResetROC          =    1, 
                     int NevPrint          =   -1,
                     const char* OutputFn  = nullptr) {

  std::ofstream outputStream;

  uint16_t  roc_reg[100];

  auto roc = DTCLib::DTC_Link_ID(Link);

  DTCLib::DTC* dtc = new DTC(DTCLib::DTC_SimMode_NoCFO,-1,0x1<<4*Link,"");
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

  mu2edev* dev = dtc->GetDevice();

  print_dtc_registers(dtc);
//-----------------------------------------------------------------------------
// print the ROC registers 11,13,14 - previously used in the readout
//-----------------------------------------------------------------------------
//  print_roc_registers(&dtc,roc,"000");
//-----------------------------------------------------------------------------
// now to reading the data
//-----------------------------------------------------------------------------
  dtc->SetSequenceNumberDisable(); 
  auto initTime = dev->GetDeviceTime();
  dev->ResetDeviceTime();

  bool useCFOEmulator   = true;
  int  packetCount      = 0;
  bool stickyDebugType  = true;
  bool quiet            = false;
  bool asyncRR          = false; 
  bool forceNoDebug     = true;
  bool useCFODRP        = false;

  DTCLib::DTCSoftwareCFO cfo(dtc,useCFOEmulator,packetCount,
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
  monica_var_link_config(dtc,Link);
  monica_digi_clear     (dtc,Link);

  dev->write_register(0x91a8,100,heartbeatInterval);

  for (int i=0; i<NEvents; i++) {
//-----------------------------------------------------------------------------
// reset and go back to 25.6 us
//-----------------------------------------------------------------------------
    if (ResetROC != 0) {
      monica_digi_clear     (dtc,Link);
      // monica_var_link_config(dtc,Link);                 // shouldn't need any more
      dev->write_register(0x91a8,100,heartbeatInterval);
    }
//-----------------------------------------------------------------------------
// emulate timing signals of the next event 
//-----------------------------------------------------------------------------
    int nev = 1;
    cfo.SendRequestsForRange(nev,
                             DTCLib::DTC_EventWindowTag(uint64_t(i)),
                             incrementTimestamp,
                             heartbeatInterval,
                             requestsAhead,
                             heartbeatsAfter);
    std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

    dev->ResetDeviceTime();
    // cfo.SendRequestForTimestamp(DTCLib::DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

    // print_roc_registers(&dtc,roc,"001 [after cfo.SendRequestForTimestamp]");

    size_t nbytes     (0);
    bool   timeout    (false);
    bool   readSuccess(false);

    // printf(" ----------------------------------------------------------------------- reading event %i\n",i);
    
    mu2e_databuff_t* buffer = readDTCBuffer(dev, readSuccess, timeout, nbytes);
    //    int sts = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);
    
    if (OutputFn) {
      if (readSuccess and (not timeout)) {
        char* ptr = (char*) buffer;
        outputStream.write(ptr ,nbytes);
      }
    }

    if (i < NevPrint) {
      // print_roc_registers(&dtc,Link,"002 [after readDTCBuffer]");
      printf(" --- read event %6i readSuccess:%i timeout:%i nbytes: %5lu\n",i,readSuccess,timeout,nbytes);
      DTCLib::Utilities::PrintBuffer(buffer, nbytes, 0);
    }
//-----------------------------------------------------------------------------
// 2. need to add one more fragment of debug type with the diagnostics registers
//    8 bytes per register - (register number, value)
//-----------------------------------------------------------------------------
    printf(" -------------------- 2. read DTC registers:\n");
    int      nreg(4);
    uint16_t reg[] = { 0x9004, 0x9100, 0x9190, 0x9194} ;
    uint32_t data[200];
    
    readDTCRegisters(dtc,reg,nreg,data);

    if (i < NevPrint) {
      DTCLib::Utilities::PrintBuffer(data, nreg*8+4, 0);
    }
//-----------------------------------------------------------------------------
// 3. read SPI
// after writing to reg 258, sleep foror 2 msec, then wait till reg 128 becomes non-zero
// don't go below 1.5msec of sleep
//-----------------------------------------------------------------------------
    printf(" -------------------- 3. read SPI:\n");

    // dtc->WriteROCRegister   (roc,258,0x0000,false,100);
    // std::this_thread::sleep_for(std::chrono::microseconds(2000));

    // printf(" -------------------- 3.1 read r128\n");

    // uint16_t u; 
    // while ((u = dtc->ReadROCRegister(roc,128,100)) == 0) {}; 
    // printf("reg 0x%02x u = 0x%04x\n",128,u);

    // printf(" -------------------- 3.2 read r129\n");
    // int nw = dtc->ReadROCRegister(roc,129,100); printf("reg_0x%02x val:0x%04x\n",129,nw);

    // printf(" -------------------- 3.3 readROCBlock\n");
    // std::vector<uint16_t> spi;
    // nw = nw-4;                          // Monica tells about 4 extra words

    // dtc->ReadROCBlock(spi,roc,258,nw,false,100);

    // DTCLib::Utilities::PrintBuffer(spi.data(), nw*2, 0);

    printf(" -------------------- 4 release DMA engine\n");
    dev->read_release(DTC_DMA_Engine_DAQ, 1);

    if (gSleepTimeDMA > 0) {
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
