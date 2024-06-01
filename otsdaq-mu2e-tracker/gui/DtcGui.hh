// Mainframe macro generated from application: root.exe
// By ROOT version 6.18/04 on 2020-05-11 18:09:54

#ifndef __otsdaq_mu2e_tracker_DtcGui_hh__
#define __otsdaq_mu2e_tracker_DtcGui_hh__

#include "stdlib.h"
#include "stdio.h"

#include "TApplication.h"

// #ifndef ROOT_TGDockableFrame
// #include "TGDockableFrame.h"
// #endif
// #ifndef ROOT_TGMenu
// #include "TGMenu.h"
// #endif
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

// #include "scripts/cfo.C"
#include "otsdaq-mu2e-tracker/ui/CfoInterface.hh"
#include "otsdaq-mu2e-tracker/ui/DtcInterface.hh"

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
    int     fLink;
  };

  struct DtcData_t {
    TString    fName;         // 
    int        fPcieIndex;

    RocData_t  fRocData[6];
    RocData_t* fActiveRoc;

    DtcData_t(const char* Name = "", int PcieIndex = 0) {
      fName      = Name; 
      fPcieIndex = PcieIndex;

      fActiveRoc = nullptr;
      for (int i=0;i<6; i++) {
        fRocData[i].fName = Form("ROC%i",i);
        fRocData[i].fLink = i;
      }
    }
  };


  struct RocTabElement_t {
    TGCompositeFrame* fFrame;
    TGTabElement*     fTab;             // its own tab element

    TGTextEntry*      fRegW;
    TGTextEntry*      fRegR;
    TGTextEntry*      fValW;
    TGLabel*          fValR;

    Pixel_t           fColor;
  };


  struct DtcTabElement_t {
    TGCompositeFrame* fFrame;
    TGTabElement*     fTab;             // its own tab element

    TGTextEntry*      fRegW;
    TGTextEntry*      fRegR;
    TGTextEntry*      fValW;
    TGLabel*          fValR;

    trkdaq::DtcInterface*     fDTC_i;           // driver interface
    trkdaq::CfoInterface*     fCFO_i;
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
  Pixel_t             fValidatedColor;

  int                 fDebugLevel;
  
  DtcGui(const char* Hostname, const TGWindow *p, UInt_t w, UInt_t h, int DebugLevel = 0);
  virtual ~DtcGui();

  void     DoDtcTab          (Int_t id);
  void     DoRocTab          (Int_t id);

  void     BuildDtcTabElement(TGTab*& Tab, DtcTabElement_t& TabElement, DtcData_t* DtcData);
  void     BuildRocTabElement(TGTab*& Tab, RocTabElement_t& TabElement, RocData_t* RocData);

  void     BuildGui          (const TGWindow *Parent, UInt_t Width, UInt_t Height);

  void     ExecuteCommand(const char* Cmd, int PrintOnly = 0);

  void     cfo_launch_run_plan();

  void     clear_output       ();

  void     dtc_soft_reset     ();
  void     dtc_hard_reset     ();

  void     exit               ();

  void     init_external_cfo_readout_mode();

  void     print_firefly_temp ();
  void     print_dtc_status   ();
  void     print_roc_status   ();

  void     read_dtc_register  ();
  void     read_roc_register  ();
  void     reset_roc          ();

  void     write_dtc_register ();
  void     write_roc_register ();

}; 

#endif
