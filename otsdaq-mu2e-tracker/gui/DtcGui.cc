
#include "otsdaq-mu2e-tracker/gui/DtcGui.hh"

using namespace trkdaq;
using namespace std;
//-----------------------------------------------------------------------------
// initialization with the project data - in its rudimentary form
//-----------------------------------------------------------------------------
DtcGui::DtcGui(const char* Host, const TGWindow *p, UInt_t w, UInt_t h, int DebugLevel) {

  fHost           = Host;
  fDebugLevel     = DebugLevel;
  fSubmittedColor = 16724889;

  fNDtcs = 2;  // 0:9

  uint32_t val;

  if (fDebugLevel > 0) printf("DtcGui::%s : start\n",__func__);

  for (int pcie_addr=0; pcie_addr<2; pcie_addr++) {
    DTC dtc(DTC_SimMode_Disabled,pcie_addr,0x11);

    dtc.GetDevice()->read_register(0x9004,100,&val);
    if      ((val >> 28) == 0xd) { 
      fDtcData[pcie_addr] = DtcData_t("DTC" ,pcie_addr);
    }
    else if ((val >> 28) == 0xc) {
      fDtcData[pcie_addr] = DtcData_t("CFO" ,pcie_addr);
    }
    else {
      fDtcData[pcie_addr] = DtcData_t("NONE",pcie_addr);
    }
  }

  if (fDebugLevel > 0) printf("DtcGui::%s : before BuildGui\n",__func__);
  BuildGui(p,w,h);
  if (fDebugLevel > 0) printf("DtcGui::%s : after  BuildGui\n",__func__);
//-----------------------------------------------------------------------------
// two PCIE cards - rely on CFOLIB_CFO and DTCLIB_DTC
//-----------------------------------------------------------------------------
  for (int pcie_addr=0; pcie_addr<2; pcie_addr++) {
    if      (fDtcData[pcie_addr].fName == "DTC") {
      fDtcTel[pcie_addr].fDTC_i = DtcInterface::Instance(pcie_addr);
    }
    else if (fDtcData[pcie_addr].fName == "CFO") {
      fDtcTel[pcie_addr].fCFO_i = CfoInterface::Instance(pcie_addr);
    }
  }

  if (fDebugLevel > 0) printf("DtcGui::%s : all done, EXIT\n", __func__);
}

