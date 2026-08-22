//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __mu2edaq_dtc_interface_cc__
#define __mu2edaq_dtc_interface_cc__

#include "iostream"
#include "vector"

#include "DtcInterfaceBase.hh"
#include "TString.h"             // includes ROOT's Form
#include "TInterpreter.h"
#include "TROOT.h"
#include "TSystem.h"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterfaceBase"

using namespace DTCLib;
using namespace std;

namespace mu2edaq {
  
  DtcInterfaceBase* DtcInterfaceBase::fgInstance[2] = {nullptr, nullptr};
  
//-----------------------------------------------------------------------------
  DtcInterfaceBase::DtcInterfaceBase(int PcieAddr, uint LinkMask, bool SkipInit) {
    std::string expected_version("");              // dont check
    std::string sim_file        ("mu2esim.bin");
    std::string uid             ("");
      
    TLOG(TLVL_DEBUG) << "CONSTRUCT DTC: pcie_addr:" << PcieAddr
                          << " LinkMask:0x" << std::hex << LinkMask
                          << " SkipInit:"   << std::dec << SkipInit;
    fEnabled         = 1;                // default: enabled
    fPcieAddr        = PcieAddr;
    fSampleEdgeMode  = 0x11;             // change from 1 
    fEmulateCfo      = 0;
    fJAMode          = 0x11;             // by default, assume RTF clock and reset upon setting
    fEnableClockMarkers = 1;  // CRV will redefine
    
    fDtcID          = 0;                // needed for multi-DTC DAQ, default:0
    fPartitionID    = 0;                // use reasonable defaults, which would work for one DTC
    fMacAddrByte    = 0;                //
                                        // set initial delays to zero
    for (int i=0; i<6; i++) {
      fRocEwmDelay5ns[i]  = 0;
    }
    //                                     // forcing that to be set
    // fDigitizationStart5ns = 0;
    // fDigitizationStop5ns  = -1;

    fOnSpill        = 0;                // together: 0x0100000001
    fEventMode      = 1;

    //    fIsCrv          = 0;
    fSubsystem      = -1;
    fCounter        = 0;
    fDtc            = new DTC(DTC_SimMode_Disabled,PcieAddr,LinkMask,expected_version,SkipInit,sim_file,uid);
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
        fLinkStatus[i] = 0;             // initially
      }
    }
    else {
      fLinkMask       = LinkMask;
      fDtc->SoftReset();
    }
    
    fSleepTimeROCWrite =  2000;
    fSleepTimeROCReset =  10000; // 4000 
    //    fDtc->ClearCFOEmulationMode();
    fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
  }

//-----------------------------------------------------------------------------
  DtcInterfaceBase::~DtcInterfaceBase() { }

  // NEEDS TO BE IMPLEMNTED IN CLASSES THAT INHERIT
  DtcInterfaceBase* DtcInterfaceBase::Instance(int PcieAddr, uint LinkMask, bool SkipInit) {
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

    TLOG(TLVL_DEBUG) << "pcie_addr:"   <<             pcie_addr
                     << " LinkMask:0x" << std::hex << LinkMask
                     << " SkipInit:"   << std::dec << SkipInit;
    
    if (fgInstance[pcie_addr] == nullptr) fgInstance[pcie_addr] = new DtcInterfaceBase(pcie_addr,LinkMask,SkipInit);
    
    if (fgInstance[pcie_addr]->PcieAddr() != pcie_addr) {
      TLOG(TLVL_ERROR) << Form("DtcInterfaceBase::Instance has been already initialized with PcieAddress = %i. BAIL out\n", 
                               fgInstance[pcie_addr]->PcieAddr());
      return nullptr;
    }
    else return fgInstance[pcie_addr];
  }



  //-----------------------------------------------------------------------------
  int DtcInterfaceBase::ReadCfoLinkStatus(int PrintLevel, std::ostream& Stream) {
    int rc(0);

    TLOG(TLVL_DEBUG) << std::format("-- START");

    // read the CFO link status register
    uint32_t r9398 = ReadRegister(0x9398);
        
    // 3 low bits are then the RTF offset
    // skip bit 11 (RTF phase)
    
    uint32_t rx_tx_err  = (r9398 >> 13) & 0x1;
    uint32_t marker_err = (r9398 >>  9) & 0x3;   // bits 9 and 10
        
    uint32_t cfo_cdc_diag = ReadRegister(0x9688);
        
    uint32_t batch_slip_cnt      = (cfo_cdc_diag      ) & 0xffff;
    uint32_t parity_mismatch_cnt = (cfo_cdc_diag >> 16) & 0xffff;

    std::string msg = std::format("DTC:{} r_0x9398: 0x{:08x} rx_tx_err:{} marker_err:0x{:02x} batch_slip_cnt:{} parity_mismatch_cnt:{}\n",
                                  PcieAddr(),
                                  r9398,
                                  rx_tx_err, marker_err, batch_slip_cnt, parity_mismatch_cnt);
    TLOG(TLVL_DEBUG) << msg;

    if (PrintLevel != 0) { 
      Stream << msg;
    }

    if (rx_tx_err or marker_err or (batch_slip_cnt > 0) or (parity_mismatch_cnt > 0)) {
      rc = -1;
    }
    
    TLOG(TLVL_DEBUG) << std::format("-- END rc:{}",rc);
    return rc;
  }
  
