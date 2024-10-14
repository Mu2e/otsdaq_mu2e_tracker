//
#define __CLING__ 1

#include "scripts/trk_utils.C"

#include "dtcInterfaceLib/DTC.h"
#include "dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// measure_thresholds:
// PcieAddress : DTC address on the PCIe bus, if -1, $DTCLIB_DTC is used
// 
// on mu2edaq09, a delay > 1.4 usec is needed after WriteROCRegister(258...)
// so can't do that for every event ...
//-----------------------------------------------------------------------------
void measure_thresholds(int PcieAddress, int Link, int ROCSleepTime = 2000) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);

  int roc_mask = 1 << (4*Link);
  DTC dtc(DTC_SimMode_NoCFO,PcieAddress,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
//-----------------------------------------------------------------------------
// write parameters into reg 264 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
  // int16_t  chan_num       (-1);
  uint16_t chan_mask[6] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};


  vector<uint16_t> vec;
  // vec.push_back(chan_num);
  vec.push_back(chan_mask[0]);
  vec.push_back(chan_mask[1]);
  vec.push_back(chan_mask[2]);
  vec.push_back(chan_mask[3]);
  vec.push_back(chan_mask[4]);
  vec.push_back(chan_mask[5]);

  bool increment_address(false);
  dtc.WriteROCBlock   (roc,270,vec,false,increment_address,100);
  std::this_thread::sleep_for(std::chrono::microseconds(ROCSleepTime));

  // 0x86 = 0x82 + 4
  uint16_t u; 
  while ((u = dtc.ReadROCRegister(roc,128,100)) != 0x8000) {}; 
  printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
  int nw = dtc.ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

  nw = nw-4;
  vector<uint16_t> v2;
  dtc.ReadROCBlock(v2,roc,270,nw,false,100);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  dtc.WriteROCRegister(roc,14,0x01,false,1000); 

  print_buffer(v2.data(),nw);
  // expect nw=288 = 96*3, if not - in trouble
  
  for (int i=0; i<96; i++) {
    float hw  = (-1000. + v2[i]*2000./1024.)/10.;
    float cal = (-1000. + v2[96+i]*2000./1024.)/10.;
    float tot = (-1000. + v2[192+i]*2000./1024.)/10.;

    printf(" i, hw, cal, tot : %3i %10.3f %10.3f %10.3f\n",i,hw,cal,tot);
  }
  
//-----------------------------------------------------------------------------
// sleep
//-----------------------------------------------------------------------------
  // std::this_thread::sleep_for(std::chrono::milliseconds(SPISleepTime));
}
