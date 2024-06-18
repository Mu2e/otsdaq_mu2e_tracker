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

// #include "cfo_init.C"
// #include "cfo_read_register.C"
// #include "cfo_print_register.C"
// #include "cfo_print_status.C"
// #include "cfo_write_register.C"

// #include "dtc_init.C"
// #include "dtc_print_roc_status.C"
// #include "dtc_print_status.C"

// #include "dtc_read_register.C"
// #include "dtc_reset_roc.C"

// #include "dtc_set_roc_pattern_mode.C"

#include "otsdaq-mu2e-tracker/Ui/CfoInterface.hh"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

using namespace trkdaq;

// DTC control register : 0x9100 
enum { 
  kCFOEmulationEnableBit = 30,
  kAutogenDRPBit         = 23,
};


// Dtc: PCIE index
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
// first 8 bytes contain nbytes, but written into the CFO are 0x10000 bytes
// (the sizeof(mu2e_databuff_t) 0
//-----------------------------------------------------------------------------
void cfo_set_run_plan(const char* Fn = "commands.bin", int PcieAddress = -1) {

  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress);
  cfo_i->SetRunPlan(Fn);

  // CFO* cfo = CfoInterface::Instance(PcieAddress)->Cfo(); 

	// std::ifstream file(Fn, std::ios::binary | std::ios::ate);

  //                                       // read binary file
	// mu2e_databuff_t inputData;
	// auto inputSize = file.tellg();
	// uint64_t dmaSize = static_cast<uint64_t>(inputSize) + 8;
	// file.seekg(0, std::ios::beg);

	// memcpy(&inputData[0], &dmaSize, sizeof(uint64_t));
	// file.read((char*) (&inputData[8]), inputSize);
	// file.close();

  // cfo->GetDevice()->write_data(DTC_DMA_Engine_DAQ, inputData, sizeof(inputData));
	// usleep(10);	
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
void dtc_write_register(uint16_t Register, uint32_t Data, int PcieAddress = -1) {
  int tmo_ms(150);

  mu2edev* dev = DtcInterface::Instance(PcieAddress)->Dtc()->GetDevice();
  dev->write_register(Register,tmo_ms,Data);
}

//-----------------------------------------------------------------------------
void dtc_write_roc_register(int Roc, uint16_t Register, uint16_t Data, int PcieAddress = -1) {
  int tmo_ms(150);

  DTC*     dtc = DtcInterface::Instance(PcieAddress)->Dtc();
  dtc->WriteROCRegister(DTC_Link_ID(Roc),Register,Data,tmo_ms,false);
}

//-----------------------------------------------------------------------------
void dtc_read_roc_register(int Roc, uint16_t Register, uint16_t& Data, int PcieAddress = -1) {
  int timeout(150);

  DTC*     dtc = DtcInterface::Instance(PcieAddress)->Dtc();

  Data = dtc->ReadROCRegister(DTC_Link_ID(Roc),Register,timeout);
}

//-----------------------------------------------------------------------------
void dtc_hard_reset(int PcieAddress = -1) {
  DtcInterface::Instance(PcieAddress)->Dtc()->HardReset();
}

// //-----------------------------------------------------------------------------
// // current ROC link mask for different online machines
// // this is a part of the configuration
// //-----------------------------------------------------------------------------
// int dtc_init_link_mask() {
// 
//   char buf[100], host[100];
// 
//   TString cmd = "hostname -s";
//   FILE* pipe = gSystem->OpenPipe(cmd,"r");
//   while (fgets(buf,100,pipe)) { 
//     sscanf(buf,"%s",host);
//   }
//   gSystem->ClosePipe(pipe);
// 
//   TString hostname = host;
// 
//   int linkmask = 0x111111; // all links enabled
// 
//   if      (hostname == "mu2edaq07") { // TS2 connected as ROC1
//     linkmask = 0x10;
//   }
//   else if (hostname == "mu2edaq09") { // TS1 connected as ROC0
//     linkmask = 0x01;
//   }
//   else if (hostname == "mu2edaq22") {
//     linkmask = 0x011;
//   }
// 
//   return linkmask;
// }
// 
//-----------------------------------------------------------------------------
void dtc_soft_reset(int PcieAddress = -1) {
  DtcInterface::Instance(PcieAddress)->Dtc()->SoftReset();
}