//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------
  int DtcInterfaceBase::SetupCfoLink(int PrintLevel, std::ostream& Stream) {
    int rc(0);

    TLOG(TLVL_DEBUG) << std::format("-- START");

    rc = ReadCfoLinkStatus(PrintLevel,Stream);
    
    if (rc < 0) { 
      // need to flip the edge
      uint32_t r9100 = ReadRegister(0x9100);
      int bit5       = (r9100 >> 5) & 0x1;
      SetBit(0x9100,5,1-bit5);
      usleep(200000);

      fDtc->SoftReset();
    
      uint32_t new_r9100 = ReadRegister(0x9100);

      std::string msg = std::format("DTC:{} edge flipped from:{} to:{}, new r0x9100:0x{:04x}",
                                        PcieAddr(),bit5,1-bit5,new_r9100);
      TLOG(TLVL_WARNING) << msg;
      
      if (PrintLevel != 0) {
        Stream << msg;
      }
//-----------------------------------------------------------------------------
// after flipping, re-read and check status again
//-----------------------------------------------------------------------------
      rc = ReadCfoLinkStatus(PrintLevel,Stream);
    }
      
    TLOG(TLVL_DEBUG) << std::format("-- END rc:{}",rc);
    return rc;
  }
//-----------------------------------------------------------------------------
// clear status words of all links
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::ClearLinkStatus(int Link) {
    int rc(0), lnk1(Link), lnk2(Link+1);

    if (Link == -1) { lnk1 = 0; lnk2 = 6; }
    
    for (int lnk=lnk1; lnk<lnk2; lnk++) {
      fLinkStatus[lnk] = 0;
    }
    return rc;
  }
  
//-----------------------------------------------------------------------------
// Source=0: sync to internal clock ; =1: RTF
// ClockSource and Reset do no need to be cached
// on success, returns 0
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::ConfigureJA(int ClockSource, int Reset, std::ostream& Stream) {
    int rc(0);
    
    int nmax_iter(10);
    int clock_source(ClockSource), reset(Reset);
    
    if (reset        == -1) reset        = (fJAMode     ) & 0xf;
    if (clock_source == -1) clock_source = (fJAMode >> 4) & 0xf;
    
    TLOG(TLVL_DEBUG) << std::format("-- START: PCIE:{} clock_source:{} reset:{}",fPcieAddr,clock_source,reset);
    
    fDtc->SetJitterAttenuatorSelect(clock_source,reset);    // 0:internal clock sync, 1:RTF
    usleep(100000);
    bool ok(false);
    for (int i=0; i<nmax_iter; i++) {
      try {
        ok = fDtc->ReadJitterAttenuatorLocked();              // in case of success, returns true
        TLOG(TLVL_DEBUG) << std::format("iter:{} ok:{}",i,ok);
      }
      catch (...) {
        rc = -2;
        std::string msg = std::format("iter:{} failed to >ReadJitterAttenuatorLocked(), rc:{}",i,rc);
        TLOG(TLVL_ERROR) << msg;
        
        Stream << std::format("ERROR: {} rc:{}\n",msg,rc);
        return rc;
      }

      TLOG(TLVL_DEBUG) << std::format("ok:{}",ok);
      
      if (ok) {
        std::string msg = std::format("JA configured with JAMode:0x{:02x}",fJAMode);
        // Stream << std::format("{}\n",msg);
        TLOG(TLVL_DEBUG) << std::format("-- END  : commented out streaming {} rc:{}",msg,rc);
        return rc;
      }
      usleep(100000);
    }
    
    TLOG(TLVL_ERROR) << std::format("failed to configure JA for clock_source={} and reset={} in {} attempts",
                                    clock_source,reset,nmax_iter);
    return -1;
  }

