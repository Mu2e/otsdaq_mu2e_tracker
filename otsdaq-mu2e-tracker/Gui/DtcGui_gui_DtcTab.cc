
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcGui_DtcTab"

using namespace trkdaq;
using namespace std;

//-----------------------------------------------------------------------------
void DtcGui::BuildDtcTabElement(TGTab*& Tab, DtcTabElement_t& DtcTel, DtcData_t* DtcData) {

  DtcTel.fData = DtcData;
  
  DtcTel.fDTC_i = DtcInterface::Instance(DtcData->fPcieAddr,DtcData->fLinkMask);
  DtcTel.fDTC_i->SetRocReadoutMode(DtcData->fReadoutMode);

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

  DtcTel.fRocTab = new TGTab(group,10,10);
  group->AddFrame(DtcTel.fRocTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  for (int i=0; i<6; i++) {
    BuildRocTabElement(DtcTel.fRocTab,DtcTel.fRocTel[i], &fDtcData->fRocData[i]);
  }
  TLOG(TLVL_DEBUG) << Form("checkpoint one\n");
//-----------------------------------------------------------------------------
// position the ROC tab inside the DTC tab
//-----------------------------------------------------------------------------
  DtcTel.fRocTab->MoveResize(520,10,360,190);
  DtcTel.fRocTab->Connect("Selected(Int_t)", "DtcGui", this, "DoRocTab(Int_t)");

  DtcTel.fRocTab->SetTab(id);
//-----------------------------------------------------------------------------
// set active ROC tab
//-----------------------------------------------------------------------------
  DtcTel.fActiveRocID  = id;

  DtcTel.fActiveRocTel = &DtcTel.fRocTel[id];
  DtcTel.fRocTabColor  = DtcTel.fActiveRocTel->fTab->GetBackground();
    
  DtcTel.fActiveRocTel->fTab->ChangeBackground(fYellow);
//------------------------------------------------------------------------------
// a) graphics context changes
//-----------------------------------------------------------------------------
//  TString* cmd;
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
// (column 1 row 1) :  status - different for DTC and CFO
//-----------------------------------------------------------------------------
  TGTextButton* tb;
  int x1(10), y0(20), dx1(70), dy(25);

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins    (0,0,0,0);
  tb->SetWrapLength (-1);
  
  tb->MoveResize(x1,y0,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "print_dtc_status()");
  tb->ChangeBackground(fValidatedColor);
  tb->SetUserData(nullptr);
//-----------------------------------------------------------------------------
// (column 1 row 2) : button : DTC write register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"writeR",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
 
  tb->MoveResize(x1,y0+dy+5,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "write_dtc_register()");
  tb->ChangeBackground(fValidatedColor);
  tb->SetUserData(nullptr);
//-----------------------------------------------------------------------------
// column 1 raw 3: DTC read register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"readR",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x1,y0+(dy+5)*2,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "read_dtc_register()");
  tb->ChangeBackground(fValidatedColor);
  tb->SetUserData(nullptr);
//-----------------------------------------------------------------------------
// column 1 raw 4: DTC soft reset 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"soft reset",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x1,y0+(dy+5)*3,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
  tb->ChangeBackground(fValidatedColor);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
  tb->SetUserData((void*) &DtcGui::dtc_soft_reset);
#pragma GCC diagnostic pop
//-----------------------------------------------------------------------------
// column 1 raw 5: DTC hard reset 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"hard reset",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x1,y0+(dy+5)*4,dx1,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
  tb->ChangeBackground(fValidatedColor);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
  tb->SetUserData((void*) &DtcGui::dtc_hard_reset);
#pragma GCC diagnostic pop
//-----------------------------------------------------------------------------
// column 2 raw 1 : label "register"
//-----------------------------------------------------------------------------
  int x2  = x1+dx1+10;
  int dx2 = 80;
  TGLabel* lab = new TGLabel(group,"register");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x2, y0, dx2,dy);
//-----------------------------------------------------------------------------
// column 2 raw 2 : write register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                     ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x9114");
  wr->MoveResize(x2,y0+dy+5,dx2,dy);
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
  rr->MoveResize(x2,y0+2*(dy+5),dx2,dy);
  DtcTel.fRegR = rr;
//-----------------------------------------------------------------------------
// column 2 raw 4 : print DTC Firefly temp, different for CFO
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"FF temp",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x2,y0+(dy+5)*3,dx2,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
  tb->ChangeBackground(fValidatedColor);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
  tb->SetUserData((void*) &DtcGui::dtc_print_firefly_temp);
#pragma GCC diagnostic pop
//-----------------------------------------------------------------------------
// column 2 raw 5 : init readout mode with the external CFO
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"init ext CFO",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x2,y0+(dy+5)*4,dx2,dy);
  tb->Connect("Pressed()", "DtcGui", this, "init_external_cfo_readout_mode()");
  tb->ChangeBackground(fValidatedColor);
  tb->SetUserData(nullptr);
//-----------------------------------------------------------------------------
// column 2 raw 6 : ROC patterns - OBSOLETE
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"ROC status",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x2,y0+(dy+5)*4,dx2,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
  tb->SetUserData((void*) &DtcGui::dtc_print_all_rocs);
#pragma GCC diagnostic pop
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 3 row 1: write value: 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  int x3  = x2+dx2+10;
  int dx3 = 100;

  lab = new TGLabel(group,"value");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x3,y0       ,dx3,dy);
