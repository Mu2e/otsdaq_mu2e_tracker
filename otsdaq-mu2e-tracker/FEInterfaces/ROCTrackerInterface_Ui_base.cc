
// clang-format off

//-----------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//-----------------------------------------------------------------------------


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form

#include <filesystem>


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"


//==============================================================================
///	Ui_base_InitReadout()
/// InitReadout : in most cases, no parameters
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_InitReadout(int EmulateCfo, int RocReadoutMode, std::ostream* Stream)
{
    int rc(0);

    if (EmulateCfo     != -1) fEmulateCfo     = EmulateCfo;
    if (RocReadoutMode != -1) fRocReadoutMode = RocReadoutMode;

    TLOG(TLVL_DEBUG) << "-- START : PCIE addr:" << fPcieAddr << " EmulateCFO=" << fEmulateCfo
                     << " ROC ReadoutMode:" << fRocReadoutMode;

    getDTC()->SoftReset();  // 2026-01-29 , suggested by Ryan
//-----------------------------------------------------------------------------
// both emulated and external modes perform soft reset of the DTC
//-----------------------------------------------------------------------------
    if (fEmulateCfo == 0) {
      rc = Ui_base_InitExternalCFOReadoutMode();
    }
    else {
//-----------------------------------------------------------------------------
// bit_30 will be restored on the 'emulated CFO side", in the call to Ui_base_InitEmulatedCFOReadoutMode
//-----------------------------------------------------------------------------
      rc = Ui_base_InitEmulatedCFOReadoutMode();
    }
    if (rc < 0) return rc;
//-----------------------------------------------------------------------------
// both of Init_XX_CFOReadoutMode disable all links, need to re-enable
// do we need to reset the ROCs at this point ?
//-----------------------------------------------------------------------------
    rc = Ui_base_ResetLinks();
    if (rc < 0) return rc;
                                        // this should do for now, later - set the partition ID
                                        // at begin run, for example, as follows

    uint8_t id           = fDtcID       & 0xff;
    uint8_t event_mode   = fEventMode   & 0xff;
    uint8_t partition_id = fPartitionID & 0xff;
    uint8_t mac_byte     = fMacAddrByte & 0xff;
    getDTC()->SetEVBInfo(id,event_mode,partition_id,mac_byte);
//-----------------------------------------------------------------------------
// Init*CFOReadoutMode functions disable all links, at this point the links
// should still be disabled
//-----------------------------------------------------------------------------
    rc = Ui_InitRocReadoutMode(Stream);
    getDTC()->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);

    TLOG(TLVL_DEBUG) << "-- END rc:" << rc;
    return rc;
  } // end Ui_base_InitReadout()

//==============================================================================
///	Ui_base_InitRocReadoutMode()
/// This needs to be implemented specific for the subsystems
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_InitRocReadoutMode(std::ostream* Stream)
{
    return 0;
  } // end Ui_base_InitRocReadoutMode()

//==============================================================================
///	Ui_base_ConfigureJA()
/// Source=0: sync to internal clock ; =1: RTF
/// on success, returns 1
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_ConfigureJA(int ClockSource, int Reset)
{
    int nmax_iter(10);
    int clock_source(ClockSource), reset(Reset);

    if (reset        == -1) reset        = (fJAMode     ) & 0xf;
    if (clock_source == -1) clock_source = (fJAMode >> 4) & 0xf;

    getDTC()->SetJitterAttenuatorSelect(clock_source,reset);    // 0:internal clock sync, 1:RTF
    usleep(100000);
    int ok(0);
    for (int i=0; i<nmax_iter; i++) {
      ok = getDTC()->ReadJitterAttenuatorLocked();              // in case of success, returns true
      usleep(100000);
      if (ok == 1) break;
    }

    int rc = 0;
    if (ok == 0) {
      TLOG(TLVL_ERROR) << std::format("failed to configure JA for clock_source={} and reset={} in {} attempts",
                                      clock_source,reset,nmax_iter);
      rc = -1;
    }

    return rc;
  } // end Ui_base_ConfigureJA()

