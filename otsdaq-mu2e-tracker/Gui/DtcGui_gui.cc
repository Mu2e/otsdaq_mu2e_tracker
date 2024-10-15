
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

#include "TROOT.h"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcGui_gui"

using namespace trkdaq;
using namespace std;
//-----------------------------------------------------------------------------
// initialization with the project data - in its rudimentary form
//-----------------------------------------------------------------------------
DtcGui::DtcGui(const char* Project, const TGWindow *p, UInt_t w, UInt_t h, int DebugLevel) {

  printf(" --------------------------------- EMOE 001\n");
  //
  char buf[100];
  FILE* pip = gSystem->OpenPipe("hostname | awk -F . '{print $1}'","r");
  fgets(buf,100,pip); // expect just one line
  gSystem->ClosePipe(pip);

  if (fDebugLevel > 0) printf("DtcGui::%s: .buf. =.%s.\n",__func__,buf);
  printf(" --------------------------------- EMOE 002\n");

  fHostname       = buf;
  fHostname       = fHostname.Strip(TString::kTrailing,'\n');
  fDebugLevel     = DebugLevel;
  fSubmittedColor = 0xFF3399; // 16724889;
  fRunningColor   = 0xFF3399; // 16724889;
  fStoppedColor   = 0xcccccc; // perhaps , gray

  if (fDebugLevel > 0) printf("DtcGui::%s: host:%s project:%s\n",__func__,fHostname.Data(),Project);

  InitRunConfiguration(Project);

  if (fDebugLevel > 0) printf("DtcGui::%s: before BuildGui\n",__func__);
  BuildGui(p,w,h);
  if (fDebugLevel > 0) printf("DtcGui::%s: after  BuildGui\n",__func__);

// //-----------------------------------------------------------------------------
// // two PCIE cards
// //-----------------------------------------------------------------------------
//   for (int i=0; i<2; i++) {
//     DtcTabElement_t* dtel = &fDtcTel[i];
//     int pcie_addr = fDtcData[i].fPcieAddr;
//     if      (fDtcData[i].IsDtc()) {
//       dtel->fDTC_i = DtcInterface::Instance(pcie_addr,fDtcData[i].fLinkMask);
//       dtel->fDTC_i->SetRocReadoutMode(fDtcData[i].fReadoutMode);
//     }
//     else if (fDtcData[i].IsCfo()) {
//       dtel->fCFO_i = CfoInterface::Instance(pcie_addr,fDtcData[i].fLinkMask);
// //-----------------------------------------------------------------------------
// // there is only one CFO, so initialize the CFO pointer - by copying it from the tab
// //-----------------------------------------------------------------------------
//       fCFO_i = dtel->fCFO_i;
//     }
//   }

  fEmuCfoTC.fTp          = nullptr;
  fEmuCfoTC.fStop        = 0;
  fEmuCfoTC.fPause       = 0;
  fEmuCfoTC.fCmd         = 0;
  fEmuCfoTC.fRunning     = 0;
  fEmuCfoTC.fPrintLevel  = 1;
  fEmuCfoTC.fSleepTimeMs = 2000;

  fExtCfoTC.fTp          = nullptr;
  fExtCfoTC.fStop        = 0;
  fExtCfoTC.fPause       = 0;
  fExtCfoTC.fCmd         = 0;
  fExtCfoTC.fRunning     = 0;
  fExtCfoTC.fPrintLevel  = 1;
  fExtCfoTC.fSleepTimeMs = 2000;

  fReaderTC.fTp          = nullptr;
  fReaderTC.fStop        = 0;
  fReaderTC.fPause       = 0;
  fReaderTC.fCmd         = 0;
  fReaderTC.fRunning     = 0;
  fReaderTC.fPrintLevel  = 2;
  fReaderTC.fSleepTimeMs = 2000;

  fValidate              = 0;
  fCfoPrintFreq          = 1;

  if (fDebugLevel > 0) printf("DtcGui::%s : all done, EXIT\n", __func__);
}

//-----------------------------------------------------------------------------
DtcGui::~DtcGui() {
  fMainFrame->Cleanup();

  if (fEmuCfoTC.fTp) fEmuCfoTC.fTp->Join();
  if (fExtCfoTC.fTp) fExtCfoTC.fTp->Join();
  if (fReaderTC.fTp) fReaderTC.fTp->Join();
}