//-----------------------------------------------------------------------------
// CFOEmulationMode: 1:emulation, 0:external
//-----------------------------------------------------------------------------
void dtc_setup_cfo_interface(int PcieAddress, int CFOEmulationMode, int ForceCFOEdge, int EnableCFORxTx, int EnableAutogenDRP) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddress);
  dtc_i->SetupCfoInterface(CFOEmulationMode,ForceCFOEdge,EnableCFORxTx,EnableAutogenDRP);
}

//-----------------------------------------------------------------------------
// Value: 0 or 1
//-----------------------------------------------------------------------------
void dtc_set_bit(DTC* Dtc, int Register, int Bit, int Value) {
  int timeout(100);
  uint32_t w = (1 << Bit);

  uint32_t data;
  Dtc->GetDevice()->read_register(Register,timeout,&data);

  data = (data ^ w) | (Value << Bit);
  Dtc->GetDevice()->write_register(Register,timeout,data);
    
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

	// dtc->DisableCFOEmulation();
	// dtc->DisableAutogenDRP();

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

  // dtc->DisableCFOEmulation  ();
  // dtc->DisableAutogenDRP();

  // dtc->HardReset();                        // write bit 0
  // dtc->SoftReset();                     // write bit 31

  // dtc->SetCFOEmulationEventWindowInterval(EWLength);  
  // dtc->SetCFOEmulationNumHeartbeats      (NMarkers);
  // dtc->SetCFOEmulationTimestamp          (DTC_EventWindowTag(FirstEWTag));

  //  int EWMode             = 1;
  // dtc->SetCFOEmulationEventMode          (EWMode);

  int EnableClockMarkers = 0;
  dtc->SetCFO40MHzClockMarkerEnable      (DTC_Link_0,EnableClockMarkers);

  int SampleEdgeMode     = 0;
  dtc->SetExternalCFOSampleEdgeMode      (SampleEdgeMode);

  dtc->EnableAutogenDRP();

  dtc->ClearCFOEmulationMode();                                // r_0x9100:bit_15 = 0
  // dtc->SetCFOEmulationMode();                                // r_0x9100:bit_15 = 1

  dtc->DisableCFOEmulation  ();
  // dtc->EnableCFOEmulation();                                 // r_0x9100:bit_30 = 1 

  dtc->EnableReceiveCFOLink ();                                // r_0x9114:bit_14 = 1
}

