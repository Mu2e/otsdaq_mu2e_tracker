//
#define __CLING__ 1

#include "scripts/trk_utils.C"

#include "dtcInterfaceLib/DTC.h"
#include "dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;
//-----------------------------------------------------------------------------
// rates: not implemented yet
// on mu2edaq09, a delay > 1.4 usec is needed after WriteROCRegister(258...)
// so can't do that for every event ...
// Firstchannel values: 0 to 7
//-----------------------------------------------------------------------------
void rates( ,              , int PrintLevel = 0, int ROCSleepTime = 5000) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);

  int roc_mask = 1 << (4*Link);
  DTC dtc(DTC_SimMode_NoCFO,-1,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  dtc.WriteROCRegister(roc,14,0x01,false,1000);  // reset the roc
//-----------------------------------------------------------------------------
// write parameters into reg ***  (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
// chan mask always includes the first channel
//-----------------------------------------------------------------------------
  vector<uint16_t> vec;

  uint16_t chan_mask = 0x10 | (0x1 << FirstChannel);

  vec.push_back(uint16_t(chan_mask));
  vec.push_back(uint16_t(DutyCycle));

  // next go two halves of the delay, no frequency

  uint16_t w1 = ((Delay      ) & 0xffff);
  uint16_t w2 = ((Delay >> 16) & 0xffff);

  vec.push_back(w1);
  vec.push_back(w2);

  printf("vec[0]=0x%04x\n",vec[0]);
  printf("vec[1]=%i\n"    ,vec[1]);
  printf("vec[2]=%i\n"    ,vec[2]);
  printf("vec[3]=%i\n"    ,vec[3]);

  bool increment_address(false);

  dtc.ReadROCRegister(roc,129,100);

  dtc.WriteROCBlock   (roc,***,vec,false,false,1000);
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
  dtc.ReadROCBlock(v2,roc,***,nw,false,100);
//-----------------------------------------------------------------------------
// print output - in two formats
//-----------------------------------------------------------------------------
  print_buffer(v2.data(),nw);

  int XXXX      = v2[0];
  int XXX  = v2[1];
  int XX       = v2[2] | (v2[3] << 16);

  printf(" chmask = 0x%04x duty cycle = %5i delay = 0x%08x\n",chmask, duty_cycle, delay); 
//-----------------------------------------------------------------------------
// sleep
//-----------------------------------------------------------------------------
  // std::this_thread::sleep_for(std::chrono::milliseconds(SPISleepTime));
}
