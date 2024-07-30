
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;
using namespace std;

#include "TThread.h"
#include "TStopwatch.h"

#include "tracemf.h"

#define TRACE_NAME "DtcGui_threads"

using namespace CFOLib;

//-----------------------------------------------------------------------------
// because of the DTC initializations, to be started earlier than the CFO thread
// take all initializations out
//-----------------------------------------------------------------------------
void* DtcGui::ReaderThread(void* Context) {
  // int rc(0);

  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_dtc_blocks;
  
  int              match_ts  (0);
  ulong            offset    (0);       // used in validation mode
  int              nerr_tot  (0);
  int              nerr_roc[6], nerr_roc_tot[6];
  ulong            nbytes_tot(0);

  DtcGui*          dtc_gui = (DtcGui*) Context;
  ThreadContext_t* tc      = &dtc_gui->fReaderTC;
  ulong            tstamp  = dtc_gui->fFirstTS->GetIntNumber();

  DtcInterface*    dtc_i   = tc->fDtel->fDTC_i;
  DTC*             dtc     = dtc_i->Dtc();

  int              print_freq = dtc_gui->fPrintFreq->GetIntNumber();
  
  TLOG(TLVL_INFO) << "START print_freq:" << print_freq << std::endl;
//-----------------------------------------------------------------------------
// reset connected ROC's - event tag issue
//-----------------------------------------------------------------------------
  ulong ew_tag        =  dtc_gui->fFirstTS->GetNumberEntry()->GetIntNumber();

  if (tc->fPrintLevel > 0) {
    // print header
    cout << Form("   CPU T   Real T      event  DTC    EW Tag   nbytes   nbytes_tot  ------------- ROC status ----------------  nerr nerr_tot\n");
    cout << Form("-------------------------------------------------------------------------------------------------------------------------\n");
  }

  for (int ir=0; ir<6; ir++) {
    nerr_roc_tot[ir] = 0;
  }
  
  TStopwatch timer;
  timer.Start();

  while (tc->fStop == 0) {
    if (tc->fPause != 0) {
      TLOG(TLVL_INFO+1) << "thread alive, sleep for " << tc->fSleepTimeMs << "ms" << std::endl;
      gSystem->Sleep(tc->fSleepTimeMs);
                                                            continue;
    }

    if (tc->fPrintLevel > 100) printf(" emoe : %10lu\n",tstamp);

    DTC_EventWindowTag event_tag = DTC_EventWindowTag(tstamp);

    int sz(-1);
    try {
//-----------------------------------------------------------------------------
// read events until done; tstamp is incremented only upon a successful read
//-----------------------------------------------------------------------------
      while (1) {
        list_of_dtc_blocks = dtc->GetSubEventData(event_tag, match_ts);
        sz = list_of_dtc_blocks.size();
        
        for (int i=0; i<sz; i++) {
          DTC_SubEvent* dtc_block = list_of_dtc_blocks[i].get();
          int      nbytes         = dtc_block->GetSubEventByteCount();
          ew_tag                  = dtc_block->GetEventWindowTag().GetEventWindowTag(true);
          char*    data           = (char*) dtc_block->GetRawBufferPointer();
          nbytes_tot             += nbytes;
          
          int nerr(0);
          
          if (dtc_gui->fValidate) {
            nerr      = dtc_i->ValidateDtcBlock((ushort*)data,ew_tag,&offset,tc->fPrintLevel,nerr_roc);
            nerr_tot += nerr;
            for (int ir=0; ir<6; ir++) {
              nerr_roc_tot[ir] += nerr_roc[ir];
            }
          }
          
          char* roc_data  = data+0x30;

          ushort rs[6];
          for (int roc=0; roc<6; roc++) {
            int nb    = *((ushort*) roc_data);
            rs[roc]   = *((ushort*)(roc_data+0x0c));
            roc_data += nb;
          }

          if (tstamp % print_freq == 0) {
            if (tc->fPrintLevel > 1) {
              float ct = timer.CpuTime();
              float rt = timer.RealTime();
              timer.Continue();
              cout << Form("%8.2f %8.2f %10lu  %1i  %10lu %6i %13li",ct,rt,tstamp,i,ew_tag,nbytes,nbytes_tot)
                   << Form(" 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x",rs[0],rs[1],rs[2],rs[3],rs[4],rs[5])
                   << Form(" %3i %5i\n",nerr,nerr_tot);
              if (tc->fPrintLevel > 10) {
                dtc_i->PrintBuffer(dtc_block->GetRawBufferPointer(),dtc_block->GetSubEventByteCount()/2);
              }
            }
          }
          else if (dtc_gui->fValidate > 0) {
            if ((nerr > 0) or (tc->fPrintLevel > 1)) {
              float ct = timer.CpuTime();
              float rt = timer.RealTime();
              timer.Continue();
              cout << Form("%8.2f %8.2f %10lu  %1i  %10lu %6i %13li",ct,rt,tstamp,i,ew_tag,nbytes,nbytes_tot)
                   << Form(" 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x",rs[0],rs[1],rs[2],rs[3],rs[4],rs[5])
                   << Form(" %3i %5i\n",nerr,nerr_tot);
            }
          }
        }

        if (sz == 0) {
//-----------------------------------------------------------------------------
// sz === NDTCs = 0: zero length event - end-of-read- print only if PrintLevel > 10
// everything is read in, safe to release all buffers
//-----------------------------------------------------------------------------
          dtc_i->Dtc()->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
          
          if (tc->fPrintLevel > 10) {
            cout << Form(">>>> ------- tstamp = %10lu event_tg:%10lu NDTCs:%2i\n",tstamp,event_tag.GetEventWindowTag(true),sz);
          }
          break;
        }
//-----------------------------------------------------------------------------
// increment time stamp only in the very end of the loop
//-----------------------------------------------------------------------------
        tstamp++;
      }
    }
    catch (...) {
      TLOG(TLVL_ERROR) << "reading tstamp = " << tstamp << std::endl;
      break;
    }
  }

  timer.Stop();
//-----------------------------------------------------------------------------
// print summary
//-----------------------------------------------------------------------------  
  if (tc->fPrintLevel > 0) {
    ulong nev = tstamp-dtc_gui->fFirstTS->GetIntNumber();
    cout << Form("nevents: %10li nbytes_tot: %12li\n",nev, nbytes_tot);
    cout << Form("nerr_tot: %8i nerr_roc_tot: %8i %8i %8i %8i %8i %8i\n",
                 nerr_tot,
                 nerr_roc_tot[0],nerr_roc_tot[1],nerr_roc_tot[2],
                 nerr_roc_tot[3],nerr_roc_tot[4],nerr_roc_tot[5]);
  }
  
  TLOG(TLVL_INFO) << "END , nerr_tot: " << nerr_tot << std::endl;
  return nullptr;
}