//-----------------------------------------------------------------------------
// to be executed on each node with the DTC
// 'dtc_init_link_mask' defines node-specific link mask
//-----------------------------------------------------------------------------
void dtc_init_external_cfo_readout_mode() {
  DtcInterface* dtc_i = DtcInterface::Instance(-1);

  dtc_i->Dtc()->SoftReset();
  dtc_i->InitExternalCFOReadoutMode();
  dtc_i->RocPatternConfig();            // DtcInterface knows its link mask

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
void dtc_buffer_test_emulated_cfo(int NEvents=3, int PrintData = 1, uint64_t FirstTS=0, const char* OutputFn = nullptr) {

  DtcInterface* dtc_i = DtcInterface::Instance(-1); // assume already initialized

  dtc_i->RocPatternConfig();
                                        // 68x25 = 1700 ns
  
  dtc_i->InitEmulatedCFOReadoutMode(68,NEvents+1,0);

  //  dtc_read_subevents(dtc,PrintData,FirstTS,&vev);
  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;
  dtc_i->ReadSubevents(list_of_subevents,FirstTS,PrintData,OutputFn);
}

//-----------------------------------------------------------------------------
// for now, assume just one line
//-----------------------------------------------------------------------------
void cfo_init_readout(const char* RunPlan = "commands.bin", int CfoLink = 0, int NDtcs = 1) {

  CfoInterface* cfo_i = CfoInterface::Instance(-1);  // assume already initialized
  CFO* cfo = cfo_i->Cfo();

  cfo->SoftReset();

  CFO_Link_ID link = CFO_Link_ID(CfoLink);

  cfo->EnableLink(link,DTC_LinkEnableMode(true,true),1);

  cfo->SetMaxDTCNumber(link,NDtcs);

  cfo_i->SetRunPlan   (RunPlan);
  // cfo_launch_run_plan();
}

//-----------------------------------------------------------------------------
// to be executed on each node with a DTC, after the CFO run plan was launched
//-----------------------------------------------------------------------------
int dtc_read_events(uint64_t FirstTS = 0, int PrintData = 1, const char* OutputFn = nullptr) {
  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;

  DtcInterface* dtc_i = DtcInterface::Instance(-1);
  dtc_i->ReadSubevents(list_of_subevents,FirstTS,PrintData,OutputFn);
  return list_of_subevents.size();
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void dtc_buffer_test_external_cfo(const char* RunPlan = "commands.bin", int PrintData = 1, int NDtcs = 1,  const char* OutputFn = nullptr) {

  int cfo_link = 0;
  cfo_init_readout(RunPlan,cfo_link,NDtcs);

  // CfoInterface* cfo_i = CfoInterface::Instance(-1);  // assume already initialized
  // CFO* cfo = cfo_i->Cfo();
  // cfo->SoftReset();
  // cfo->EnableLink(CFO_Link_0,DTC_LinkEnableMode(true,true),1);
  // cfo->SetMaxDTCNumber(CFO_Link_0,2);
  // cfo_set_run_plan   (RunPlan);


  // DtcInterface* dtc_i = DtcInterface::Instance(-1);
  // dtc_i->Dtc()->SoftReset();                         // soft reset here seems to be critical
  // dtc_i->InitExternalCFOReadoutMode();
  // dtc_i->RocPatternConfig();

  dtc_init_external_cfo_readout_mode();

  cfo_launch_run_plan();

  uint64_t firstTS = 0;
  dtc_read_events(firstTS,PrintData,OutputFn);
}

//-----------------------------------------------------------------------------
struct DaqEvent_t {
  int ts {0};
  int n_subevents{0};      // 
  int nbtot{0};
  int rs[6]{0,0,0,0,0,0};  // ROC status
};

struct Hist_t {
  TH1F* nev;
  TH1F* roc_status[6];
} Hist;

//-----------------------------------------------------------------------------
void dtc_buffer_test_external_cfo_1(int N) {

  CfoInterface* cfo_i = CfoInterface::Instance(-1);
  CFO* cfo = cfo_i->Cfo();

  DtcInterface* dtc_i = DtcInterface::Instance(-1);

  Hist.nev        = new TH1F("nev","nev",200,0,200);

  for (int i=0; i<6; i++) {
    Hist.roc_status[i] = new TH1F(Form("roc_status_%i",i),Form("roc_status[%i]",i),2000,0,2000);
  }

  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;

  for (int i=0; i<N; i++) {
    printf(" ------------- loop # %3i\n",i);

    dtc_i->Dtc()->SoftReset();
    cfo->SoftReset();

    dtc_i->InitExternalCFOReadoutMode();

    cfo->EnableLink     (CFO_Link_0,DTC_LinkEnableMode(true,true),1);
    cfo->SetMaxDTCNumber(CFO_Link_0,1);

    dtc_i->RocPatternConfig();

    cfo_set_run_plan   ("run_066.bin");
    cfo_launch_run_plan();

    uint64_t firstTS    = 0;
    int      print_data = 1;

    list_of_subevents.clear();
    dtc_i->ReadSubevents(list_of_subevents,firstTS,print_data);
//-----------------------------------------------------------------------------
// analysis part
//-----------------------------------------------------------------------------
    int nev = list_of_subevents.size();
    int rs[6];  // roc status word

    for (int iev=0; iev<nev; iev++) {
      DTC_SubEvent* ev = list_of_subevents[iev].get();

      char* data = (char*) ev->GetRawBufferPointer();

      char* roc_data = data+0x30;

      for (int roc=0; roc<6; roc++) {
        int nb    = *((ushort*)(roc_data     ));
        rs[roc]   = *((ushort*)(roc_data+0x0c));
        roc_data += nb;
      }


      Hist.nev->Fill(nev);

      for (int k=0; k<6; k++) {
        Hist.roc_status[k]->Fill(rs[k]);
      }
    }
  }

  Hist.nev->Draw();

  // printf(" ---- 2\n"); dtc_print_status();
}
