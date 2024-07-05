//
#define __CLING__ 1

#include "iostream"

#include "TH1.h"
#include "TStopwatch.h"

#include "dtcInterfaceLib/DTC.h"
#include "cfoInterfaceLib/CFO.h"
#include "cfoInterfaceLib/CFO_Compiler.hh"

using namespace CFOLib;
using namespace DTCLib;

#include "print_buffer.C"

#include "otsdaq-mu2e-tracker/Ui/CfoInterface.hh"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;

// DTC control register : 0x9100 
enum { 
  kCFOEmulationEnableBit = 30,
  kAutogenDRPBit         = 23,
};

namespace daq_scripts {
  int EWLength    = 68;                 // in units of 25 ns, = 1700 ns
  int EdgeMode    = 0x0;                // two bits
  int CFOLinkMask = 0x1;
};

//-----------------------------------------------------------------------------
void cfo_measure_delay(int PcieAddress, CFO_Link_ID xLink) {

  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress); 
  if (cfo_i == nullptr) return;

  // comment out , for now 
  // CFO* cfo = cfo_i->Cfo();

	// cfo->ResetDelayRegister();	                 // reset 0x9380
	// cfo->DisableLinks();	                       // reset 0x9114
	//                                              // configure the DTC (to configure the ROC in a loop)
	// cfo->EnableLink(xLink, DTC_LinkEnableMode(true, true)); // enable Tx and Rx
	// cfo->EnableDelayMeasureMode(xLink);
	// cfo->EnableDelayMeasureNow(xLink);

	// uint32_t delay = cfo->ReadCableDelayValue(xLink);	// read delay

	// cout << "Delay measured: " << delay << " (ns) on link: " <<  xLink << std::endl;

	// // reset registers
	// cfo->ResetDelayRegister();
	// cfo->DisableLinks();

  // 	printf(" delay = %ui\n",delay);

}

//-----------------------------------------------------------------------------
// ROOT CLI
//-----------------------------------------------------------------------------
void cfo_soft_reset(int PcieAddress = -1) {
  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress); 
  cfo_i->Cfo()->SoftReset();
}

//-----------------------------------------------------------------------------
void cfo_compile_run_plan(const char* InputFn, const char* OutputFn) {
  CFOLib::CFO_Compiler compiler;

  std::string fn1(InputFn );
  std::string fn2(OutputFn);

  compiler.processFile(fn1,fn2);
}

//-----------------------------------------------------------------------------
// assume one timing chain
//-----------------------------------------------------------------------------
void cfo_init_readout_ext(const char* RunPlan, int NDtcs) {
  int ndtcs[8] = {NDtcs,0,0,0,0,0,0,0};
  
  CfoInterface* cfo_i = CfoInterface::Instance();  // assume already initialized
  cfo_i->InitReadout(RunPlan,ndtcs);
}

//-----------------------------------------------------------------------------
void cfo_launch_run_plan(int PcieAddress = -1) {
  CfoInterface::Instance(PcieAddress)->LaunchRunPlan(); 
//-----------------------------------------------------------------------------
// this is what it really is
//-----------------------------------------------------------------------------
  // cfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
  // cfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  // cfo->SoftReset();

	// usleep(10);	
	// cfo->EnableBeamOffMode (CFO_Link_ID::CFO_Link_ALL);
}

//-----------------------------------------------------------------------------
// first 8 bytes contain nbytes, but written into the CFO are 0x10000 bytes
// (the sizeof(mu2e_databuff_t) 0
//-----------------------------------------------------------------------------
void cfo_set_run_plan(const char* Fn = "commands.bin", int PcieAddress = -1) {

  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress);
  cfo_i->SetRunPlan(Fn);
}

//-----------------------------------------------------------------------------
// doesn't seem to be needed - launch does this anyway
//-----------------------------------------------------------------------------
void cfo_reset_run_plan(int PcieAddress = -1) {
  CFO* cfo = CfoInterface::Instance(PcieAddress)->Cfo(); 

  cfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
  cfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  cfo->SoftReset();
}

//-----------------------------------------------------------------------------
// for convenience (CLI)
//-----------------------------------------------------------------------------
int cfo_configure_ja(int Clock, int Reset, int PcieAddress = -1) {
  return CfoInterface::Instance(PcieAddress)->ConfigureJA(Clock,Reset);
}

