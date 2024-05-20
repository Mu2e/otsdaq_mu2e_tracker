// Mainframe macro generated from application: root.exe
// By ROOT version 6.18/04 on 2020-05-11 18:09:54

#include "TApplication.h"

#ifndef ROOT_TGDockableFrame
#include "TGDockableFrame.h"
#endif
#ifndef ROOT_TGMenu
#include "TGMenu.h"
#endif
// #ifndef ROOT_TGMdiDecorFrame
// #include "TGMdiDecorFrame.h"
// #endif
// #ifndef ROOT_TG3DLine
// #include "TG3DLine.h"
// #endif
// #ifndef ROOT_TGMdiFrame
// #include "TGMdiFrame.h"
// #endif
// #ifndef ROOT_TGMdiMainFrame
// #include "TGMdiMainFrame.h"
// #endif
// #ifndef ROOT_TGMdiMenu
// #include "TGMdiMenu.h"
// #endif
// #ifndef ROOT_TGListBox
// #include "TGListBox.h"
// #endif
#ifndef ROOT_TGNumberEntry
#include "TGNumberEntry.h"
#endif
// #ifndef ROOT_TGScrollBar
// #include "TGScrollBar.h"
// #endif
// #ifndef ROOT_TGComboBox
// #include "TGComboBox.h"
// #endif
// #ifndef ROOT_TGuiBldHintsEditor
// #include "TGuiBldHintsEditor.h"
// #endif
#ifndef ROOT_TGuiBldNameFrame
#include "TGuiBldNameFrame.h"
#endif
#ifndef ROOT_TGFrame
#include "TGFrame.h"
#endif
// #ifndef ROOT_TGFileDialog
// #include "TGFileDialog.h"
// #endif
// #ifndef ROOT_TGShutter
// #include "TGShutter.h"
// #endif
// #ifndef ROOT_TGButtonGroup
// #include "TGButtonGroup.h"
// #endif
#ifndef ROOT_TGCanvas
#include "TGCanvas.h"
#endif
#ifndef ROOT_TGFSContainer
#include "TGFSContainer.h"
#endif
// #ifndef ROOT_TGuiBldEditor
// #include "TGuiBldEditor.h"
// #endif
#ifndef ROOT_TGColorSelect
#include "TGColorSelect.h"
#endif
#ifndef ROOT_TGButton
#include "TGButton.h"
#endif
// #ifndef ROOT_TGFSComboBox
// #include "TGFSComboBox.h"
// #endif
#ifndef ROOT_TGLabel
#include "TGLabel.h"
#endif
// #ifndef ROOT_TRootGuiBuilder
// #include "TRootGuiBuilder.h"
// #endif
#ifndef ROOT_TGTab
#include "TGTab.h"
#endif
#ifndef ROOT_TGListView
#include "TGListView.h"
#endif
// #ifndef ROOT_TGSplitter
// #include "TGSplitter.h"
// #endif
// #ifndef ROOT_TGStatusBar
// #include "TGStatusBar.h"
// #endif
// #ifndef ROOT_TGListTree
// #include "TGListTree.h"
// #endif
// #ifndef ROOT_TGuiBldGeometryFrame
// #include "TGuiBldGeometryFrame.h"
// #endif
// #ifndef ROOT_TGToolTip
// #include "TGToolTip.h"
// #endif
// #ifndef ROOT_TGToolBar
// #include "TGToolBar.h"
// #endif

#ifndef ROOT_TGuiBldDragManager
#include "TGuiBldDragManager.h"
#endif

#ifndef ROOT_TSystem
#include "TSystem.h"
#endif

#ifndef ROOT_TDatime
#include "TDatime.h"
#endif

#ifndef ROOT_TGTextViewStream
#include "TGTextViewStream.h"
#endif

// #ifndef ROOT_Riostream
// #include "Riostream.h"
// #endif

#include "scripts/cfo.C"

//-----------------------------------------------------------------------------
class DtcGui {
  // RQ_OBJECT("DtcGui")
public: 

  enum { 
    kIN_PROGRESS = 0,
    kSUBMITTED   = 1,
    kCOMPLETED   = 2
  };

  struct RocData_t {
    TString fName;         // 
  };

  struct DtcData_t {
    TString    fName;         // 
    
    RocData_t  fRocData[6];
    RocData_t* fActiveRoc;

    DtcData_t(const char* Name = "") {
      fName      = Name;
      fActiveRoc = nullptr;
      for (int i=0;i<6; i++) {
        fRocData[i].fName = Form("ROC%i",i);
      }
    }
  };


