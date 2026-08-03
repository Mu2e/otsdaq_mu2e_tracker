///////////////////////////////////////////////////////////////////////////////
// tracker DQM module
///////////////////////////////////////////////////////////////////////////////
#include "TRACE/tracemf.h"
#define  TRACE_NAME "TrkDqmNew"

#include "TSystem.h"
#include "TROOT.h"

#include "artdaq-core-mu2e/Overlays/DTC_Packets/DTC_EventHeader.h"

#include "otsdaq-mu2e-tracker/ArtModules/TrkDqmNew_module.hh"

//-----------------------------------------------------------------------------
// Level:     0: debug
//            1: info
//            2: warning
//            3: error
//------------------------------------------------------------------------------
void mu2e::TrkDqmNew::print_(int Level, const std::string& Message, const std::source_location& location) {

  struct xxx {
    
    std::vector<std::string> splitString(const std::string& str, const std::string& delimiter) {
      std::vector<std::string> result;
      std::regex re(delimiter);
      std::sregex_token_iterator it(str.begin(), str.end(), re, -1);
      std::sregex_token_iterator end;
      while (it != end) {
        result.push_back(*it++);
      }
      return result;
    }
  } xx;
    
  std::string s;
  const art::Event* artevt = _evtdata.artevt;
  
  if (artevt) s = std::format("event: {}:{}:{} ",artevt->run(),artevt->subRun(),artevt->event());

  std::vector<std::string> ss = xx.splitString(location.file_name(),"/");

  if (Level == e_DEBUG) {
                                        // debug
    MF_LOG_TRACE("MAKE_DIGI_NT") << s << ss.back() << ":" << location.line() << " : " << Message;
  } 
  else if (Level == e_INFO) {
                                        // info
    MF_LOG_VERBATIM("MAKE_DIGI_NT")
      << s << ss.back() << ":" << location.line() 
      //            << location.function_name()
      << " : " << Message;
  }
  else if (Level == e_WARNING) {                // warning
    MF_LOG_PRINT("MAKE_DIGI_NT") << "WARNING: " << s << ss.back() << ":" << location.line() << " : " << Message;
  }

  else if (Level == e_ERROR) {                // 
    MF_LOG_PROBLEM("MAKE_DIGI_NT") << "ERROR: " << s << ss.back() << ":" << location.line() << " : " << Message;
  }

  else if (Level == e_SEVERE) {                // 
    MF_LOG_ABSOLUTE("MAKE_DIGI_NT") << "SEVERE: " << s << ss.back() << ":" << location.line() << " : " << Message;
  }
}

//-----------------------------------------------------------------------------
mu2e::TrkDqmNew::TrkDqmNew(art::EDAnalyzer::Table<Config> const& conf) : 
  art::EDAnalyzer   (conf),
  _debugMode        (conf().debugMode     ()), 
  _sdCollTag        (conf().sdCollTag     ()),
  _shCollTag        (conf().shCollTag     ()),
  _port             (conf().port          ())
{
//-----------------------------------------------------------------------------
// parse debug bits
//-----------------------------------------------------------------------------
  for (int i=0; i<100; ++i) _debugBit[i] = 0;

                                        // a flag is an integer!
  std::vector<std::string> debugBits (conf().debugBits());
  int nbits = debugBits.size();
  for (int i=0; i<nbits; ++i) {
    int index(0), value(0);
    const char* key = debugBits[i].data();
    sscanf(key,"bit%i:%i",&index,&value);
    _debugBit[index]  = value;

    TLOG(TLVL_DEBUG+1) << Form("... TrkDqmNew: bit=%4i is set to %i\n",index,_debugBit[index]);
  }

  //  double f0(31.29e6);                   // oscillator frequency 31.29 MHz

  _timeWindow      = 200.; // us  // conf().timeWindow()*25.;  // in ns
  _interactiveMode = 1;

  // if      (_pulserFrequency ==  60) _freq = f0/(pow(2,9)+1);     // ~ 60 kHz
  // else if (_pulserFrequency == 250) _freq = f0/(pow(2,7)+1);     // ~250 kHz

  // _dt   = 1/_freq*1.e9;               // in ns
//------------------------------------------------------------------------------
// default map, Richie says TS1 may have an old firmware with some bugs
//-----------------------------------------------------------------------------
  // int adc_index_0[96] = {
  //   91, 85, 79, 73, 67, 61, 55, 49,
  //   43, 37, 31, 25, 19, 13,  7,  1,
  //   90, 84, 78, 72, 66, 60, 54, 48,
      
  //   42, 36, 30, 24, 18, 12,  6,  0,
  //   93, 87, 81, 75, 69, 63, 57, 51,
  //   45, 39, 33, 27, 21, 15,  9,  3,
      
  //   44, 38, 32, 26, 20, 14,  8,  2, 
  //   92, 86, 80, 74, 68, 62, 56, 50,
  //   47, 41, 35, 29, 23, 17, 11,  5,
      
  //   95, 89, 83, 77, 71, 65, 59, 53,
  //   46, 40, 34, 28, 22, 16, 10,  4,
  //   94, 88, 82, 76, 70, 64, 58, 52
  // };

  // _tdc_bin             = (5/256.*1e-3);       // TDC bin width (Richie), in us
  // _tdc_bin_ns          = _tdc_bin*1e3;        // convert to ns

  _initialized         = 0;
}