//-----------------------------------------------------------------------------
// 1) first check for project name like "pasha/mu2edaq09_pcie0"
// if file config/pasha/mu2edaq09_pcie0.C exists , use that
// 2) otherwise assume config file name config/$project/$hostname.C
// config file should contain function init_run_configuration(DtcGui*)
//-----------------------------------------------------------------------------
int DtcGui::InitRunConfiguration(const char* Config) {
  int           rc(0);
  TInterpreter* cint = gROOT->GetInterpreter();

  TInterpreter::EErrorCode irc;

  TString macro = Form("%s/otsdaq-mu2e-tracker/config/%s.C",gSystem->Getenv("SPACK_ENV"),Config);
  FILE* f = fopen(macro,"r");
  if (f == nullptr) {
    macro = Form("%s/otsdaq-mu2e-tracker/config/%s/%s.C",gSystem->Getenv("SPACK_ENV"),Config,fHostname.Data());
    f     = fopen(macro,"r");
    if (f == nullptr) {
      TLOG(TLVL_ERROR) << "failed to find config file for " << Config << " , EXIT" << std::endl;
      rc = -1;
    }
  }

  if (rc != 0) return rc;

  TLOG (TLVL_INFO) << Form(" loading configuration from file=%s\n",macro.Data());
  
  cint->LoadMacro(macro.Data(), &irc);

  rc = irc;
  if (rc != 0) return rc;
  
  TString cmd = Form("init_run_configuration((DtcGui*) 0x%0lx)",(long int) this);
  
  TLOG(TLVL_INFO) << Form(" cmd=%s\n",cmd.Data());
    
  gInterpreter->ProcessLine(cmd.Data(),&irc);

  return irc;
}