//-----------------------------------------------------------------------------
void* DtcGui::EmuCfoThread(void* Context) {
  //  int rc(0);

  DtcGui*          dtc_gui = (DtcGui*) Context;
  ThreadContext_t* tc      = &dtc_gui->fEmuCfoTC;
  DtcInterface*    dtc_i   = tc->fDtel->fDTC_i;

  if (dtc_i->EmulateCfo() == 0) {
    TLOG(TLVL_ERROR) << "DtcInterface::fEmulateCfo = 0, cant run in emulated mode" << std::endl;
    return nullptr;
  }

  int   ew_length          = dtc_gui->fEWLength->GetIntNumber();
  ulong nevents            = dtc_gui->fNEvents->GetIntNumber();
  ulong first_ts           = dtc_gui->fFirstTS->GetIntNumber();
  int   sleep_us           = dtc_gui->fSleepUS->GetIntNumber();

  TLOG(TLVL_DEBUG) << Form("%s: START ew_length: %5i nevents: %10lu first_tx:%10lu sleep_us:%8i\n",
                          __func__,ew_length,nevents,first_ts,sleep_us);

  int t0 = first_ts/dtc_gui->fCfoPrintFreq;

  //  dtc_i->InitReadout();
  
  while (tc->fStop == 0) {
    // gSystem->Sleep(sleep_us);
    usleep(sleep_us);
    
    dtc_i->LaunchRunPlanEmulatedCfo(ew_length,nevents+1,first_ts);
    
    first_ts = first_ts+nevents;
    int t1 = first_ts/dtc_gui->fCfoPrintFreq;
    if (t1 > t0) {
      TLOG(TLVL_DEBUG) << Form("first_ts: %10lu",first_ts);
      t0 = t1;
    }
  }

  TLOG(TLVL_DEBUG) << "END" << std::endl;
  return nullptr;
}