//-----------------------------------------------------------------------------
int dtc_configure_ja(int Clock, int Reset, int PcieAddress = -1) {
  return DtcInterface::Instance(PcieAddress)->ConfigureJA(Clock,Reset);
}

//-----------------------------------------------------------------------------
// EW length         : in units of 25 ns (clock)
// EWMOde            : 1 for buffer test
// EnableClockMarkers: set to 0
// EnableAutogenDRP  : set to 1
//-----------------------------------------------------------------------------
void dtc_init_emulated_cfo_mode(DTC* dtc, int EWLength, int NMarkers, int FirstEWTag) { 
  //                                 int EWMode, int EnableClockMarkers, int EnableAutogenDRP) {

  

  int EWMode             = 1;
  int EnableClockMarkers = 0;
  int EnableAutogenDRP   = 1;

	dtc->DisableCFOEmulation();
	dtc->DisableAutogenDRP();

  dtc->SoftReset();                     // write bit 31

  dtc->SetCFOEmulationEventWindowInterval(EWLength);  
  dtc->SetCFOEmulationNumHeartbeats      (NMarkers);
  dtc->SetCFOEmulationTimestamp          (DTC_EventWindowTag(FirstEWTag));
  dtc->SetCFOEmulationEventMode          (EWMode);
  dtc->SetCFO40MHzClockMarkerEnable      (DTC_Link_ALL,EnableClockMarkers);

  dtc->EnableAutogenDRP();                                      // r_0x9100:bit_23 = 1
  dtc->SetCFOEmulationMode();                                   // r_0x9100:bit_15 = 1 
  dtc->EnableCFOEmulation();                                    // r_0x9100:bit_30 = 1 

  dtc->EnableReceiveCFOLink();                                  // r_0x9114:bit_14 = 1
}

//-----------------------------------------------------------------------------
// write value 0x10800244 to register 0x9100
// write value 0x00004141 to register 0x9114
// write value 0x10800244 to register 0x9100

// DTC doesn' know about an external CFO, so it should only prepare itself to receive 
// EVMs/HBs from the outside
//-----------------------------------------------------------------------------
void dtc_init_external_cfo_mode(DTC* dtc) { 
  //                                 int EWMode, int EnableClockMarkers, int EnableAutogenDRP) {

  // int EnableAutogenDRP   = 1;

  dtc->DisableCFOEmulation  ();
  dtc->DisableAutogenDRP();

  // dtc->HardReset();                        // write bit 0
  // dtc->SoftReset();                     // write bit 31

  //  int EWMode             = 1;
  // dtc->SetCFOEmulationEventMode          (EWMode);

  int EnableClockMarkers = 0;
  dtc->SetCFO40MHzClockMarkerEnable      (DTC_Link_0,EnableClockMarkers);

  dtc->SetExternalCFOSampleEdgeMode      (daq_scripts::EdgeMode);

  dtc->EnableAutogenDRP();

  dtc->ClearCFOEmulationMode();         // r_0x9100:bit_15 = 0
  // dtc->SetCFOEmulationMode();        // r_0x9100:bit_15 = 1

  // dtc->DisableCFOEmulation();
  // dtc->EnableCFOEmulation ();        // r_0x9100:bit_30 = 1 

  dtc->EnableReceiveCFOLink ();         // r_0x9114:bit_14 = 1
}

//-----------------------------------------------------------------------------
// to be executed on each node with the DTC
// 'dtc_init_link_mask' defines node-specific link mask
//-----------------------------------------------------------------------------
void dtc_init_external_cfo_readout_mode() {
  DtcInterface* dtc_i = DtcInterface::Instance(-1);

  dtc_i->Dtc()->SoftReset();
  dtc_i->InitExternalCFOReadoutMode(daq_scripts::EdgeMode);
  dtc_i->RocConfigurePatternMode(dtc_i->fLinkMask);            // DtcInterface knows its link mask

  //  dtc_i->fDtc->SetCFOEmulationMode();
}