//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::book_panel_histograms(art::TFileDirectory* Dir, PanelHist_t* Hist, Index_t* Index) {
  std::string prefix = std::format("run {:06d}:",Index->rn);

  std::string title;

  title = std::format("{} occup[0]",prefix);
  Hist->h_occup[0] = Dir->make<TH1F>("h_occup_0", title.data(), 100, 0.,    100.);

  title = std::format("{} occup[1]",prefix);
  Hist->h_occup[1] = Dir->make<TH1F>("h_occup_1", title.data(), 100, 0.,    100.);

  return 0;
}

//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::book_station_histograms(art::TFileDirectory* Dir, StationHist_t* Hist, Index_t* Index) {

  std::string prefix = std::format("run {:06d}:",Index->rn);
  std::string title;

  title = std::format("{} occup[0]",prefix);
  Hist->h_occup[0] = Dir->make<TH1F>("h_occup_0", title.data(), 1152, 0.,    1152.);

  title = std::format("{} occup[1]",prefix);
  Hist->h_occup[1] = Dir->make<TH1F>("h_occup_1", title.data(), 1152, 0.,    1152.);
  
  for (int ip=0; ip<12; ip++) {
    std::string folder_name = std::format("pnl_{:02d}",ip);
    art::TFileDirectory pnl_dir = Dir->mkdir(folder_name.data());
    Hist->panel[ip] = new PanelHist_t;

    Index->pnl12 = Index->stn*12+ip;
    book_panel_histograms(&pnl_dir,Hist->panel[ip],Index);
  }

  return 0;
}

//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::book_event_histograms(art::TFileDirectory* Dir, EventHist_t* Hist, Index_t* Index) {

  std::string prefix = std::format("run {:06d}:",Index->rn);
  std::string title;

  title = std::format("{} n hits total",prefix);
  Hist->h_nhits          = Dir->make<TH1F>("nhits"      , title.data(), 1000, 0.,   5000.);

  return 0;
}


//-----------------------------------------------------------------------------
// for now - make the interface work with one station only
//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::book_histograms(int RunNumber) {
  TLOG(TLVL_INFO) << std::format("-- START:");
  
  art::ServiceHandle<art::TFileService> tfs;
    
  TH1::AddDirectory(kFALSE);
    
  char folder_name[100];

  Index_t  index;
  index.rn = RunNumber;

  art::TFileDirectory evt_dir = tfs->mkdir("evt");
  _hist.event                 = new EventHist_t;
  book_event_histograms(&evt_dir,_hist.event,&index);
    
  for (int ist=0; ist<18; ist++) {
    sprintf(folder_name,"stn_%02i",ist);
    art::TFileDirectory stn_dir = tfs->mkdir(folder_name);
    _hist.station[ist] = new StationHist_t;

    index.stn = ist;
    book_station_histograms(&stn_dir,_hist.station[ist],&index);
  }
  
  TLOG(TLVL_INFO) << std::format("pointer to the module: 0x{:8p}",(void*) this);
  return 0;
}

//-----------------------------------------------------------------------------
void mu2e::TrkDqmNew::beginJob() {
  TLOG(TLVL_INFO) << "-- START";

  if (_interactiveMode != 0) {
    int           tmp_argc(2);
    char**        tmp_argv(nullptr);

    tmp_argv    = new char*[2];
    tmp_argv[0] = new char[100];
    tmp_argv[1] = new char[100];

    strcpy(tmp_argv[0],"-b");
    strcpy(tmp_argv[1],Form("--web=server:%d",_port));
    
    _app = new TApplication("TrkDqmNew", &tmp_argc, tmp_argv);
    gROOT->SetWebDisplay(Form("server:%d",_port));
    
    // _app->Run()
    _app->Run(true);
    // delete [] tmp_argv;
  }

}