//-----------------------------------------------------------------------------
// 1) first check for project name like "pasha/mu2edaq09_pcie0"
// if file config/pasha/mu2edaq09_pcie0.C exists , use that
// 2) otherwise assume config file name config/$project/$hostname.C
// config file should contain function init_run_configuration(DtcGui*)
//
// assumes that MU2E_DAQ_DIR points to the directory from where root is started
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::InitConfiguration(const char* ConfigName, int DeviceID, mu2edaq::DtcInputData_t* DtcData) {
    int           rc(0);

    TLOG(TLVL_DEBUG+1) << std::format("-- START");

    TInterpreter* cint = gROOT->GetInterpreter();
    
    TInterpreter::EErrorCode irc;
  
    std::string macro = Form("%s/config/dtc_gui/%s.C",gSystem->Getenv("MU2E_DAQ_DIR"),ConfigName);
    FILE* f = fopen(macro.data(),"r");
    if (f == nullptr) {
      char buf[128];
      gethostname(buf,128);
      std::string hn = buf;
      std::string hostname = hn.substr(0,hn.find('.'));
      macro = std::format("{}/config/dtc_gui/{}.C",gSystem->Getenv("MU2E_DAQ_DIR"),ConfigName);
      f     = fopen(macro.data(),"r");
      if (f == nullptr) {
        TLOG(TLVL_ERROR) << std::format("failed to find config {}, EXIT.",macro);
        rc = -1;
        return rc;
      }
    }
    
    if (not cint->IsLoaded(macro.data())) {
      TLOG(TLVL_DEBUG+1) << std::format(" loading configuration from {}",macro);
      cint->LoadMacro(macro.data(), &irc);
      rc = irc;
    }
    else {
      TLOG(TLVL_DEBUG+1) << std::format(" configuration from {} is already loaded",macro);
    }

    TLOG (TLVL_DEBUG+1) << std::format("rc:{}",rc);
    if (rc != 0) return rc;
    
    std::string cmd = std::format("init_run_configuration((mu2edaq::DtcInputData_t*) 0x{:08x},{:d});",(long int) DtcData,DeviceID);
    
    TLOG(TLVL_DEBUG+1) << std::format("cmd:{}",cmd);
    
    gInterpreter->ProcessLine(cmd.data(),&irc);
    
    TLOG(TLVL_DEBUG+1) << std::format("-- END irc:{}",(uint32_t) irc);
    return irc;
  }

