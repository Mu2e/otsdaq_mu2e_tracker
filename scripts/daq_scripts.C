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
void cfo_init_readout_ext(const char* RunPlan, uint DtcMask) {
  CfoInterface* cfo_i = CfoInterface::Instance();  // assume already initialized
  cfo_i->InitReadout(RunPlan,DtcMask);
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
void dtc_init_emulated_cfo_readout_mode(int PcieAddr = -1) { 
  //                                 int EWMode, int EnableClockMarkers, int EnableAutogenDRP) {

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->InitEmulatedCFOReadoutMode();
}

//-----------------------------------------------------------------------------
// to be executed on each node with the DTC
// 'dtc_init_link_mask' defines node-specific link mask
//-----------------------------------------------------------------------------
void dtc_init_external_cfo_readout_mode(int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->InitExternalCFOReadoutMode(daq_scripts::EdgeMode);
}

//-----------------------------------------------------------------------------
void dtc_init_readout(int EmulateCfo, int RocReadoutMode, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->InitReadout(EmulateCfo,RocReadoutMode);
}

//-----------------------------------------------------------------------------
int dtc_configure_roc_readout_mode(int ReadoutMode, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);    // assume already initialized
  dtc_i->SetRocReadoutMode(ReadoutMode);                     // 0:patterns 1:digis
  dtc_i->InitRocReadoutMode();
  return 0;
}

//-----------------------------------------------------------------------------
// to be executed on each node with a DTC, after the CFO run plan was launched
// don't validate
// a read should always end with releasing  buffers ???
//-----------------------------------------------------------------------------
int dtc_read_subevents(uint64_t FirstTS = 0, int PrintLevel = 1, int Validate = 0, int PcieAddr = -1, const char* OutputFn = nullptr) {
  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ReadSubevents(list_of_subevents,FirstTS,PrintLevel,Validate,OutputFn);
  
  return list_of_subevents.size();
}

//-----------------------------------------------------------------------------
// LinkMask : hex digit per link, i.e. 0x111 for links 0,1,2
//-----------------------------------------------------------------------------
int dtc_reset_roc(int LinkMask, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ResetRoc(LinkMask);
  return 0;
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
// do this per ROC
//-----------------------------------------------------------------------------
void dtc_read_spi(int Link, int PrintLevel = 2, int PcieAddr = -1) {
  vector<uint16_t>   spi_data;

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ReadSpiData(Link,spi_data,PrintLevel);
}

//-----------------------------------------------------------------------------
// this test is performed in a pattern readout mode
//-----------------------------------------------------------------------------
void dtc_buffer_test_emulated_cfo_patterns(int NEvents=3, int PrintLevel = 1, uint64_t FirstTS=0, int Validate = 0, const char* OutputFn = nullptr) {
  int pcie_addr(-1);                                 // assume initialized
  
  DtcInterface* dtc_i = DtcInterface::Instance(-1);  // assume already initialized

  int emulate_cfo      = 1;
  int roc_readout_mode = 0;
  dtc_i->InitReadout(emulate_cfo,roc_readout_mode);

  // dtc_i->SetRocReadoutMode(0);
  // dtc_i->InitRocReadoutMode();                       // readout ROC patterns

                                                     // 68x25ns = 1700 ns

                                                     // this call actually sends EWMs
  
  dtc_i->LaunchRunPlanEmulatedCfo(daq_scripts::EWLength,NEvents+1,FirstTS);

                                                    // in emulated mode, always read after 

  dtc_read_subevents(FirstTS,PrintLevel,Validate, pcie_addr,OutputFn);
}

//-----------------------------------------------------------------------------
// this test is performed in a pattern readout mode
// Mode : 0xXXRRVVPP
//        PP : print level - up to 256
//        VV : validation level
//        RR : ROC readout mode
//        XX : reserved
//-----------------------------------------------------------------------------
void dtc_buffer_test_emulated_cfo_new(int NEvents=3, int Mode = 0x1, uint64_t FirstTS=0, int Validate = 0, const char* OutputFn = nullptr) {
  int pcie_addr(-1);                                 // assume initialized
  
  DtcInterface* dtc_i = DtcInterface::Instance(pcie_addr);  // assume already initialized

  int print_level      = (Mode >>  0) & 0xff;
  int validation_level = (Mode >>  8) & 0xff;
  int roc_readout_mode = (Mode >> 16) & 0xff;
                                                     // 68x25ns = 1700 ns
                                                     // this call doesn't send EWMs
  int emulate_cfo = 1;
  dtc_i->InitReadout(emulate_cfo,roc_readout_mode);
                                                    // in emulated mode, always read after
  
  dtc_i->LaunchRunPlanEmulatedCfo(daq_scripts::EWLength,NEvents+1,FirstTS); // 

  dtc_read_subevents(FirstTS,print_level,validation_level,pcie_addr,OutputFn);
}

//-----------------------------------------------------------------------------
void dtc_buffer_test_external_cfo(const char* RunPlan   = "commands.bin",
                                  int         Mode      = 0x1           ,
                                  uint        DtcMask   = 0x1           ,
                                  const char* OutputFn  = nullptr       ) {
  int pcie_addr = -1; // assume initialized

  int print_level      = (Mode >>  0) & 0xff;
  int validation_level = (Mode >>  8) & 0xff;
  int roc_readout_mode = (Mode >> 16) & 0xff;

  int emulate_cfo = 0;
  dtc_init_readout(emulate_cfo,roc_readout_mode,pcie_addr);
//-----------------------------------------------------------------------------
// for now, assume only one time chain, but provide for future
//-----------------------------------------------------------------------------
  cfo_init_readout_ext(RunPlan,DtcMask);      // for now, assume one time chain
  cfo_launch_run_plan();
//-----------------------------------------------------------------------------
// read events
//-----------------------------------------------------------------------------
  uint64_t first_ts = 0;
  dtc_read_subevents(first_ts,print_level,validation_level,pcie_addr,OutputFn);
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

void set_digi_serial_readout(unsigned dtc_pcie, unsigned roc_link) {
  auto dtc_i = DtcInterface::Instance(dtc_pcie);
  auto dtc = dtc_i->Dtc();
  dtc->WriteROCRegister(DTC_Link_ID(roc_link), 8, 0x30F, false, 100);
  dtc->WriteROCRegister(DTC_Link_ID(roc_link), 29, 1, false, 100);
}
