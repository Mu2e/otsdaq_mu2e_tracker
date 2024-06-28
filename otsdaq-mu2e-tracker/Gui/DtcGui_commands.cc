
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;
using namespace std;

//-----------------------------------------------------------------------------
void DtcGui::ExecuteCommand(const char* Cmd, int PrintOnly) {

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  // Will output our Hello World! from above.
  cout << strCout.str();

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::ExecuteCommand : cmd: " << Cmd << std::endl;

  //  printf("# %s : -------------- DtcGui::ExecuteCommand executing cmd: %s\n",x1.AsSQLString(),Cmd);

  if (PrintOnly != 1) {
    char buf[10001];
    FILE* pipe = gSystem->OpenPipe(Cmd,"r");
    while (fgets(buf,10000,pipe)) { 
      *fTextView << buf << std::endl;
    }
    gSystem->ClosePipe(pipe);
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << gSystem->GetFromPipe(Cmd).Data() 
             << " DtcGui::ExecuteCommand : DONE " <<  std::endl;

  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
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

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::ExecuteCommand : cmd: " << "print_status" << std::endl;

  //  printf("Active TAB ID: %i\n",fActiveDtcID);

  if      (dtel->fData->fName == "CFO") {
    try {
      dtel->fCFO_i->PrintStatus();
    }
    catch (...) {
      *fTextView << Form("ERROR : coudn't read CFO") << std::endl;
    }
  }
  else if (dtel->fData->fName == "DTC") {
    try {
      dtel->fDTC_i->PrintStatus();
    }
    catch (...) {
      *fTextView << Form("ERROR : coudn't read DTC") << std::endl;
    }
  }

  TDatime x2;
  *fTextView << strCout.str() 
             << x2.AsSQLString() 
             << " DtcGui::ExecuteCommand : DONE " <<  std::endl;

  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );

  // ExecuteCommand(cmd.Data(),fDebugLevel);
}


//-----------------------------------------------------------------------------
// this one chould be called for CFO only
//-----------------------------------------------------------------------------
void DtcGui::cfo_init_readout() {
                                        // redirect cout
  streambuf* oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1;  *fTextView << x1.AsSQLString() << "DtcGui:: : " << __func__ << ": START" << std::endl;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  if (dtel->fData->fName == "CFO") {
    try         { 
//-----------------------------------------------------------------------------
// extract parameters, assume only one time chain
//-----------------------------------------------------------------------------
      int ndtcs[8] = {0,0,0,0,0,0,0,0};

      // sscanf(dtel->fTimeChainLink->GetText(),"%i",&time_chain_link);
      sscanf(dtel->fNDtcs->GetText()        ,"%i",&ndtcs[0]);

      const char* run_plan = dtel->fRunPlan->GetText();

      if (fDebugLevel > 0) {
        *fTextView << Form("run_pan, ndtcs[0]: %s %i",run_plan,ndtcs[0]) << std::endl; 
      }

      dtel->fCFO_i->InitReadout(run_plan,ndtcs); 
    }
    catch (...) { *fTextView << Form("ERROR : coudn't launch run plan... BAIL OUT") << std::endl; }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ << ": DONE " << std::endl;
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

  TDatime x1;  *fTextView << x1.AsSQLString() << "DtcGui:: : " << __func__ << ": START" << std::endl;

  try         { fCFO_i->LaunchRunPlan(); }
  catch (...) { *fTextView << Form("ERROR : coudn't launch run plan... BAIL OUT") << std::endl; }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ << ": DONE " << std::endl;
  fTextView->ShowBottom();
                                        // restore cout
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::clear_output() {
  fTextView->Clear();
}

//-----------------------------------------------------------------------------
void DtcGui::exit() {
  gApplication->Terminate();
}

//-----------------------------------------------------------------------------
void DtcGui::print_firefly_temp() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  //  int              roc  = dtel->fActiveRocID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui:: : " << __func__ << ": START" << std::endl;

  if (dtel->fData->fName == "DTC") {
    try         { dtel->fDTC_i->PrintFireflyTemp(); }
    catch (...) { *fTextView << Form("ERROR, BAIL OUT") << std::endl; }
  }

  *fTextView << strCout.str() ;  // endl is there
  TDatime x2; *fTextView << x2.AsSQLString() << " DtcGui::" << __func__ << ": DONE" << std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
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
  if (dtel->fData->fName == "DTC") {
    try         { dtel->fDTC_i->PrintRocStatus(roc); }
    catch (...) { *fTextView << Form("ERROR : coudn't read ROC %i ... BAIL OUT",roc) << std::endl; }
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::" << __func__ 
             << ": DONE; OK=reg(75) printed" << std::endl;
  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
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
    try         { dtel->fDTC_i->ResetRoc(roc); }
    catch (...) { *fTextView << Form("ERROR : coudn't reset ROC %i ... BAIL OUT",roc) << std::endl; }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("%s DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );
}

//-----------------------------------------------------------------------------
void DtcGui::dtc_soft_reset() {

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
    try         { 
      *fTextView << "resetting the DTC" << std::endl; 
      dtel->fDTC_i->Dtc()->SoftReset();
      *fTextView << "done resetting the DTC" << std::endl; 
    }
    catch (...) { 
      *fTextView << "ERROR : coudn't soft reset DTC ... BAIL OUT" << std::endl; 
    }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("%s DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf(oldCoutStreamBuf);
}

//-----------------------------------------------------------------------------
void DtcGui::dtc_hard_reset() {

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
    try         { 
      *fTextView << "resetting the DTC" << std::endl; 
      dtel->fDTC_i->Dtc()->SoftReset();
      *fTextView << "done resetting the DTC" << std::endl; 
    }
    catch (...) { 
      *fTextView << "ERROR : coudn't hard reset the DTC ... BAIL OUT" << std::endl; 
    }
  }
  else if (dtel->fData->fName == "CFO") {
    try         { 
      *fTextView << "resetting the CFO" << std::endl; 
      dtel->fCFO_i->Cfo()->HardReset();
      *fTextView << "done resetting the CFO" << std::endl; 
    }
    catch (...) { 
      *fTextView << "ERROR : coudn't hard reset the CFO ... BAIL OUT" << std::endl; 
    }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form(" %s: DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf(oldCoutStreamBuf);
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
      dtel->fDTC_i->InitExternalCFOReadoutMode(0);
      int linkmask = dtel->fDTC_i->fLinkMask;
      dtel->fDTC_i->RocPatternConfig(linkmask);
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

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " 
             << "write_roc_register DTC:" << fActiveDtcID << " ROC:" << roc << std::endl;

  uint reg, val;
  sscanf(rtel->fRegW->GetText(),"0x%x",&reg);
  sscanf(rtel->fValW->GetText(),"0x%x",&val);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (dtel->fData->fName == "DTC") {
    try {
      int timeout_ms(150);
      dtel->fDTC_i->Dtc()->WriteROCRegister(DTC_Link_ID(roc),reg,val,timeout_ms,false);
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't write ROC %i ... BAIL OUT",__func__,roc) << std::endl;
    }
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
