#define __CLING__ 1

#ifndef __dtc_reset_roc_C__
#define __dtc_reset_roc_C__

#include "dtc_init.C"

//-----------------------------------------------------------------------------
// rely on DTCLIB_DTC
// assume that the link is enabled
//-----------------------------------------------------------------------------
void dtc_reset_roc(int Link, int PcieAddress=-1) {

  int link_mask    = 1<<4*Link;
  DTC* dtc         = dtc_init(PcieAddress);
  DTC_Link_ID link = DTC_Link_ID(Link);

  dtc->WriteROCRegister(link,14,     1,false,1000);                // 1 --> r14: reset ROC
}


#endif