//-----------------------------------------------------------------------------
// according to Ryan, disabling the CFO emulation is critical, otherwise NMarkers
// would be cached for the next time
// EW length         : in units of 25 ns (clock)
// EWMOde            : 1 for buffer test
// EnableClockMarkers: set to 0
// EnableAutogenDRP  : set to 1
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::InitEmulatedCFOReadoutMode(std::ostream& Stream) {
    int rc(0);

    TLOG(TLVL_DEBUG) << Form("-- START");
//-----------------------------------------------------------------------------
// if requested, configure the jitter attenuator, then do soft reset
//-----------------------------------------------------------------------------
    rc = ConfigureJA(-1,-1,Stream);
    if (rc < 0) {
      TLOG(TLVL_ERROR) << std::format("PCIE:{} failed to configure the JA, rc:{}. BAIL OUT",fPcieAddr,rc);
      return rc;
    }
//-----------------------------------------------------------------------------
// after setting the JA, perform a "soft reset"  (2026-01-29 , suggested by Ryan)
//-----------------------------------------------------------------------------
    fDtc->SoftReset();                                             // write 0x9100:bit_31 = 1
//-----------------------------------------------------------------------------
// in a clean state. disable links - CFO, hardware EVB, and ROCs
//-----------------------------------------------------------------------------
    fDtc->DisableReceiveCFOLink();      // r_0x9114:bit_14 = 0
    fDtc->DisableLink(DTCLib::DTC_Link_EVB);
                                        // this one doesn't take DTC_Link_ALL gently
    for (int i=0; i<6; i++) {
      fDtc->DisableLink(DTC_Link_ID(i),DTC_LinkEnableMode(true,true));
    }

    fDtc->DisableCFOEmulation();                                   // r_0x9100:bit_30 = 0
    fDtc->DisableAutogenDRP();
    
    fDtc->EnableReceiveCFOLink();                                  // r_0x9114:bit_14 = 1
                                                                   // this one is OK...
    int enable_clock_markers = 0;
    fDtc->SetCFO40MHzClockMarkerEnable(DTC_Link_ALL,enable_clock_markers);

    fDtc->EnableCFOEmulatorDRP();                                  // r_0x9100:bit_24 = 1
    fDtc->EnableAutogenDRP();                                      // r_0x9100:bit_23 = 1

    fDtc->SetCFOEmulationMode();                                   // r_0x9100:bit_15 = 1
    
    int force_cfo_edge = 0x0;                                      // two bits matter
    fDtc->SetExternalCFOSampleEdgeMode(force_cfo_edge);            // r_0x9100:bit6 = 0 bit_5=0
    fDtc->EnableTransmitCFOLink();                                 // r_0x9114:bit_06 = 1
//-----------------------------------------------------------------------------
// bit_30 will be restored on the 'emulated CFO side", in the call to InitEmulatedCFOReadoutMode
// 0x9100:bit_30 is still zero, it looks that it enables sending markers immediately
// and is set in LaunchRunPlanEmulatedCfo
// ROC links are still disabled at this point, re-enabled later, in InitReadout()
// set the partition ID etc bytes
//-----------------------------------------------------------------------------
    uint8_t id           = fDtcID       & 0xff;
    uint8_t event_mode   = fEventMode   & 0xff;
    uint8_t partition_id = fPartitionID & 0xff;
    uint8_t mac_byte     = fMacAddrByte & 0xff;
    fDtc->SetEVBInfo(id,event_mode,partition_id,mac_byte);

    fDtc->ResetSERDESRX(DTC_Link_ID::DTC_Link_ALL);
    fDtc->ResetSERDESTX(DTC_Link_ID::DTC_Link_ALL);
//-----------------------------------------------------------------------------
// at this point the ROC links should still be disabled,
// re-enable the links and reset the ROCs
//-----------------------------------------------------------------------------
    rc = ResetLinks();

    TLOG(TLVL_DEBUG) << "-- END, rc:" << rc;
    return rc;
  }

