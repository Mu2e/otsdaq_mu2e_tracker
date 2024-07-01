//
#define __CLING__ 1

#include "iostream"

#include "TH1.h"

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

  CFO* cfo = cfo_i->Cfo();

	cfo->ResetDelayRegister();	                 // reset 0x9380
	cfo->DisableLinks();	                       // reset 0x9114
	                                             // configure the DTC (to configure the ROC in a loop)
	cfo->EnableLink(xLink, DTC_LinkEnableMode(true, true)); // enable Tx and Rx
	cfo->EnableDelayMeasureMode(xLink);
	cfo->EnableDelayMeasureNow(xLink);

	uint32_t delay = cfo->ReadCableDelayValue(xLink);	// read delay

	cout << "Delay measured: " << delay << " (ns) on link: " <<  xLink << std::endl;

	// reset registers
	cfo->ResetDelayRegister();
	cfo->DisableLinks();

	printf(" delay = %ui\n",delay);

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


//-----------------------------------------------------------------------------
void dtc_buffer_test_emulated_cfo(int NEvents=3, uint64_t FirstTS=0, int PrintData = 1, const char* OutputFn = nullptr) {
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
