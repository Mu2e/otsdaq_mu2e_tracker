/////////////////////////////////////////////////////////////////////////////
#include "TSystem.h"
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcGui_commands"


using namespace trkdaq;
using namespace std;
using namespace CFOLib;

//-----------------------------------------------------------------------------
// report DTC/CFO status
//-----------------------------------------------------------------------------
void DtcGui::cfo_print_status(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         {  Dtel->fCFO_i->PrintStatus(); }
  catch (...) { *TextView << Form("ERROR in DtcGui::%s: coudn't read the CFO\n",__func__); }
}


//-----------------------------------------------------------------------------
// report DTC/CFO status
//-----------------------------------------------------------------------------
void DtcGui::print_dtc_status() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  TString cmd;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" DtcGui::%s:START:\n",__func__);

  //  printf("Active TAB ID: %i\n",fActiveDtcID);

  if      (dtel->fData->fName == "CFO") {
    try {
      dtel->fCFO_i->PrintStatus();
    }
    catch (...) {
      *fTextView << Form("ERROR in DtcGui::%s: coudn't read the CFO\n",__func__);
    }
  }
  else if (dtel->fData->fName == "DTC") {
    try {
      dtel->fDTC_i->PrintStatus();
    }
    catch (...) {
      *fTextView << Form("ERROR in DtcGui::%s: coudn't read the DTC\n",__func__);
    }
  }

  TDatime x2; *fTextView << strCout.str() << x2.AsSQLString() << Form(" DtcGui::%s:DONE\n",__func__);

  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}


//-----------------------------------------------------------------------------
// this one chould be called for CFO only
//-----------------------------------------------------------------------------
void DtcGui::cfo_init_readout(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  TLOG(TLVL_INFO) << "START" << std::endl;
  try { 
//-----------------------------------------------------------------------------
// extract parameters, assume only one time chain
// also, assume that run plans are stored in ~/test_stand/cfo_run_plans directory
//-----------------------------------------------------------------------------
    uint dtc_mask;
    sscanf(Dtel->fDtcMask->GetText(),"0x%08x",&dtc_mask);

    std::string  run_plan = Form("%s/test_stand/cfo_run_plans/%s",
                                 gSystem->Getenv("HOME"),Dtel->fRunPlan->GetText());

    TLOG(TLVL_INFO) << Form("run_plan, dtc_mask: %s 0x%08x\n",run_plan.data(),dtc_mask);

    Dtel->fCFO_i->InitReadout(run_plan.data(),dtc_mask);
  }
  catch (...) { *fTextView << Form("ERROR : coudn't launch run plan... BAIL OUT\n"); }
  TLOG(TLVL_INFO) << "END" << std::endl;
}