  struct RocTabElement_t {
    TGCompositeFrame* fFrame;
    TGTabElement*     fTab;             // its own tab element
    Pixel_t           fColor;
  };


  struct DtcTabElement_t {
    TGCompositeFrame* fFrame;
//    TGTextEntry*      fRoc;
    TGTabElement*     fTab;             // its own tab element
    DTC*              fDTC;             // driver interface
    CFO*              fCFO;
    Pixel_t           fColor;

    TGTab*            fRocTab;
    RocTabElement_t   fRocTel[6];
    RocTabElement_t*  fActiveRocTel;
    int               fActiveRocID;
    Pixel_t           fRocTabColor;	// non-active roc tab tip
    DtcData_t*        fData;
  };

  TGMainFrame*        fMainFrame;
  TGTab*              fDtcTab ;
  TGTextViewostream*  fTextView;
  TGVerticalFrame*    fContents;

  TString             fHost;

  TString             fDevice;
  TString             fIStage;
  TString             fTime;

  int                 fNDtcs;
  DtcTabElement_t     fDtcTel[2];

  TGTabElement*       fActiveDtcTab;
  int                 fActiveDtcID;

  DtcData_t           fDtcData[2];
  DtcData_t*          fActiveDtc;

  int                 fNRocs;

  Pixel_t             fGreen;		    // completed stage tab tip
  Pixel_t             fYellow;		  // active tab tip
  Pixel_t             fDtcTabColor;	// non-active tab tip
  Pixel_t             fSubmittedColor;

  int                 fDebugLevel;
  
  DtcGui(const char* Hostname, const TGWindow *p, UInt_t w, UInt_t h, int DebugLevel = 0);
  virtual ~DtcGui();

  void     DoDtcTab          (Int_t id);
  void     DoRocTab          (Int_t id);

  void     BuildDtcTabElement(TGTab*& Tab, DtcTabElement_t& TabElement, DtcData_t* DtcData);
  void     BuildRocTabElement(TGTab*& Tab, RocTabElement_t& TabElement, RocData_t* RocData);

  void     BuildGui          (const TGWindow *Parent, UInt_t Width, UInt_t Height);

  void     ExecuteCommand(const char* Cmd, int PrintOnly = 0);

  void     clear_output       ();
  void     exit               ();

  void     dtc_soft_reset     ();
  void     dtc_hard_reset     ();

  void     print_dtc_status   ();
  void     read_dtc_register  ();
  void     write_dtc_register ();

  void     read_roc_register  ();
  void     write_roc_register ();
  void     print_roc_status   ();
  void     reset_roc          ();

}; 