//-----------------------------------------------------------------------------
DtcGui::~DtcGui() {
   fMainFrame->Cleanup();
}


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
void DtcGui::cfo_launch_run_plan() {
                                        // redirect cout
  streambuf* oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1;  *fTextView << x1.AsSQLString() << "DtcGui:: : " << __func__ << ": START" << std::endl;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  if (dtel->fData->fName == "CFO") {
    try         { dtel->fCFO_i->LaunchRunPlan(); }
    catch (...) { *fTextView << Form("ERROR : coudn't launch run plan... BAIL OUT") << std::endl; }
  }

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
void DtcGui::init_external_cfo_readout_mode() {

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;
  // int roc               = dtel->fActiveRocID;

  streambuf*    oldCoutStreamBuf = cout.rdbuf();
  ostringstream strCout;
  cout.rdbuf(strCout.rdbuf());

  TDatime x1; *fTextView << x1.AsSQLString() << Form(" %s: DTC ID: %i \n",__func__,fActiveDtcID);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (dtel->fData->fName == "DTC") {
    try         { 
      dtel->fDTC_i->InitExternalCFOReadoutMode();
    }
    catch (...) { *fTextView << Form("ERROR : filed  BAIL OUT") << std::endl; }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("%s DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf(oldCoutStreamBuf);
}

//-----------------------------------------------------------------------------
void DtcGui::read_dtc_register() {
  //  TString cmd;

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
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " 
             << "read_register DTC ID:" << fActiveDtcID << " text: " << dtel->fRegR->GetText() 
             << " register: " << reg << std::endl;
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (dtel->fData->fName == "DTC") {
    try {
      *fTextView << Form("%s: reading DTC %i register 0x%08x\n",__func__,fActiveDtcID,reg);
      int timeout_ms(150);
      uint32_t val;
      dtel->fDTC_i->Dtc()->GetDevice()->read_register(reg,timeout_ms,&val);
      dtel->fValR->SetText(Form("0x%08x",val));
    }
    catch (...) {
      *fTextView << Form("ERROR in %s: coudn't read DTC reg 0x%4x... BAIL OUT",__func__,reg) << std::endl;
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


//-----------------------------------------------------------------------------
// Tab here is the list of ROC tabs
//-----------------------------------------------------------------------------
void DtcGui::BuildRocTabElement(TGTab*& Tab, RocTabElement_t& RocTel, RocData_t* RocData) {

  const char* device_name = RocData->fName.Data();

  //  printf("[DtcGui::BuildTabElement] title: %s\n",title);

  RocTel.fFrame = Tab->AddTab(device_name);
  int ntabs         = Tab->GetNumberOfTabs();

  // TabElement.fFrame->SetLayoutManager(new TGVerticalLayout(TabElement.fFrame));

  RocTel.fTab   = Tab->GetTabTab(ntabs-1);
  RocTel.fColor = RocTel.fTab->GetBackground();

  TGGroupFrame* group = new TGGroupFrame(RocTel.fFrame,Form("%s",device_name));
  group->SetLayoutBroken(kTRUE);
//-----------------------------------------------------------------------------
// 1st column: commands : a) ROC status, b) write c) read
//-----------------------------------------------------------------------------
  TGTextButton* tb;

  int x0, y0, dx, dy;
  x0 = 10; y0 = 20; dx = 60; dy = 25; 

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);

  tb->MoveResize(x0,y0,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "print_roc_status()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// ROC read/write register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"write",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+dy+5,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "write_roc_register()");
  tb->ChangeBackground(fValidatedColor);

  tb = new TGTextButton(group,"read",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*2,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "read_roc_register()");
  tb->ChangeBackground(fValidatedColor);

  tb = new TGTextButton(group,"reset",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*3,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "reset_roc()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// ROC register
//-----------------------------------------------------------------------------
  int dx2 = dx;
  TGLabel* lab = new TGLabel(group,"register");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x0+dx+10, y0, dx2,dy);
//-----------------------------------------------------------------------------
// graphics context changes
//-----------------------------------------------------------------------------
  TGFont* ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-16-*-*-*-*-*-iso8859-1");

  GCValues_t valEntry791;
  valEntry791.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#000000",valEntry791.fForeground);
  gClient->GetColorByName("#e8e8e8",valEntry791.fBackground);
  valEntry791.fFillStyle         = kFillSolid;
  valEntry791.fFont              = ufont->GetFontHandle();
  valEntry791.fGraphicsExposures = kFALSE;
  TGGC* uGC  = gClient->GetGC(&valEntry791, kTRUE); // will reflect user GC changes
//-----------------------------------------------------------------------------
// write register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x0000");
  // wr->Resize(160,reg->GetDefaultHeight());
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->MoveResize(x0+dx+10,y0+dy+5,dx2,dy);
  RocTel.fRegW = wr;

  TGTextEntry*  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText("0x0000");
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->MoveResize(x0+dx+10,y0+(dy+5)*2,dx2,dy);
  RocTel.fRegR = rr;
//-----------------------------------------------------------------------------
// 3rd column: write value: 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"value");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x0+dx+10+dx2+10, y0        ,dx,dy);

  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  wval->SetMaxLength(4096);
  wval->SetAlignment(kTextLeft);
  wval->SetText("0x0000");
  group->AddFrame(wval, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wval->MoveResize(x0+dx+10+dx2+10,y0+(dy+5) ,dx,dy);
  RocTel.fValW = wval;
 
  lab = new TGLabel(group,"0x0000");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x0+dx+10+dx2+10,y0+2*(dy+5),dx,dy);
  RocTel.fValR = lab;
//-----------------------------------------------------------------------------
// finish composition of the ROC tab element
//-----------------------------------------------------------------------------
  RocTel.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(350,20,340,160);
}

//-----------------------------------------------------------------------------
void DtcGui::BuildDtcTabElement(TGTab*& Tab, DtcTabElement_t& DtcTel, DtcData_t* DtcData) {

  DtcTel.fData = DtcData;

  const char* device_name = DtcData->fName.Data();

  DtcTel.fFrame = Tab->AddTab(Form("%s:%i", DtcData->fName.Data(),DtcData->fPcieIndex));
  int ntabs     = Tab->GetNumberOfTabs();
  DtcTel.fFrame->SetLayoutManager(new TGVerticalLayout(DtcTel.fFrame));
  DtcTel.fTab   = Tab->GetTabTab(ntabs-1);
  DtcTel.fColor = DtcTel.fTab->GetBackground();

  TGGroupFrame* group = new TGGroupFrame(DtcTel.fFrame,Form("%s : %s",fHost.Data(),device_name));
  group->SetLayoutBroken(kTRUE);
//-----------------------------------------------------------------------------
// DTC's have ROCs, CFO's - do not
//-----------------------------------------------------------------------------
  int id = 0;

  if (DtcData->fName == "DTC") {
    DtcTel.fRocTab = new TGTab(group,10,10);
    group->AddFrame(DtcTel.fRocTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    for (int i=0; i<6; i++) {
      BuildRocTabElement(DtcTel.fRocTab,DtcTel.fRocTel[i], &fDtcData->fRocData[i]);
    }
//-----------------------------------------------------------------------------
// position the ROC tab inside the DTC tab
//-----------------------------------------------------------------------------
    DtcTel.fRocTab->MoveResize(520,20,360,190);
    DtcTel.fRocTab->Connect("Selected(Int_t)", "DtcGui", this, "DoRocTab(Int_t)");

    DtcTel.fRocTab->SetTab(id);

//-----------------------------------------------------------------------------
// set active ROC tab
//-----------------------------------------------------------------------------
    DtcTel.fActiveRocID  = id;

    DtcTel.fActiveRocTel = &DtcTel.fRocTel[id];
    DtcTel.fRocTabColor  = DtcTel.fActiveRocTel->fTab->GetBackground();
    
    DtcTel.fActiveRocTel->fTab->ChangeBackground(fYellow);
  }
//------------------------------------------------------------------------------
// a) graphics context changes
//-----------------------------------------------------------------------------
  TGFont *ufont;         // will reflect user font changes
  ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-16-*-*-*-*-*-iso8859-1");

  TGGC   *uGC;           // will reflect user GC changes

  GCValues_t valEntry791;
  valEntry791.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
  gClient->GetColorByName("#000000",valEntry791.fForeground);
  gClient->GetColorByName("#e8e8e8",valEntry791.fBackground);
  valEntry791.fFillStyle = kFillSolid;
  valEntry791.fFont = ufont->GetFontHandle();
  valEntry791.fGraphicsExposures = kFALSE;
  uGC  = gClient->GetGC(&valEntry791, kTRUE);
//-----------------------------------------------------------------------------
// status - different for DTC and CFO
//-----------------------------------------------------------------------------
  TGTextButton* tb;
  int x0, y0, dx, dy;

  x0 = 20; y0 = 30; dx = 70; dy = 30; 

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins    (0,0,0,0);
  tb->SetWrapLength (-1);
  
  tb->MoveResize(x0,y0,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "print_dtc_status()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// DTC write register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"write",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
 
  tb->MoveResize(x0,y0+dy+5,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "write_dtc_register()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 1 raw 3: DTC read register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"read",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*2,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "read_dtc_register()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 1 raw 4: DTC soft reset 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"soft reset",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*3,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "dtc_soft_reset()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 1 raw 5: DTC/CFO hard reset 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"hard reset",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*4,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "dtc_hard_reset()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 2 raw 1 : label "register"
//-----------------------------------------------------------------------------
  int dx2 = 80;
  TGLabel* lab = new TGLabel(group,"register");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x0+dx+10, y0, dx2,dy);
//-----------------------------------------------------------------------------
// column 2 raw 2 : write register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x9114");
  wr->MoveResize(x0+dx+10,y0+dy+5,dx2,dy);
  DtcTel.fRegW = wr;
//-----------------------------------------------------------------------------
// column 2 raw 3 : wread register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText("0x9100");
  rr->MoveResize(x0+dx+10,y0+2*(dy+5),dx2,dy);
  DtcTel.fRegR = rr;

  if (DtcData->fName == "DTC") {
//-----------------------------------------------------------------------------
// column 2 raw 4 : print DTC Firefly temp, different for CFO
//-----------------------------------------------------------------------------
    tb = new TGTextButton(group,"FF temp",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
    group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    tb->SetTextJustify(36);
    tb->SetMargins(0,0,0,0);
    tb->SetWrapLength(-1);
  
    tb->MoveResize(x0+dx+10,y0+(dy+5)*3,dx2,dy);
    tb->Connect("Pressed()", "DtcGui", this, "print_firefly_temp()");
    tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 2 raw 5 : init readout mode with the external CFO
//-----------------------------------------------------------------------------
    tb = new TGTextButton(group,"init ext CFO",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
    group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    tb->SetTextJustify(36);
    tb->SetMargins(0,0,0,0);
    tb->SetWrapLength(-1);
  
    tb->MoveResize(x0+dx+10,y0+(dy+5)*4,dx2,dy);
    tb->Connect("Pressed()", "DtcGui", this, "init_external_cfo_readout_mode()");
    tb->ChangeBackground(fValidatedColor);
  }
  else if (DtcData->fName == "CFO") {
//-----------------------------------------------------------------------------
// column 2 raw 4 : launch current run plan
//-----------------------------------------------------------------------------
    tb = new TGTextButton(group,"launch",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
    group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    tb->SetTextJustify(36);
    tb->SetMargins(0,0,0,0);
    tb->SetWrapLength(-1);
  
    tb->MoveResize(x0+dx+10,y0+(dy+5)*3,dx2,dy);
    tb->Connect("Pressed()", "DtcGui", this, "cfo_launch_run_plan()");
    tb->ChangeBackground(fValidatedColor);
  }
//-----------------------------------------------------------------------------
// 3rd column: write value: 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  int dx3 = 100;
  lab = new TGLabel(group,"value");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x0+dx+10+dx2+10, y0        ,dx3,dy);

  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wval, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wval->SetMaxLength(4096);
  wval->SetAlignment(kTextLeft);
  wval->SetText("0x00000000");
  wval->MoveResize(x0+dx+10+dx2+10,y0+(dy+5) ,dx3,dy);
  DtcTel.fValW = wval;
 
  lab = new TGLabel(group,"0x00000000");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x0+dx+10+dx2+10,y0+2*(dy+5),dx3,dy);
  DtcTel.fValR = lab;
//-----------------------------------------------------------------------------
// resize the DTC group panel
//-----------------------------------------------------------------------------
  DtcTel.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(10,10,910,250);
}


//-----------------------------------------------------------------------------
void DtcGui::BuildGui(const TGWindow *Parent, UInt_t Width, UInt_t Height) {

  gClient->GetColorByName("#ccffcc",fValidatedColor);  // light green
//-----------------------------------------------------------------------------
// main frame
//-----------------------------------------------------------------------------
  fMainFrame = new TGMainFrame(gClient->GetRoot(),Width,Height,kMainFrame | kVerticalFrame);
  fMainFrame->SetLayoutBroken(kTRUE);
  fMainFrame->SetWindowName(Form("%s",fHost.Data()));
  fMainFrame->SetName("MainFrame");

  gClient->GetColorByName("yellow", fYellow);
  gClient->GetColorByName("green", fGreen);
//-----------------------------------------------------------------------------
// add tab holder and multiple tabs (tab elements) for two DTCs or a DTC and a CFO) 
//-----------------------------------------------------------------------------
  fDtcTab = new TGTab(fMainFrame,10,10);
  fMainFrame->AddFrame(fDtcTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  for (int i=0; i<fNDtcs; i++) {
    BuildDtcTabElement(fDtcTab,fDtcTel[i],&fDtcData[i]);
  }

  int y0 = 310;
  fDtcTab->MoveResize(10,10,930,290);  // this defines the size of the tab below the tabs line
  fDtcTab->Connect("Selected(Int_t)", "DtcGui", this, "DoDtcTab(Int_t)");
//-----------------------------------------------------------------------------
// common buttons on fMainFrame, they are the same for different DTCs and ROCs
//-----------------------------------------------------------------------------
  TGTextButton* tb;

  int button_dx = 150;
  int button_sx = 150+10;
  int button_sy =  30;

  tb = new TGTextButton(fMainFrame,"clear",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  tb->MoveResize(10,y0,button_dx,25);
  fMainFrame->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "clear_output()");

  tb = new TGTextButton(fMainFrame,"exit",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  tb->MoveResize(10+button_sx,y0,button_dx,25);
  fMainFrame->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "exit()");
  //-----------------------------------------------------------------------------
  // set active DTC tab
  //-----------------------------------------------------------------------------
  fActiveDtcID  = 0;
  fActiveDtc = &fDtcData[0];
  fDtcTab->SetTab(fActiveDtcID);

  fActiveDtcTab = fDtcTel[fActiveDtcID].fTab; // fDtcTab->GetTabTab(0);
  fDtcTabColor  = fActiveDtcTab->GetBackground();

  fActiveDtcTab->ChangeBackground(fYellow);
//-----------------------------------------------------------------------------
// TextView
//-----------------------------------------------------------------------------
  fTextView = new TGTextViewostream(fMainFrame,10,10);
  fTextView->MoveResize(10,y0+button_sy+5,930,620);
  fMainFrame->AddFrame(fTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
//-----------------------------------------------------------------------------
// concluding operations
//-----------------------------------------------------------------------------
  fMainFrame->SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
  fMainFrame->MapSubwindows();
  fMainFrame->Resize(fMainFrame->GetDefaultSize());

  fMainFrame->MapRaised();
}