//-----------------------------------------------------------------------------
void mu2e::TrkDqmNew::endJob() {
  // delete _app;
  // delete _canvas[0];
  // delete _canvas[1];
  // delete _canvas[2];
}

//-----------------------------------------------------------------------------
void mu2e::TrkDqmNew::beginRun(const art::Run& ArtRun) {
  int rn = ArtRun.run();
  
  TLOG(TLVL_INFO) << std::format("-- START: run_number:{} _initialized:{} _interactiveMode:{}",
                                 rn,_initialized,_interactiveMode);
  
  if (_initialized != 0) return;
  _initialized = 1;
//-----------------------------------------------------------------------------
// as a last step, book histograms - need to know the number of active links
//----------------------------------------------------------------------------- 
  book_histograms(rn);

  if (_interactiveMode != 0) {
    _canvas[0] = new TCanvas("canvas_000");
    _canvas[0]->Divide(2,2);
    
    _canvas[1] = new TCanvas("canvas_001");
    _canvas[1]->Divide(3,2);
    
    _canvas[2] = new TCanvas("canvas_002");
    _canvas[2]->Divide(2,2);
      
    _browser   = new TBrowser();

//-----------------------------------------------------------------------------
// canvas #0 : for now, event number and total number of hits
//-----------------------------------------------------------------------------
    _canvas[0]->cd(1);
    _hist.event->h_evn->Draw();
    _canvas[0]->cd(2);
    _hist.event->h_nhits->Draw();
  
//-----------------------------------------------------------------------------
// canvas #1 : first station
//-----------------------------------------------------------------------------
    _canvas[1]->cd(1);
    _hist.station[0]->h_occup[0]->Draw();
    _canvas[1]->cd(2);
    _hist.station[0]->h_occup[1]->Draw();
    // _canvas[1]->cd(3);
    // _hist.event[0]->n_cid_errors->Draw();
    // _canvas[1]->cd(4);
    // _hist.event[0]->n_nch_errors->Draw();

//-----------------------------------------------------------------------------
// canvas #2 : first plane edep:straw
//-----------------------------------------------------------------------------
    _canvas[2]->cd(1);
    _hist.station[0]->panel[0]->h_occup[0]->Draw();
    _canvas[2]->cd(2);
    _hist.station[0]->panel[0]->h_occup[0]->Draw();
    // _canvas[1]->cd(3);
    // _hist.event[0]->n_cid_errors->Draw();
    // _canvas[1]->cd(4);
    // _hist.event[0]->n_nch_errors->Draw();
  }
  TLOG(TLVL_INFO) << std::format("-- END:");
}

//-----------------------------------------------------------------------------
// in the end of the run need to save histograms - will figure that out later
//-----------------------------------------------------------------------------
void mu2e::TrkDqmNew::endRun(const art::Run& aRun) {
  //  int rn  = aRun.run();

  _initialized = 0;
}


// //-----------------------------------------------------------------------------
// void TrkDqmNew::fill_channel_histograms(ChannelHist_t* Hist, ChannelData_t* Data) {
//   Hist->nhits->Fill(Data->nhits());
// }

// //-----------------------------------------------------------------------------
// void TrkDqmNew::fill_dtc_histograms(DtcHist_t* Hist, StationData_t* Sd, int IDtc) {
// }
    
//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::fill_panel_histograms(PanelHist_t* Hist, EventData_t* Ed) {

  TLOG(TLVL_DEBUG+1) << std::format("-- START:");
  int straw = Ed->sh->strawId().straw();
  
  Hist->h_occup[0]->Fill(straw+1);
  if (Ed->sh->energyDep() > 0.0005) {
    Hist->h_occup[1]->Fill(straw+1);
  }
  
  //  Hist->h_edep_vs_straw->Fill(straw+1,Ed->sh->energyDep());
  
  TLOG(TLVL_DEBUG+1) << std::format("-- END:");
  return 0;
}
   
//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::fill_station_histograms(StationHist_t* Hist, EventData_t* Ed) {

  TLOG(TLVL_DEBUG+1) << std::format("-- START:");

  int straw   = Ed->sd->strawId().straw();
  int panel   = Ed->sd->strawId().panel();
  int plane   = Ed->sd->strawId().plane();
  //  int station = Ed->sd->strawId().station();

  int pnl12   = (plane % 2)*6 + panel;
  int bin     = pnl12*96+straw+1;
  
  Hist->h_occup[0]->Fill(bin); // per station occupancy
  if (Ed->sh->energyDep() > 0.0005) {
    Hist->h_occup[1]->Fill(bin); // per station occupancy
  }

  TLOG(TLVL_DEBUG+1) << std::format("-- END:");
  return 0;
  
}
   