//-----------------------------------------------------------------------------
// a read should always end with releasing  buffers
//-----------------------------------------------------------------------------
void dtc_read_subevents(DtcInterface* Dtc_i, 
                        std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& ListOfSubevents, 
                        uint64_t FirstTs, int PrintData) {

  uint64_t ts       = FirstTs;
  bool     match_ts = false;

  int nevents = 0;

  Dtc_i->ReadSubevents(ListOfSubevents,FirstTs,PrintData);

  int rs[6]; // roc status, for printout

  int nbtot = 0;
  for (int i=0; i<(int)ListOfSubevents.size(); i++) {
    //    DaqEvent_t evt;

    DTC_SubEvent* ev = ListOfSubevents[i].get();
    int nb = ev->GetSubEventByteCount();

    int nw = nb/2;

    nbtot += nb;
      
    uint64_t ew_tag = ev->GetEventWindowTag().GetEventWindowTag(true);

    char* data = (char*) ev->GetRawBufferPointer();

    char* roc_data = data+0x30;

    for (int roc=0; roc<6; roc++) {
      int nb    = *((ushort*) roc_data);
      rs[roc]   = *((ushort*)(roc_data+0x0c));
      roc_data += nb;
    }
        
    if (PrintData > 0) {
      cout << Form(" DTC:%2i EWTag:%10li nbytes: %4i ROC status: 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x ",
                   i,ew_tag,nb,rs[0],rs[1],rs[2],rs[3],rs[4],rs[5]);
    }

    if (PrintData > 1) {
      cout << std::endl;
      print_buffer(ev->GetRawBufferPointer(),ev->GetSubEventByteCount()/2);
    }
  }

  if (PrintData > 0) cout << std::endl;
}

//-----------------------------------------------------------------------------
int dtc_init_external_readout(int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);         // assume already initialized
  dtc_i->InitExternalCFOReadoutMode(daq_scripts::EdgeMode);
  dtc_i->RocConfigurePatternMode(dtc_i->fLinkMask);  // readout ROC patterns
  return 0;
}

//-----------------------------------------------------------------------------
// to be executed on each node with a DTC, after the CFO run plan was launched
//-----------------------------------------------------------------------------
int dtc_read_subevents(uint64_t FirstTS = 0, int PrintData = 1, int PcieAddr = -1, const char* OutputFn = nullptr) {
  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ReadSubevents(list_of_subevents,FirstTS,PrintData,OutputFn);
  
  return list_of_subevents.size();
}


struct RocData_t {
  ushort  nb;
  ushort  header;
  ushort  n_data_packets;  // n data packets, 16 bytes each
  ushort  ewt[3];
  ushort  status;
  ushort  xxx2;
  ushort  data; // array, use it just for memory mapping
};
  