//==============================================================================
///	Ui_base_InitEmulatedCFOReadoutMode()
/// according to Ryan, disabling the CFO emulation is critical, otherwise NMarkers
/// would be cached for the next time
/// EW length         : in units of 25 ns (clock)
/// EWMOde            : 1 for buffer test
/// EnableClockMarkers: set to 0
/// EnableAutogenDRP  : set to 1
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_InitEmulatedCFOReadoutMode()
{
    //                                 int EWMode, int EnableClockMarkers, int EnableAutogenDRP) {
    int rc(0);

    TLOG(TLVL_DEBUG) << Form("-- START");

    getDTC()->DisableCFOEmulation();                                   // r_0x9100:bit_30 = 0
    getDTC()->DisableReceiveCFOLink();                                 // r_0x9114:bit_14 = 0
                                                                   // this one doesn't take DTCLib::DTC_Link_ALL gently
    for (int i=0; i<6; i++) {
      getDTC()->DisableLink(DTCLib::DTC_Link_ID(i),DTCLib::DTC_LinkEnableMode(true,true));
    }

    getDTC()->DisableAutogenDRP();

    getDTC()->SoftReset();                                             // write 0x9100:bit_31 = 1

    int clock_source = (fJAMode >> 4) & 0x1;
    int reset        = fJAMode & 0x1;

    rc = Ui_base_ConfigureJA(clock_source,reset);
    getDTC()->EnableReceiveCFOLink();                                  // r_0x9114:bit_14 = 1
                                                                   // this one is OK...
    int EnableClockMarkers = 0;
    getDTC()->SetCFO40MHzClockMarkerEnable      (DTCLib::DTC_Link_ALL,EnableClockMarkers);

    getDTC()->EnableCFOEmulatorDRP();                                  // r_0x9100:bit_24 = 1
    getDTC()->EnableAutogenDRP();                                      // r_0x9100:bit_23 = 1

    getDTC()->SetCFOEmulationMode();                                   // r_0x9100:bit_15 = 1
    int force_cfo_edge = 0x0;                                      // two bits matter
    getDTC()->SetExternalCFOSampleEdgeMode(force_cfo_edge);            // r_0x9100:bit6 = 0 bit_5=0
    getDTC()->EnableTransmitCFOLink();                                 // r_0x9114:bit_06 = 1

    // ROC links are disabled here, but re-enabled later, in Ui_InitReadout()

    TLOG(TLVL_DEBUG) << "-- END, rc:" << rc;
    return rc;
  } // end Ui_base_InitEmulatedCFOReadoutMode()

//==============================================================================
///	Ui_base_InitExternalCFOReadoutMode()
/// example
/// write value 0x10800244 to register 0x9100 - disable emulated CFO bits
/// write value 0x00004141 to register 0x9114 - set link mask
/// DTC doesn' know about an external CFO, so it should only prepare itself to receive
/// EVMs/HBs from the outside
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_InitExternalCFOReadoutMode(int SampleEdgeMode)
{
    int rc(0);

    if (SampleEdgeMode != -1) fSampleEdgeMode = SampleEdgeMode;

    TLOG(TLVL_DEBUG) << "START .. PCIE addr:" << fPcieAddr << " SampleEdgeMode:" << fSampleEdgeMode;

    // this one doesn't take DTCLib::DTC_Link_ALL gently
    for (int i=0; i<6; i++) {
      getDTC()->DisableLink(DTCLib::DTC_Link_ID(i),DTCLib::DTC_LinkEnableMode(true,true));
    }

    // getDTC()->HardReset();                  // write 0x9100:bit_00=1
    getDTC()->SoftReset();                 // write 0x9100:bit_31=1

    getDTC()->DisableCFOEmulation  ();         // r_0x9100:bit_30 = 0
    getDTC()->DisableCFOEmulatorDRP();         // r_0x9100:bit_24 = 0
    getDTC()->DisableAutogenDRP    ();         // r_0x9100:bit_23 = 0

    // do it only when the bit is set ?
    getDTC()->ClearCFOEmulationMode();         // r_0x9100:bit_15 = 0

    int clock_source = (fJAMode >> 4) & 0x1;
    int reset        = fJAMode & 0x1;

    rc = Ui_base_ConfigureJA(clock_source,reset);
    if (rc < 0) {
      TLOG(TLVL_ERROR) << "failed to configure the JA for PCIE:" << fPcieAddr;
      return rc;
    }
                                        // which ROC links should be enabled ? - all active ?
    int EnableClockMarkers = 0;         // for now
                                        // this function handles DTCLib::DTC_Link_ALL correctly
    getDTC()->SetCFO40MHzClockMarkerEnable(DTCLib::DTC_Link_ALL,EnableClockMarkers);

    getDTC()->SetExternalCFOSampleEdgeMode(fSampleEdgeMode);

    getDTC()->EnableAutogenDRP();           // r_0x9100:bit_23

    // dtc->SetCFOEmulationMode();      // r_0x9100:bit_15 = 1

    // dtc->EnableCFOEmulation();       // r_0x9100:bit_30 = 1

    getDTC()->EnableReceiveCFOLink ();      // r_0x9114:bit_14 = 1
    //    getDTC()->EnableTransmitCFOLink();      // r_0x9114:bit_06 = 1 (if the dTC is in the middle of the chain)

    // ROC links are disabled here, but re-enabled later, in Ui_InitReadout()

    TLOG(TLVL_DEBUG) << "END PCIE addr:" << fPcieAddr;
    return rc;
  } // end Ui_base_InitExternalCFOReadoutMode()