//-----------------------------------------------------------------------------
// example
// write value 0x10800244 to register 0x9100 - disable emulated CFO bits
// write value 0x00004141 to register 0x9114 - set link mask
// DTC doesn' know about an external CFO, so it should only prepare itself to receive 
// EVMs/HBs from the outside
// SampleEdgeMode=0: force rising  edge
//                1: force falling edge
//                2: auto
// -1 means use the pre-fetched one
// success: returns rc=0
// if rc < 0, can't continue
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::InitExternalCFOReadoutMode(std::ostream& Stream) {
    int rc(0);

    TLOG(TLVL_DEBUG) << "-- START: .. PCIE addr:" << fPcieAddr << " SampleEdgeMode:" << fSampleEdgeMode;

    fDtc->SoftReset();
    fDtc->ClearControlRegister();
//-----------------------------------------------------------------------------
// if requested, configure the jitter attenuator, then do soft reset
//-----------------------------------------------------------------------------
    rc = ConfigureJA(-1,-1,Stream);
    if (rc < 0) {
      TLOG(TLVL_ERROR) << std::format("PCIE:{} failed to configure the JA, rc:{}. BAIL OUT",fPcieAddr,rc);
      return rc;
    }
//-----------------------------------------------------------------------------
// after setting the JA, perform a "soft reset"  (2026-01-29 , suggested by Ryan)
//-----------------------------------------------------------------------------
    fDtc->SoftReset();
//-----------------------------------------------------------------------------
// in a clean state. disable links - CFO, hardware EVB, and ROCs
//-----------------------------------------------------------------------------
//    fDtc->DisableReceiveCFOLink();      // r_0x9114:bit_14 = 0
//    fDtc->EnableLink(DTCLib::DTC_Link_CFO); // 2026-08-15 : is this a typo? why enabling the CFO link ?
    fDtc->DisableLink(DTCLib::DTC_Link_CFO);
    fDtc->DisableLink(DTCLib::DTC_Link_EVB);
                                        // this one doesn't take DTC_Link_ALL gently
    for (int i=0; i<6; i++) {
      fDtc->DisableLink(DTC_Link_ID(i),DTC_LinkEnableMode(true,true));
    }

    // fDtc->DisableCFOEmulation  ();      // r_0x9100:bit_30 = 0
    // fDtc->DisableCFOEmulatorDRP();      // r_0x9100:bit_24 = 0
    // fDtc->DisableAutogenDRP    ();      // r_0x9100:bit_23 = 0
    // fDtc->ClearCFOEmulationMode();      // r_0x9100:bit_15 = 0
    
                                        // which ROC links should be enabled ? - all active ?
                                        // this function handles DTC_Link_ALL correctly

    fDtc->ResetSERDESRX(DTC_Link_ID::DTC_Link_ALL);
    fDtc->ResetSERDESTX(DTC_Link_ID::DTC_Link_ALL);
    fDtc->ResetSERDES  (DTC_Link_ID::DTC_Link_ALL);

    usleep(100);
    fDtc->SoftReset();

                                        // start setting bits

    fDtc->EnableAutogenDRP();           // r_0x9100:bit_23
    fDtc->EnableDCSReception();         // r_0x9100:bit_02
    fDtc->DisableCFOLoopback();         // r_0x9100:bit_28

    fDtc->SetExternalCFOSampleEdgeMode(fSampleEdgeMode);
    if (fSubsystem == kCRV) {
      fDtc->SetPunchEnable();     // need only for CRV
    }
//-----------------------------------------------------------------------------
// set the partition ID etc bytes
//-----------------------------------------------------------------------------
    uint8_t id           = fDtcID       & 0xff;
    uint8_t event_mode   = fEventMode   & 0xff;
    uint8_t partition_id = fPartitionID & 0xff;
    uint8_t mac_byte     = fMacAddrByte & 0xff;
    fDtc->SetEVBInfo(id,event_mode,partition_id,mac_byte);

    fDtc->EnableReceiveCFOLink ();      // r_0x9114:bit_14 = 1   (2026-08-15 - enabled before)
    fDtc->EnableTransmitCFOLink();      // r_0x9114:bit_06 = 1 (if the dTC is in the middle of the chain)
//-----------------------------------------------------------------------------
// at this point the links should still be disabled,
// re-enable the links and reset the ROCs
//-----------------------------------------------------------------------------
    rc = ResetLinks();
//-----------------------------------------------------------------------------
// enable clock markers to enabled links, CRV doesn't that
//-----------------------------------------------------------------------------
    if (fEnableClockMarkers) {
      int enable_clock_markers = 1 ; // 2026-08-15 // 0;       // for now
      for (int i=0; i<6; i++) {
        if (LinkEnabled(i)) {
          fDtc->SetCFO40MHzClockMarkerEnable(DTC_Link_ALL,enable_clock_markers);
        }
      }
    }
   

    TLOG(TLVL_DEBUG) << std::format("-- END  : PCIE addr:{} rc:{}",fPcieAddr,rc);
    return rc;
  }

//-----------------------------------------------------------------------------
// InitReadout : in most cases, no parameters
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::InitReadout(int EmulateCfo, int RocReadoutMode, std::ostream& Stream) {
    int rc(0);

    if (EmulateCfo     != -1) fEmulateCfo     = EmulateCfo;
    if (RocReadoutMode != -1) fRocReadoutMode = RocReadoutMode;
    
    TLOG(TLVL_DEBUG) << "-- START : PCIE addr:" << fPcieAddr << " EmulateCFO=" << fEmulateCfo
                     << " ROC ReadoutMode:" << fRocReadoutMode;
    
//-----------------------------------------------------------------------------
// both emulated and external modes DO NOT perform soft reset of the DTC
//-----------------------------------------------------------------------------
    if (fEmulateCfo == 0) rc = InitExternalCFOReadoutMode(Stream);
    else                  rc = InitEmulatedCFOReadoutMode(Stream);

    if (rc < 0) {
      TLOG(TLVL_ERROR) << std::format("failure to initialize the CFO readout mode, rc:{}. BAIL OUT",rc);
      return rc;
    }

    rc = InitRocReadoutMode(Stream);  // this one is virtual
    if (rc < 0) {
      TLOG(TLVL_ERROR) << std::format("failure in InitRocReadoutMode, rc:{}. BAIL OUT",rc);
      return rc;
    }

    try {
      fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
    }
    catch (...) {
      rc = -10;
      TLOG(TLVL_ERROR) << std::format("failed to release bufferse, rc:{}. BAIL OUT",rc);
    }
    
    TLOG(TLVL_DEBUG) << "-- END rc:" << rc;
    return rc;
  }

