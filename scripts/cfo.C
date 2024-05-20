//
#define __CLING__ 1

#include "iostream"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO_Compiler.hh"

using namespace CFOLib;
using namespace DTCLib;

#include "print_buffer.C"

#include "cfo_init.C"
#include "cfo_read_register.C"
#include "cfo_print_register.C"
#include "cfo_print_status.C"
#include "cfo_write_register.C"

#include "dtc_init.C"
#include "dtc_print_roc_status.C"
#include "dtc_print_status.C"

#include "dtc_read_register.C"
#include "dtc_reset_roc.C"

#include "dtc_set_roc_pattern_mode.C"

// DTC control register : 0x9100 
enum { 
  kCFOEmulationEnableBit = 30,
  kAutogenDRPBit         = 23,
};


// Dtc: PCIE index
//-----------------------------------------------------------------------------
void cfo_measure_delay(int PcieAddress, CFO_Link_ID xLink) {

  CFO* cfo = cfo_init(PcieAddress); 
  if (cfo == nullptr) return;

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
  CFO* cfo = cfo_init(PcieAddress);
  cfo->SoftReset();
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
  CFO* cfo = cfo_init(PcieAddress);

	std::ifstream file(Fn, std::ios::binary | std::ios::ate);

                                        // read binary file
	mu2e_databuff_t inputData;
	auto inputSize = file.tellg();
	uint64_t dmaSize = static_cast<uint64_t>(inputSize) + 8;
	file.seekg(0, std::ios::beg);

	memcpy(&inputData[0], &dmaSize, sizeof(uint64_t));
	file.read((char*) (&inputData[8]), inputSize);
	file.close();

  cfo->GetDevice()->write_data(DTC_DMA_Engine_DAQ, inputData, sizeof(inputData));
	usleep(10);	
}

//-----------------------------------------------------------------------------
void cfo_reset_run_plan(int PcieAddress = -1) {
  CFO* cfo = cfo_init(PcieAddress);

  cfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
  cfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  cfo->SoftReset();
}


//-----------------------------------------------------------------------------
void cfo_launch_run_plan(int PcieAddress = -1) {
  CFO* cfo = cfo_init(PcieAddress);

  cfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
  cfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  cfo->SoftReset();

	usleep(10);	
	cfo->EnableBeamOffMode (CFO_Link_ID::CFO_Link_ALL);
}

//-----------------------------------------------------------------------------
void dtc_write_register(uint16_t Register, uint32_t Data, int PcieAddress = -1) {

  int timeout(150);

  DTC*     dtc = dtc_init(PcieAddress);
  mu2edev* dev = dtc->GetDevice();

  dev->write_register(Register,timeout,Data);
}

//-----------------------------------------------------------------------------
void dtc_write_roc_register(int Roc, uint16_t Register, uint16_t Data, int PcieAddress = -1) {

  int timeout(150);

  DTC*     dtc = dtc_init(PcieAddress);

  dtc->WriteROCRegister(DTC_Link_ID(Roc),Register,Data,timeout,false);
}

//-----------------------------------------------------------------------------
void dtc_read_roc_register(int Roc, uint16_t Register, uint16_t& Data, int PcieAddress = -1) {

  int timeout(150);

  DTC*     dtc = dtc_init(PcieAddress);

  Data = dtc->ReadROCRegister(DTC_Link_ID(Roc),Register,timeout);
}

//-----------------------------------------------------------------------------
void dtc_hard_reset(int PcieAddress = -1) {
  DTC* dtc = dtc_init(PcieAddress);
  dtc->HardReset();
}

//-----------------------------------------------------------------------------
void dtc_soft_reset(int PcieAddress = -1) {
  DTC* dtc = dtc_init(PcieAddress);
  dtc->SoftReset();
}

//-----------------------------------------------------------------------------
// CFOEmulationMode: 1:emulation, 0:external
//-----------------------------------------------------------------------------
void dtc_setup_cfo_interface(int PcieAddress, int CFOEmulationMode, int ForceCFOEdge, int EnableCFORxTx, int EnableAutogenDRP) {
  DTC* dtc = dtc_init(PcieAddress);

  if (CFOEmulationMode == 0) dtc->ClearCFOEmulationMode();
  else                       dtc->SetCFOEmulationMode  ();

  dtc->SetExternalCFOSampleEdgeMode(ForceCFOEdge);

  if (EnableCFORxTx == 0) {
    dtc->DisableReceiveCFOLink ();
    dtc->DisableTransmitCFOLink();
  }
  else {
    dtc->EnableReceiveCFOLink  ();
    dtc->EnableTransmitCFOLink ();
  }

  if (EnableAutogenDRP == 0) dtc->DisableAutogenDRP();
  else                       dtc->EnableAutogenDRP ();

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

	dtc->DisableCFOEmulation();
	dtc->DisableAutogenDRP();

  dtc->SoftReset();                     // write bit 31

  dtc->SetCFOEmulationEventWindowInterval(EWLength);  
  dtc->SetCFOEmulationNumHeartbeats      (NMarkers);
  dtc->SetCFOEmulationTimestamp          (DTC_EventWindowTag(FirstEWTag));
  dtc->SetCFOEmulationEventMode          (EWMode);
  dtc->SetCFO40MHzClockMarkerEnable      (DTC_Link_ALL,EnableClockMarkers);

  // dtc_set_bit(dtc,0x9100,kAutogenDRPBit,EnableAutogenDRP);
  dtc->EnableAutogenDRP();                                      // r_0x9100:bit_23 = 1
  dtc->SetCFOEmulationMode();                                   // r_0x9100:bit_15 = 1 
  dtc->EnableCFOEmulation();                                    // r_0x9100:bit_30 = 1 

  dtc->EnableReceiveCFOLink();                                  // r_0x9114:bit_14 = 1
}