//-----------------------------------------------------------------------------
void DtcGui::cfo_disable_beam_off() {
                                        // redirect cout
  streambuf* oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1;  *fTextView << x1.AsSQLString() << "DtcGui::" << __func__ << ": START" << std::endl;

  try         { fCFO_i->Cfo()->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL); }
  catch (...) { *fTextView << Form("ERROR : DtcGui::cfo_disable_beam_off failed\n"); }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("DtcGui::%s: DONE\n",__func__);
  fTextView->ShowBottom();
                                        // restore cout
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::cfo_enable_beam_off() {
                                        // redirect cout
  streambuf* oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1;  *fTextView << x1.AsSQLString() << "DtcGui::" << __func__ << ": START" << std::endl;

  try         { fCFO_i->Cfo()->EnableBeamOffMode(CFO_Link_ID::CFO_Link_ALL); }
  catch (...) { *fTextView << Form("ERROR : DtcGui::cfo_enable_beam_off failed\n"); }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("DtcGui::%s: DONE\n",__func__);
  fTextView->ShowBottom();
                                        // restore cout
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::cfo_launch_run_plan() {
                                        // redirect cout
  streambuf* oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1;  *fTextView << x1.AsSQLString() << Form("DtcGui::%s: START\n",__func__);

  try         { fCFO_i->LaunchRunPlan(); }
  catch (...) { *fTextView << Form("ERROR : coudn't launch run plan... BAIL OUT") << std::endl; }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("DtcGui::%s: DONE\n",__func__);
  fTextView->ShowBottom();
                                        // restore cout
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::cfo_soft_reset(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {

  try         { 
    *fTextView << "soft resetting the CFO" << std::endl; 
    Dtel->fCFO_i->Cfo()->SoftReset();
    *fTextView << "done soft resetting the CFO" << std::endl; 
  }
  catch (...) { 
    *fTextView << "ERROR : coudn't soft reset CFO ... BAIL OUT" << std::endl; 
  }
}

//-----------------------------------------------------------------------------
void DtcGui::cfo_hard_reset(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {

  try         { 
    *fTextView << "hard resetting the CFO" << std::endl; 
    Dtel->fCFO_i->Cfo()->HardReset();
    *fTextView << "done hard resetting the CFO" << std::endl; 
  }
  catch (...) { 
    *fTextView << "ERROR : coudn't hard reset the CFO ... BAIL OUT" << std::endl; 
  }
}

//-----------------------------------------------------------------------------
void DtcGui::cfo_set_ja_mode() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  
  try         {
    int mode;
    sscanf(dtel->fJAMode->GetText(),"0x%x",&mode);
    dtel->fCFO_i->SetJAMode(mode);
  }
  catch (...) { *fTextView << Form("ERROR in DtcGui::%s: failed to execute",__func__); }
}

//-----------------------------------------------------------------------------
void DtcGui::clear_output() {
  fTextView->Clear();
  fTextView->ShowBottom();
}

//-----------------------------------------------------------------------------
// assume DtcInterface is initialized, prepare to read out ROC-regenerated patterns
//-----------------------------------------------------------------------------
void DtcGui::configure_roc_pattern_mode() {

  streambuf* oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime t1; *fTextView << t1.AsSQLString() << " DtcGui::" << __func__ << ": START" << std::endl;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  dtel->fDTC_i->RocConfigurePatternMode();
  
  TDatime t2; *fTextView << t1.AsSQLString() << " DtcGui::" << __func__ << ": DONE" << std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::exit() {
  gApplication->Terminate();
}

//-----------------------------------------------------------------------------
void DtcGui::dtc_print_firefly_temp(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         { Dtel->fDTC_i->PrintFireflyTemp(); }
  catch (...) { *fTextView << Form("ERROR, BAIL OUT") << std::endl; }
}

//-----------------------------------------------------------------------------
void DtcGui::dtc_print_all_rocs(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         { Dtel->fDTC_i->PrintRocStatus(1,-1); }
  catch (...) { *fTextView << Form("ERROR, BAIL OUT") << std::endl; }
}

//-----------------------------------------------------------------------------
void DtcGui::print_roc_status() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  int              roc  = dtel->fActiveRocID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << "DtcGui:: : " << __func__ << ": START" << std::endl;
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  try         { dtel->fDTC_i->PrintRocStatus(1,1<<4*roc); }
  catch (...) { *fTextView << Form("ERROR : coudn't read ROC %i ... BAIL OUT",roc) << std::endl; }

  TDatime x2;
  *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ 
             << ": DONE; OK=reg(75) printed" << std::endl;
  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
// set DTC JA mode - 
//-----------------------------------------------------------------------------
void DtcGui::dtc_set_ja_mode() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  try         {
    int mode;
    sscanf(dtel->fJAMode->GetText(),"0x%x",&mode);
    dtel->fDTC_i->SetJAMode(mode);
  }
  catch (...) { *fTextView << Form("ERROR in DtcGui::%s: failed to execute",__func__); }
}

//-----------------------------------------------------------------------------
// init (preconfigured) readout
//-----------------------------------------------------------------------------
void DtcGui::dtc_init_readout(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         {
    int emulate_cfo      = Dtel->fEmulateCfo->GetIntNumber();
    int roc_readout_mode = Dtel->fRocReadoutMode->GetIntNumber();
    Dtel->fDTC_i->InitReadout(emulate_cfo,roc_readout_mode);
  }
  catch (...) { *TextView << Form("ERROR in DtcGui::%s: failed to execute DtcInterface::InitReadout\n",__func__); }
}

//-----------------------------------------------------------------------------
// init (preconfigured) readout
//-----------------------------------------------------------------------------
void DtcGui::dtc_launch_run_plan_emulated_cfo(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         {
    int ew_length  = fEWLength->GetIntNumber();
    int nevents    = fNEvents->GetIntNumber();
    int first_ewm  = fFirstTS->GetIntNumber();
    Dtel->fDTC_i->LaunchRunPlanEmulatedCfo(ew_length,nevents+1,first_ewm);
  }
  catch (...) { *TextView << Form("ERROR in DtcGui::%s: failed to execute DtcInterface::LaunchRunPlanEmulatedCfo\n",
                                  __func__); }
}

//-----------------------------------------------------------------------------
void DtcGui::dtc_soft_reset(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         { Dtel->fDTC_i->Dtc()->SoftReset(); }
  catch (...) { *TextView << "ERROR : coudn't soft reset DTC ... BAIL OUT" << std::endl; }
}

//-----------------------------------------------------------------------------
void DtcGui::dtc_hard_reset(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         { Dtel->fDTC_i->Dtc()->HardReset(); }
  catch (...) { *TextView << Form("ERROR : coudn't soft reset DTC ... BAIL OUT\n"); }
}

//-----------------------------------------------------------------------------
// read events buffered on ROCs just once. Always validate
//-----------------------------------------------------------------------------
void DtcGui::dtc_read_subevents(DtcTabElement_t* Dtel, TGTextViewostream* TextView) {
  try         {
    std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_dtc_blocks;

    int         first_ewm   = fFirstTS->GetIntNumber();
    int         print_level = 1;
    int         validate    = 1;
    const char* output_fn   = nullptr;
    Dtel->fDTC_i->ReadSubevents(list_of_dtc_blocks,first_ewm,print_level,validate,output_fn);
  }
  catch (...) { *TextView << Form("ERROR : coudn't read the DTC ... BAIL OUT\n"); }
}

//-----------------------------------------------------------------------------
void DtcGui::init_external_cfo_readout_mode() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  // int roc               = dtel->fActiveRocID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" %s: DTC ID: %i \n",__func__,fActiveDtcID);
//-----------------------------------------------------------------------------
// don't hide SoftReset in InitExternalCfoReadoutMode
//-----------------------------------------------------------------------------
  if (dtel->fData->fName == "DTC") {
    try         { 
      dtel->fDTC_i->Dtc()->SoftReset();
      dtel->fDTC_i->InitExternalCFOReadoutMode();
      dtel->fDTC_i->RocConfigurePatternMode();
    }
    catch (...) { *fTextView << Form("ERROR : filed  BAIL OUT") << std::endl; }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form(" %s: DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf(oldCoutStreamBuf);
}

//-----------------------------------------------------------------------------
void DtcGui::read_dtc_register() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  //  int roc               = dtel->fActiveRocID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );
//-----------------------------------------------------------------------------
// figure out the register to read
//-----------------------------------------------------------------------------
  uint reg;
  sscanf(dtel->fRegR->GetText(),"0x%x",&reg);
  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__  
             << " DTC ID:" << fActiveDtcID << " text: " << dtel->fRegR->GetText() 
             << " register: " << reg << std::endl;
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  int      timeout_ms(150);
  uint32_t val;
  if (dtel->fData->fName == "DTC") {
    try {
      *fTextView << Form("%s: reading DTC %i register 0x%08x\n",__func__,fActiveDtcID,reg);
      dtel->fDTC_i->Dtc()->GetDevice()->read_register(reg,timeout_ms,&val);
      *fTextView << Form(" value: 0x%08x\n",val);
      dtel->fValR->SetText(Form("0x%08x",val));
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't read DTC reg 0x%4x... BAIL OUT",__func__,reg) << std::endl;
    }
  }
  else if (dtel->fData->fName == "CFO") {
    try {
      *fTextView << Form("%s: reading CFO %i register 0x%08x",__func__,fActiveDtcID,reg);
      dtel->fCFO_i->Cfo()->GetDevice()->read_register(reg,timeout_ms,&val);
      *fTextView << Form(" value: 0x%08x\n",val);
      dtel->fValR->SetText(Form("0x%08x",val));
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't read CFO reg 0x%4x... BAIL OUT",__func__,reg) << std::endl;
    }
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ << " : DONE " <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::read_roc_register() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " << "read_roc_register" << std::endl;

  if (fDebugLevel > 1) printf("DtcGui::%s : Active DTC ID: %i\n",__func__,fActiveDtcID);

  int roc = dtel->fActiveRocID;

  RocTabElement_t* rtel = &dtel->fRocTel[roc];
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (dtel->fData->fName == "DTC") {
    uint reg;
    sscanf(rtel->fRegR->GetText(),"0x%x",&reg);
    try {
      int timeout_ms(150);
      uint16_t val = dtel->fDTC_i->Dtc()->ReadROCRegister(DTC_Link_ID(roc),reg,timeout_ms);
      rtel->fValR->SetText(Form("0x%04x",val));
      *fTextView << Form("%s: roc: %i reg : 0x%04x val: 0x%04x",__func__,roc,reg,val) << std::endl;
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't read ROC %i ... BAIL OUT",__func__,roc) << std::endl;
    }
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ << " : DONE " <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::read_subevents() {
  //  TString cmd;

  DtcTabElement_t* dtel        = fDtcTel+fActiveDtcID;
  streambuf*       old_cout_sb = cout.rdbuf();

  ostringstream str_cout;
  cout.rdbuf(str_cout.rdbuf());

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << std::endl;

  //  if (fDebugLevel > 1) printf("DtcGui::%s : Active DTC ID: %i\n",__func__,fActiveDtcID);

  // int              roc  = dtel->fActiveRocID;
  //  RocTabElement_t* rtel = &dtel->fRocTel[roc];
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (dtel->fData->fName == "DTC") {
    // uint reg;
    // sscanf(rtel->fRegR->GetText(),"0x%x",&reg);
    try {
      std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;
      // int timeout_ms(150);
      dtel->fDTC_i->ReadSubevents(list_of_subevents,0,1,0);
      // *fTextView << Form("%s: roc: %i reg : 0x%04x val: 0x%04x",__func__,roc,reg,val) << std::endl;
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't read event BAIL OUT",__func__) << std::endl;
    }
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << str_cout.str() << " DtcGui::" << __func__ << " : DONE " <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf(old_cout_sb);
}

//-----------------------------------------------------------------------------
void DtcGui::reset_roc() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  int roc               = dtel->fActiveRocID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" %s: DTC ID: %i roc: %i\n",__func__,fActiveDtcID,roc);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (dtel->fData->fName == "DTC") {
    int mask = 1<<4*roc;
    try         { dtel->fDTC_i->ResetRoc(mask); }
    catch (...) { *fTextView << Form("ERROR : coudn't reset ROC %i ... BAIL OUT",roc) << std::endl; }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form(" %s DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}


//-----------------------------------------------------------------------------
void DtcGui::set_nevents() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  int roc               = dtel->fActiveRocID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" %s: DTC ID: %i roc: %i\n",__func__,fActiveDtcID,roc);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  int nevents = 0;
  try         {
    nevents = fNEvents->GetNumberEntry()->GetIntNumber();
  }
  catch (...) { *fTextView << Form("ERROR : coudn't reset ROC %i ... BAIL OUT",roc) << std::endl; }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form(" %s DONE nevents = %10i\n",__func__,nevents);
  fTextView->ShowBottom();
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::set_ew_length() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  int roc               = dtel->fActiveRocID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" %s: DTC ID: %i roc: %i\n",__func__,fActiveDtcID,roc);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  int ew_length = 0;
  try         {
    ew_length = fEWLength->GetIntNumber();
  }
  catch (...) { *fTextView << Form("ERROR : coudn't read EW Length ... BAIL OUT",roc) << std::endl; }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form(" %s DONE ew_length = %10i\n",__func__,ew_length);
  fTextView->ShowBottom();
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::set_emulate_cfo() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" %s: DTC ID: %i\n",__func__,fActiveDtcID);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  int emulate_cfo = 0;
  try         {
    emulate_cfo = dtel->fEmulateCfo->GetNumberEntry()->GetIntNumber();
  }
  catch (...) { *fTextView << Form("ERROR : coudn't set emulate CFO flag... BAIL OUT\n"); }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form(" %s DONE, emulate_cfo = %i\n",__func__,emulate_cfo);
  fTextView->ShowBottom();
  cout.rdbuf( oldCoutStreamBuf );
}


//-----------------------------------------------------------------------------
void DtcGui::set_roc_readout_mode() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" %s: DTC ID: %i\n",__func__,fActiveDtcID);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  int roc_readout_mode = 0;
  try         {
    roc_readout_mode = dtel->fRocReadoutMode->GetHexNumber();
    dtel->fDTC_i->SetRocReadoutMode(roc_readout_mode);
  }
  catch (...) { *fTextView << Form("ERROR : coudn't set ROC readout mode... BAIL OUT\n"); }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form(" %s DONE, roc_readout_mode = %i\n",
                                                                      __func__,roc_readout_mode);
  fTextView->ShowBottom();
  cout.rdbuf( oldCoutStreamBuf );
}


//-----------------------------------------------------------------------------
void DtcGui::write_dtc_register() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " << "read_register" << std::endl;

  //  printf("Active TAB ID: %i\n",fActiveDtcID);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
// figure out the register to read
//-----------------------------------------------------------------------------
  uint reg, val;
  sscanf(dtel->fRegW->GetText(),"0x%x",&reg);
  sscanf(dtel->fValW->GetText(),"0x%x",&val);

  if (dtel->fData->fName == "DTC") {
    try {
      int timeout_ms(150);
      dtel->fDTC_i->Dtc()->GetDevice()->write_register(reg,timeout_ms,val);
      *fTextView << Form("%s: DTC %i reg 0x%8x val: 0x%8x",__func__,fActiveDtcID,reg,val) << std::endl;
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't write DTC reg 0x%4x... BAIL OUT",__func__,reg) << std::endl;
    }
  }
  else if (dtel->fData->fName == "CFO") {
    try {
      int timeout_ms(150);
      dtel->fCFO_i->Cfo()->GetDevice()->write_register(reg,timeout_ms,val);
      *fTextView << Form("%s: CFO %i reg 0x%8x val: 0x%8x",__func__,fActiveDtcID,reg,val) << std::endl;
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't write CFO reg 0x%4x... BAIL OUT",__func__,reg) << std::endl;
    }
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ << " : DONE " <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::write_roc_register() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  int roc               = dtel->fActiveRocID;
  RocTabElement_t* rtel = &dtel->fRocTel[roc];

  streambuf*    oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  uint reg, val;
  sscanf(rtel->fRegW->GetText(),"0x%x",&reg);
  sscanf(rtel->fValW->GetText(),"0x%x",&val);

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__
             << " DTC: " << fActiveDtcID
             << " ROC:" << roc
             << " reg:0x" << std::hex << reg
             << " val:0x" << std::hex << val
             << std::endl;

//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  try {
    int timeout_ms(150);
    dtel->fDTC_i->Dtc()->WriteROCRegister(DTC_Link_ID(roc),reg,val,false,timeout_ms);
  }
  catch (...) {
    *fTextView << Form("ERROR in %s: coudn't write ROC %i ... BAIL OUT",__func__,roc) << std::endl;
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ << " : DONE " <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
// set new active ROC tab
//-----------------------------------------------------------------------------
void DtcGui::DoRocTab(Int_t id) {

  DtcTabElement_t* dtel = &fDtcTel[fActiveDtcID];
  int old_active_id     = dtel->fActiveRocID;

  if (fDebugLevel > 0) {
    printf ("%s: active_dtc_id: %i new_active_id = %i old_active_id = %i\n",__func__, fActiveDtcID, id, old_active_id);
  }

  if (id != old_active_id) {
    RocTabElement_t* old_active_tel = &dtel->fRocTel[old_active_id];
    RocTabElement_t* tel            = &dtel->fRocTel[id];
    old_active_tel->fTab->ChangeBackground(tel->fTab->GetBackground());
    tel->fTab->ChangeBackground(fYellow);

    dtel->fActiveRocID  = id;
    dtel->fActiveRocTel = tel;
  }

  if (fDebugLevel > 0) {
    printf("%s: DONE\n",__func__);
  }
}

//-----------------------------------------------------------------------------
// set new active tab
//-----------------------------------------------------------------------------
void DtcGui::DoDtcTab(Int_t id) {

  if (id != fActiveDtcID) {
    int           prev_id = fActiveDtcID;
    TGTabElement* tel     = fDtcTab->GetTabTab(id);

    if (fActiveDtcTab != tel) {
      tel->ChangeBackground(fYellow);
      fActiveDtcTab->ChangeBackground(fDtcTel[prev_id].fColor);
      fActiveDtcTab = tel;
    }

    fActiveDtcID = id;
    fActiveDtc   = &fDtcData[id];
  }
}

//-----------------------------------------------------------------------------
// this is a "hub" for exacuting all commands interactive commands
//-----------------------------------------------------------------------------
int DtcGui::execute_command() {

  TGButton* btn = (TGButton*) gTQSender;

  //  TString*  cmd = (TString*) btn->GetUserData();

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  //  int roc               = dtel->fActiveRocID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());
  
  TDatime x1; *fTextView << x1.AsSQLString() << Form(" DtcGui::%s\n",__func__);
  void (*fun)(DtcGui*,DtcTabElement_t*, TGTextViewostream*);
  fun = (void (*)(DtcGui*, DtcGui::DtcTabElement_t*, TGTextViewostream*)) btn->GetUserData();
  fun(this,dtel,fTextView);
  
  *fTextView << strCout.str();
  TDatime x2; *fTextView << x2.AsSQLString() << Form(" DtcGui::%s: FINISHED\n",__func__);
  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );

  return 0;
}