//-----------------------------------------------------------------------------
// This needs to be implemented specific for the subsystems
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::InitRocReadoutMode(std::ostream& Stream) {
    return 0;
  }

//-----------------------------------------------------------------------------
// run plan already defined in InitEmulatedCFOReadoutMode
// this function can be executed in a loop, after InitEmulatedCFOReadoutMode
// has been executed once
//-----------------------------------------------------------------------------
  void DtcInterfaceBase::LaunchRunPlanEmulatedCfo(int EWLength, int NMarkers, int FirstEWTag) {

    TLOG(TLVL_DEBUG+1) << "--- START";
    
    fDtc->DisableCFOEmulation();
    fDtc->SoftReset();                                             // write 0x9100:bit_31 = 1

    fDtc->SetCFOEmulationEventWindowInterval(EWLength);  
    fDtc->SetCFOEmulationNumHeartbeats      (NMarkers);

    uint64_t ew_mode = EventMode();     // this really is the event mode

    fDtc->SetCFOEmulationEventMode          (ew_mode  );

    fDtc->SetCFOEmulationTimestamp          (DTC_EventWindowTag((uint64_t) FirstEWTag));

                                        // this command sends the EWM's by setting bit30 high
    fDtc->EnableCFOEmulation();         // r_0x9100:bit_30 = 1 - this immediately start sending markers

    TLOG(TLVL_DEBUG+1) << Form("EWLength=%i NMarkers=%i FirstEWTag=%i EventMode=0x%08lx\n",
                                EWLength,NMarkers,FirstEWTag,ew_mode);
    TLOG(TLVL_DEBUG+1) << "--- END";
  }
    

//-----------------------------------------------------------------------------
  uint32_t DtcInterfaceBase::ReadRegister(uint16_t Register) {
    int tmo_ms(100);

    uint32_t data(0);
    int      timeout(150);
    
    mu2edev* dev = fDtc->GetDevice();
    try {
      dev->read_register(Register,timeout,&data);
    }
    catch (...) {
      TLOG(TLVL_ERROR) << std::format("DTC:{} read of reg 0x{:04x} failed, timeout was set at {} ms",
                                      PcieAddr(), Register, tmo_ms);
      data = 0;
    }
    
    return data;
  }

//-----------------------------------------------------------------------------
  int DtcInterfaceBase::LinkLocked(int Link) {
    uint32_t dat = ReadRegister(0x9140);
    return (dat >> Link) & 0x1;
  }

//-----------------------------------------------------------------------------
// update the link mask, then reset the enabled ROC's
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::ResetLinks(int LinkMask, int SetNewMask) {
    int rc(0);
    TLOG(TLVL_DEBUG+1) << "-- START:";
    if ((LinkMask != 0) and (SetNewMask != 0)) fLinkMask = LinkMask;

    SetLinkMask();
    
    for (int i=0; i<6; i++) {
      if (LinkEnabled(i)) {
        int ret = ResetLink(i);   // actually reset the ROC, this function is virtual and subdetector-specific
        rc += ret;
      }
    }
    TLOG(TLVL_DEBUG+1) << std::format("-- END  : rc:{}",rc);
    return rc;
  }

//-----------------------------------------------------------------------------
// default implementation is empty, returns 0 or -1
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::ResetLink(int Link) {
    return 0;
  }

