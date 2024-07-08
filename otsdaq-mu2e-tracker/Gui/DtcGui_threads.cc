
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;
using namespace std;

#include "TThread.h"

#include "tracemf.h"

#define TRACE_NAME "DtcGui_threads"

//-----------------------------------------------------------------------------
void* DtcGui::ReaderThread(void* Context) {
  // int rc(0);

  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_dtc_blocks;
  
  int              match_ts(0);
  ulong            offset  (0);      // used in validation mode
  int              nerr_tot(0);

  DtcGui*          dtc_gui = (DtcGui*) Context;
  ThreadContext_t* tc      = &dtc_gui->fReaderTC;
  ulong            tstamp  = dtc_gui->fFirstTS->GetIntNumber();

  DtcInterface*    dtc_i = tc->fDtel->fDTC_i;
  DTC*             dtc   = dtc_i->Dtc();

  int              print_freq = dtc_gui->fPrintFreq->GetIntNumber();
  
  TLOG(TLVL_INFO) << "START print_freq:" << print_freq << std::endl;
//-----------------------------------------------------------------------------
// reset connected ROC's - event tag issue
//-----------------------------------------------------------------------------
  ulong ew_tag        =  dtc_gui->fFirstTS->GetNumberEntry()->GetIntNumber();

  while (tc->fStop == 0) {

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
          int      nb             = dtc_block->GetSubEventByteCount();
          ew_tag                  = dtc_block->GetEventWindowTag().GetEventWindowTag(true);
          char*    data           = (char*) dtc_block->GetRawBufferPointer();
          
          int nerr(0);
          
          if (dtc_gui->fValidate) {
            nerr = dtc_i->ValidateDtcBlock((ushort*)data,ew_tag,&offset,tc->fPrintLevel);
          }
          nerr_tot += nerr;
          
          char* roc_data  = data+0x30;

          ushort rs[6];
          for (int roc=0; roc<6; roc++) {
            int nb    = *((ushort*) roc_data);
            rs[roc]   = *((ushort*)(roc_data+0x0c));
            roc_data += nb;
          }

          if (tstamp % print_freq == 0) {
              cout << Form(">>> ts:%10lu DTC:%2i EWTag:%10lu nbytes: %4i ROC status: 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x",
                           tstamp,i,ew_tag,nb,rs[0],rs[1],rs[2],rs[3],rs[4],rs[5])
                   << Form(" nerr:%3i nerr_tot:%5i\n",nerr,nerr_tot);
          }
          else if (dtc_gui->fValidate > 0) {
            if ((nerr > 0) or (tc->fPrintLevel > 1)) {
              cout << Form(">>> ts:%10lu DTC:%2i EWTag:%10lu nbytes: %4i ROC status: 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x",
                           tstamp,i,ew_tag,nb,rs[0],rs[1],rs[2],rs[3],rs[4],rs[5])
                   << Form(" nerr:%3i nerr_tot:%5i\n",nerr,nerr_tot);
            }
          }
          else if (tc->fPrintLevel > 1) {
            cout << Form(">>> ts:%10lu DTC:%2i EWTag:%10lu nbytes: %4i ROC status: 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x ",
                         tstamp,i,ew_tag,nb,rs[0],rs[1],rs[2],rs[3],rs[4],rs[5])
                 << Form(" nerr:%3i nerr_tot:%5i\n",nerr,nerr_tot);
          }
        }

        if (sz == 0) {
//-----------------------------------------------------------------------------
// sz === NDTCs = 0: zero length event - end-of-read- print only if PrintLevel > 10
//-----------------------------------------------------------------------------
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
                                        // do we really need this ?
  // TThread::Lock();
  // dtc_gui->fFirstTS->SetNumber(ew_tag);
  // TThread::UnLock();

  TLOG(TLVL_INFO) << "END , nerr_tot: " << nerr_tot << std::endl;
  return nullptr;
}

//-----------------------------------------------------------------------------
void* DtcGui::EmuCfoThread(void* Context) {
  //  int rc(0);

  DtcGui*          dtc_gui = (DtcGui*) Context;
  ThreadContext_t* tc      = &dtc_gui->fEmuCfoTC;
  DtcInterface*    dtc_i   = tc->fDtel->fDTC_i;

  int   ew_length          = dtc_gui->fEWLength->GetIntNumber();
  ulong nevents            = dtc_gui->fNEvents->GetIntNumber();
  ulong first_ts           = dtc_gui->fFirstTS->GetIntNumber();
  int   sleep_ms           = dtc_gui->fSleepMS->GetIntNumber();
  
  printf("%s: START ew_length: %5i nevents: %10lu first_tx:%10lu sleep_ms:%5i\n",__func__,ew_length,nevents,first_ts,sleep_ms);

  while (tc->fStop == 0) {
    gSystem->Sleep(sleep_ms);
    
    dtc_i->InitEmulatedCFOReadoutMode(ew_length,nevents+1,first_ts);
    first_ts = first_ts+nevents;
    printf("%s: first_ts: %10lu\n",__func__,first_ts);
  }

  printf("%s: END\n",__func__);
  return nullptr;
}

//-----------------------------------------------------------------------------
void* DtcGui::ExtCfoThread(void* Context) {
  // int rc(0);

  DtcGui*          dtc_gui = (DtcGui*) Context;
  ThreadContext_t* tc      = &dtc_gui->fExtCfoTC;
  // DtcInterface*    dtc_i   = tc->fDtel->fDTC_i;

  while (tc->fStop == 0) {
    gSystem->Sleep(200);
    
    if (tc->fCmd) {
      
      TThread::Lock();
      // TGButton* btn = (TGButton*) gTQSender;
      // btn->ChangeBackground(dtc->fSubmittedColor);
      TThread::UnLock();

      // if (tc->fCmd == kSTART_NEW_RUN) {
      //   rc = dtc->run();
      // }
//-----------------------------------------------------------------------------
// mark command as executed
// in case of failure, leave fCommand != 0
//-----------------------------------------------------------------------------
      TThread::Lock();
      // dtc->fCommand = 0;
      TThread::UnLock();
    }
  }
  return nullptr;
}

//-----------------------------------------------------------------------------
int DtcGui::manage_emu_cfo_thread() {
  int rc(1);
  
  printf("%s: START fRunning:%i\n",__func__,fEmuCfoTC.fRunning);

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
    btn->ChangeBackground(fStoppedColor);
    btn->SetName("Stop EmuCFO");
    TThread::UnLock();
  }
  
  printf("%s: END fRunning:%i\n",__func__,fEmuCfoTC.fRunning);
  return rc;
}


//-----------------------------------------------------------------------------
int DtcGui::manage_ext_cfo_thread() {
  int rc(1);
  
  printf("%s: START fRunning:%i\n",__func__,fExtCfoTC.fRunning);

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
    fExtCfoTC.fTp      = new TThread("cfo_thread",DtcGui::ExtCfoThread,this);
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
    btn->ChangeBackground(fStoppedColor);
    TThread::UnLock();
  }
  
  printf("%s: END fRunning:%i\n",__func__,fExtCfoTC.fRunning);
  return rc;
}


//-----------------------------------------------------------------------------
int DtcGui::manage_read_thread() {
  int rc(1);

  printf("%s: START\n",__func__);
  
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
    btn->ChangeBackground(fStoppedColor);
    TThread::UnLock();
  }

  printf("%s: END\n",__func__);
  return rc;
}
