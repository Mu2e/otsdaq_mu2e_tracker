//
#define __CLING__ 1

#include "iostream"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

#include "print_buffer.C"

using namespace CFOLib;
using namespace DTCLib;

DTC* gDTC[2] = {nullptr, nullptr};
CFO* gCfo[2] = {nullptr, nullptr};

namespace trk_daq {
  int gSleepTimeDTC      =  1000;  // [us]
  int gSleepTimeROC      =  2000;  // [us]
  int gSleepTimeROCReset =  4000;  // [us]
}

// DTC control register : 0x9100 
enum { 
  kCFOEmulationEnableBit = 30,
  kAutogenDRPBit         = 23,
};

//-----------------------------------------------------------------------------
// PcieAddress - CFO card index on PCIE bus
//-----------------------------------------------------------------------------
CFO* cfo_init(int PcieAddress) {
  int addr = PcieAddress;
  if (addr < 0) {
    if (gSystem->Getenv("CFOLIB_CFO") != nullptr) addr = atoi(gSystem->Getenv("CFOLIB_CFO"));
    else {
      printf (" ERROR: PcieAddress < 0 and $CFOLIB_CFO is not defined. BAIL out\n");
      return nullptr;
    }
  }

  if (gCfo[addr] == nullptr) gCfo[addr] = new CFO(DTCLib::DTC_SimMode_Disabled,addr,"",true);

  return gCfo[addr];
}