//-----------------------------------------------------------------------------
// assume that the jitter attenuator is configured
//-----------------------------------------------------------------------------
void* DtcGui::ExtCfoThread(void* Context) {

  DtcGui*          dtc_gui = (DtcGui*) Context;
                                        // should point to the CFO tab element
  DtcTabElement_t* dtel    = dtc_gui->fDtcTel+dtc_gui->fActiveDtcID;
         
  ThreadContext_t* tc      = &dtc_gui->fExtCfoTC;
  CfoInterface*    cfo_i   = dtc_gui->fCFO_i;
  CFOLib::CFO*     cfo     = cfo_i->Cfo();
//-----------------------------------------------------------------------------
// for now, assume that the run plans are stored in ~/test_stand/cfo_run_plans directory
//-----------------------------------------------------------------------------
  std::string  run_plan = Form("~/test_stand/cfo_run_plans/%s",dtel->fRunPlan->GetText());
  int          dtc_mask;
  sscanf(dtel->fDtcMask->GetText(),"0x%x",&dtc_mask);

  TLOG(TLVL_INFO) << Form("START : InitReadout run_plan: %s DTC mask = 0x%08x\n",run_plan.data(),dtc_mask);

  //  cfo_i->InitReadout(run_plan.data(),dtc_mask);
//-----------------------------------------------------------------------------
// execute the run plan once, assume it can be infinite
//-----------------------------------------------------------------------------
  TLOG(TLVL_INFO) << "START executing run plan " << run_plan << std::endl;
  // cfo->EnableBeamOffMode (CFO_Link_ID::CFO_Link_ALL);
  cfo_i->LaunchRunPlan();
//-----------------------------------------------------------------------------
// execute the run plan once and wait after that
//-----------------------------------------------------------------------------
  while (tc->fStop == 0) {
    TLOG(TLVL_INFO+1) << Form("thread alive\n");
    gSystem->Sleep(tc->fSleepTimeMs);
  }
  
  cfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  TLOG(TLVL_INFO) << "END" << std::endl;
  return nullptr;
}

//-----------------------------------------------------------------------------
int DtcGui::manage_emu_cfo_thread() {
  int rc(1);
  
  TLOG(TLVL_DEBUG) << Form("START: fEmuCfoTC.fRunning = %i\n",fEmuCfoTC.fRunning);

  if (fEmuCfoTC.fRunning == 0) {
//-----------------------------------------------------------------------------
// start thread
//-----------------------------------------------------------------------------
    if (fEmuCfoTC.fTp != nullptr) {
      printf("%s: delete previous thread\n",__func__);
      fEmuCfoTC.fTp->Kill() ;
      delete fEmuCfoTC.fTp ;
    }

    fEmuCfoTC.fDtel    = fDtcTel+fActiveDtcID;
    fEmuCfoTC.fRunning = 1;
    fEmuCfoTC.fStop    = 0;
    fEmuCfoTC.fCmd     = 0;
    fEmuCfoTC.fTp      = new TThread("emu_cfo_thread",DtcGui::EmuCfoThread,this);
    fEmuCfoTC.fTp->Run();
    rc = 0;
    
    TThread::Lock();
    TGButton* btn = (TGButton*) gTQSender;
    btn->ChangeBackground(fRunningColor);
    TThread::UnLock();
  }
  else {
//-----------------------------------------------------------------------------
// stop thread
//-----------------------------------------------------------------------------
    fEmuCfoTC.fStop    = 1;
    fEmuCfoTC.fRunning = 0;
    TThread::Lock();
    TGButton* btn = (TGButton*) gTQSender;
    btn->ChangeBackground(fValidatedColor);
    TThread::UnLock();
  }
  
  TLOG(TLVL_DEBUG) << Form("END: fEmuCfoTC.fRunning = %i\n",fEmuCfoTC.fRunning);
  return rc;
}