//-----------------------------------------------------------------------------
// write value 0x10800244 to register 0x9100
// write value 0x00004141 to register 0x9114
// write value 0x10800244 to register 0x9100

void dtc_init_external_cfo_mode(DTC* dtc) { 
  //                                 int EWMode, int EnableClockMarkers, int EnableAutogenDRP) {

  int EWMode             = 1;
  int EnableClockMarkers = 0;
  int EnableAutogenDRP   = 1;
  int SampleEdgeMode     = 2;

  dtc->DisableCFOEmulation  ();
  dtc->DisableAutogenDRP();

  dtc->HardReset();                     // write bit 31
  dtc->EnableLink(DTC_Link_0,DTC_LinkEnableMode(true,true));

  // dtc->SetCFOEmulationEventWindowInterval(EWLength);  
  // dtc->SetCFOEmulationNumHeartbeats      (NMarkers);
  // dtc->SetCFOEmulationTimestamp          (DTC_EventWindowTag(FirstEWTag));
  // dtc->SetCFOEmulationEventMode          (EWMode);

  dtc->SetCFO40MHzClockMarkerEnable      (DTC_Link_0,EnableClockMarkers);
  dtc->SetExternalCFOSampleEdgeMode      (SampleEdgeMode);

  dtc->EnableAutogenDRP();

  dtc->EnableReceiveCFOLink ();                                // r_0x9114:bit_14 = 1

  dtc->ClearCFOEmulationMode();                                // r_0x9100:bit_15 = 0
  // dtc->SetCFOEmulationMode();                                // r_0x9100:bit_15 = 0

  // dtc->DisableCFOEmulation  ();
   dtc->EnableCFOEmulation();                                 // r_0x9100:bit_30 = 1 

}

//-----------------------------------------------------------------------------
// a read should always end with releasing  buffers
//-----------------------------------------------------------------------------
void dtc_read_subevents(DTC* dtc, int PrintData, uint64_t FirstTs) {

  uint64_t ts = FirstTs;
  
  bool match_ts = false;
  // std::vector<std::unique_ptr<DTCLib::DTC_Event>> events = dtc->GetData(event_tag, match_ts);

  while(1) {
    DTC_EventWindowTag event_tag = DTC_EventWindowTag(ts);
    std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> subevents = dtc->GetSubEventData(event_tag, match_ts);
    int sz = subevents.size();

    printf(">>>> ------------------------------ ts = %li   subevents.size = %i \n", ts,sz);
    if (sz == 0) break;
    ts++;

    for (int i=0; i<sz; i++) {
      DTC_SubEvent* ev = subevents[i].get();
      // ushort* xdata = (ushort*) ev->GetRawBufferPointer();
      int nb = ev->GetSubEventByteCount();
      int nw = nb/2;
      
      uint64_t ew_tag = ev->GetEventWindowTag().GetEventWindowTag(true);

      if (PrintData) {
        printf(">>>> -----------subevent = %i nbytes = %4i ew_tag: %10li\n", i,nb,ew_tag);
        print_buffer(ev->GetRawBufferPointer(),ev->GetSubEventByteCount()/2);
        std::this_thread::sleep_for(std::chrono::microseconds(2000));  
      }
    }
  }

  dtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
}


//-----------------------------------------------------------------------------
void dtc_buffer_test_emulated_cfo(int NEvents=3, int PrintData = 1, uint64_t FirstTS=0) {

  DTC* dtc = dtc_init(1,Dtc::DefaultSimMode,0x1);

  dtc_set_roc_pattern_mode(0x1);
  dtc_init_emulated_cfo_mode(dtc,68,NEvents+1,0);
  std::this_thread::sleep_for(std::chrono::microseconds(2000));  
  dtc_read_subevents(dtc,PrintData,FirstTS);
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void dtc_buffer_test_external_cfo(const char* RunPlan = "commands.bin", int PrintData = 1, uint64_t FirstTS=0) {

  CFO* cfo = cfo_init(0);
  DTC* dtc = dtc_init(1,DTC_SimMode_NoCFO,0x1);

  dtc_init_external_cfo_mode(dtc);

  cfo->SetMaxDTCNumber(CFO_Link_0,1);

  dtc_set_roc_pattern_mode(0x1);

  cfo_set_run_plan   (RunPlan);
  cfo_launch_run_plan();

  dtc_read_subevents(dtc,PrintData,FirstTS);
}
