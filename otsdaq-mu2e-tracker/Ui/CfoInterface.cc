//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
// most repeats Ryan's code, but w/o dependency on xdaq, GUI-based UI, and otsdaq everything
//-----------------------------------------------------------------------------
#ifndef __trkdaq_cfo_interface_cc__
#define __trkdaq_cfo_interface_cc__

#define __CLING__ 1

#include "TString.h"
#include "iostream"
#include "CfoInterface.hh"
#include "cfoInterfaceLib/CFO_Compiler.hh"

using namespace CFOLib;
using namespace DTCLib;

namespace trkdaq {

  CfoInterface* CfoInterface::fgInstance = nullptr;

//-----------------------------------------------------------------------------
  CfoInterface::CfoInterface(int PcieAddr, uint DtcMask, DTC_SimMode SimMode, bool SkipInit) {
    std::string expected_version("");              // dont check
    std::string sim_file        ("mu2esim.bin");
    std::string uid             ("");

    fPcieAddr = PcieAddr;
    fCfo      = new CFO(SimMode,PcieAddr,expected_version,SkipInit,uid);
//-----------------------------------------------------------------------------
// init DTC mask, less 16 DTCs per tine chain
//-----------------------------------------------------------------------------
    fDtcMask  = DtcMask;
    int link_mask = 0;
    for (int i=0; i<8; i++) {
      int ndtcs = (fDtcMask >> 4*i) & 0xf;
      if (ndtcs > 0) fCfo->EnableLink (CFO_Link_ID(i),DTC_LinkEnableMode(true ,true ),ndtcs);
      else           fCfo->DisableLink(CFO_Link_ID(i),DTC_LinkEnableMode(false,false));
    }
  }

//-----------------------------------------------------------------------------
  CfoInterface::~CfoInterface()  {
    if (fgInstance) {
      delete fgInstance->fCfo;
      fgInstance = nullptr;
    }
  }

//-----------------------------------------------------------------------------
  CfoInterface* CfoInterface::Instance(int PcieAddr, uint DtcMask) {
    int pcie_addr = PcieAddr;
    if (pcie_addr < 0) {
//-----------------------------------------------------------------------------
// PCIE address is not specified, check environment
//-----------------------------------------------------------------------------
      if (getenv("CFOLIB_CFO") != nullptr) pcie_addr = atoi(getenv("CFOLIB_CFO"));
      else {
        printf (" ERROR: PcieAddr < 0 and $CFOLIB_CFO is not defined. BAIL out\n");
        return nullptr;
      }
    }

    if (fgInstance == nullptr) fgInstance = new CfoInterface(pcie_addr,DtcMask,DTC_SimMode_NoCFO);
      
    if (fgInstance->PcieAddr() != pcie_addr) {
      printf (" ERROR: CfoInterface::Instance has been already initialized with PcieAddress = %i. BAIL out\n", 
              fgInstance->PcieAddr());
      return nullptr;
    }
    else return fgInstance;
  }


//-----------------------------------------------------------------------------
// Source=0: sync to internal clock ; 1: RTF
// on success, returns 1
// CFO JA CSR :0x9500
//-----------------------------------------------------------------------------
  int CfoInterface::ConfigureJA(int ClockSource, int Reset) {
    fCfo->SetJitterAttenuatorSelect(ClockSource,Reset);     // 0:internal clock sync, 1:RTF
    usleep(100000);
    int ok(0);
    for (int i=0; i<3; i++) {
      ok = fCfo->ReadJitterAttenuatorLocked();              // in case of success, returns true
      usleep(100000);
      if (ok == 1) break;
    }
    
    fCfo->FormatJitterAttenuatorCSR();

    if (ok == 0) printf("ERROR in DtcInterface::%s: failed to setup JA\n",__func__); 

    return ok;
  }

//-----------------------------------------------------------------------------
// really ? 
//-----------------------------------------------------------------------------
  void CfoInterface::Halt() {
    fCfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
    fCfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  }
  
//-----------------------------------------------------------------------------
// looks that it is only for the off-spill
//-----------------------------------------------------------------------------
  void CfoInterface::LaunchRunPlan() {
    fCfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
    fCfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);

    fCfo->SoftReset();
    usleep(10);	

    fCfo->EnableBeamOffMode (CFO_Link_ID::CFO_Link_ALL);
  }
  