//-----------------------------------------------------------------------------
// fill_roc_histograms also fills the channel histograms
// if in error, only histogram the error code
//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::fill_histograms(Hist_t* Hist, EventData_t* Ed) {
  TLOG(TLVL_DEBUG+1) << std::format("-- START : _interactiveMode:{}",_interactiveMode);

  for (int i=0; i<_nstrawdigis; i++) {
    const mu2e::StrawDigi* sd    = &_sdc->at(i);
    const mu2e::StrawHit*  sh    = &_shc->at(i);
    
    int pln   = sd->strawId().plane();
    int pnl   = sd->strawId().panel();
    //    int straw = sd->strawId().straw();
    int stn   = (pln / 2);
    int pnl12 = (pln % 2)*6+pnl;

    Ed->sd = sd;
    Ed->sh = sh;
    
    //    int bin = pnl12*96+straw+1;

    StationHist_t* stn_hist = Hist->station[stn];
    fill_station_histograms(stn_hist,Ed);

    PanelHist_t* pnl_hist = stn_hist->panel[pnl12];
    fill_panel_histograms(pnl_hist,Ed);
  }

  if (_interactiveMode != 0) {
//-----------------------------------------------------------------------------
// update predefined plots
//-----------------------------------------------------------------------------
    for (int i=0; i<3; i++) {
      _canvas[i]->Modified();
      _canvas[i]->Update();
    }
  }
  TLOG(TLVL_DEBUG+1) << std::format("--END:");
  return 0;
}

//-----------------------------------------------------------------------------
int mu2e::TrkDqmNew::get_data(const art::Event& ArtEvent) {
//-----------------------------------------------------------------------------
// tracker
//-----------------------------------------------------------------------------
  art::Handle<mu2e::StrawDigiCollection>            sdch;
  art::Handle<mu2e::StrawDigiADCWaveformCollection> sdawfch;
  art::Handle<mu2e::StrawHitCollection>             shch;
  //  art::Handle<mu2e::ComboHitCollection>             chch;

  _nstrawdigis   = 0;
  _nstrawhits    = 0;
  //  _ncombohits    = 0;
  //  _ntimeclusters = 0;
  //  _ntracks       = 0;
  // _ncalodigis  = 0;
  // _ncrvdigis   = 0;
  // _nstmdigis   = 0;

  _sdc         = nullptr;
  //  _sdawfc      = nullptr;
  _shc         = nullptr;
  //  _chc         = nullptr;

  bool ok;

  ok = ArtEvent.getByLabel(_sdCollTag,sdch);
  if (ok) { 
    _sdc         = sdch.product();
    _nstrawdigis = _sdc->size();
  }
  else {
    print_(e_ERROR,std::format("ERROR: StrawDigiCollection:{:s} is not available. Bail out\n",
                               _sdCollTag.encode().data()));
    return -1;
  }

  // ok =  ArtEvent.getByLabel(_sdCollTag,sdawfch);
  // if (ok) { 
  //   _sdawfc = sdawfch.product();
  // }
  // else {
  //   print_(e_WARNING,std::format("WARNING: StrawDigiADCWaveformCollection:{:s} is not available. Bail out\n",
  //                                _sdCollTag.encode().data()));
  //   return -1;
  // }

  ok = ArtEvent.getByLabel(_shCollTag,shch);
  if (ok) { 
    _shc         = shch.product();
    _nstrawhits = _shc->size();
  }
  else {
    print_(e_ERROR,std::format("ERROR: StrawHitCollection:{:s} is not available. Bail out\n",
                               _shCollTag.encode().data()));
    return -1;
  }
  return 0;
}

//--------------------------------------------------------------------------------
// assume that we only have tracker fragment(s)
//-----------------------------------------------------------------------------
void mu2e::TrkDqmNew::analyze(const art::Event& ArtEvent) {
  TLOG(TLVL_DEBUG+1) << Form(" -- START\n");
  
  _evtdata.artevt = &ArtEvent;
  
  get_data(ArtEvent);
  
  fill_histograms(&_hist,&_evtdata);
  
  gSystem->ProcessEvents();

  TLOG(TLVL_DEBUG+1) << Form(" -- END\n");
}



DEFINE_ART_MODULE(mu2e::TrkDqmNew)
