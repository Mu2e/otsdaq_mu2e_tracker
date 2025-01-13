//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __mu2edaq_dtc_interface_cc__
#define __mu2edaq_dtc_interface_cc__

#define __CLING__ 1

#include "iostream"
#include "vector"

#include "artdaq-core-mu2e/Data/TrackerDataDecoder.hh"

#include "DtcInterface.hh"
#include "TString.h"    // includes ROOT's Form

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterface"

using namespace DTCLib;
using namespace std;

namespace mu2edaq {
  

  DtcInterface* DtcInterface::fgInstance[2] = {nullptr, nullptr};
  
//-----------------------------------------------------------------------------
  DtcInterface::DtcInterface(int PcieAddr, uint LinkMask, bool SkipInit) {
    std::string expected_version("");              // dont check
    std::string sim_file        ("mu2esim.bin");
    std::string uid             ("");
      
    TLOG(TLVL_DEBUG) << "CONSTRUCT DTC: pcie_addr:" << PcieAddr
                          << " LinkMask:0x" << std::hex << LinkMask
                          << std::dec
                          << " SkipInit:" << SkipInit << std::endl;
    fEnabled         = 1;                // default: enabled
    fPcieAddr        = PcieAddr;
    fSampleEdgeMode  = 1;
    fEmulateCfo      = 0;
    fJAMode          = 0x11;             // by default, assume RTF clock and reset upon setting
    
    fDtcID          = 0;                // needed for multi-DTC DAQ, default:0
    fPartitionID    = 0;                // use reasonable defaults, which would work for one DTC
    fMacAddrByte    = 0;                //

    fOnSpill        = 0;                // together: 0x0100000001
    fEventMode      = 1;

    fIsCrv          = 0;
    fCounter        = 0;
    fDtc            = new DTC(DTC_SimMode_NoCFO,PcieAddr,LinkMask,expected_version,SkipInit,sim_file,uid);
//-----------------------------------------------------------------------------
// constructor performs soft reset
// if SkipInit = true, links are not initialized
//-----------------------------------------------------------------------------
    if (SkipInit) {
                                        // get link mask from the DTC
      
      uint32_t link_mask  = ReadRegister(0x9114);
      fLinkMask = 0;
      for (int i=0; i<6; i++) {
        if ((link_mask >> i) & 0x1) {
          fLinkMask |= (0x1 << 4*i);
        }
      }
    }
    else {
      fLinkMask       = LinkMask;
    }
    
    fDtc->SoftReset();

    fSleepTimeROCWrite =  2000;
    fSleepTimeROCReset =  4000; 
    //    fDtc->ClearCFOEmulationMode();
    //    fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
  }

//-----------------------------------------------------------------------------
  DtcInterface::~DtcInterface() { }

//-----------------------------------------------------------------------------
// in many cases, want SkipInit=false
//-----------------------------------------------------------------------------
  DtcInterface* DtcInterface::Instance(int PcieAddr, uint LinkMask, bool SkipInit) {
    int pcie_addr = PcieAddr;
    if (pcie_addr < 0) {
//-----------------------------------------------------------------------------
// PCIE address is not specified, check environment
//-----------------------------------------------------------------------------
      if (getenv("DTCLIB_DTC") != nullptr) pcie_addr = atoi(getenv("DTCLIB_DTC"));
      else {
        TLOG(TLVL_ERROR) << Form("PcieAddr < 0 and $DTCLIB_DTC is not defined. BAIL out\n");
        return nullptr;
      }
    }

                                    
    TLOG(TLVL_DEBUG) << "pcie_addr:" << pcie_addr
                     << " LinkMask:0x" << std::hex << LinkMask
                     << std::dec
                     << " SkipInit:" << SkipInit << std::endl;
    
    if (fgInstance[pcie_addr] == nullptr) fgInstance[pcie_addr] = new DtcInterface(pcie_addr,LinkMask,SkipInit);
    
    if (fgInstance[pcie_addr]->PcieAddr() != pcie_addr) {
      TLOG(TLVL_ERROR) << Form("DtcInterface::Instance has been already initialized with PcieAddress = %i. BAIL out\n", 
                               fgInstance[pcie_addr]->PcieAddr());
      return nullptr;
    }
    else return fgInstance[pcie_addr];
  }

//-----------------------------------------------------------------------------
// Init Readout 
//-----------------------------------------------------------------------------
  int DtcInterface::InitReadout(int EmulateCfo, int RocReadoutMode) {
    int rc(0);

    if (EmulateCfo     != -1) fEmulateCfo     = EmulateCfo;
    if (RocReadoutMode != -1) fRocReadoutMode = RocReadoutMode;
    
    TLOG(TLVL_DEBUG) << "START : PCIE addr:" << fPcieAddr << " Emulates CFO=" << fEmulateCfo
                     << " ROC ReadoutMode:" << fRocReadoutMode; 
//-----------------------------------------------------------------------------
// both emulated and external modes perform soft reset of the DTC
//-----------------------------------------------------------------------------
    if (fEmulateCfo == 0) {
      rc = InitExternalCFOReadoutMode();
    }
    else {
//-----------------------------------------------------------------------------
// bit_30 will be restored on the 'emulated CFO side", in the call to InitEmulatedCFOReadoutMode
//-----------------------------------------------------------------------------
      rc = InitEmulatedCFOReadoutMode();
    }
    if (rc < 0) return rc;
//-----------------------------------------------------------------------------
// the DTC link mask could be reset by the previous DTC hard reset, so restore it
// also, release all buffers from the previous read - this is the initialization
//-----------------------------------------------------------------------------
    SetLinkMask();
                                        // this should do for now, later - set the partition ID
                                        // at begin run, for example, as follows

    uint8_t id           = fDtcID       & 0xff;
    uint8_t event_mode   = fEventMode   & 0xff;
    uint8_t partition_id = fPartitionID & 0xff;
    uint8_t mac_byte     = fMacAddrByte & 0xff;
    fDtc->SetEVBInfo(id,event_mode,partition_id,mac_byte);
                                           
    InitRocReadoutMode();
    fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
    
    TLOG(TLVL_DEBUG) << "PCIE addr:" << fPcieAddr << " END" << std::endl;
    return rc;
  }

//-----------------------------------------------------------------------------
// This needs to be implemented specific for the subsystems
//-----------------------------------------------------------------------------
void DtcInterface::InitRocReadoutMode() {
}

//-----------------------------------------------------------------------------
// Source=0: sync to internal clock ; =1: RTF
// on success, returns 1
//-----------------------------------------------------------------------------
  int DtcInterface::ConfigureJA(int ClockSource, int Reset) {
    int nmax_iter(10);
    
    fDtc->SetJitterAttenuatorSelect(ClockSource,Reset);     // 0:internal clock sync, 1:RTF
    usleep(100000);
    int ok(0);
    for (int i=0; i<nmax_iter; i++) {
      ok = fDtc->ReadJitterAttenuatorLocked();              // in case of success, returns true
      usleep(100000);
      if (ok == 1) break;
    }
    
    // fDtc->FormatJitterAttenuatorCSR();

    int rc = 0;
    if (ok == 0) {
      TLOG(TLVL_ERROR) << Form("failed to setup JA for ClockSource=%i and Reset=%i in %i attempts\n",
                               ClockSource,Reset,nmax_iter);
      rc = -1;
    }

    return rc;
  }

  

//-----------------------------------------------------------------------------
// according to Ryan, disabling the CFO emulation is critical, otherwise NMarkers
// would be cached for the next time
// EW length         : in units of 25 ns (clock)
// EWMOde            : 1 for buffer test
// EnableClockMarkers: set to 0
// EnableAutogenDRP  : set to 1
//-----------------------------------------------------------------------------
  int DtcInterface::InitEmulatedCFOReadoutMode() {
    //                                 int EWMode, int EnableClockMarkers, int EnableAutogenDRP) {
    int rc(0);

    TLOG(TLVL_DEBUG) << Form("START\n");

    fDtc->DisableCFOEmulation();
    fDtc->DisableReceiveCFOLink();      // r_0x9114:bit_14 = 0
                                        // this one doesn't take DTC_Link_ALL gently
    for (int i=0; i<6; i++) {
      fDtc->DisableLink(DTC_Link_ID(i),DTC_LinkEnableMode(true,true));
    }

    fDtc->DisableAutogenDRP();
    
    fDtc->SoftReset();                                             // write 0x9100:bit_31 = 1

    int clock_source = (fJAMode >> 4) & 0x1;
    int reset        = fJAMode & 0x1;
    
    rc = ConfigureJA(clock_source,reset);
    if (rc < 0) return rc;
                                        // this one is OK...
    int EnableClockMarkers = 0;
    fDtc->SetCFO40MHzClockMarkerEnable      (DTC_Link_ALL,EnableClockMarkers);

    fDtc->EnableCFOEmulatorDRP();                                  // r_0x9100:bit_24 = 1
    fDtc->EnableAutogenDRP();                                      // r_0x9100:bit_23 = 1

    fDtc->SetCFOEmulationMode();                                   // r_0x9100:bit_15 = 1

    fDtc->EnableReceiveCFOLink();                                  // r_0x9114:bit_14 = 1

    TLOG(TLVL_DEBUG) << Form("END\n");
    return rc;
  }

//-----------------------------------------------------------------------------
// example
// write value 0x10800244 to register 0x9100 - disable emulated CFO bits
// write value 0x00004141 to register 0x9114 - set link mask
// DTC doesn' know about an external CFO, so it should only prepare itself to receive 
// EVMs/HBs from the outside
//-----------------------------------------------------------------------------
  int DtcInterface::InitExternalCFOReadoutMode(int SampleEdgeMode) {
    int rc(0);

    if (SampleEdgeMode != -1) fSampleEdgeMode = SampleEdgeMode;

    TLOG(TLVL_DEBUG) << "START .. PCIE addr:" << fPcieAddr << " SampleEdgeMode:" << fSampleEdgeMode;

    // this one doesn't take DTC_Link_ALL gently
    for (int i=0; i<6; i++) {
      fDtc->DisableLink(DTC_Link_ID(i),DTC_LinkEnableMode(true,true));
    }

    // fDtc->HardReset();                  // write 0x9100:bit_00=1
    fDtc->SoftReset();                 // write 0x9100:bit_31=1   

    fDtc->DisableCFOEmulation  ();         // r_0x9100:bit_30 = 0
    fDtc->DisableCFOEmulatorDRP();         // r_0x9100:bit_24 = 0
    fDtc->DisableAutogenDRP    ();         // r_0x9100:bit_23 = 0

    // do it only when the bit is set ? 
    fDtc->ClearCFOEmulationMode();         // r_0x9100:bit_15 = 0

    int clock_source = (fJAMode >> 4) & 0x1;
    int reset        = fJAMode & 0x1;
    
    rc = ConfigureJA(clock_source,reset);
    if (rc < 0) return rc;
                                        // which ROC links should be enabled ? - all active ?
    int EnableClockMarkers = 0;         // for now
                                        // this function handles DTC_Link_ALL correctly
    fDtc->SetCFO40MHzClockMarkerEnable(DTC_Link_ALL,EnableClockMarkers);

    fDtc->SetExternalCFOSampleEdgeMode(fSampleEdgeMode);
    
    fDtc->EnableAutogenDRP();           // r_0x9100:bit_23

    // dtc->SetCFOEmulationMode();      // r_0x9100:bit_15 = 1

    // dtc->EnableCFOEmulation();       // r_0x9100:bit_30 = 1 

    fDtc->EnableReceiveCFOLink ();      // r_0x9114:bit_14 = 1

    TLOG(TLVL_DEBUG) << "END PCIE addr:" << fPcieAddr;
    return rc;
  }



    
//-----------------------------------------------------------------------------
// run plan already defined in InitEmulatedCFOReadoutMode
// this function can be executed in a loop, after InitEmulatedCFOReadoutMode
// has been executed once
//-----------------------------------------------------------------------------
  void DtcInterface::LaunchRunPlanEmulatedCfo(int EWLength, int NMarkers, int FirstEWTag) {

    fDtc->DisableCFOEmulation();
    fDtc->SoftReset();                                             // write 0x9100:bit_31 = 1

    fDtc->SetCFOEmulationEventWindowInterval(EWLength);  
    fDtc->SetCFOEmulationNumHeartbeats      (NMarkers);

    uint64_t ew_mode = EventMode();     // this really is the event mode

    fDtc->SetCFOEmulationEventMode          (ew_mode  );

    fDtc->SetCFOEmulationTimestamp          (DTC_EventWindowTag((uint64_t) FirstEWTag));

                                        // this command sends the EWM's
    fDtc->EnableCFOEmulation();         // r_0x9100:bit_30 = 1

    TLOG(TLVL_DEBUG+10) << Form("EWLength=%i NMarkers=%i FirstEWTag=%i EventMode=0x%08lx\n",
                                EWLength,NMarkers,FirstEWTag,ew_mode);
  }
    

//-----------------------------------------------------------------------------
  uint32_t DtcInterface::ReadRegister(uint16_t Register) {

    uint32_t data;
    int      timeout(150);
    
    mu2edev* dev = fDtc->GetDevice();
    dev->read_register(Register,timeout,&data);
    
    return data;
  }


//-----------------------------------------------------------------------------
// configure itself to use a CFO
//-----------------------------------------------------------------------------
  void DtcInterface::SetBit(int Register, int Bit, int Value) {
    int tmo_ms(100);

    uint32_t data;
    fDtc->GetDevice()->read_register(Register,tmo_ms,&data);
    
    uint32_t w = (1 << Bit);
    
    data = (data ^ w) | (Value << Bit);
    fDtc->GetDevice()->write_register(Register,tmo_ms,data);
  }

//-----------------------------------------------------------------------------
// by default, enable/disable both TX and RX:
// DTC_LinkEnableMode() = DTC_LinkEnableMode(true,true)
//-----------------------------------------------------------------------------
  void DtcInterface::SetLinkMask(int Mask) {
    if (Mask != 0) fLinkMask = Mask;
    
    for (int i=0; i<6; i++) {
      int used = (fLinkMask >> 4*i) & 0x1;
      if (used) fDtc->EnableLink (DTC_Link_ID(i),DTC_LinkEnableMode());
      else      fDtc->DisableLink(DTC_Link_ID(i),DTC_LinkEnableMode());
    }
  }

//-----------------------------------------------------------------------------
// configure itself to use a CFO
//-----------------------------------------------------------------------------
  void DtcInterface::SetupCfoInterface(int CFOEmulationMode, int ForceCFOEdge,
                                       int EnableCFORxTx   , int EnableAutogenDRP) {
    // int tmo_ms(150);

    if (CFOEmulationMode == 0) fDtc->ClearCFOEmulationMode();
    else                       fDtc->SetCFOEmulationMode  ();

// ForceCFOEdge: defines bit_6 and bit_5 of the control register 0x9100
// bit_6: 1:force       0:auto
// bit_5: 0:rising edge 1:falling edge
// ForceCFOEdge = 0 : force use of the rising  edge
//              = 1 : force use of the falling edge
//              = 2 : auto

    fDtc->SetExternalCFOSampleEdgeMode(ForceCFOEdge);

    if (EnableCFORxTx == 0) {
      fDtc->DisableReceiveCFOLink ();
      fDtc->DisableTransmitCFOLink();
    }
    else {
      fDtc->EnableReceiveCFOLink  ();
      fDtc->EnableTransmitCFOLink ();
    }
    
    if (EnableAutogenDRP == 0) fDtc->DisableAutogenDRP();
    else                       fDtc->EnableAutogenDRP ();
  }