//==============================================================================
///	Ui_base_InitConfiguration()
/// 1) first check for project name like "pasha/mu2edaq09_pcie0"
/// if file config/pasha/mu2edaq09_pcie0.C exists , use that
/// 2) otherwise assume config file name config/$project/$hostname.C
/// config file should contain function init_run_configuration(DtcGui*)
/// assumes that MU2E_DAQ_DIR points to the directory from where root is started
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_InitConfiguration(const char* ConfigName, mu2edaq::DtcInputData_t* DtcData)
{
    int           rc(0);
    TInterpreter* cint = gROOT->GetInterpreter();

    TInterpreter::EErrorCode irc;

    TString macro = Form("%s/config/dtc_gui/%s.C",gSystem->Getenv("MU2E_DAQ_DIR"),ConfigName);
    FILE* f = fopen(macro,"r");
    if (f == nullptr) {
      char buf[128];
      gethostname(buf,128);
      std::string hn = buf;
      std::string hostname = hn.substr(0,hn.find('.'));
      macro = std::format("{}/config/dtc_gui/{}/{}.C",gSystem->Getenv("MU2E_DAQ_DIR"),ConfigName,hostname);
      f     = fopen(macro,"r");
      if (f == nullptr) {
        TLOG(TLVL_ERROR) << "failed to find config file for " << ConfigName << " , EXIT" << std::endl;
        rc = -1;
      }
    }

    if (rc != 0) return rc;

    TLOG (TLVL_DEBUG+1) << Form(" loading configuration from file=%s\n",macro.Data());

    cint->LoadMacro(macro.Data(), &irc);

    rc = irc;
    if (rc != 0) return rc;

    TString cmd = Form("init_run_configuration((mu2edaq::DtcInputData_t*) 0x%0lx);",(long int) DtcData);

    TLOG(TLVL_DEBUG+1) << Form(" cmd=%s\n",cmd.Data());

    gInterpreter->ProcessLine(cmd.Data(),&irc);

    return irc;
  } // end Ui_base_InitConfiguration()

//==============================================================================
///	Ui_base_LaunchRunPlanEmulatedCfo()
/// run plan already defined in InitEmulatedCFOReadoutMode
/// this function can be executed in a loop, after InitEmulatedCFOReadoutMode
/// has been executed once
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_base_LaunchRunPlanEmulatedCfo(int EWLength, int NMarkers, int FirstEWTag)
{

    TLOG(TLVL_DEBUG+1) << "--- START";

    getDTC()->DisableCFOEmulation();
    getDTC()->SoftReset();                                             // write 0x9100:bit_31 = 1

    getDTC()->SetCFOEmulationEventWindowInterval(EWLength);
    getDTC()->SetCFOEmulationNumHeartbeats      (NMarkers);

    uint64_t ew_mode = ((((int64_t)fOnSpill) << 32) | ((int64_t)fEventMode));     // this really is the event mode

    TLOG(TLVL_DEBUG+1) << " checkpoint 001";

    getDTC()->SetCFOEmulationEventMode          (ew_mode  );

    getDTC()->SetCFOEmulationTimestamp          (DTCLib::DTC_EventWindowTag((uint64_t) FirstEWTag));

                                        // this command sends the EWM's by setting bit30 high
    getDTC()->EnableCFOEmulation();         // r_0x9100:bit_30 = 1

    TLOG(TLVL_DEBUG+1) << Form("EWLength=%i NMarkers=%i FirstEWTag=%i EventMode=0x%08lx\n",
                                EWLength,NMarkers,FirstEWTag,ew_mode);
    TLOG(TLVL_DEBUG+1) << "--- END";
  } // end Ui_base_LaunchRunPlanEmulatedCfo()

//==============================================================================
///	Ui_base_ReadRegister()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
uint32_t ROCTrackerInterface::Ui_base_ReadRegister(uint16_t Register)
{

    uint32_t data;
    int      timeout(150);

    mu2edev* dev = getDTC()->GetDevice();
    dev->read_register(Register,timeout,&data);

    return data;
  } // end Ui_base_ReadRegister()

//==============================================================================
///	Ui_base_LinkLocked()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_LinkLocked(int Link)
{
    uint32_t dat = Ui_ReadRegister(0x9140);
    return (dat >> Link) & 0x1;
  } // end Ui_base_LinkLocked()

//==============================================================================
///	Ui_base_ResetLinks()
/// generic function, should be used after HardReset()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_ResetLinks(int LinkMask, int SetNewMask)
{
    int rc(0);
    if ((LinkMask != 0) and (SetNewMask != 0)) fLinkMask = LinkMask;

    Ui_base_SetLinkMask();

    for (int i=0; i<6; i++) {
      if (LinkEnabled(i)) {
        int ret = Ui_ResetLink(i);   // this function is virtual
        rc += ret;
      }
    }
    return rc;
  } // end Ui_base_ResetLinks()

