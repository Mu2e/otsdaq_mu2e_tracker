//
#define __CLING__ 1

#include "scripts/trk_utils.C"

#include "dtcInterfaceLib/DTC.h"
#include "dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// measure_thresholds:
// on mu2edaq09, a delay > 1.4 usec is needed after WriteROCRegister(258...)
// so can't do that for every event ...
// 
//-----------------------------------------------------------------------------
void set_threshold(int Link, int ChannelID, int Threshold, int PreampType, int ROCSleepTime = 2000) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);

  int roc_mask = 1 << (4*Link);
  DTC dtc(DTC_SimMode_NoCFO,-1,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
//-----------------------------------------------------------------------------
// write parameters into reg 266 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
  vector<uint16_t> vec;
  vec.push_back(uint16_t(ChannelID));
  vec.push_back(uint16_t(Threshold));
  vec.push_back(uint16_t(PreampType));

  bool increment_address(false);
  dtc.WriteROCBlock   (roc,266,vec,false,increment_address,100);
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
  dtc.ReadROCBlock(v2,roc,267,nw,false,100);

  print_buffer(v2.data(),nw);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  dtc.WriteROCRegister(roc,14,0x01,false,1000); 
  
//-----------------------------------------------------------------------------
// sleep
//-----------------------------------------------------------------------------
  // std::this_thread::sleep_for(std::chrono::milliseconds(SPISleepTime));
}
