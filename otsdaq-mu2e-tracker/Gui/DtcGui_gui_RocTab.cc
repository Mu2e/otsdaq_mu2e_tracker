//////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;
using namespace std;
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
// column 1 row 1: ROC status, b) write c) read
//-----------------------------------------------------------------------------
  TGTextButton* tb;

  int x1(10), y0(20), dx1(60), dy(25);

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);

  tb->MoveResize(x1,y0,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "print_roc_status()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 1 row 2: ROC write register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"write",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x1,y0+(dy+5)*1,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "write_roc_register()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 1 row 3: ROC read register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"read",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x1,y0+(dy+5)*2,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "read_roc_register()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 2 row 1: label "register"
//-----------------------------------------------------------------------------
  int x2  = x1+dx1+5;
  int dx2 = dx1;
  TGLabel* lab = new TGLabel(group,"register");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x2, y0, dx2,dy);
//-----------------------------------------------------------------------------
//  (graphics context changes)
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
// column 2 row 2 : text entry for write register number 
//-----------------------------------------------------------------------------
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                     ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x0000");
  // wr->Resize(160,reg->GetDefaultHeight());
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->MoveResize(x2,y0+dy+5,dx2,dy);
  RocTel.fRegW = wr;
//-----------------------------------------------------------------------------
// column 2 row 3 : text entry for read  register number 
//-----------------------------------------------------------------------------
  TGTextEntry*  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                     ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText("0x0000");
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->MoveResize(x2,y0+(dy+5)*2,dx2,dy);
  RocTel.fRegR = rr;
//-----------------------------------------------------------------------------
// column 3 row 1 : label "value" 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  int x3  = x2+dx2+5;
  int dx3 = dx2;
  
  lab = new TGLabel(group,"value");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x3, y0        ,dx3,dy);
//-----------------------------------------------------------------------------
// column 3 row 2 : label "value" 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                       ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  wval->SetMaxLength(4096);
  wval->SetAlignment(kTextLeft);
  wval->SetText("0x0000");
  group->AddFrame(wval, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wval->MoveResize(x3,y0+(dy+5) ,dx3,dy);
  RocTel.fValW = wval;
//-----------------------------------------------------------------------------
// column 3 row 3 : label 0 value of the read register
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"0x0000");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x3,y0+2*(dy+5),dx3,dy);
  RocTel.fValR = lab;
//-----------------------------------------------------------------------------
// column 4 row 1: reset ROC 
//-----------------------------------------------------------------------------
  int x4  = x3+dx3+5;
  int dx4 = dx3;
  
  tb = new TGTextButton(group,"reset",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x4,y0+(dy+5)*0,dx4,dy);
  tb->Connect("Pressed()", "DtcGui", this, "reset_roc()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// finish composition of the ROC tab element
//-----------------------------------------------------------------------------
  RocTel.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(350,20,340,160);
}