//==============================================================================
///	Ui_base_ResetLink()
/// default implementation is empty, returns 0 or -1
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_base_ResetLink(int Link)
{
    return 0;
  } // end Ui_base_ResetLink()

//==============================================================================
///	Ui_base_SetBit()
/// configure itself to use a CFO
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_base_SetBit(int Register, int Bit, int Value)
{
    int tmo_ms(100);

    uint32_t data;
    getDTC()->GetDevice()->read_register(Register,tmo_ms,&data);

    uint32_t w = (1 << Bit);

    data = (data ^ w) | (Value << Bit);
    getDTC()->GetDevice()->write_register(Register,tmo_ms,data);
  } // end Ui_base_SetBit()

//==============================================================================
///	Ui_base_SetLinkMask()
/// by default, enable/disable both TX and RX:
/// DTC_LinkEnableMode() = DTC_LinkEnableMode(true,true)
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_base_SetLinkMask(int Mask)
{
    if (Mask != 0) fLinkMask = Mask;

    for (int i=0; i<6; i++) {
      int enabled= (fLinkMask >> 4*i) & 0x1;
      if (enabled) getDTC()->EnableLink (DTCLib::DTC_Link_ID(i),DTCLib::DTC_LinkEnableMode());
      else         getDTC()->DisableLink(DTCLib::DTC_Link_ID(i),DTCLib::DTC_LinkEnableMode());
    }
  } // end Ui_base_SetLinkMask()

//==============================================================================
///	Ui_base_SetupCfoInterface()
/// configure itself to use a CFO
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_base_SetupCfoInterface(int CFOEmulationMode, int ForceCFOEdge,
                                       int EnableCFORxTx   , int EnableAutogenDRP)
{
    // int tmo_ms(150);

    if (CFOEmulationMode == 0) getDTC()->ClearCFOEmulationMode();
    else                       getDTC()->SetCFOEmulationMode  ();

// ForceCFOEdge: defines bit_6 and bit_5 of the control register 0x9100
// bit_6: 1:force       0:auto
// bit_5: 0:rising edge 1:falling edge
// ForceCFOEdge = 0 : force use of the rising  edge
//              = 1 : force use of the falling edge
//              = 2 : auto

    getDTC()->SetExternalCFOSampleEdgeMode(ForceCFOEdge);

    if (EnableCFORxTx == 0) {
      getDTC()->DisableReceiveCFOLink ();
      getDTC()->DisableTransmitCFOLink();
    }
    else {
      getDTC()->EnableReceiveCFOLink  ();
      getDTC()->EnableTransmitCFOLink ();
    }

    if (EnableAutogenDRP == 0) getDTC()->DisableAutogenDRP();
    else                       getDTC()->EnableAutogenDRP ();
  } // end Ui_base_SetupCfoInterface()

//==============================================================================
///	Ui_base_GetRocRegistersNames()
/// This is just an example, needs to be implemented for each subsystem
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<std::string> ROCTrackerInterface::Ui_base_GetRocRegistersNames(bool history)
{
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
  } // end Ui_base_GetRocRegistersNames()

//==============================================================================
///	Ui_base_GetRocRegisters()
/// This is just an example, needs to be implemented for each subsystem
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<uint32_t> ROCTrackerInterface::Ui_base_GetRocRegisters(int ilink, bool history)
{
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
  } // end Ui_base_GetRocRegisters()

//==============================================================================
///	Ui_base_GetConvertedRocRegisters()
/// This is just an example, needs to be implemented for each subsystem
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<float> ROCTrackerInterface::Ui_base_GetConvertedRocRegisters(int ilink, bool history)
{
    auto registers = Ui_GetRocRegisters(ilink, history);
    return std::vector<float>(registers.begin(), registers.end());
  } // end Ui_base_GetConvertedRocRegisters()

//==============================================================================
///	Ui_base_GetRocID         ()
/// to be overriden in derived subdetector-specific classes
/// ROC ID, ROC firmware ID , and the corresponding git commit
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_base_GetRocID         (int Link)
{ return std::string("undefined"); } // end Ui_base_GetRocID         ()

//==============================================================================
///	Ui_base_GetRocDesignInfo ()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_base_GetRocDesignInfo (int Link)
{ return std::string("undefined"); } // end Ui_base_GetRocDesignInfo ()

//==============================================================================
///	Ui_base_GetRocFwGitCommit()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_base_GetRocFwGitCommit(int Link)
{ return std::string("undefined"); } // end Ui_base_GetRocFwGitCommit()

// clang-format on
