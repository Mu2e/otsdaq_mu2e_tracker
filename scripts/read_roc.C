///////////////////////////////////////////////////////////////////////////////
// 'read' command over the fiber
///////////////////////////////////////////////////////////////////////////////
#define __CLING__ 1

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "dtcInterfaceLib/DTC.h"
// #include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// measure_thresholds:
// on mu2edaq09, a delay > 1.4 usec is needed after WriteROCRegister(258...)
// so can't do that for every event ...
//-----------------------------------------------------------------------------
void read_roc(int Link, 
              int AdcMode, 
              int TdcMode,
              int ROCSleepTime = 2000) {
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
  uint16_t chan_mask[6]    = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
  int      num_triggers    = 10;
  int      num_lookback    = 8;
  int      num_samples     = 16;
  int      enable_pulser   = 0;
  int      max_total_delay = 1;
  int      marker_clock    = 0;

  vector<uint16_t> vec;
  vec.push_back(uint16_t(AdcMode));
  vec.push_back(uint16_t(TdcMode));
  vec.push_back(uint16_t(num_lookback));

  uint16_t w1 = ((num_triggers      ) & 0xffff);
  uint16_t w2 = ((num_triggers >> 16) & 0xffff);

  vec.push_back(w1);
  vec.push_back(w2);

  vec.push_back(uint16_t(chan_mask[0]));
  vec.push_back(uint16_t(chan_mask[1]));
  vec.push_back(uint16_t(chan_mask[2]));
  vec.push_back(uint16_t(chan_mask[3]));
  vec.push_back(uint16_t(chan_mask[4]));
  vec.push_back(uint16_t(chan_mask[5]));
  vec.push_back(uint16_t(num_samples));
  vec.push_back(uint16_t(enable_pulser));
  vec.push_back(uint16_t(max_total_delay));
  vec.push_back(uint16_t(marker_clock));

  bool increment_address(false);
  dtc.WriteROCBlock   (roc,265,vec,false,increment_address,100);
  std::this_thread::sleep_for(std::chrono::microseconds(ROCSleepTime));

  // 0x86 = 0x82 + 4
  uint16_t u; 
  while ((u = dtc.ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
  printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
  int nw = dtc.ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

  nw = nw-4;
  vector<uint16_t> v2;
  dtc.ReadROCBlock(v2,roc,265,nw,false,100);

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
