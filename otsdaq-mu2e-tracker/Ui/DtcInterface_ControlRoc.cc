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
  int DtcInterface::ControlRoc_Read(ControlRoc_Read_Input_t* Par, int LinkMask, bool UpdateMask, int PrintLevel) {
//-----------------------------------------------------------------------------
// write parameters into reg 266 (via block write), sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
    std::vector<uint16_t> vec;
  
    vec.push_back(Par->adc_mode);
    vec.push_back(Par->tdc_mode);
    vec.push_back(Par->num_lookback);
    
    if (Par->num_samples > 63) {
      printf("WARNING: num_samples = %i > 63, truncate to 63\n",Par->num_samples);
      Par->num_samples = 63;
    }
    vec.push_back(Par->num_samples);

    uint16_t w1 = Par->num_triggers[0];
    uint16_t w2 = Par->num_triggers[1];

    vec.push_back(w1);
    vec.push_back(w2);

    for (int i=0; i<6; i++) vec.push_back(Par->ch_mask[i]); 

    vec.push_back(Par->enable_pulser);
    vec.push_back(Par->marker_clock );
    vec.push_back(0 );
    vec.push_back(99);

    bool increment_address(false);

    if (UpdateMask and (LinkMask != 0)) fLinkMask = LinkMask;
  
    for (int i=0; i<6; i++) {
      int used = (fLinkMask >> 4*i) & 0x1;
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

      if (PrintLevel > 0) {
        PrintBuffer(v2.data(),nw);
        if (PrintLevel > 1) {
          trkdaq::ControlRoc_Read_Output_t* o = (trkdaq::ControlRoc_Read_Output_t*) v2.data();
          
          printf("adc_mode     : %i\n",o->adc_mode);
          printf("tdc_mode     : %i\n",o->tdc_mode);
          printf("num_lookback : %i\n",o->num_lookback);
          printf("num_samples  : %i\n",o->num_samples);
          printf("num_triggers : %5i %5i\n",o->num_triggers[0],o->num_triggers[1]);
          printf("ch_mask      : 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x\n",
                 o->ch_mask[0],o->ch_mask[1],o->ch_mask[2],o->ch_mask[3],o->ch_mask[4],o->ch_mask[5]);
          printf("enable_pulser : %i\n",o->enable_pulser);
          printf("marker_clock  : %i\n",o->marker_clock);
          printf("mode          : %i\n",o->mode);
          printf("clock         : %i\n",o->clock);
          printf("digi_read_0xb : 0x%04x\n",o->digi_read_0xb);
          printf("digi_read_0xe : 0x%04x\n",o->digi_read_0xe);
          printf("digi_read_0xd : 0x%04x\n",o->digi_read_0xd);
          printf("digi_read_0xc : 0x%04x\n",o->digi_read_0xc);
        }
      }
      
    }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    ResetRoc();
    return 0;
  }

};
