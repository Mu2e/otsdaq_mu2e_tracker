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
// 1st column: commands : a) ROC status, b) write c) read
//-----------------------------------------------------------------------------
  TGTextButton* tb;

  int x0, y0, dx, dy;
  x0 = 10; y0 = 20; dx = 60; dy = 25; 

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
  tb = new TGTextButton(group,"write",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+dy+5,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "write_roc_register()");
  tb->ChangeBackground(fValidatedColor);

  tb = new TGTextButton(group,"read",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*2,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "read_roc_register()");
  tb->ChangeBackground(fValidatedColor);

  tb = new TGTextButton(group,"reset",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
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
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                     ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x0000");
  // wr->Resize(160,reg->GetDefaultHeight());
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->MoveResize(x0+dx+10,y0+dy+5,dx2,dy);
  RocTel.fRegW = wr;

  TGTextEntry*  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                     ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
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

  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                       ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
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
