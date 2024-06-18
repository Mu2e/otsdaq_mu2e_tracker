
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;
using namespace std;

//-----------------------------------------------------------------------------
void DtcGui::BuildDtcTabElement(TGTab*& Tab, DtcTabElement_t& DtcTel, DtcData_t* DtcData) {

  DtcTel.fData = DtcData;

  const char* device_name = DtcData->fName.Data();

  DtcTel.fFrame = Tab->AddTab(Form("%s:%i", DtcData->fName.Data(),DtcData->fPcieAddr));
  int ntabs     = Tab->GetNumberOfTabs();
  DtcTel.fFrame->SetLayoutManager(new TGVerticalLayout(DtcTel.fFrame));
  DtcTel.fTab   = Tab->GetTabTab(ntabs-1);
  DtcTel.fColor = DtcTel.fTab->GetBackground();

  TGGroupFrame* group = new TGGroupFrame(DtcTel.fFrame,Form("%s : %s",fHostname.Data(),device_name));
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

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
  tb = new TGTextButton(group,"writeR",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
  tb = new TGTextButton(group,"readR",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
  tb = new TGTextButton(group,"soft reset",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
  tb = new TGTextButton(group,"hard reset",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                     ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x9114");
  wr->MoveResize(x0+dx+10,y0+dy+5,dx2,dy);
  DtcTel.fRegW = wr;
//-----------------------------------------------------------------------------
// column 2 raw 3 : wread register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                     ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
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
    tb = new TGTextButton(group,"FF temp",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
    tb = new TGTextButton(group,"init ext CFO",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
    tb = new TGTextButton(group,"launch",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
    group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    tb->SetTextJustify(36);
    tb->SetMargins(0,0,0,0);
    tb->SetWrapLength(-1);
  
    tb->MoveResize(x0+dx+10,y0+(dy+5)*3,dx2,dy);
    tb->Connect("Pressed()", "DtcGui", this, "cfo_launch_run_plan()");
    tb->ChangeBackground(fValidatedColor);
  }
//-----------------------------------------------------------------------------
// column 3 row 1: write value: 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  int dx3 = 100;

  lab = new TGLabel(group,"value");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x0+dx+10+dx2+10, y0        ,dx3,dy);
//-----------------------------------------------------------------------------
// column 3 row 2: 2: entry field
//-----------------------------------------------------------------------------
  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                       ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wval, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wval->SetMaxLength(4096);
  wval->SetAlignment(kTextLeft);
  wval->SetText("0x00000000");
  wval->MoveResize(x0+dx+10+dx2+10,y0+(dy+5) ,dx3,dy);
  DtcTel.fValW = wval;
//-----------------------------------------------------------------------------
// column 3 row 3: label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"0x00000000");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x0+dx+10+dx2+10,y0+2*(dy+5),dx3,dy);
  DtcTel.fValR = lab;

  if (DtcData->fName == "DTC") {
    tb = new TGTextButton(group,"read D",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
    group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

    tb->SetTextJustify(36);
    tb->SetMargins(0,0,0,0);
    tb->SetWrapLength(-1);
  
    tb->MoveResize(x0+dx+10+dx2+10,y0+(dy+5)*3,dx3,dy);
    tb->Connect("Pressed()", "DtcGui", this, "read_subevents()");
    // tb->ChangeBackground(fValidatedColor);  // not yet
  }
//-----------------------------------------------------------------------------
// resize the DTC group panel
//-----------------------------------------------------------------------------
  DtcTel.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(10,10,910,250);
}