//-----------------------------------------------------------------------------
void DtcGui::BuildGui(const TGWindow *Parent, UInt_t Width, UInt_t Height) {

  gClient->GetColorByName("#ccffcc",fValidatedColor);  // light green
//-----------------------------------------------------------------------------
// main frame
//-----------------------------------------------------------------------------
  fMainFrame = new TGMainFrame(gClient->GetRoot(),Width,Height,kMainFrame | kVerticalFrame); // ,kMainFrame | kVerticalFrame);
  // fMainFrame->SetLayoutBroken(kTRUE);
  fMainFrame->SetWindowName(Form("%s",fHostname.Data()));
  fMainFrame->SetName("MainFrame");

  gClient->GetColorByName("yellow", fYellow);
  gClient->GetColorByName("green" , fGreen);
//-----------------------------------------------------------------------------
// add tab holder and multiple tabs (tab elements) for two DTCs or a DTC and a CFO) 
//-----------------------------------------------------------------------------
  fDtcTab = new TGTab(fMainFrame,10,10);
  fMainFrame->AddFrame(fDtcTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  for (int i=0; i<fNDtcs; i++) {
    if      (fDtcData[i].IsDtc()) BuildDtcTabElement(fDtcTab,fDtcTel[i],&fDtcData[i]);
    else if (fDtcData[i].IsCfo()) BuildCfoTabElement(fDtcTab,fDtcTel[i],&fDtcData[i]);
  }

  fDtcTab->MoveResize(10,10,920,260);  // this defines the size of the tab below the tabs line
  fDtcTab->Connect("Selected(Int_t)", "DtcGui", this, "DoDtcTab(Int_t)");
//-----------------------------------------------------------------------------
// common buttons on fMainFrame, they are the same for different DTCs and ROCs
//-----------------------------------------------------------------------------
  fButtonsFrame = new TGHorizontalFrame(fMainFrame);
  fMainFrame->AddFrame(fButtonsFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));

  TGTextButton* tb;

  int y0        = 10;
  int button_dx = 50;
  int button_dy = 25;
  int button_sx = 150+10;          // includes 10 pixes between the buttons
  //  int button_sy =  30;
//-----------------------------------------------------------------------------
// 1. clear
//-----------------------------------------------------------------------------
  tb = new TGTextButton(fButtonsFrame,"clear",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  tb->MoveResize(10,y0,button_dx,button_dy);
  fButtonsFrame->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "clear_output()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// 2. exit
//-----------------------------------------------------------------------------
  tb = new TGTextButton(fButtonsFrame,"exit",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  tb->MoveResize(10+button_sx,y0,button_dx,button_dy);
  fButtonsFrame->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "exit()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// 3. ... 
//-----------------------------------------------------------------------------
  int x3offset = 10+button_sx*2;
//   tb = new TGTextButton(fButtonsFrame,"launch",-1,TGTextButton::GetDefaultGC()(),
//                         TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
//   tb->SetTextJustify(36);
//   tb->SetMargins(0,0,0,0);
//   tb->SetWrapLength(-1);
//   tb->MoveResize(x3offset,y0,button_dx,button_dy);
//   fButtonsFrame->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
//   tb->Connect("Pressed()", "DtcGui", this, "cfo_launch_run_plan()");
//   tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// 4 "NEvents" label followed by the entry field
//-----------------------------------------------------------------------------
  int x4offset = x3offset+button_sx;
  int dx4 = 80;

  TGLabel* lab = new TGLabel(fButtonsFrame,"NEvents");
  fButtonsFrame->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x4offset,y0,dx4,button_dy);

  fNEvents = new TGNumberEntry(fButtonsFrame, 66, 9,999,
                               TGNumberFormat::kNESInteger,
                               TGNumberFormat::kNEANonNegative,
                               TGNumberFormat::kNELLimitMinMax,
                               0, 100000000);

  fNEvents->Connect("ValueSet(Long_t)", "DtcGui", this, "set_nevents()");
  (fNEvents->GetNumberEntry())->Connect("ReturnPressed()","DtcGui", this,"set_nevents()");

  fButtonsFrame->AddFrame(fNEvents, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  fNEvents->MoveResize(x4offset+dx4+10,y0,dx4,button_dy);
//-----------------------------------------------------------------------------
// 5  "EW length" label followed by the entry field
//-----------------------------------------------------------------------------
  int x5offset = x4offset+dx4+10;
  int dx5 = 80;
  lab = new TGLabel(fButtonsFrame,"EW length");
  fButtonsFrame->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x5offset,y0,dx5,button_dy);
  
  fEWLength = new TGNumberEntry(fButtonsFrame, 68, 9,999,
                                TGNumberFormat::kNESInteger,
                                TGNumberFormat::kNEANonNegative,
                                TGNumberFormat::kNELLimitMinMax,
                                0, 100000);
  fEWLength->Connect("ValueSet(Long_t)", "MyMainFrame", this, "set_ew_length()");
  (fEWLength->GetNumberEntry())->Connect("ReturnPressed()","MyMainFrame", this,"set_ew_length()");

  fButtonsFrame->AddFrame(fEWLength, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
//-----------------------------------------------------------------------------
// 6: "First TS" label followed by the entry field
//-----------------------------------------------------------------------------
  lab = new TGLabel(fButtonsFrame,"First TS");
  fButtonsFrame->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  // lab->MoveResize(x4offset,y0+2*(dy+5),dx4,dy);
  
  fFirstTS = new TGNumberEntry(fButtonsFrame, 0, 9,999,
                                       TGNumberFormat::kNESInteger,
                                       TGNumberFormat::kNEANonNegative,
                                       TGNumberFormat::kNELLimitMinMax,
                                       0, 100000);
  fFirstTS->Connect("ValueSet(Long_t)", "MyMainFrame", this, "DoSetlabel()");
  (fFirstTS->GetNumberEntry())->Connect("ReturnPressed()","MyMainFrame", this,"DoSetlabel()");

  fButtonsFrame->AddFrame(fFirstTS, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
//-----------------------------------------------------------------------------
// 7: "SleepUS" label followed by the entry field
//-----------------------------------------------------------------------------
  lab = new TGLabel(fButtonsFrame,"Sleep US");
  fButtonsFrame->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  
  fSleepUS = new TGNumberEntry(fButtonsFrame, 2000000, 9,999,
                                       TGNumberFormat::kNESInteger,
                                       TGNumberFormat::kNEANonNegative,
                                       TGNumberFormat::kNELLimitMinMax,
                                       0, 100000000);
  fSleepUS->Connect("ValueSet(Long_t)", "MyMainFrame", this, "DoSetlabel()");
  (fSleepUS->GetNumberEntry())->Connect("ReturnPressed()","MyMainFrame", this,"DoSetlabel()");

  fButtonsFrame->AddFrame(fSleepUS, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
//-----------------------------------------------------------------------------
// 8: "ReadPrintFreq" label followed by the entry field
//-----------------------------------------------------------------------------
  lab = new TGLabel(fButtonsFrame,"PrintFreq");
  fButtonsFrame->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  
  fPrintFreq = new TGNumberEntry(fButtonsFrame,    1, 9,999,
                                 TGNumberFormat::kNESInteger,
                                 TGNumberFormat::kNEANonNegative,
                                 TGNumberFormat::kNELLimitMinMax,
                                 0, 100000);
  fPrintFreq->Connect("ValueSet(Long_t)", "MyMainFrame", this, "DoSetlabel()");
  (fPrintFreq->GetNumberEntry())->Connect("ReturnPressed()","MyMainFrame", this,"DoSetlabel()");

  fButtonsFrame->AddFrame(fPrintFreq, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
//-----------------------------------------------------------------------------
// set active DTC tab
//-----------------------------------------------------------------------------
  fActiveDtcID  = 0;
  fActiveDtc    = &fDtcData[0];
  fDtcTab->SetTab(fActiveDtcID);

  fActiveDtcTab = fDtcTel[fActiveDtcID].fTab; // fDtcTab->GetTabTab(0);
  fDtcTabColor  = fActiveDtcTab->GetBackground();

  fActiveDtcTab->ChangeBackground(fYellow);
//-----------------------------------------------------------------------------
// TextView
//-----------------------------------------------------------------------------
  fTextView = new TGTextViewostream(fMainFrame,930,620);
  // fTextView->MoveResize(10,y0+button_sy+5,930,620);
  fMainFrame->AddFrame(fTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 0));
//-----------------------------------------------------------------------------
// concluding operations
//-----------------------------------------------------------------------------
  fMainFrame->SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
  fMainFrame->MapSubwindows();
  fMainFrame->Resize(fMainFrame->GetDefaultSize());

  fMainFrame->MapRaised();
}

