//
#define __CLING__ 1
#define HIDE_FROM_ROOT 1
#include <iomanip>
#include <iostream>

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;
//-----------------------------------------------------------------------------
// read one event, histogram hits
//-----------------------------------------------------------------------------
void read_dtc_event(DTC* Dtc, mu2e_databuff_t*& buffer, size_t& NBytes) {


  mu2edev* dev = Dtc->GetDevice();

  bool   timeout    (false);
  bool   readSuccess(false);
  int    delay  = 200;         // 500 was OK;

  buffer = readDTCBuffer(dev,readSuccess,timeout,NBytes);
  //    int nbytes = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

  printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,NBytes);

  print_roc_registers(Dtc,DTCLib::DTC_Link_0,"002 [after readDTCBuffer]");

  DTCLib::Utilities::PrintBuffer(buffer, NBytes, 0);

  dev->read_release(DTC_DMA_Engine_DAQ, 1);

  if (delay > 0) usleep(delay);

  uint32_t res; 
  int      rc;

  rc = dev->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x\n",res); // expect: 0x40808404
  rc = dev->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x\n",res); // expect: 0x00000000

  dev->release_all(DTC_DMA_Engine_DAQ);
}

//-----------------------------------------------------------------------------
void analyze(uint16_t* Ptr, int NWords) {

  int type     = (Ptr[1] & 0xf0 ) >> 4;
  int npackets = Ptr[2];
  int nhits = npackets/2;

  printf(" fragment type: %i npackets : %5i nhits: %5i\n",type,npackets,nhits);

  printf("   i  sid    tdc0   tdc1  tot0  tot1");
  printf(" adc[ 0] adc[ 1] adc[ 2] adc[ 3] adc[ 4] adc[ 5] adc[ 6] adc[ 7] adc[ 8] adc[ 9] adc[10]");
  printf(" adc[11] adc[12] adc[13] adc[14]");
  printf("\n");
  printf("-----------------------------------------------------------------------------");
  printf("------------------------------------------------------------\n");
  for (int i=0; i<nhits; i++) {
    uint16_t* packet = Ptr + 8+16*i;
    uint16_t  sid  = packet[0];
    uint16_t  tdc0 = packet[1];
    uint16_t  tdc1 = packet[2];
    uint16_t  tot0 = (packet[3] & 0x00ff);
    uint16_t  tot1 = (packet[3] & 0xff00) >> 8;

    uint16_t  adc[15];

    adc[ 0] = ((packet[ 4] & 0x0fff)     );
    adc[ 1] = ((packet[ 5] & 0x00ff) << 4) + ((packet[ 4] & 0xf000) >> 12);
    adc[ 2] = ((packet[ 6] & 0x000f) << 8) + ((packet[ 5] & 0xff00) >>  8);
    adc[ 3] = ((packet[ 6] & 0xfff0) >> 4);
    adc[ 4] = ((packet[ 7] & 0x0fff)     );
    adc[ 5] = ((packet[ 8] & 0x00ff) << 4) + ((packet[ 7] & 0xf000) >> 12);
    adc[ 6] = ((packet[ 9] & 0x000f) << 8) + ((packet[ 8] & 0xff00) >>  8);
    adc[ 7] = ((packet[ 9] & 0xfff0) >> 4);
    adc[ 8] = ((packet[10] & 0x0fff)     );
    adc[ 9] = ((packet[11] & 0x00ff) << 4) + ((packet[10] & 0xf000) >> 12);
    adc[10] = ((packet[12] & 0x000f) << 8) + ((packet[11] & 0xff00) >>  8);
    adc[11] = ((packet[12] & 0xfff0) >> 4);
    adc[12] = ((packet[13] & 0x0fff)     );
    adc[13] = ((packet[14] & 0x00ff) << 4) + ((packet[13] & 0xf000) >> 12);
    adc[14] = ((packet[15] & 0x000f) << 8) + ((packet[14] & 0xff00) >>  8);


    printf("%4i %5i %6i %6i %5i %5i",i,sid,tdc0,tdc1,tot0,tot1);
    printf(" %7i %7i %7i %7i %7i",adc[ 0],adc[ 1],adc[ 2],adc[ 3],adc[ 4]);
    printf(" %7i %7i %7i %7i %7i",adc[ 5],adc[ 6],adc[ 7],adc[ 8],adc[ 9]);
    printf(" %7i %7i %7i %7i %7i",adc[10],adc[11],adc[12],adc[13],adc[14]);
    printf("\n");
  }
  
}


//-----------------------------------------------------------------------------
// test2: each time, CFO requests one event
// if defined, OutputFn is the name of the output raw file
//-----------------------------------------------------------------------------
void read_one_event() {

  uint16_t  roc_reg[100];

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

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
//-----------------------------------------------------------------------------
// for some reason, it seems critical that we reset device time after sending requests for range. 
// ... double check...
// definition of a timeout ?
//-----------------------------------------------------------------------------
  monica_digi_clear     (&dtc);
  monica_var_link_config(&dtc);
//-----------------------------------------------------------------------------
// back to 25.6 us
//-----------------------------------------------------------------------------
  dev->write_register(0x91a8,100,0x400);

  int nev = 1;
  int i   = 0;
  cfo.SendRequestsForRange(nev,DTC_EventWindowTag(uint64_t(i)),incrementTimestamp,cfodelay,requestsAhead,heartbeatsAfter);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  dev->ResetDeviceTime();
    // cfo.SendRequestForTimestamp(DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    // std::this_thread::sleep_for(std::chrono::milliseconds(100));

  print_roc_registers(&dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

  size_t nbytes     (0);
  printf(" ----------------------------------------------------------------------- reading event %i\n",i);
    
  mu2e_databuff_t* buffer;
  read_dtc_event(&dtc,buffer,nbytes);

  uint16_t* ptr = (uint16_t*) buffer;
  int offset(0x20);  // 0x40 bytes

  analyze(ptr+offset,nbytes/2-offset);
}