//-----------------------------------------------------------------------------
// column 3 row 2: 2: entry field
//-----------------------------------------------------------------------------
  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                                       ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wval, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wval->SetMaxLength(4096);
  wval->SetAlignment(kTextLeft);
  wval->SetText("0x00000000");
  wval->MoveResize(x3,y0+1*(dy+5),dx3,dy);
  DtcTel.fValW = wval;
//-----------------------------------------------------------------------------
// column 3 row 3: label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"0x00000000");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x3,y0+2*(dy+5),dx3,dy);
  DtcTel.fValR = lab;
//-----------------------------------------------------------------------------
// column 3 row 4: "Start (start Read Thread) RT" - manage_reader_thread
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"Start Reader",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x3,y0+(dy+5)*3,dx3,dy);
  tb->Connect("Pressed()", "DtcGui", this, "manage_reader_thread()");
  tb->ChangeBackground(fValidatedColor);
  tb->SetUserData(nullptr);
//-----------------------------------------------------------------------------
// column 3 row 5: Start CT - manage_emu_cfo_thread
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"Start emuCFO",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x3,y0+(dy+5)*4,dx3,dy);
  tb->Connect("Pressed()", "DtcGui", this, "manage_emu_cfo_thread()");
  tb->SetUserData(nullptr);
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 4 row 1:  initReadout
//-----------------------------------------------------------------------------
  int x4  = x3+dx3+10;
  int dx4 = 100;
  tb = new TGTextButton(group,"InitReadout",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x4,y0,dx4,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
  tb->SetUserData((void*) &DtcGui::dtc_init_readout);
#pragma GCC diagnostic pop
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 4 row 2: EmulateCFO label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"Emulate CFO");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x4,y0+(dy+5),dx4,dy);
  //  DtcTel.fValR = lab;
//-----------------------------------------------------------------------------
// column 4 row 3: ROCReadoutMode label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"ROC r/o mode");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x4,y0+2*(dy+5),dx4,dy);
//-----------------------------------------------------------------------------
// column 4 row 4: Clock mode label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"JA mode");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x4,y0+3*(dy+5),dx4,dy);
//-----------------------------------------------------------------------------
// column 4 row 5: launch run plan with the emulated CFO, just once
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"Launch Run",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x4,y0+4*(dy+5),dx4,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
  tb->SetUserData((void*) &DtcGui::dtc_launch_run_plan_emulated_cfo);
#pragma GCC diagnostic pop
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// column 5 row 2: EmulateCFO entry field 
//-----------------------------------------------------------------------------
  int x5  = x4+dx4+10;
  int dx5 = 80;

  TGNumberEntry* ne;
  ne = new TGNumberEntry(group, 0, 0,10,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEANonNegative,
                         TGNumberFormat::kNELLimitMinMax,
                         0, 1);
  group->AddFrame(ne, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  // ne->SetTextJustify(36);
  // ne->SetMargins(0,0,0,0);
  // ne->SetWrapLength(-1);
  
  ne->MoveResize(x5,y0+1*(dy+5),dx5,dy);

  ne->Connect("ValueSet(Long_t)", "DtcGui", this, "set_emulate_cfo()");
  (ne->GetNumberEntry())->Connect("ReturnPressed()","DtcGui", this,"set_emulate_cfo()");

  DtcTel.fEmulateCfo = ne;
//-----------------------------------------------------------------------------
// column 5 row 3: RocReadoutMode entry field 
//-----------------------------------------------------------------------------
  ne = new TGNumberEntry(group, 0, 0,10,
                         TGNumberFormat::kNESInteger,
                         TGNumberFormat::kNEANonNegative,
                         TGNumberFormat::kNELLimitMinMax,
                         0, 1);
  group->AddFrame(ne, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  
  ne->MoveResize(x5,y0+2*(dy+5),dx5,dy);

  ne->Connect("ValueSet(Long_t)", "DtcGui", this, "set_roc_readout_mode()");
  (ne->GetNumberEntry())->Connect("ReturnPressed()","DtcGui", this,"set_roc_readout_mode()");

  DtcTel.fRocReadoutMode = ne;
//-----------------------------------------------------------------------------
// column 5 row 4: JA Mode entry field 
//-----------------------------------------------------------------------------
  TLOG(TLVL_DEBUG) << Form("DONE\n");

  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),
                       ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText(Form("0x%02x",DtcTel.fDTC_i->fJAMode));
  rr->MoveResize(x5,y0+3*(dy+5),dx5,dy);
  DtcTel.fJAMode = rr;

  rr->Connect("ReturnPressed()","DtcGui", this,"dtc_set_ja_mode()");
//-----------------------------------------------------------------------------
// column 5 row 5: ReadSubEvents .. just once, whatever has been stored
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"ReadSubEvts",-1,TGTextButton::GetDefaultGC()(),
                          TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x5,y0+4*(dy+5),dx5,dy);
  tb->Connect("Pressed()", "DtcGui", this, "execute_command()");
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpmf-conversions"
  tb->SetUserData((void*) &DtcGui::dtc_read_subevents);
#pragma GCC diagnostic pop
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// resize the DTC group panel
//-----------------------------------------------------------------------------
  DtcTel.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(10,10,900,230);

  TLOG(TLVL_DEBUG) << Form("DONE\n");
}
