//
#define __CLING__ 1

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// measure_thresholds:
// on mu2edaq09, a delay > 1.4 usec is needed after WriteROCRegister(258...)
// so can't do that for every event ...
//-----------------------------------------------------------------------------
void pulser_on(int Link, int FirstChannel, int Freq = -1, int DutyCycle = 10, int Delay = 1000, 
               int ROCSleepTime = 5000) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);

  int roc_mask = 1 << (4*Link);
  DTC dtc(DTC_SimMode_NoCFO,-1,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
//-----------------------------------------------------------------------------
// write parameters into reg 268 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
  dtc.WriteROCRegister(roc,14,0x01,false,1000);  // reset teh roc

  vector<uint16_t> vec;

  uint16_t chan_mask = 16;
  if (FirstChannel >= 0) chan_mask |= (0x1 << FirstChannel);

  vec.push_back(uint16_t(FirstChannel));
  vec.push_back(uint16_t(chan_mask));
  vec.push_back(uint16_t(DutyCycle));

  // next go two halves of the delay, no frequency

  uint16_t w1 = ((Delay      ) & 0xffff);
  uint16_t w2 = ((Delay >> 16) & 0xffff);

  vec.push_back(w1);
  vec.push_back(w2);

  printf("vec[0]=%i\n",vec[0]);
  printf("vec[1]=%i\n",vec[1]);
  printf("vec[2]=%i\n",vec[2]);
  printf("vec[3]=%i\n",vec[3]);
  printf("vec[4]=%i\n",vec[4]);

  bool increment_address(false);

  dtc.ReadROCRegister(roc,129,100);

  // dtc.WriteROCBlock   (roc,278,vec,false,false,1000);
  dtc.WriteROCRegister(roc,268,FirstChannel,false,1000);
  std::this_thread::sleep_for(std::chrono::microseconds(ROCSleepTime));

  // 0x86 = 0x82 + 4
  uint16_t u; 
  while ((u = dtc.ReadROCRegister(roc,128,5000)) != 0x8000) {}; 
  printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
  int nw = dtc.ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

  nw = nw-4;
  vector<uint16_t> v2;
  dtc.ReadROCBlock(v2,roc,268,nw,false,100);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  print_buffer(v2.data(),nw);
  
//-----------------------------------------------------------------------------
// sleep
//-----------------------------------------------------------------------------
  // std::this_thread::sleep_for(std::chrono::milliseconds(SPISleepTime));
}
