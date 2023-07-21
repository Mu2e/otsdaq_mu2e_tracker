//
#define __CLING__ 1

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;


//-----------------------------------------------------------------------------
// print 16 bytes per line
// size - number of bytes to print, even
//-----------------------------------------------------------------------------
void print_buffer(const void* ptr, int nw) {

  // int     nw  = nbytes/2;
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
// read_spi: read slow control data
//-----------------------------------------------------------------------------
void read_spi() {

  DTC dtc(DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint16_t u; 
  int      rc;

  dtc.WriteROCRegister(DTC_Link_0,258,0x0000,false,100);
  u = dtc.ReadROCRegister(DTC_Link_0,roc_address_t(128),100); printf("0x%04x\n",u);
  u = dtc.ReadROCRegister(DTC_Link_0,roc_address_t(129),100); printf("0x%04x\n",u);

  vector<uint16_t> dat;
  int const nw = 36;
  dtc.ReadROCBlock(dat,DTC_Link_0,258,nw,false,100);
//-----------------------------------------------------------------------------
// print SPI data in hex 
//-----------------------------------------------------------------------------
  print_buffer(&dat[0],nw);
//-----------------------------------------------------------------------------
// parse SPI data
//-----------------------------------------------------------------------------
  const char* keys[nw] = {
    "I3.3","I2.5","I1.8HV","IHV5.0","VDMBHV5.0","V1.8HV","V3.3HV" ,"V2.5"    , 
    "A0"  ,"A1"  ,"A2"    ,"A3"    ,"I1.8CAL"  ,"I1.2"  ,"ICAL5.0","ADCSPARE",
    "V3.3","VCAL5.0","V1.8CAL","V1.0","ROCPCBTEMP","HVPCBTEMP","CALPCBTEMP","RTD",
    "ROC_RAIL_1V(mV)","ROC_RAIL_1.8V(mV)","ROC_RAIL_2.5V(mV)","ROC_TEMP(CELSIUS)",
    "CAL_RAIL_1V(mV)","CAL_RAIL_1.8V(mV)","CAL_RAIL_2.5V(mV)","CAL_TEMP(CELSIUS)",
    "HV_RAIL_1V(mV)","HV_RAIL_1.8V(mV)","HV_RAIL_2.5V(mV)","HV_TEMP(CELSIUS)"
  };
//-----------------------------------------------------------------------------
// primary source : https://github.com/bonventre/trackerScripts/blob/master/constants.py#L99
//-----------------------------------------------------------------------------
  struct constants_t {
    float iconst  = 3.3 /(4096*0.006*20);
    float iconst5 = 3.25/(4096*0.500*20);
    float iconst1 = 3.25/(4096*0.005*20);
    float toffset = 0.509;
    float tslope  = 0.00645;
    float tconst  = 0.000806;
    float tlm45   = 0.080566;  
  } constants;

  float val[nw];

  for (int i=0; i<nw; i++) {
    if (i==20 or i==21 or i==22) {
      val[i] = dat[i]*constants.tlm45;
    }
    else if (i==0 or i==1 or i==2 or i==12 or i==13) {
      val[i] = dat[i]*constants.iconst;
    }
    else if (i==3 or i==14) {
      val[i] = dat[i]*constants.iconst5 ;
    }
    else if (i==4 or i==5 or i==6 or i==7 or i==16 or i==17 or i==18 or i==19) {
      val[i] = dat[i]*3.3*2/4096 ; 
    }
    else if (i==15) {
      val[i] = dat[i]*3.3/4096;
    }
    else if (i==23) {
      val[i] = dat[i]*3.3/4096;
    }
    else if (i==8 or i==9 or i==10 or i==11) {
      val[i] = dat[i];
    }
    else if (i > 23) {
      if   ((i%4) < 3) val[i] = dat[i]/8.;
      else             val[i] = dat[i]/16.-273.15;
    }

    printf("%-20s : %10.3f\n",keys[i],val[i]);
  }
}