//-----------------------------------------------------------------------------
// returns number of found errors in the payload data
//-----------------------------------------------------------------------------
int validate_dtc_block(ulong EwTag, ushort* Data, ulong* Offset, int PrintData) {

  int nhits[64] = {
    1,   2,  3,  0,  0,  0,  7,  8,
    9,  10, 11, 12, 13, 14, 15, 16,
    0,  20, 21, 22, 12, 13, 11, 12,
    0,   0,  8,  4, 12, 11, 12, 13,
    16,  6,  3,  1, 12,  0, 16, 17,
    18, 19, 12,  1, 12, 12, 11, 11,
     0,  0,  0,  0, 13, 14, 10, 13,
    11, 14, 14, 15,  8,  9, 10, 32
  };

//-----------------------------------------------------------------------------
// calculate offsets just once
//-----------------------------------------------------------------------------
  static int   initialized(0);
  
  if (initialized == 0) {
    //    calculate_offsets(nhits,offset);
    initialized = 1;
  }
//-----------------------------------------------------------------------------
// check consistency of the lengths
// 1. total number of 2-byte words
//
//-----------------------------------------------------------------------------
  int ewt    = EwTag % 64 ;
  int nb_dtc = *Data;

  RocData_t* roc = (RocData_t*) (Data+0x18);

  int nb_rocs = 0;
  for (int i=0; i<6; i++) {
    int nb   = roc->nb;
    nb_rocs += nb;
    roc      = (RocData_t*) ( ((char*) roc) + roc->nb);
  }

  int nerr     = 0;

  if (nb_dtc != nb_rocs+0x30) {
    if (PrintData > 0) printf("ERROR: nb_dtc, nb_rocs : 0x%04x 0x%04x\n",nb_dtc,nb_rocs);
    nerr += 1;
  }
//-----------------------------------------------------------------------------
// event length checks out, check ROC payload
// check the ROC payload, assume a hit = 2 packets
//-----------------------------------------------------------------------------
  roc = (RocData_t*) (Data+0x18);
  for (int i=0; i<6; i++) {
    int nb = roc->nb;
//-----------------------------------------------------------------------------
// validate ROC header
//-----------------------------------------------------------------------------
    // ... TODO
    ulong ewtag_roc = ulong(roc->ewt[0]) | (ulong(roc->ewt[1]) << 16) | (ulong(roc->ewt[2]) << 32);

    if (ewtag_roc != EwTag) {
      if (PrintData > 0) printf("ERROR: roc EwTag ewt_roc : %i 0x%08lx 0x%08lx\n",i,EwTag,ewtag_roc);
      nerr += 1;
    }
    
    if (roc->nb > 0x10) { 
//-----------------------------------------------------------------------------
// non-zero payload
//-----------------------------------------------------------------------------
      uint*   pattern  = (uint*) &roc->data;
      if (PrintData > 10) printf("data[0]  = nb = 0x%04x\n",pattern[0]);
 
      int npackets     = roc->n_data_packets;
      int npackets_exp = nhits[ewt]*2;       // assume two packets per hit (this number is stored somewhere)

      if (npackets != npackets_exp) {
        if (PrintData > 0) printf("ERROR: EwTag roc npackets npackets_exp: 0x%08lx %i %5i %5i\n",
                                  EwTag,i,npackets,npackets_exp);
        nerr += 1;
      }
      
      if (PrintData > 10) {
        printf("EwTag, ewt, npackets, npackets_exp,  offset: %10lu %3i %2i %2i %10lu\n",
               EwTag,  ewt, npackets, npackets_exp, *Offset);
      }

      uint nw      = npackets*4;        // N 4-byte words
    
      for (uint i=0; i<nw; i++) {
        uint exp_pattern = (i+*Offset) & 0xffffffff;
    
        if (pattern[i] != exp_pattern) {
          nerr += 1;
          if (PrintData > 0) {
            printf("ERROR: EwTag, ewt i  payload[i]  offset exp_word: %10lu %3i %3i 0x%08x 0x%08lx 0x%08x\n",
                   EwTag, ewt, i, pattern[i],*Offset,exp_pattern);
          }
        }
      }
    }
    roc = (RocData_t*) (((char*) roc) + roc->nb);
  }
  
  *Offset += 2*4*nhits[ewt];

  if (PrintData > 10) printf("EwTag = %10lx, nb_dtc = %i nerr = %i\n",EwTag,nb_dtc,nerr);

  return nerr;
}

