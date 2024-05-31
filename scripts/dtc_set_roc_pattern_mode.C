#define __CLING__ 1

#ifndef __dtc_set_roc_pattern_mode_C__
#define __dtc_set_roc_pattern_mode_C__

#include "dtc_init.C"

namespace trk_daq {
  int gSleepTimeDTC      =  1000;  // [us]
  int gSleepTimeROC      =  2000;  // [us]
  int gSleepTimeROCReset =  4000;  // [us]
}

//-----------------------------------------------------------------------------
// rely on DTCLIB_DTC
// handle LinkMask in the future
//-----------------------------------------------------------------------------
void dtc_set_roc_pattern_mode(int LinkMask, int PcieAddress=-1) {

  printf("%s : LinkMask = 0x%08x\n",__func__,LinkMask);

  DTC* dtc = trkdaq::DtcInterface::Instance(PcieAddress,LinkMask)->Dtc();

  for (int i=0; i<6; i++) {
    int used = (LinkMask >> 4*i) & 0x1;
    printf("dtc_set_roc_pattern_mode: link=%i, used = %i\n",i,used);
    if (used != 0) {
      auto link = DTC_Link_ID(i);

      dtc->EnableLink(link,DTC_LinkEnableMode(true,true));

      dtc->WriteROCRegister(link,14,     1,false,1000);                // 1 --> r14: reset ROC
      std::this_thread::sleep_for(std::chrono::microseconds(10*trk_daq::gSleepTimeROCReset));

      dtc->WriteROCRegister(link, 8,0x0010,false,1000);              // configure ROC to send patterns
      // std::this_thread::sleep_for(std::chrono::microseconds(10*trk_daq::gSleepTimeROC));

      dtc->WriteROCRegister(link,30,0x0000,false,1000);                // r30: mode, write 0 into it 
      // std::this_thread::sleep_for(std::chrono::microseconds(10*trk_daq::gSleepTimeROC));

      dtc->WriteROCRegister(link,29,0x0001,false,1000);                // r29: data version, currently 1
      // std::this_thread::sleep_for(std::chrono::microseconds(10*trk_daq::gSleepTimeROC));
    }
  }

  printf("%s : DONE\n",__func__);
}


#endif