//-----------------------------------------------------------------------------
// configure itself to use a CFO
//-----------------------------------------------------------------------------
  void DtcInterfaceBase::SetBit(int Register, int Bit, int Value) {
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
  void DtcInterfaceBase::SetLinkMask(int Mask) {
    if (Mask != 0) fLinkMask = Mask;
    
    for (int i=0; i<6; i++) {
      int enabled= (fLinkMask >> 4*i) & 0x1;
      if (enabled) fDtc->EnableLink (DTC_Link_ID(i),DTC_LinkEnableMode());
      else         fDtc->DisableLink(DTC_Link_ID(i),DTC_LinkEnableMode());
    }
  }

//-----------------------------------------------------------------------------
// configure itself to use a CFO
//-----------------------------------------------------------------------------
  void DtcInterfaceBase::SetupCfoInterface(int CFOEmulationMode, int ForceCFOEdge,
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
  std::vector<std::string> DtcInterfaceBase::GetRocRegistersNames(bool history) {
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
  std::vector<uint32_t> DtcInterfaceBase::GetRocRegisters(int ilink, bool history) {
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
  std::vector<float> DtcInterfaceBase::GetConvertedRocRegisters(int ilink, bool history) {
    auto registers = GetRocRegisters(ilink, history);
    return std::vector<float>(registers.begin(), registers.end());
  }



//-----------------------------------------------------------------------------
// tracker ROC reset : write 0x1 to register 14
// if Fn = "", don't write the output file
//-----------------------------------------------------------------------------
  int DtcInterfaceBase::ReadSubevents(std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& VSub, 
                                  ulong             FirstEWT   ,
                                  int               PrintLevel ,
                                  std::ostream&     Stream     ,
                                  int               Validation ,
                                  const std::string Fn         ) {
    int rc(0);
    
    TLOG(TLVL_DEBUG+1) << std::format("-- START");
    ulong    ewt      = FirstEWT;
    bool     match_ts = false;
    int      nerr_tot  (0);
    ulong    nbytes_tot(0);
    ulong    offset    (0);               // used in validation mode
    int      nerr_roc[6], nerr_roc_tot[6];

    FILE*    file(nullptr);
    
    if (Fn != "") {
//-----------------------------------------------------------------------------
// check if Fn exists 
//-----------------------------------------------------------------------------
      if((file = fopen(Fn.data(),"r")) != NULL) {
        // file exists
        fclose(file);
        TLOG(TLVL_ERROR) << "file " << Fn << " already exists, BAIL OUT";
        return -1;
      }
      else {
//-----------------------------------------------------------------------------
// Fn doesn't exist, open it 
//-----------------------------------------------------------------------------
        TLOG(TLVL_DEBUG+1) << std::format("opening output binary file {}",Fn);
        file = fopen(Fn.data(),"w");
        if (file == nullptr) {
          TLOG(TLVL_ERROR) <<  "failed to open " << Fn << " , BAIL OUT";
          return -2;
        }
      }
    }
//-----------------------------------------------------------------------------
// reset per-roc error counters
//-----------------------------------------------------------------------------
    for (int i=0; i<6; i++) {
      nerr_roc    [i] = 0;
      nerr_roc_tot[i] = 0;
    }
//-----------------------------------------------------------------------------
// always read an event into the same external buffer (VSub), 
// so no problem with the memory management
//-----------------------------------------------------------------------------
    int header_printed = 0;
    while(1) {
      // sleep(1);
      DTC_EventWindowTag event_tag = DTC_EventWindowTag(ewt);
      try {
        if (PrintLevel > 0) {
//-----------------------------------------------------------------------------
// print header
//-----------------------------------------------------------------------------
          if ((Validation and PrintLevel > 1) or (header_printed == 0)) {
            Stream << Form("      event  DTC     EW Tag nbytes   nbytes_tot  link0   nb0  link1   nb1  link2   nb2  link3   nb3  link4   nb4  link5   nb5  nerr nerr_tot\n");
            Stream << Form("--------------------------------------------------------------------------------------------------------------------------------------------\n");
            header_printed = 1;
          }
        }
        VSub   = fDtc->GetSubEventData(event_tag, match_ts);
        int sz = VSub.size();
        if (sz == 0) {
          if (PrintLevel > 0) {
            Stream << Form(">>>> ------- ewt = %5li NDTCs:%2i END_OF_DATA\n",ewt,sz);
          }
          break;
        }
//-----------------------------------------------------------------------------
// a subevent contains data of a single DTC
//-----------------------------------------------------------------------------
        int rs[6];
        std::vector<uint8_t> dtc_block;
        
        for (int i=0; i<sz; i++) {
          DTC_SubEvent* ev  = VSub[i].get();
          uint64_t ew_tag   = ev->GetEventWindowTag().GetEventWindowTag(true);
          char*    raw_data = (char*) ev->GetRawBufferPointer();

          int      nbytes  = ev->GetSubEventByteCount();
//-----------------------------------------------------------------------------
// create a local copy of the DTC data block
//-----------------------------------------------------------------------------
          dtc_block.reserve(nbytes);
          memcpy(dtc_block.data(),raw_data,nbytes);

          nbytes_tot += nbytes;

          int nerr(0);
          
          if (Validation > 0) {
            nerr = Validate((ushort*) dtc_block.data(),ew_tag,&offset,PrintLevel,nerr_roc);
              
            nerr_tot += nerr;
            for (int ir=0; ir<6; ir++) nerr_roc_tot[ir] += nerr_roc[ir];
          }

          uint8_t* roc_data  = dtc_block.data()+0x30;

          int nb_roc[6];
          for (int roc=0; roc<6; roc++) {
            nb_roc[roc] = *((ushort*) roc_data);
            rs[roc]     = *((ushort*)(roc_data+0x0c));
            roc_data   += nb_roc[roc];
          }
        
          if (PrintLevel > 0) {
            Stream << Form(" %10li  %2i  %10li %5i %13li 0x%04x %5i 0x%04x %5i 0x%04x %5i 0x%04x %5i 0x%04x %5i 0x%04x %5i %5i %8i %4i %4i %4i %4i %4i %4i\n",
                           ewt,i,ew_tag,nbytes,nbytes_tot,
                           rs[0],nb_roc[0],rs[1],nb_roc[1],rs[2],nb_roc[2],rs[3],nb_roc[3],rs[4],nb_roc[4],rs[5],nb_roc[5],
                           nerr,nerr_tot,
                           nerr_roc[0],nerr_roc[1],nerr_roc[2],nerr_roc[3],nerr_roc[4],nerr_roc[5] );
            if (((nerr > 0) and (PrintLevel > 1)) or (PrintLevel > 2)) {
              PrintBuffer(ev->GetRawBufferPointer(),ev->GetSubEventByteCount()/2,0x0,Stream);
            }
          }
          
          if (file) {
//-----------------------------------------------------------------------------
// write event to output file
//-----------------------------------------------------------------------------
            int nbb = fwrite(dtc_block.data(),1,nbytes,file);
            if (nbb == 0) {
              TLOG(TLVL_ERROR) << Form("failed to write event %10li , close file and BAIL OUT\n",ew_tag);
              fclose(file);
              return -3;
            }
          }
        }
        
        ewt++;                          // event in sequence
      }
      catch (...) {
        TLOG(TLVL_ERROR) << std::format("error reading event_tag:{} ewt:{}",event_tag.GetEventWindowTag(true),ewt);
        break;
      }
    }

    //    fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
//-----------------------------------------------------------------------------
// print summary
//-----------------------------------------------------------------------------
    ulong nev = ewt-FirstEWT;
    TLOG(TLVL_DEBUG+1) << Form("nevents: %10li nbytes_tot: %13li Validation:%i\n",nev, nbytes_tot,Validation)
                       << Form("nerr_tot:%10i nerr_roc_tot: %8i %8i %8i %8i %8i %8i\n",
                               nerr_tot,
                               nerr_roc_tot[0],nerr_roc_tot[1],nerr_roc_tot[2],
                               nerr_roc_tot[3],nerr_roc_tot[4],nerr_roc_tot[5]);
//-----------------------------------------------------------------------------
// to simplify first steps, assume that in a file writing mode all events 
// are read at once, so close the file on exit
//-----------------------------------------------------------------------------
    if (file) {
      fclose(file);
    }
    TLOG(TLVL_DEBUG+1) << std::format("-- END: rc:{}",rc);
    return rc;
  }


//-----------------------------------------------------------------------------
// to be overriden in derived subdetector-specific classes
// ROC ID, ROC firmware ID , and the corresponding git commit
//-----------------------------------------------------------------------------
  std::string  DtcInterfaceBase::GetRocID         (int Link) { return std::string("undefined"); }
  std::string  DtcInterfaceBase::GetRocDesignInfo (int Link) { return std::string("undefined"); }
  std::string  DtcInterfaceBase::GetRocFwGitCommit(int Link) { return std::string("undefined"); }

//-----------------------------------------------------------------------------
// to be overloaded, does nothing...
//-----------------------------------------------------------------------------
  int  DtcInterfaceBase::Validate(ushort* Data, uint64_t EwTag, uint64_t* Offset, int PrintLevel, int* NErrRoc) {
    for (int i=0; i<6; i++) {
      NErrRoc[i] = 0;
    }
    return 0;
  }

};
#endif