//-----------------------------------------------------------------------------
// types of errors:
// 1. wrong event window marker
// 2. bit errors in the payload words
// 3. 
//-----------------------------------------------------------------------------
int dtc_read_and_validate(uint64_t NEvents, int PrintData = 1, uint64_t FirstTS = 0, int PcieAddr = -1) {

  ulong   n_read_events(0);
  int     nerrors      (0);
  bool    match        (false);
  ulong   offset       (0);
  
  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  DTC*          dtc   = dtc_i->Dtc();

  while(1) {
//-----------------------------------------------------------------------------
// read events
//-----------------------------------------------------------------------------
    ulong ts = FirstTS+n_read_events;
    DTC_EventWindowTag expected_tag = DTC_EventWindowTag(ts);
    list_of_subevents = dtc->GetSubEventData(expected_tag,match);
    
    int   ndtcs         = list_of_subevents.size();
    if (ndtcs == 0) break;
    
    int   nerr          = 0;

    if (PrintData > 10) printf(" --- ts, ndtcs : %10lu %i\n",ts,ndtcs);
    
    for (int idtc=0; idtc<ndtcs; idtc++) {
      DTC_SubEvent* dtc_block = list_of_subevents[idtc].get();
      int           nb        = dtc_block->GetSubEventByteCount();
      ulong         tag       = dtc_block->GetEventWindowTag().GetEventWindowTag(true);
      ushort*       data      = (ushort*) dtc_block->GetRawBufferPointer();

      if (PrintData > 10) dtc_i->PrintBuffer(data,nb/2);

      ulong exp_tag = expected_tag.GetEventWindowTag(true);
      
      if (tag != exp_tag) {
        if (PrintData > 0) printf("ERROR: mismatched event tag: read: %10li  expected: %10lu\n",tag,exp_tag);
        nerr += 1;
      }

      int rc(0);
      rc = validate_dtc_block(tag,data,&offset,PrintData);

      if ((rc > 0) and (PrintData > 0)) {
        dtc_i->PrintBuffer(data,nb/2);
      }

      nerr  += rc;

      if (PrintData > 10) printf("  tag: %li   rc : %i\n",tag,rc);
      
      if (PrintData > 0) {
        printf(">>>>> ---- n_read_events   tag idtc nerr: %10lu %10lu %3i %5i\n",n_read_events,tag,idtc,nerr);
      }
    }

    if (PrintData > 10) printf("  number of dtc blocks: ndtcs:  %i nerr = %i\n",ndtcs,nerr);
    nerrors       += nerr;
    n_read_events += 1;
//-----------------------------------------------------------------------------
// it looks that a delay is needed, start from one millisecond
//-----------------------------------------------------------------------------
    // std::this_thread::sleep_for(std::chrono::microseconds(10000));
  }

  printf("[%s]: n_read_events nerrors :  %10lu %10i\n",__func__,n_read_events,nerrors);
  return nerrors;
}

//-----------------------------------------------------------------------------
void dtc_val_test_emulated_cfo(int NEvents=3, int PrintData = 1, uint64_t FirstTS=0, int PcieAddr=-1) {
  TStopwatch timer;
  timer.Start();
  
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr); // assume already initialized
  dtc_i->RocConfigurePatternMode(dtc_i->fLinkMask);
                                        // 68x25ns = 1700 ns
  
  dtc_i->InitEmulatedCFOReadoutMode(68,NEvents+1,FirstTS);

  dtc_read_and_validate(NEvents,PrintData,FirstTS,PcieAddr);

  timer.Stop();
  printf(" timing: RT=%7.3f s, Cpu=%7.3f s\n",timer.RealTime(),timer.CpuTime());
  
}

//-----------------------------------------------------------------------------
void dtc_buffer_test_emulated_cfo(int NEvents=3, int PrintData = 1, uint64_t FirstTS=0, const char* OutputFn = nullptr) {
  int pcie_addr(-1);                                 // assume initialized
  
  DtcInterface* dtc_i = DtcInterface::Instance(-1);  // assume already initialized
  dtc_i->RocConfigurePatternMode(dtc_i->fLinkMask);  // readout ROC patterns

                                                     // 68x25ns = 1700 ns

                                                     // this call actually sends EWMs
  
  dtc_i->InitEmulatedCFOReadoutMode(daq_scripts::EWLength,NEvents+1,0);

                                                    // in emulated mode, always read after 

  dtc_read_subevents(FirstTS,PrintData,pcie_addr,OutputFn);
}

//-----------------------------------------------------------------------------
void dtc_buffer_test_external_cfo(const char* RunPlan = "commands.bin", int PrintData = 1, int NDtcs = 1,
                                  const char* OutputFn = nullptr) {
  int pcie_addr = -1; // assume initialized

  dtc_init_external_readout(pcie_addr);
//-----------------------------------------------------------------------------
// for now, assume only one time chain, but provide for future
//-----------------------------------------------------------------------------
  cfo_init_readout_ext(RunPlan,NDtcs);      // for now, assume one time chain

  cfo_launch_run_plan();
//-----------------------------------------------------------------------------
// read events
//-----------------------------------------------------------------------------
  uint64_t first_ts = 0;
  dtc_read_subevents(first_ts,PrintData,pcie_addr,OutputFn);
}

//-----------------------------------------------------------------------------
// .L dtc_gui.C
//  x = dtc_gui("test",1)
//-----------------------------------------------------------------------------
DtcGui* dtc_gui(const char* Project = "test", int DebugLevel = 0) {
  // 950x1000: dimensions of the main frame
  DtcGui* x = new DtcGui(Project,gClient->GetRoot(),950,1000,DebugLevel);
  return x;
} 
