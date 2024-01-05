//
#define __CLING__ 1

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

struct TrkSpiData_t {
  uint16_t  I3_3;
  uint16_t  I2_5;
  uint16_t  I1_8HV;
  uint16_t  IHV5_0;
  uint16_t  VDMBHV5_0;
  uint16_t  V1_8HV;
  uint16_t  V3_3HV;
  uint16_t  V2_5;
  uint16_t  A0;
  uint16_t  A1;
  uint16_t  A2;
  uint16_t  A3;
  uint16_t  I1_8CAL;
  uint16_t  I1_2;
  uint16_t  ICAL5_0;
  uint16_t  ADCSPARE;
  uint16_t  V3_3;
  uint16_t  VCAL5_0;
  uint16_t  V1_8CAL;
  uint16_t  V1_0;
  uint16_t  ROCPCBTEMP;
  uint16_t  HVPCBTEMP;
  uint16_t  CALPCBTEMP;
  uint16_t  RTD;
  uint16_t  ROC_RAIL_1V;
  uint16_t  ROC_RAIL_1_8V;
  uint16_t  ROC_RAIL_2_5V;
  uint16_t  ROC_TEMP;
  uint16_t  CAL_RAIL_1V;
  uint16_t  CAL_RAIL_1_8V;
  uint16_t  CAL_RAIL_2_5V;
  uint16_t  CAL_TEMP;
  uint16_t  HV_RAIL_1V;
  uint16_t  HV_RAIL_1_8V;
  uint16_t  HV_RAIL_2_5V;
  uint16_t  HV_TEMP;

  TrkSpiData_t*  spiData() { return (TrkSpiData_t*) &I3_3; }
  static int     nWords () { return sizeof(TrkSpiData_t)/2;      }
};


//-----------------------------------------------------------------------------
void parse_spi_data(uint16_t* dat, int nw) {

  const char* keys[] = {
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

//-----------------------------------------------------------------------------
// read_spi: read slow control data from one ROC at a given link 
// on mu2edaq09, a delay > 1.4 usec is needed after WriteROCRegister(258...)
// so can't do that for every event ...
//-----------------------------------------------------------------------------
void read_spi(int Link, int NEvents=1, int ROCSleepTime = 2000, int PrintParsedSPIData = 0) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);

  int roc_mask = 1 << (4*Link);
  DTC dtc(DTC_SimMode_NoCFO,-1,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  monica_var_link_config(&dtc,roc);

  for (int ievent=0; ievent<NEvents; ievent++) {
    monica_digi_clear     (&dtc,roc);
//-----------------------------------------------------------------------------
// after writing into reg 258, sleep for some time, 
// then wait till reg 128 returns non-zero
//-----------------------------------------------------------------------------
    dtc.WriteROCRegister   (roc,258,0x0000,false,100);
    std::this_thread::sleep_for(std::chrono::microseconds(ROCSleepTime));

    uint16_t u; 
    while ((u = dtc.ReadROCRegister(roc,128,100)) == 0) {}; 
    printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
    int nw = dtc.ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

    nw = nw-4;
    vector<uint16_t> spi;
    dtc.ReadROCBlock(spi,roc,258,nw,false,100);
//-----------------------------------------------------------------------------
// print SPI data in hex 
//-----------------------------------------------------------------------------
    print_buffer(spi.data(),nw);
//-----------------------------------------------------------------------------
// parse SPI data and print them
//-----------------------------------------------------------------------------
    if (PrintParsedSPIData) {
      parse_spi_data(spi.data(),nw);  // &spi[0]
    }
//-----------------------------------------------------------------------------
// sleep
//-----------------------------------------------------------------------------
    // std::this_thread::sleep_for(std::chrono::milliseconds(SPISleepTime));
  }

}
