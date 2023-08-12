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
unsigned int reverseBits(unsigned int num) {
  unsigned int numOfBits = 10; // sizeof(num) * 8; // Number of bits in an unsigned int

  unsigned int reversedNum = 0;
  for (unsigned int i = 0; i < numOfBits; ++i) {
    if ((num & (1 << i)) != 0)
      reversedNum |= 1 << ((numOfBits - 1) - i);
  }

  return reversedNum;
}

//-----------------------------------------------------------------------------
// print 16 bytes per line
// size - number of bytes to print, even
//-----------------------------------------------------------------------------
void print_buffer(const void* ptr, int sz) {

  int     nw  = sz/2;
  ushort* p16 = (ushort*) ptr;
  int     n   = 0;

  for (int i=0; i<nw; i++) {
    if (n == 0) printf(" 0x%08x: ",i*2);

    ushort  word = p16[i];
    printf("0x%04x ",word);

    n   += 1;
    if (n == 8) {
      printf("\n");
      n = 0;
    }
  }

  if (n != 0) printf("\n");
}


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

  // DTCLib::Utilities::PrintBuffer(buffer, NBytes, 0);

  dev->read_release(DTC_DMA_Engine_DAQ, 1);

  if (delay > 0) usleep(delay);

  uint32_t res; 
  int      rc;

  rc = dev->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x\n",res); // expect: 0x40808404
  rc = dev->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x\n",res); // expect: 0x00000000
}

//-----------------------------------------------------------------------------
void analyze(uint16_t* Ptr, int NWords) {

  int type     = (Ptr[1] & 0xf0 ) >> 4;
  int npackets = Ptr[2];
  int nhits = npackets/2;

  printf(" fragment type: %i npackets : %5i nhits: %5i\n",type,npackets,nhits);

  printf("   i   sid       tdc0       tdc1 tot0 tot1 ewmc errf nadc pmp");
  printf(" adc[ 0] adc[ 1] adc[ 2] adc[ 3] adc[ 4] adc[ 5] adc[ 6] adc[ 7] adc[ 8] adc[ 9] adc[10]");
  printf(" adc[11] adc[12] adc[13] adc[14]");
  printf("\n");
  printf("-----------------------------------------------------------------------------");
  printf("--------------------------------------------");
  printf("------------------------------------------------------------\n");
  for (int i=0; i<nhits; i++) {
    uint16_t* dat = Ptr + 8+16*i;

    uint16_t  sid  =  dat[0];
    uint32_t  tdc0 =  ((uint32_t) dat[1]) | (((uint32_t)(dat[2] & 0x00ff)) << 16);
    uint16_t  tot0 = (dat[2] & 0x0f00) >>  8;
    uint16_t  ewmc = (dat[2] & 0xf000) >> 12;

    uint32_t  tdc1 =  ((uint32_t) dat[3]) | (((uint32_t)(dat[4] & 0x00ff)) << 16);
    uint16_t  tot1 = (dat[4] & 0x0f00) >>  8;
    uint16_t  errf = (dat[4] & 0xf000) >> 12;

    uint16_t  nadc = (dat[5] & 0x003f);
    uint16_t  pmp  = (dat[5] & 0xffc0) >> 6;

    uint16_t  adc[15];

    adc[ 0] = reverseBits((uint32_t) ((dat[ 6] & 0x03ff)     ));
    adc[ 1] = reverseBits((uint32_t) ((dat[ 6] & 0xfc00) >> 10) + ((dat[ 7] & 0x000f) << 6));
    adc[ 2] = reverseBits((uint32_t) ((dat[ 7] & 0x3ff0) >>  4));

    adc[ 3] = reverseBits(((dat[ 8] & 0x03ff)     ));
    adc[ 4] = reverseBits(((dat[ 8] & 0xfc00) >> 10) + ((dat[ 9] & 0x000f) << 6));
    adc[ 5] = reverseBits(((dat[ 9] & 0x3ff0) >>  4));

    adc[ 6] = reverseBits(((dat[10] & 0x03ff)     ));
    adc[ 7] = reverseBits(((dat[10] & 0xfc00) >> 10) + ((dat[11] & 0x000f) << 6));
    adc[ 8] = reverseBits(((dat[11] & 0x3ff0) >>  4));

    adc[ 9] = reverseBits(((dat[12] & 0x03ff)     ));
    adc[10] = reverseBits(((dat[12] & 0xfc00) >> 10) + ((dat[13] & 0x000f) << 6));
    adc[11] = reverseBits(((dat[13] & 0x3ff0) >>  4));

    adc[12] = reverseBits(((dat[14] & 0x03ff)     ));
    adc[13] = reverseBits(((dat[14] & 0xfc00) >> 10) + ((dat[15] & 0x000f) << 6));
    adc[14] = reverseBits(((dat[15] & 0x3ff0) >>  4));

    printf("%4i %5i %10i %10i %4i %4i %4i %4i",i,sid,tdc0,tdc1,tot0,tot1,ewmc,errf);
    printf(" %4i %3i",nadc,pmp);

    printf(" %7i %7i %7i %7i %7i %7i",adc[ 0],adc[ 1],adc[ 2],adc[ 3],adc[ 4],adc[ 5]);
    printf(" %7i %7i %7i %7i"        ,adc[ 6],adc[ 7],adc[ 8],adc[ 9]);
    printf(" %7i %7i %7i %7i %7i",adc[10],adc[11],adc[12],adc[13],adc[14]);
    printf("\n");
  }
  
}


