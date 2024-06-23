
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

#include "TROOT.h"

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
  fSubmittedColor = 16724889;

  if (fDebugLevel > 0) printf("DtcGui::%s: host:%s project:%s\n",__func__,fHostname.Data(),Project);

  InitRunConfiguration(Project);

  if (fDebugLevel > 0) printf("DtcGui::%s: before BuildGui\n",__func__);
  BuildGui(p,w,h);
  if (fDebugLevel > 0) printf("DtcGui::%s: after  BuildGui\n",__func__);
//-----------------------------------------------------------------------------
// two PCIE cards
//-----------------------------------------------------------------------------
  for (int i=0; i<2; i++) {
    DtcTabElement_t* dtel = &fDtcTel[i];
    int pcie_addr = fDtcData[i].fPcieAddr;
    if      (fDtcData[i].IsDtc()) {
      dtel->fDTC_i  = DtcInterface::Instance(pcie_addr,fDtcData[i].fLinkMask);
    }
    else if (fDtcData[i].IsCfo()) {
      dtel->fCFO_i = CfoInterface::Instance(pcie_addr);
//-----------------------------------------------------------------------------
// there is only one CFO, so initialize the CFO pointer - by copying it from the tab
//-----------------------------------------------------------------------------
      fCFO_i = CfoInterface::Instance(pcie_addr);
    }
  }

  if (fDebugLevel > 0) printf("DtcGui::%s : all done, EXIT\n", __func__);
}

//-----------------------------------------------------------------------------
DtcGui::~DtcGui() {
  fMainFrame->Cleanup();
}

// config/$hostname/$project.C should contain function init_run_configuration(DtcGui*)
//-----------------------------------------------------------------------------
int DtcGui::InitRunConfiguration(const char* Config) {
  int           rc(0);
  TInterpreter* cint = gROOT->GetInterpreter();

  TInterpreter::EErrorCode irc;

  TString macro = Form("tdaq-v3_01_00/otsdaq-mu2e-tracker/config/%s/%s.C",Config,fHostname.Data());

  if (fDebugLevel > 0) printf("DtcGui::%s : loading %s\n",__func__,macro.Data());
  
  cint->LoadMacro(macro.Data(), &irc);

  if (rc == 0) {
    TString cmd = Form("init_run_configuration((DtcGui*) 0x%0lx)",(long int) this);

    if (fDebugLevel > 0) printf("DtcGui::%s : cmd=%s\n", __func__,cmd.Data());
    
    gInterpreter->ProcessLine(cmd.Data());
  }

  return rc;
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
    else {
      BuildDtcTabElement(fDtcTab,fDtcTel[i],&fDtcData[i]);
    }
  }

  fDtcTab->MoveResize(10,10,930,290);  // this defines the size of the tab below the tabs line
  fDtcTab->Connect("Selected(Int_t)", "DtcGui", this, "DoDtcTab(Int_t)");
//-----------------------------------------------------------------------------
// common buttons on fMainFrame, they are the same for different DTCs and ROCs
//-----------------------------------------------------------------------------
  fButtonsFrame = new TGHorizontalFrame(fMainFrame);
  fMainFrame->AddFrame(fButtonsFrame, new TGLayoutHints(kLHintsTop | kLHintsExpandX, 0, 0, 0, 0));

  TGTextButton* tb;

  int y0        = 10;
  int button_dx = 150;
  int button_sx = 150+10;          // includes 10 pixes between the buttons
  int button_sy =  30;
//-----------------------------------------------------------------------------
// 1. clear
//-----------------------------------------------------------------------------
  tb = new TGTextButton(fButtonsFrame,"clear",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  tb->MoveResize(10,y0,button_dx,25);
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
  tb->MoveResize(10+button_sx,y0,button_dx,25);
  fButtonsFrame->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "exit()");
  tb->ChangeBackground(fValidatedColor);
//-----------------------------------------------------------------------------
// 3. launch
//-----------------------------------------------------------------------------
  tb = new TGTextButton(fButtonsFrame,"launch",-1,TGTextButton::GetDefaultGC()(),
                        TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  tb->MoveResize(10+button_sx*2,y0,button_dx,25);
  fButtonsFrame->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "cfo_launch_run_plan()");
  tb->ChangeBackground(fValidatedColor);
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

