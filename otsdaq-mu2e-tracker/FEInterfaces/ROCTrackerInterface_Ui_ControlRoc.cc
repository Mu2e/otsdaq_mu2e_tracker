
//-----------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Wed Dec  3 23:47:06 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//-----------------------------------------------------------------------------


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"


//==============================================================================
/// a boilerplate for a generic control_ROC.py CLI command - do we need it at  all 
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Wed Dec  3 23:47:06 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
  int ROCTrackerInterface::Ui_ControlRoc_ControlRoc(const char* Command, void* Par)
{
    return 0;
  } // end Ui_ControlRoc_ControlRoc()

//==============================================================================
/// digi_rw over the fiber: reg 26
/// if Link = -1, use fLinkMask, otherwise operate assuming a single lin
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Wed Dec  3 23:47:06 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
  int ROCTrackerInterface::Ui_ControlRoc_ControlRoc_DigiRW(ControlRoc_DigiRW_Input_t*  Input     ,
                                      ControlRoc_DigiRW_Output_t* Output    ,
                                      int                         Link      ,
                                      int                         PrintLevel,
                                      std::ostream&               Stream    )
{
//-----------------------------------------------------------------------------
//    const int  reg (263);  // for digi_rw
    
    std::vector<uint16_t> vec;
  
    vec.push_back(Input->rw     );
    vec.push_back(Input->hvcal  );
    vec.push_back(Input->address);
    vec.push_back(Input->data[0]);
    vec.push_back(Input->data[1]);
    
    if (PrintLevel > 0) {
      Stream << Form("Input->rw           : %i\n"    ,Input->rw);
      Stream << Form("Input->hwcal        : 0x%04x\n",Input->hvcal);
      Stream << Form("Input->address      : 0x%04x\n",Input->address);
      Stream << Form("Input->data[0]      : 0x%04x\n",Input->data[0]);
      Stream << Form("Input->data[1]      : 0x%04x\n",Input->data[1]);
    }
    
    bool increment_address(false);
//-----------------------------------------------------------------------------
// if Link != -1, use it, but don't redefine fLinkMask - that would be wa-a-ay too smart !
//-----------------------------------------------------------------------------
    int link1(Link), link2(Link+1);
    if (Link == -1) {
      link1 = 0;
      link2 = 6;
    }
//-----------------------------------------------------------------------------
// loop over the links and execute
//-----------------------------------------------------------------------------
    for (int i=link1; i<link2; i++) {
      if (not LinkEnabled(i))                                 continue;
      auto roc  = DTCLib::DTC_Link_ID(i);
      try {
        getDTC()->WriteROCBlock   (roc,REG_DIGIRW,vec,false,increment_address,100);
        std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
      
        uint16_t u; 
        while ((u = getDTC()->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
        if (PrintLevel & 0x8) Stream << Form("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
        int nw = getDTC()->ReadROCRegister(roc,129,100);
        if (PrintLevel & 0x8) Stream << Form("reg:%03i val:0x%04x\n",129,nw);

        nw = nw-4;
        std::vector<uint16_t> v2;
        getDTC()->ReadROCBlock(v2,roc,REG_DIGIRW,nw,false,100);

        if (PrintLevel > 0) {
          if (PrintLevel & 0x8) Stream << " ---------------- link:" << i << ":";
          if (PrintLevel & 0x1) DTCLib::Utilities::PrintBuffer(v2.data(),nw,&Stream);
          if (PrintLevel & 0x2) {
            Stream << std::endl;

            trkdaq::ControlRoc_DigiRW_Output_t* o = (trkdaq::ControlRoc_DigiRW_Output_t*) v2.data();
          
            Stream << Form("rw           : %i\n"        ,o->rw);
            Stream << Form("hvcal        : 0x%04x\n"    ,o->hvcal);
            Stream << Form("address      : 0x%04x\n"    ,o->address);
            Stream << Form("data[32 bit] : 0x%04x%04x\n",o->data[1],o->data[0]);
            Stream << Form("adc_num      : 0x%04x\n"    ,o->adc_num);
            Stream << Form("adc_mask     : 0x%04x\n"    ,o->adc_mask);
          }
        }
      }
      catch (...) {
        TLOG(TLVL_ERROR) << "ERROR reading link:" << i;
        Stream << std::format("ERROR reading link:{}",i) << std::endl;
      }
    }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    // ResetLinks();
    return 0;
  } // end Ui_ControlRoc_ControlRoc_DigiRW()