  // This is just an example, needs to be implemented for each subsystem
  std::vector<std::string> DtcInterface::GetRocRegistersNames(bool history = false) {
    std::vector<std::string> registers;
    // Basic ROC registers
    if(history) {
      registers.push_back("ROC_HIST_1");
      registers.push_back("ROC_HIST_2");
      registers.push_back("ROC_HIST_3");
    } else {
      registers.push_back("ROC_NON-HIST_1");
      registers.push_back("ROC_NON-HIST_2");
      registers.push_back("ROC_NON-HIST_3");
      registers.push_back("ROC_NON-HIST_3");
    }
    return registers;
  }

  // This is just an example, needs to be implemented for each subsystem
  std::vector<uint32_t> DtcInterface::GetRocRegisters(int ilink, bool history = false) {
    std::vector<uint32_t> val;
    // Basic ROC registers
    if(history) {
      val.push_back(42);
      val.push_back(fCounter);
      val.push_back(ilink);
      fCounter++;
    } else {
      val.push_back(fCounter);
      val.push_back(42);
      val.push_back(ilink);
      val.push_back(fCounter & 0xfe);
    }
    return val;
  }

  // This is just an example, needs to be implemented for each subsystem
  std::vector<float> DtcInterface::GetConvertedRocRegisters(int ilink, bool history = false) {
    auto registers = GetRocRegisters(ilink, history);
    return std::vector<float>(registers.begin(), registers.end());
  }

};
#endif
