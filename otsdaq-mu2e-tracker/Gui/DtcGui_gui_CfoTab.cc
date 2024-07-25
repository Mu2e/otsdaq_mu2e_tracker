
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;
using namespace std;

//-----------------------------------------------------------------------------
void DtcGui::BuildCfoTabElement(TGTab*& Tab, DtcTabElement_t& DtcTel, DtcData_t* DtcData) {

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
//  int id = 0;
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
// write register 
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
// column 1 raw 3: read register 
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
// column 1 raw 4: soft reset 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"soft reset",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*3,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "cfo_soft_reset()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 1 raw 5: hard reset 
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
//-----------------------------------------------------------------------------
// column 2 raw 5 : enable beam off
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"EnableBmOff",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0+dx+10,y0+(dy+5)*4,dx2,dy);
  tb->Connect("Pressed()", "DtcGui", this, "cfo_enable_beam_off()");
  //  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 2 raw 6 : disable beam off
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"DisableBmOff",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0+dx+10,y0+(dy+5)*5,dx2,dy);
  tb->Connect("Pressed()", "DtcGui", this, "cfo_disable_beam_off()");
  //  tb->ChangeBackground(fValidatedColor);
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
// column 3 row 3: label (value of the read register)
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"0x00000000");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x0+dx+10+dx2+10,y0+2*(dy+5),dx3,dy);
  DtcTel.fValR = lab;
//-----------------------------------------------------------------------------
// column 3 row 4: manage_cfo_thread
//-----------------------------------------------------------------------------
  // tb = new TGTextButton(group,"Emultd CT",-1,TGTextButton::GetDefaultGC()(),
  //                         TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  // group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  // tb->SetTextJustify(36);
  // tb->SetMargins(0,0,0,0);
  // tb->SetWrapLength(-1);
  
  // tb->MoveResize(x0+dx+10+dx2+10,y0+(dy+5)*3,dx3,dy);
  // tb->Connect("Pressed()", "DtcGui", this, "manage_emu_cfo_thread()");
  // tb->ChangeBackground(fValidatedColor);  // not yet
//-----------------------------------------------------------------------------
// column 3 row 5: manage_external_cfo_thread
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"Extrnl CT",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0+dx+10+dx2+10,y0+(dy+5)*4,dx3,dy);
  tb->Connect("Pressed()", "DtcGui", this, "manage_ext_cfo_thread()");
  // tb->ChangeBackground(fValidatedColor);  // not yet
//-----------------------------------------------------------------------------
// column 4 row 1: label: CFO timing chain link 
//-----------------------------------------------------------------------------
  int c4_dx = x0+dx+10+dx2+10+dx3+10;

  // lab = new TGLabel(group,"CFO link");
  // group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  // lab->SetTextJustify(36);
  // lab->SetMargins(0,0,0,0);
  // lab->SetWrapLength(-1);
  // lab->MoveResize(c4_dx,y0,dx3,dy);
  //  DtcTel.fCfoLink = lab;
//-----------------------------------------------------------------------------
// column 4 row 2: label: N(DTCs) in the timing chain 
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"DTC mask)");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(c4_dx,y0+(dy+5),dx3,dy);
//-----------------------------------------------------------------------------
// column 4 row 3: label: run plan
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"Run Plan");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(c4_dx,y0+2*(dy+5),dx3,dy);
//-----------------------------------------------------------------------------
// column 5 row 1: input field: CFO timing chain link 
//-----------------------------------------------------------------------------
  int c5_dx = c4_dx+dx3+10;
  int dx5   = 150;
//-----------------------------------------------------------------------------
// column 5 row 2: input field: DTC mask
//-----------------------------------------------------------------------------
  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                       ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText("0x00000001");
  rr->MoveResize(c5_dx,y0+(dy+5),dx5,dy);
  DtcTel.fDtcMask = rr;
//-----------------------------------------------------------------------------
// column 5 row 3: input field: N(DTCs)
//-----------------------------------------------------------------------------
  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                       ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText("run_00001_hz.bin");
  rr->MoveResize(c5_dx,y0+2*(dy+5),dx5,dy);
  DtcTel.fRunPlan = rr;
//-----------------------------------------------------------------------------
// column (4+5) raw 4 : "Init Run Plan" for given CFO time link and N(DTCs
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"init readout",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(c4_dx,y0+3*(dy+5),dx3+10+dx5,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
  TString* cmd = new TString("cfo_init_readout");
  tb->SetUserData(cmd);
  // tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// resize the DTC group panel
//-----------------------------------------------------------------------------
  DtcTel.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(10,10,910,250);
}
