#ifndef __TrkDqmNew_module_hh__
#define __TrkDqmNew_module_hh__

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic pop

#include <algorithm>

#include "fhiclcpp/ParameterSet.h"

#include "art/Framework/Core/EDAnalyzer.h"
// #include "art/Framework/Core/ModuleMacros.h"
#include "art/Framework/Principal/Event.h"
#include "art/Framework/Principal/Handle.h"
#include "art_root_io/TFileService.h"

#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/Table.h"

#include "TBufferFile.h"
#include "TProfile.h"
#include "TH1.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TBrowser.h"

#include "Offline/RecoDataProducts/inc/StrawDigi.hh"
#include "Offline/RecoDataProducts/inc/StrawHit.hh"
// #include "Offline/TrkHitReco/inc/PeakFit.hh"
#include "Offline/DataProducts/inc/StrawId.hh"
// #include "Offline/DataProducts/inc/TrkTypes.hh"

#include "artdaq-core/Data/Fragment.hh"

#include "artdaq-core-mu2e/Overlays/Decoders/TrackerDataDecoder.hh"
#include "artdaq-core-mu2e/Overlays/DTCEventFragment.hh"
#include "artdaq-core-mu2e/Overlays/FragmentType.hh"

namespace mu2e {
  class TrkDqmNew;
}


//-----------------------------------------------------------------------------
class mu2e::TrkDqmNew : public art::EDAnalyzer {
public:  

  enum {
    e_DEBUG   = 0,
    e_INFO    = 1,
    e_WARNING = 2,
    e_ERROR   = 3,
    e_SEVERE  = 4,
  };
  
  struct Config {
    using Name    = fhicl::Name;
    using Comment = fhicl::Comment;
    
    fhicl::Atom<int>             debugMode     {Name("debugMode"    ), Comment("0 or 1"              )};
    fhicl::Sequence<std::string> debugBits     {Name("debugBits"    ), Comment("debug bits"          )};
    fhicl::Atom<art::InputTag>   sdCollTag     {Name("sdCollTag"    ), Comment("straw digi coll tag" )};
    fhicl::Atom<art::InputTag>   shCollTag     {Name("shCollTag"    ), Comment("straw hit  coll tag" )};
    fhicl::Atom<int>             port          {Name("port"         ), Comment("port"                )};
  };
  
  struct EventData_t {
    const art::Event*      artevt;
    const mu2e::StrawDigi* sd;
    const mu2e::StrawHit*  sh;
  };

  struct Index_t {
    int  rn;
    int  stn;
    int  pln;
    int  pnl6;
    int  pnl12;
  };
                                        // TODO use constants from StrawID
//-----------------------------------------------------------------------------
// per-channel histograms
//-----------------------------------------------------------------------------
  struct ChannelHist_t {
    TH1F*         nhits;
    TH1F*         time[2];
    TH1F*         t0  [2];            // early times in ns
    TH1F*         t1  [2];            // late  times in ns
    TH1F*         tot [2];
    TH1F*         pmp;
    TH1F*         dt01[2];            // T0-T1 for each hit, with different binning
    TH1F*         dt0;                // T0 distance between the two consequtive pulses
    TH1F*         dt1;                // T1 distance between the two consequtive pulses
    TH1F*         dt2;                // T2 = (dt1+dt2)/2
    TH1F*         dt0r;               // T0(ich,0)-T0(ref,0)
    TH1F*         dt1r;               // T1(ich,0)-distance between the two pulses (if more than one)

    TH1F*         fsample;
    TH1F*         bline;
    TH1F*         pheight;
    TH1F*         q;                  // waveform charge Q
    TH1F*         qt;                 // tail charge Qt
    TH1F*         qtq;                // Qt/Q
  };
//-----------------------------------------------------------------------------
// per-panel histograms (or per-panel) histograms
//-----------------------------------------------------------------------------
  struct PanelHist_t {
    TH1F*         h_occup[2];                   // without and with the eDep cut
    //    TH2F*         h_edep_vs_straw;
    ChannelHist_t channel[96];
  };

//-----------------------------------------------------------------------------
// forgetting, for now, about multiple DTC's
//-----------------------------------------------------------------------------
  struct EventHist_t {
    TH1F*          h_evn;
    TH1F*          h_nhits;
  };
                                        // assume running after the straw digi maker
  struct StationHist_t {
    TH1F*          h_occup[2]; // occupancy for 1152 channels, to be displayed
    PanelHist_t*   panel[12];
  };
                                        // assume running after the straw digi maker
  struct Hist_t {
    EventHist_t*   event;
    StationHist_t* station[18];
  } _hist;

//-----------------------------------------------------------------------------
// talk-to parameters
//-----------------------------------------------------------------------------
  art::Event*      _artevt;
  
  art::InputTag    _sdCollTag;
  art::InputTag    _shCollTag;
  
  const mu2e::StrawDigiCollection*             _sdc;
  // const mu2e::StrawDigiADCWaveformCollection*  _sdawfc;
  const mu2e::StrawHitCollection*              _shc;

  EventData_t      _evtdata;
                                        // diagnostics
  int              _debugMode;
  int              _debugBit[100];

  int              _nstrawdigis;
  int              _nstrawhits;
  
  float            _timeWindow;
  float            _tdc_bin;            // us
  float            _tdc_bin_ns;         // ns
  int              _interactiveMode;
  int              _initialized;
  int              _port;

  art::ServiceHandle<art::TFileService> tfs;

  TApplication*      _app;
  TCanvas*           _canvas[100];
  TBrowser*          _browser;
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
public:
  
  explicit     TrkDqmNew(art::EDAnalyzer::Table<Config> const& conf);
  
  void         analyze (art::Event const& anEvent) override;
  void         beginJob()                          override;
  void         beginRun(art::Run   const& aRun   ) override;
  void         endJob  ()                          override;
  void         endRun  (art::Run   const& aRun   ) override;

  int          book_event_histograms  (art::TFileDirectory* Dir, EventHist_t*   Hist, Index_t* Index);
  int          book_panel_histograms  (art::TFileDirectory* Dir, PanelHist_t*   Hist, Index_t* Index);
  int          book_station_histograms(art::TFileDirectory* Dir, StationHist_t* Hist, Index_t* Index);

  int          book_histograms        (int RunNumber);
  void         debug                  (const art::Event& ArtEvent);
  
  int          DebugBit(int I)        {return _debugBit[I]; }

  int          fill_event_histograms  (EventHist_t*   Hist, EventData_t* Data);
  int          fill_panel_histograms  (PanelHist_t*   Hist, EventData_t* Data);
  int          fill_station_histograms(StationHist_t* Hist, EventData_t* Data);
  int          fill_histograms        (Hist_t*        Hist, EventData_t* Data);

  int          get_data               (const art::Event& AnEvent);
  
  void         print_                 (int Level, const std::string& Message,
                                       const std::source_location& location = std::source_location::current());
};

#endif
