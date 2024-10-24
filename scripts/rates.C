//
#define __CLING__ 1

#include "scripts/trk_utils.C"

#include "dtcInterfaceLib/DTC.h"
#include "dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// should be 96*3*2+2*2 = 580 16-bit words
// 3 words per channel (straw)
//-----------------------------------------------------------------------------
void print_rates(uint16_t* data, int nw) {
  if (nw != 580) {
    printf("ERROR: nw = %5i != 580. BAIL OUT\n",nw);
    return;
  }

  printf(" channel  Total(HV) Total(CAL) Total(HV.and.CAL)\n");
  printf("------------------------------------------------\n");
  int loc(0);
  for (int ich=0; ich<96; ich++) {
    loc           = 6*ich;
    int rate_hv   = int(data[loc  ])+(int(data[loc+1]) << 16);
    int rate_cal  = int(data[loc+2])+(int(data[loc+3]) << 16);
    int rate_coic = int(data[loc+4])+(int(data[loc+5]) << 16);
    printf(" %5i %10i %10i %10i\n",ich,rate_hv, rate_cal, rate_coic);
  }
  // finally, the last two words - totatl counts
  loc = 576;
  int iw1   = int(data[loc  ])+(int(data[loc+1]) << 16);
  int iw2   = int(data[loc+2])+(int(data[loc+3]) << 16);
  printf(" total_hv: %10i total_cal: %10i\n",iw1,iw2);
}

//-----------------------------------------------------------------------------
// PrintLevel=0: no printout
//            1: hex dump
//            2: formatted printout
//-----------------------------------------------------------------------------
void rates(int Link, int PrintLevel = 0) {
//-----------------------------------------------------------------------------
// convert into enum
// DTC has already been initialized, don't reco
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);


  DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);
  DTCLib::DTC*  dtc   = dtc_i->Dtc();
  
  int roc_mask        = 1 << (4*Link);

  dtc->WriteROCRegister(roc,14,0x01,false,1000);  // reset the roc
//-----------------------------------------------------------------------------
// write parameters into reg ***  (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
// chan mask always includes the first channel
//-----------------------------------------------------------------------------
  vector<uint16_t> vec;

  int num_lookback = 100;
  vec.push_back(uint16_t(num_lookback));
  int num_samples = 10;
  vec.push_back(uint16_t(num_samples));

  uint16_t ch_mask[6];
  for (int i=0; i<6; i++) {
    ch_mask[i] = 0xffff;
    vec.push_back(ch_mask[i]);
  }
  
  dtc->WriteROCBlock   (roc,271,vec,false,false,1000);
  std::this_thread::sleep_for(std::chrono::microseconds(1000));

  // 0x86 = 0x82 + 4
  uint16_t u; 
  while ((u = dtc->ReadROCRegister(roc,128,5000)) != 0x8000) {}; 
  printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
  int nw = dtc->ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

  nw = nw-4;
  vector<uint16_t> v2;
  dtc->ReadROCBlock(v2,roc,271,nw,false,100);
//-----------------------------------------------------------------------------
// print output - in two formats
//-----------------------------------------------------------------------------
  if (PrintLevel > 0) {
    print_buffer(v2.data(),nw);
  }
  if (PrintLevel > 1) {
    print_rates(v2.data(),nw);
  }
}
