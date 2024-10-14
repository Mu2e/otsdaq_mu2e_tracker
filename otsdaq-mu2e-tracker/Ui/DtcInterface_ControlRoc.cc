//
#include <vector>
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterface_ControlRoc"

#include "TString.h"     // includes ROOT's Form

using namespace DTCLib;

namespace  trkdaq {
//-----------------------------------------------------------------------------
// a boilerplate for a generic control_ROC.py CLI command - do we need it at  all ?
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc(const char* Command, void* Par) {
    return 0;
  }
  
//-----------------------------------------------------------------------------
// what should it return ? - for now, return 0
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_Read(ControlRoc_Read_Par_t* Par, int LinkMask, bool UpdateMask, int PrintLevel) {
//-----------------------------------------------------------------------------
// write parameters into reg 266 (via block write), sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
    std::vector<uint16_t> vec;
  
    vec.push_back(Par->adc_mode);
    vec.push_back(Par->tdc_mode);
    vec.push_back(Par->lookback);

    uint16_t w1 = ((Par->triggers      ) & 0xffff);
    uint16_t w2 = ((Par->triggers >> 16) & 0xffff);

    vec.push_back(w1);
    vec.push_back(w2);

    for (int i=0; i<6; i++) vec.push_back(Par->chan_mask[i]); 

    vec.push_back(Par->samples);
    vec.push_back(Par->pulser );
    vec.push_back(Par->delay  );
    vec.push_back(Par->mode   );

    bool increment_address(false);

    if (UpdateMask) fLinkMask = LinkMask;
  
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (not used)                                           continue;
      auto roc  = DTC_Link_ID(i);
      fDtc->WriteROCBlock   (roc,265,vec,false,increment_address,100);
      std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
      
      // 0x86 = 0x82 + 4
      uint16_t u; 
      while ((u = fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
      TLOG(TLVL_DEBUG+1) << Form("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
      int nw = fDtc->ReadROCRegister(roc,129,100);
      TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",129,nw);

      nw = nw-4;
      std::vector<uint16_t> v2;
      fDtc->ReadROCBlock(v2,roc,265,nw,false,100);
      
      PrintBuffer(v2.data(),nw);
      
      Par->hvcal_mask1         = v2[13];
      Par->hvcal_mask2         = v2[14];
      Par->hvcal_mask3         = v2[15];
      Par->hvcal_enable_pulser = v2[16];
      Par->output_mode         = v2[17];
      Par->return_code         = 0;         // TBD
    }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    ResetRoc();
    return 0;
  }

};