//-----------------------------------------------------------------------------
int DtcGui::manage_ext_cfo_thread() {
  int rc(1);
  
  TLOG(TLVL_DEBUG) << Form("START: fExtCfoTC.fRunning = %i\n",fExtCfoTC.fRunning);

  if (fExtCfoTC.fRunning == 0) {
//-----------------------------------------------------------------------------
// start thread
//-----------------------------------------------------------------------------
    if (fExtCfoTC.fTp != nullptr) {
      printf("%s: delete previous thread\n",__func__);
      fExtCfoTC.fTp->Kill() ;
      delete fExtCfoTC.fTp ;
    }

    fExtCfoTC.fDtel    = fDtcTel+fActiveDtcID;
    fExtCfoTC.fRunning = 1;
    fExtCfoTC.fStop    = 0;
    fExtCfoTC.fCmd     = 0;
    fExtCfoTC.fTp      = new TThread("ext_cfo_thread",DtcGui::ExtCfoThread,this);
    fExtCfoTC.fTp->Run();
    rc = 0;
    TThread::Lock();
    TGButton* btn = (TGButton*) gTQSender;
    btn->ChangeBackground(fRunningColor);
    TThread::UnLock();
  }
  else {
//-----------------------------------------------------------------------------
// stop thread
//-----------------------------------------------------------------------------
    fExtCfoTC.fStop  = 1;
    fExtCfoTC.fRunning = 0;
    TThread::Lock();
    TGButton* btn = (TGButton*) gTQSender;
    btn->ChangeBackground(fValidatedColor);
    btn->SetName("Stop ExtCFO");
    TThread::UnLock();
  }
  
  TLOG(TLVL_DEBUG) <<  Form("EN: fExtCfoTC.fRunning = %i\n",fExtCfoTC.fRunning);
  return rc;
}


//-----------------------------------------------------------------------------
int DtcGui::manage_reader_thread() {
  int rc(1);

  TLOG(TLVL_DEBUG) <<  Form("START fReaderTC.fRunning=%i\n",fReaderTC.fRunning);
  
  if (fReaderTC.fRunning == 0) {
//-----------------------------------------------------------------------------
// start thread
//-----------------------------------------------------------------------------
    if (fReaderTC.fTp != nullptr) {
      printf("%s: delete previous thread\n",__func__);
      fReaderTC.fTp->Kill() ;
      delete fReaderTC.fTp;
    }
  
    fReaderTC.fDtel    = fDtcTel+fActiveDtcID;
    fReaderTC.fRunning = 1;
    fReaderTC.fStop    = 0;
    fReaderTC.fCmd     = 0;
    fReaderTC.fTp      = new TThread("reader_thread",DtcGui::ReaderThread,this);
    fReaderTC.fTp->Run();
    rc = 0;
    TThread::Lock();
    TGButton* btn = (TGButton*) gTQSender;
    btn->ChangeBackground(fRunningColor);
    TThread::UnLock();
  }
  else {
//-----------------------------------------------------------------------------
// stop thread
//-----------------------------------------------------------------------------
    fReaderTC.fStop    = 1;
    fReaderTC.fRunning = 0;
    TThread::Lock();
    TGButton* btn = (TGButton*) gTQSender;
    btn->ChangeBackground(fValidatedColor);
    TThread::UnLock();
  }

  TLOG(TLVL_DEBUG) << Form("END fReaderTC.fRunning=%i\n",fReaderTC.fRunning);
  return rc;
}
