
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
// column 1 raw 5: DTC hard reset 
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

