#define __CLING__ 1

#ifndef __dtc_roc_reset_C__
#define __dtc_roc_reset_C__

#include "dtc_init.C"

namespace trk_daq {
  int gSleepTimeDTC      =  1000;  // [us]
  int gSleepTimeROC      =  2000;  // [us]
  int gSleepTimeROCReset =  4000;  // [us]
}

//-----------------------------------------------------------------------------
// assume that the DTC is already initialized
//-----------------------------------------------------------------------------
void dtc_roc_reset(int Link, int PcieAddress=-1) {

  int mask = 1 << Link*4;
  DTC* dtc = dtc_init(PcieAddress,Dtc::DefaultSimMode,mask);
  if (dtc == nullptr) return;

  auto link = DTC_Link_ID(Link);
  dtc->WriteROCRegister(link,14,     1,false,1000);                // 1 --> r14: reset ROC
  std::this_thread::sleep_for(std::chrono::microseconds(10*trk_daq::gSleepTimeROCReset));
}


#endif