//-----------------------------------------------------------------------------
// initialization with the project data - in its rudimentary form
//-----------------------------------------------------------------------------
DtcGui::DtcGui(const char* Host, const TGWindow *p, UInt_t w, UInt_t h, int DebugLevel) {

  fHost           = Host;
  fDebugLevel     = DebugLevel;
  fSubmittedColor = 16724889;

  fNDtcs = 2;  // 0:9
  fDtcData[0] = DtcData_t("CFO:0");
  fDtcData[1] = DtcData_t("DTC:1");

  BuildGui(p,w,h);
//-----------------------------------------------------------------------------
// two PCIE cards
//-----------------------------------------------------------------------------

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
  TString cmd;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::ExecuteCommand : cmd: " << "print_status" << std::endl;

  printf("Active TAB ID: %i\n",fActiveDtcID);

  if      (fActiveDtcID == 0) {
    try {
      cfo_print_status();
    }
    catch (...) {
      *fTextView << Form("ERROR : coudn't read CFO") << std::endl;
    }
  }
  else if (fActiveDtcID == 1) {
    try {
      dtc_print_status();
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
void DtcGui::clear_output() {
  fTextView->Clear();
}

//-----------------------------------------------------------------------------
void DtcGui::exit() {
  gApplication->Terminate();
}

//-----------------------------------------------------------------------------
void DtcGui::print_roc_status() {
  //  TString cmd;

  DtcTabElement_t* tab = fDtcTel+fActiveDtcID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::ExecuteCommand : cmd: " << "print_roc_status" << std::endl;

  printf("Active TAB ID: %i\n",fActiveDtcID);

  int roc = tab->fActiveRocID;
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (fActiveDtcID == 1) {
    try { dtc_print_roc_status(roc); }
    catch (...) { *fTextView << Form("ERROR : coudn't read ROC %i ... BAIL OUT",roc) << std::endl; }
  }

  TDatime x2;
  *fTextView << x2.AsSQLString() << strCout.str() << " DtcGui::ExecuteCommand : DONE " <<  std::endl;

  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );

  //  ExecuteCommand(cmd.Data(),fDebugLevel);
  //  tab->fColor = fSubmittedColor;
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
  if (fActiveDtcID == 1) {
    try         { dtc_reset_roc(roc); }
    catch (...) { *fTextView << Form("ERROR : coudn't reset ROC %i ... BAIL OUT",roc) << std::endl; }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("%s DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();

  // Restore old cout.
  cout.rdbuf( oldCoutStreamBuf );

  //  ExecuteCommand(cmd.Data(),fDebugLevel);
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
  if (fActiveDtcID == 1) {
    try         { dtc_soft_reset(); }
    catch (...) { *fTextView << Form("ERROR : coudn't soft reset DTC %i ... BAIL OUT",roc) << std::endl; }
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
  if (fActiveDtcID == 1) {
    try         { dtc_hard_reset(); }
    catch (...) { *fTextView << Form("ERROR : coudn't hard reset DTC %i ... BAIL OUT",roc) << std::endl; }
  }

  TDatime x2; *fTextView << x2.AsSQLString() << strCout.str() << Form("%s DONE",__func__) <<  std::endl;
  fTextView->ShowBottom();
                                        // Restore old cout.
  cout.rdbuf(oldCoutStreamBuf);
}

//-----------------------------------------------------------------------------
void DtcGui::read_dtc_register() {
  //  TString cmd;

  DtcTabElement_t* tab = fDtcTel+fActiveDtcID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " << "read_register" << std::endl;

  printf("Active TAB ID: %i\n",fActiveDtcID);

  int roc = tab->fActiveRocID;
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  uint reg = 0x9004;
  if (fActiveDtcID == 1) {
    try {
      dtc_read_register(reg);
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
void DtcGui::write_dtc_register() {
  //  TString cmd;

  DtcTabElement_t* tab = fDtcTel+fActiveDtcID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " << "read_register" << std::endl;

  printf("Active TAB ID: %i\n",fActiveDtcID);
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  uint reg = 0x9004;
  if (fActiveDtcID == 1) {
    try {
      uint val = dtc_read_register(reg);
      *fTextView << Form("%s: DTC %i reg 0x%4x val: %0x8x",__func__,fActiveDtcID,reg,val) << std::endl;
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
void DtcGui::read_roc_register() {
  //  TString cmd;

  DtcTabElement_t* dtel = fDtcTel+fActiveDtcID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " << "read_roc_register" << std::endl;

  printf("Active DTC ID: %i\n",fActiveDtcID);

  int roc = dtel->fActiveRocID;
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (fActiveDtcID == 1) {
    uint16_t reg = 0;
    try {
      uint16_t val;
      dtc_read_roc_register(roc,reg,val);
      *fTextView << Form("%s: roc: %i reg : %0x04x val: 0x%04x",__func__,roc,reg,val) << std::endl;
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
void DtcGui::write_roc_register() {
  //  TString cmd;

  DtcTabElement_t* tab = fDtcTel+fActiveDtcID;

  streambuf* oldCoutStreamBuf = cout.rdbuf();

  ostringstream strCout;
  cout.rdbuf( strCout.rdbuf() );

  TDatime x1;
  *fTextView << x1.AsSQLString() << " DtcGui::" << __func__ << ": cmd: " << "write_roc_register" << std::endl;

  printf("Active TAB ID: %i\n",fActiveDtcID);

  int roc = tab->fActiveRocID;
//-----------------------------------------------------------------------------
// CFO doesn't have ROC's
//-----------------------------------------------------------------------------
  if (fActiveDtcID == 1) {
    uint16_t reg = 0;
    try {
      dtc_write_roc_register(roc,reg,0);
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
    int old_active = fActiveDtcID;

    TGTabElement* tel = fDtcTab->GetTabTab(id);

    if (fActiveDtcTab != tel) {
      tel->ChangeBackground(fYellow);
      fActiveDtcTab->ChangeBackground(fDtcTel[old_active].fColor);
      fActiveDtcTab = tel;
    }

    fActiveDtcID = id;
    fActiveDtc      = &fDtcData[id];
  }

  printf("Tab ID: %3i stage: %-15s  %-15s title: %-15s\n",
         id,
         fActiveDtc->fName.Data(),
         fDtcTel[id].fRocTel->fTab->GetText()->Data(),
         fActiveDtcTab->GetText()->Data());
}


//-----------------------------------------------------------------------------
// Tab here is the list of ROC tabs
//-----------------------------------------------------------------------------
void DtcGui::BuildRocTabElement(TGTab*& Tab, RocTabElement_t& TabElement, RocData_t* RocData) {

  const char* device_name = RocData->fName.Data();

  //  printf("[DtcGui::BuildTabElement] title: %s\n",title);

  TabElement.fFrame = Tab->AddTab(device_name);
  int ntabs         = Tab->GetNumberOfTabs();

  // TabElement.fFrame->SetLayoutManager(new TGVerticalLayout(TabElement.fFrame));

  TabElement.fTab   = Tab->GetTabTab(ntabs-1);
  TabElement.fColor = TabElement.fTab->GetBackground();

  TGGroupFrame* group = new TGGroupFrame(TabElement.fFrame,Form("%s",device_name));
  group->SetLayoutBroken(kTRUE);
//-----------------------------------------------------------------------------
// 1st column: commands : a) ROC status, b) write c) read
//-----------------------------------------------------------------------------
  TGTextButton* tb;

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);

  int x0, y0, dx, dy;
  x0 = 10; y0 = 20; dx = 60; dy = 25; 

  tb->MoveResize(x0,y0,dx,dy);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "print_roc_status()");
//-----------------------------------------------------------------------------
// ROC read/write register 
//-----------------------------------------------------------------------------
  tb = new TGTextButton(group,"write",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+dy+5,dx,dy);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->Connect("Pressed()", "DtcGui", this, "write_roc_register()");

  tb = new TGTextButton(group,"read",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*2,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "read_roc_register()");

  tb = new TGTextButton(group,"reset",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  tb->SetTextJustify(36);
  tb->SetMargins(0,0,0,0);
  tb->SetWrapLength(-1);
  
  tb->MoveResize(x0,y0+(dy+5)*3,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "reset_roc()");
//-----------------------------------------------------------------------------
// ROC register
//-----------------------------------------------------------------------------
  int dx2 = 70;
  TGLabel* lab = new TGLabel(group,"register");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x0+dx+10, y0, dx2,dy);
//-----------------------------------------------------------------------------
// graphics context changes
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
// write register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x0000");
  // wr->Resize(160,reg->GetDefaultHeight());
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->MoveResize(x0+dx+10,y0+dy+5,dx2,dy);

  TGTextEntry*  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText("0x0000");
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->MoveResize(x0+dx+10,y0+(dy+5)*2,dx2,dy);
//-----------------------------------------------------------------------------
// 3rd column: write value: 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"value");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x0+dx+10+dx2+10, y0        ,dx2,dy);

  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  wval->SetMaxLength(4096);
  wval->SetAlignment(kTextLeft);
  wval->SetText("0x0000");
  group->AddFrame(wval, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wval->MoveResize(x0+dx+10+dx2+10,y0+(dy+5) ,dx2,dy);
 
  lab = new TGLabel(group,"0x0000");
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->MoveResize(x0+dx+10+dx2+10,y0+2*(dy+5),dx2,dy);
//-----------------------------------------------------------------------------
// finish composition of the ROC tab element
//-----------------------------------------------------------------------------
  TabElement.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(350,20,450,200);
}
//-----------------------------------------------------------------------------
void DtcGui::BuildDtcTabElement(TGTab*& Tab, DtcTabElement_t& TabElement, DtcData_t* DtcData) {

  const char* device_name = DtcData->fName.Data();

  //  printf("[DtcGui::BuildTabElement] title: %s\n",title);

  TabElement.fFrame = Tab->AddTab(device_name);
  int ntabs         = Tab->GetNumberOfTabs();
  TabElement.fFrame->SetLayoutManager(new TGVerticalLayout(TabElement.fFrame));
  TabElement.fTab   = Tab->GetTabTab(ntabs-1);
  TabElement.fColor = TabElement.fTab->GetBackground();

  TGGroupFrame* group = new TGGroupFrame(TabElement.fFrame,Form("%s : %s",fHost.Data(),device_name));
  group->SetLayoutBroken(kTRUE);

  //  TabElement.fRocTab = new TGTab(TabElement.fFrame,300,200);
  //  TabElement.fFrame->AddFrame(TabElement.fRocTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  TabElement.fRocTab = new TGTab(group,300,200);
  group->AddFrame(TabElement.fRocTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  for (int i=0; i<6; i++) {
    BuildRocTabElement(TabElement.fRocTab,TabElement.fRocTel[i], &fDtcData->fRocData[i]);
  }

  TabElement.fRocTab->MoveResize(380,20,500,240);
  TabElement.fRocTab->Connect("Selected(Int_t)", "DtcGui", this, "DoRocTab(Int_t)");
//-----------------------------------------------------------------------------
// set active ROC tab
//-----------------------------------------------------------------------------
  int id = 0;
  TabElement.fActiveRocID  = id;
  // TabElement.fActiveRoc    = &fRocData[id];
  TabElement.fRocTab->SetTab(id);

  TabElement.fActiveRocTel = &TabElement.fRocTel[id];
  TabElement.fRocTabColor  = TabElement.fActiveRocTel->fTab->GetBackground();

  TabElement.fActiveRocTel->fTab->ChangeBackground(fYellow);
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

  x0 = 20; y0 = 30; dx = 80; dy = 30; 

  tb = new TGTextButton(group,"status",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
  group->AddFrame(tb, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

  tb->SetTextJustify(36);
  tb->SetMargins    (0,0,0,0);
  tb->SetWrapLength (-1);
  
  tb->MoveResize(x0,y0,dx,dy);
  tb->Connect("Pressed()", "DtcGui", this, "print_dtc_status()");

  ULong_t ucolor;        // will reflect user color changes
  //  gClient->GetColorByName("#ffffcc",ucolor);
  gClient->GetColorByName("#ccffcc",ucolor);  // light green
  tb->ChangeBackground(ucolor);
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
//-----------------------------------------------------------------------------
// second column
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
// write register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  wr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wr->SetMaxLength(4096);
  wr->SetAlignment(kTextLeft);
  wr->SetText("0x0000");
  wr->MoveResize(x0+dx+10,y0+dy+5,dx2,dy);
//-----------------------------------------------------------------------------
// read register  to interact with
//-----------------------------------------------------------------------------
  TGTextEntry*  rr = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(rr, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  rr->SetMaxLength(4096);
  rr->SetAlignment(kTextLeft);
  rr->SetText("0x0000");
  rr->MoveResize(x0+dx+10,y0+2*(dy+5),dx2,dy);
//-----------------------------------------------------------------------------
// 3rd column: write value: 1. label , 2: entry field  3: label
//-----------------------------------------------------------------------------
  lab = new TGLabel(group,"value");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x0+dx+10+dx2+10, y0        ,dx2,dy);

  TGTextEntry*  wval = new TGTextEntry(group, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
  group->AddFrame(wval, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  wval->SetMaxLength(4096);
  wval->SetAlignment(kTextLeft);
  wval->SetText("0x0000");
  wval->MoveResize(x0+dx+10+dx2+10,y0+(dy+5) ,dx2,dy);
 
  lab = new TGLabel(group,"0x0000");
  group->AddFrame(lab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
  lab->SetTextJustify(36);
  lab->SetMargins(0,0,0,0);
  lab->SetWrapLength(-1);
  lab->MoveResize(x0+dx+10+dx2+10,y0+2*(dy+5),dx2,dy);
//-----------------------------------------------------------------------------
// resize the group panel
//-----------------------------------------------------------------------------
  TabElement.fFrame->AddFrame(group, new TGLayoutHints(kLHintsNormal));
  group->MoveResize(10,10,1080,280);
}


//-----------------------------------------------------------------------------
void DtcGui::BuildGui(const TGWindow *Parent, UInt_t Width, UInt_t Height) {
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
   fDtcTab = new TGTab(fMainFrame,380,500);
   fMainFrame->AddFrame(fDtcTab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));

   for (int i=0; i<fNDtcs; i++) {
     BuildDtcTabElement(fDtcTab,fDtcTel[i],&fDtcData[i]);
   }

   int y0 = 330;
   fDtcTab->MoveResize(10,10,1100,310);  // this defines the size of the tab below the tabs line
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
   fTextView->MoveResize(10,y0+button_sy+5,1100,500);
   fMainFrame->AddFrame(fTextView, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 5, 5, 5, 5));
//-----------------------------------------------------------------------------
// concluding operations
//-----------------------------------------------------------------------------
   fMainFrame->SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
   fMainFrame->MapSubwindows();
   fMainFrame->Resize(fMainFrame->GetDefaultSize());

   fMainFrame->MapRaised();
   // fMainFrame->Resize(590,y0+4*button_sy);  // window size 
}

//-----------------------------------------------------------------------------
DtcGui* dtc_gui(int DebugLevel = 0) {
  // 1200x800: dimensions of the main frame
  
  const char* hostname = gSystem->Getenv("HOSTNAME");

  DtcGui* x = new DtcGui(hostname,gClient->GetRoot(),1200,900,DebugLevel);
  return x;
} 