// Dtc: PCIE index
//-----------------------------------------------------------------------------
void cfo_measure_delay(int PcieAddress, CFO_Link_ID xLink) {

  CFO* cfo = cfo_init(PcieAddress);

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
uint32_t cfo_read_register(int PcieAddress, uint16_t Register) {

  CFO* cfo = cfo_init(PcieAddress);
  
  mu2edev* dev = cfo->GetDevice();

  uint32_t data;

  int timeout(150);
  dev->read_register(Register,timeout,&data);

  return data;
}

//-----------------------------------------------------------------------------
void cfo_write_register(int PcieAddress, uint16_t Register, uint32_t Data) {

  int timeout(150);

  CFO*     cfo = cfo_init(PcieAddress);
  mu2edev* dev = cfo->GetDevice();

  dev->write_register(Register,timeout,Data);
}

//-----------------------------------------------------------------------------
void cfo_print_register(int PcieAddress, uint16_t Register, const char* Title = "") {
  printf("%s (0x%04x) : 0x%08x\n",Title,Register,cfo_read_register(PcieAddress,Register));
}

//-----------------------------------------------------------------------------
void cfo_soft_reset(int PcieAddress) {
  CFO* cfo = cfo_init(PcieAddress);
  cfo->SoftReset();
}

//-----------------------------------------------------------------------------
void cfo_print_status(int PcieAddress) {
  
  CFO* cfo = cfo_init(PcieAddress);

  printf("-----------------------------------------------------------------\n");
  cfo_print_register(PcieAddress,0x9004,"CFO version                                ");
  cfo_print_register(PcieAddress,0x9030,"Kernel driver version                      ");
  cfo_print_register(PcieAddress,0x9100,"CFO control register                       ");
  cfo_print_register(PcieAddress,0x9104,"DMA Transfer Length                        ");
  cfo_print_register(PcieAddress,0x9108,"SERDES loopback enable                     ");
  cfo_print_register(PcieAddress,0x9114,"CFO link enable                            ");
  cfo_print_register(PcieAddress,0x9128,"CFO PLL locked                             ");
  cfo_print_register(PcieAddress,0x9140,"SERDES RX CDR lock                         ");
  cfo_print_register(PcieAddress,0x9144,"Beam On Timer Preset                       ");
  cfo_print_register(PcieAddress,0x9148,"Enable Beam On Mode                        ");
  cfo_print_register(PcieAddress,0x914c,"Enable Beam Off Mode                       ");
  cfo_print_register(PcieAddress,0x918c,"Number of DTCs                             ");

  cfo_print_register(PcieAddress,0x9200,"Receive  Byte   Count Link 0               ");
  cfo_print_register(PcieAddress,0x9220,"Receive  Packet Count Link 0               ");
  cfo_print_register(PcieAddress,0x9240,"Transmit Byte   Count Link 0               ");
  cfo_print_register(PcieAddress,0x9260,"Transmit Packet Count Link 0               ");
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
}

//-----------------------------------------------------------------------------
void cfo_reset_run_plan(int PcieAddress = -1) {
  CFO* cfo = cfo_init(PcieAddress);

  cfo->DisableBeamOnMode(CFOLib::CFO_Link_ID::CFO_Link_ALL);
  cfo->DisableBeamOffMode(CFOLib::CFO_Link_ID::CFO_Link_ALL);
  cfo->SoftReset();
}


//-----------------------------------------------------------------------------
void cfo_launch_run_plan(int PcieAddress = -1) {
  CFO* cfo = cfo_init(PcieAddress);

  cfo->DisableBeamOnMode (CFOLib::CFO_Link_ID::CFO_Link_ALL);
  cfo->DisableBeamOffMode(CFOLib::CFO_Link_ID::CFO_Link_ALL);
  cfo->SoftReset();

	usleep(10);	
	cfo->EnableBeamOffMode (CFOLib::CFO_Link_ID::CFO_Link_ALL);
}


//-----------------------------------------------------------------------------
// PcieAddress - DTC card index on PCIE bus
// by default, leave Link=0
//-----------------------------------------------------------------------------
DTC* dtc_init(int PcieAddress, DTC_SimMode Mode = DTC_SimMode_Disabled, uint LinkMask = 0x1) {
  int addr = PcieAddress;
  if (addr < 0) {
    if (gSystem->Getenv("DTCLIB_DTC") != nullptr) addr = atoi(gSystem->Getenv("DTCLIB_DTC"));
    else {
      printf(">>> ERROR: PcieAddress < 0 and $DTCLIB_DTC is not defined. BAIL OUT\n");
      return nullptr;
    }
  }

  if (gDTC[addr] == nullptr) gDTC[addr] = new DTC(Mode,addr,LinkMask,"",false,"","");

  return gDTC[addr];
}


//-----------------------------------------------------------------------------
uint32_t dtc_read_register(int PcieAddress, uint16_t Register) {

  DTC* dtc = dtc_init(PcieAddress);
  
  mu2edev* dev = dtc->GetDevice();

  uint32_t data;

  int timeout(150);
  dev->read_register(Register,timeout,&data);

  return data;
}

//-----------------------------------------------------------------------------
void dtc_write_register(int PcieAddress, uint16_t Register, uint32_t Data) {

  int timeout(150);

  DTC*     dtc = dtc_init(PcieAddress);
  mu2edev* dev = dtc->GetDevice();

  dev->write_register(Register,timeout,Data);
}


//-----------------------------------------------------------------------------
void dtc_print_register(int PcieAddress, uint16_t Register, const char* Title = "") {
  printf("%s (0x%04x) : 0x%08x\n",Title,Register,dtc_read_register(PcieAddress,Register));
}

//-----------------------------------------------------------------------------
void dtc_print_status(int PcieAddress = -1) {
  
  DTC* dtc = dtc_init(PcieAddress);

  printf("-----------------------------------------------------------------\n");
  dtc_print_register(PcieAddress,0x9000,"DTC firmware link speed and design version ");
  dtc_print_register(PcieAddress,0x9004,"DTC version                                ");
  dtc_print_register(PcieAddress,0x9008,"Design status                              ");
  dtc_print_register(PcieAddress,0x900c,"Vivado version                             ");
  dtc_print_register(PcieAddress,0x9100,"DTC control register                       ");
  dtc_print_register(PcieAddress,0x9104,"DMA transfer length                        ");
  dtc_print_register(PcieAddress,0x9108,"SERDES loopback enable                     ");
  dtc_print_register(PcieAddress,0x9110,"ROC Emulation enable                       ");
  dtc_print_register(PcieAddress,0x9114,"Link Enable                                ");
  dtc_print_register(PcieAddress,0x9128,"SERDES PLL Locked                          ");
  dtc_print_register(PcieAddress,0x9140,"SERDES RX CDR lock (locked fibers)         ");
  dtc_print_register(PcieAddress,0x9144,"DMA Timeout Preset                         ");
  dtc_print_register(PcieAddress,0x9148,"ROC reply timeout                          ");
  dtc_print_register(PcieAddress,0x914c,"ROC reply timeout error                    ");
  dtc_print_register(PcieAddress,0x9158,"Event Builder Configuration                ");
  dtc_print_register(PcieAddress,0x91a8,"CFO Emulation Heartbeat Interval           ");
  dtc_print_register(PcieAddress,0x91ac,"CFO Emulation Number of HB Packets         ");
  dtc_print_register(PcieAddress,0x91bc,"CFO Emulation Number of Null HB Packets    ");
  dtc_print_register(PcieAddress,0x91f4,"CFO Emulation 40 MHz Clock Marker Interval ");
  dtc_print_register(PcieAddress,0x91f8,"CFO Marker Enables                         ");

  dtc_print_register(PcieAddress,0x9200,"Receive  Byte   Count Link 0               ");
  dtc_print_register(PcieAddress,0x9220,"Receive  Packet Count Link 0               ");
  dtc_print_register(PcieAddress,0x9240,"Transmit Byte   Count Link 0               ");
  dtc_print_register(PcieAddress,0x9260,"Transmit Packet Count Link 0               ");

  dtc_print_register(PcieAddress,0x9218,"Receive  Byte   Count CFO                  ");
  dtc_print_register(PcieAddress,0x9238,"Receive  Packet Count CFO                  ");
  dtc_print_register(PcieAddress,0x9258,"Transmit Byte   Count CFO                  ");
  dtc_print_register(PcieAddress,0x9278,"Transmit Packet Count CFO                  ");
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

  dtc->SoftReset();                     // write bit 31

  dtc->SetCFOEmulationEventWindowInterval(EWLength);  
  dtc->SetCFOEmulationNumHeartbeats      (NMarkers);
  dtc->SetCFOEmulationTimestamp          (DTC_EventWindowTag(FirstEWTag));
  dtc->SetCFOEmulationEventMode          (EWMode);
  dtc->SetCFO40MHzClockMarkerEnable      (DTC_Link_ALL,EnableClockMarkers);

  dtc_set_bit(dtc,0x9100,kAutogenDRPBit,EnableAutogenDRP);  // bit 23

  dtc->EnableReceiveCFOLink();                               // r_0x9114:bit_14 = 1
  dtc->SetCFOEmulationMode();                                // r_0x9100:bit_15 = 1 
  dtc->EnableCFOEmulation();                                 // r_0x9100:bit_30 = 1 
}

//-----------------------------------------------------------------------------
// rely on DTCLIB_DTC
//-----------------------------------------------------------------------------
void dtc_set_roc_pattern_mode(int LinkMask, int PcieAddress=-1) {

  DTC* dtc = dtc_init(PcieAddress,DTC_SimMode_Disabled,LinkMask);

  for (int i=0; i<6; i++) {
    int used = (LinkMask >> 4*i) & 0x1;
    if (used != 0) {
      auto link = DTC_Link_ID(i);
      dtc->WriteROCRegister(link,14,     1,false,1000);                // 1 --> r14: reset ROC
      std::this_thread::sleep_for(std::chrono::microseconds(trk_daq::gSleepTimeROCReset));

      dtc->WriteROCRegister(link, 8,0x0010,false,1000);              // configure ROC to send patterns
      std::this_thread::sleep_for(std::chrono::microseconds(trk_daq::gSleepTimeROC));

      dtc->WriteROCRegister(link,30,0x0000,false,1000);                // r30: mode, write 0 into it 
      std::this_thread::sleep_for(std::chrono::microseconds(trk_daq::gSleepTimeROC));

      dtc->WriteROCRegister(link,29,0x0001,false,1000);                // r29: data version, currently 1
      std::this_thread::sleep_for(std::chrono::microseconds(trk_daq::gSleepTimeROC));
    }
  }
}

//-----------------------------------------------------------------------------
void dtc_read_subevents(DTC* dtc, uint64_t FirstTs) {


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

      printf(">>>> -----------subevent = %i nbytes = %4i ew_tag: %10li\n", i,nb,ew_tag);
      print_buffer(ev->GetRawBufferPointer(),ev->GetSubEventByteCount()/2);
    }
  }

}

//-----------------------------------------------------------------------------
void dtc_buffer_test(int NEvents=3, uint64_t FirstTS=0) {
  DTC* dtc = dtc_init(1,DTC_SimMode_Disabled,0x1);

  dtc_set_roc_pattern_mode(0x1);
  dtc_init_emulated_cfo_mode(dtc,68,NEvents+1,0);
  dtc_read_subevents(dtc,FirstTS);
}