//-----------------------------------------------------------------------------
// test2: each time, CFO requests one event
// if defined, OutputFn is the name of the output raw file
//-----------------------------------------------------------------------------
void read_one_event(int NEvents = 1, int Debug = 0) {

  uint16_t  roc_reg[100];

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

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
  std::this_thread::sleep_for(std::chrono::milliseconds(10));

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

  std::this_thread::sleep_for(std::chrono::milliseconds(10));

  int incrementTimestamp =    1;
  int heartbeatInterval  =  512; // was 1024;
  int requestsAhead      =    1;
  int heartbeatsAfter    =   16;

  int tmo_ms             = 100;  // was 1500
//-----------------------------------------------------------------------------
// for some reason, it seems critical that we reset device time after sending requests for range. 
// ... double check...
// definition of a timeout ?
//-----------------------------------------------------------------------------
  monica_digi_clear     (&dtc);
  monica_var_link_config(&dtc);
//-----------------------------------------------------------------------------
// back to 25.6 us - the readout doesn't seem to depend on this
//-----------------------------------------------------------------------------
//  dev->write_register(0x91a8,100,0x400);
  dev->write_register(0x91a8,100,0x100);

  int i   = 0;
  cfo.SendRequestsForRange(NEvents+1,DTC_EventWindowTag(uint64_t(i)),incrementTimestamp,heartbeatInterval,requestsAhead,heartbeatsAfter);
  std::this_thread::sleep_for(std::chrono::milliseconds(1000));

  dev->ResetDeviceTime();
    // cfo.SendRequestForTimestamp(DTC_EventWindowTag(uint64_t(i+1)), heartbeatsAfter);
    // std::this_thread::sleep_for(std::chrono::milliseconds(10));

  // print_roc_registers(&dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

  for (int ievent=0; i<NEvents; i++) {

    size_t nbytes     (0);
    printf(" ----------------------------------------------------------------------- reading event %i\n",i);
    
    mu2e_databuff_t* buffer;
    read_dtc_event(&dtc,buffer,nbytes);

    uint16_t* ptr = (uint16_t*) buffer;
    int offset(0x20);  // 0x40 bytes

		if (Debug > 0) {
			print_buffer(ptr,nbytes);
		}
    
    analyze(ptr+offset,nbytes/2-offset);
  };

   dev->release_all(DTC_DMA_Engine_DAQ);
}