//-----------------------------------------------------------------------------
  void CfoInterface::CompileRunPlan(const char* InputFn, const char* OutputFn) {
    CFOLib::CFO_Compiler compiler;

    std::string fn1(InputFn );
    std::string fn2(OutputFn);
    
    compiler.processFile(fn1,fn2);
  }

//-----------------------------------------------------------------------------
// launch is a separate step, could be repeated multiple times
// this is a one-time initialization
// CFO soft reset apparently restarts the execution , so keep the beam modes disabled
//-----------------------------------------------------------------------------
  void CfoInterface::InitReadout(const char* RunPlan, uint DtcMask) {

    // fCfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
    // fCfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
    
    fCfo->SoftReset();

    SetRunPlan   (RunPlan);

    if (DtcMask != 0) fDtcMask = DtcMask;
    for (int i=0; i<8; i++) {
      int ndtcs = (fDtcMask >> 4*i) & 0xf;
      if (ndtcs > 0) fCfo->EnableLink (CFO_Link_ID(i),DTC_LinkEnableMode(true,true),ndtcs);
      else           fCfo->DisableLink(CFO_Link_ID(i),DTC_LinkEnableMode(true,true));
    }

    fCfo->EnableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  }

//-----------------------------------------------------------------------------
  uint32_t CfoInterface::ReadRegister(uint16_t Register) {

    uint32_t data;
    int      timeout(150);
    
    mu2edev* dev = fCfo->GetDevice();
    dev->read_register(Register,timeout,&data);
    
    return data;
  }


//-----------------------------------------------------------------------------
  void CfoInterface::PrintRegister(uint16_t Register, const char* Title) {
    std::cout << Form("%s (0x%04x) : 0x%08x\n",Title,Register,ReadRegister(Register));
  }

//-----------------------------------------------------------------------------
  void CfoInterface::PrintStatus() {
    std::cout << Form("-----------------------------------------------------------------\n");
    PrintRegister(0x9004,"CFO version                                ");
    PrintRegister(0x9030,"Kernel driver version                      ");
    PrintRegister(0x9100,"CFO control register                       ");
    PrintRegister(0x9104,"DMA Transfer Length                        ");
    PrintRegister(0x9108,"SERDES loopback enable                     ");
    PrintRegister(0x9114,"CFO link enable                            ");
    PrintRegister(0x9128,"CFO PLL locked                             ");
    PrintRegister(0x9140,"SERDES RX CDR lock                         ");
    PrintRegister(0x9144,"Beam On Timer Preset                       ");
    PrintRegister(0x9148,"Enable Beam On Mode                        ");
    PrintRegister(0x914c,"Enable Beam Off Mode                       ");
    PrintRegister(0x918c,"Number of DTCs                             ");
    
    PrintRegister(0x9200,"Receive  Byte   Count Link 0               ");
    PrintRegister(0x9220,"Receive  Packet Count Link 0               ");
    PrintRegister(0x9240,"Transmit Byte   Count Link 0               ");
    PrintRegister(0x9260,"Transmit Packet Count Link 0               ");

    PrintRegister(0x9500,"CFO Jitter Attenuator CSR                  ");  // CFO_Register_JitterAttenuatorCSR = 0x9500,
  }

//-----------------------------------------------------------------------------
// TODO
//-----------------------------------------------------------------------------
  void CfoInterface::SetOffspillRunPlan(int NEvents, int EWLength) {
    printf("ERROR: %s not implemented yet",__func__);
  }

//-----------------------------------------------------------------------------
// first 8 bytes contain nbytes, but written into the CFO are 0x10000 bytes
// (sizeof(mu2e_databuff_t)
//-----------------------------------------------------------------------------
  void CfoInterface::SetRunPlan(const char* Fn) {

    std::ifstream file(Fn, std::ios::binary | std::ios::ate);

    // read binary file
    mu2e_databuff_t inputData;
    auto inputSize = file.tellg();
    uint64_t dmaSize = static_cast<uint64_t>(inputSize) + 8;
    file.seekg(0, std::ios::beg);

    memcpy(&inputData[0], &dmaSize, sizeof(uint64_t));
    file.read((char*) (&inputData[8]), inputSize);
    file.close();
    // this was the change from previos version
    mu2edev* dev = fCfo->GetDevice();
    dev->begin_dcs_transaction();
    dev->write_data(DTC_DMA_Engine_DCS, inputData, sizeof(inputData));
    dev->end_dcs_transaction(); // 
  }

};

#endif
