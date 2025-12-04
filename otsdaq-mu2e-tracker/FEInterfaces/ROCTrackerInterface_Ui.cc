
//-----------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//-----------------------------------------------------------------------------


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"


//==============================================================================
/// Source=0: sync to internal clock ; =1: RT
/// on success, returns 
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ConfigureJA(int ClockSource, int Reset)
{
	int nmax_iter(10);

	getDTC()->SetJitterAttenuatorSelect(ClockSource, Reset);  // 0:internal clock sync, 1:RTF
	usleep(100000);
	int ok(0);
	for(int i = 0; i < nmax_iter; i++)
	{
		ok = getDTC()->ReadJitterAttenuatorLocked();  // in case of success, returns true
		usleep(100000);
		if(ok == 1)
			break;
	}

	// getDTC()->FormatJitterAttenuatorCSR();

	int rc = 0;
	if(ok == 0)
	{
		TLOG(TLVL_ERROR) << Form(
		    "failed to setup JA for ClockSource=%i and Reset=%i in %i attempts\n",
		    ClockSource,
		    Reset,
		    nmax_iter);
		rc = -1;
	}

	return rc;
} // end Ui_ConfigureJA()

//==============================================================================
/// according to Ryan, disabling the CFO emulation is critical, otherwise NMarker
/// would be cached for the next tim
/// EW length         : in units of 25 ns (clock
/// EWMOde            : 1 for buffer tes
/// EnableClockMarkers: set to 
/// EnableAutogenDRP  : set to 
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_InitEmulatedCFOReadoutMode()
{
	//                                 int EWMode, int EnableClockMarkers, int
	//                                 EnableAutogenDRP) {
	int rc(0);

	TLOG(TLVL_DEBUG) << Form("START\n");

	getDTC()->DisableCFOEmulation();
	getDTC()->DisableReceiveCFOLink();  // r_0x9114:bit_14 = 0
	                                // this one doesn't take DTCLib::DTC_Link_ALL gently
	for(int i = 0; i < 6; i++)
	{
		getDTC()->DisableLink(DTCLib::DTC_Link_ID(i), DTCLib::DTC_LinkEnableMode(true, true));
	}

	getDTC()->DisableAutogenDRP();

	getDTC()->SoftReset();  // write 0x9100:bit_31 = 1

	int clock_source = (fJAMode >> 4) & 0x1;
	int reset        = fJAMode & 0x1;

	rc = Ui_ConfigureJA(clock_source, reset);
	if(rc < 0)
		return rc;
	// this one is OK...
	int EnableClockMarkers = 0;
	getDTC()->SetCFO40MHzClockMarkerEnable(DTCLib::DTC_Link_ALL, EnableClockMarkers);

	getDTC()->EnableCFOEmulatorDRP();  // r_0x9100:bit_24 = 1
	getDTC()->EnableAutogenDRP();      // r_0x9100:bit_23 = 1

	getDTC()->SetCFOEmulationMode();  // r_0x9100:bit_15 = 1

	getDTC()->EnableReceiveCFOLink();  // r_0x9114:bit_14 = 1

	TLOG(TLVL_DEBUG) << Form("END\n");
	return rc;
} // end Ui_InitEmulatedCFOReadoutMode()

//==============================================================================
/// exampl
/// write value 0x10800244 to register 0x9100 - disable emulated CFO bit
/// write value 0x00004141 to register 0x9114 - set link mas
/// DTC doesn' know about an external CFO, so it should only prepare itself to receiv
/// EVMs/HBs from the outsid
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_InitExternalCFOReadoutMode(int SampleEdgeMode)
{
	int rc(0);
	TLOG(TLVL_DEBUG) << Form("START SampleEdgeMode=%i\n", fSampleEdgeMode);

	if(SampleEdgeMode != -1)
		fSampleEdgeMode = SampleEdgeMode;

	// this one doesn't take DTCLib::DTC_Link_ALL gently
	for(int i = 0; i < 6; i++)
	{
		getDTC()->DisableLink(DTCLib::DTC_Link_ID(i), DTCLib::DTC_LinkEnableMode(true, true));
	}

	// getDTC()->HardReset();                  // write 0x9100:bit_00=1
	getDTC()->SoftReset();  // write 0x9100:bit_31=1

	getDTC()->DisableCFOEmulation();    // r_0x9100:bit_30 = 0
	getDTC()->DisableCFOEmulatorDRP();  // r_0x9100:bit_24 = 0
	getDTC()->DisableAutogenDRP();      // r_0x9100:bit_23 = 0

	// do it only when the bit is set ?
	getDTC()->ClearCFOEmulationMode();  // r_0x9100:bit_15 = 0

	int clock_source = (fJAMode >> 4) & 0x1;
	int reset        = fJAMode & 0x1;

	rc = Ui_ConfigureJA(clock_source, reset);
	if(rc < 0)
		return rc;
	// which ROC links should be enabled ? - all active ?
	int EnableClockMarkers = 0;  // for now
	                             // this function handles DTCLib::DTC_Link_ALL correctly
	getDTC()->SetCFO40MHzClockMarkerEnable(DTCLib::DTC_Link_ALL, EnableClockMarkers);

	getDTC()->SetExternalCFOSampleEdgeMode(fSampleEdgeMode);

	getDTC()->EnableAutogenDRP();  // r_0x9100:bit_23

	// dtc->SetCFOEmulationMode();      // r_0x9100:bit_15 = 1

	// dtc->EnableCFOEmulation();       // r_0x9100:bit_30 = 1

	getDTC()->EnableReceiveCFOLink();  // r_0x9114:bit_14 = 1

	TLOG(TLVL_DEBUG) << Form("END\n");
	return rc;
} // end Ui_InitExternalCFOReadoutMode()

//==============================================================================
/// Init Readou
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_InitReadout(int EmulateCfo, int RocReadoutMode)
{
	int rc(0);

	if(EmulateCfo != -1)
		fEmulateCfo = EmulateCfo;
	if(RocReadoutMode != -1)
		fRocReadoutMode = RocReadoutMode;

	TLOG(TLVL_DEBUG) << "START : Emulates CFO=" << fEmulateCfo
	                 << " ROC ReadoutMode:" << fRocReadoutMode << std::endl;
	//-----------------------------------------------------------------------------
	// both emulated and external modes perform soft reset of the DTC
	//-----------------------------------------------------------------------------
	if(fEmulateCfo == 0)
	{
		rc = Ui_InitExternalCFOReadoutMode();
	}
	else
	{
		//-----------------------------------------------------------------------------
		// bit_30 will be restored on the 'emulated CFO side", in the call to
		// Ui_InitEmulatedCFOReadoutMode
		//-----------------------------------------------------------------------------
		rc = Ui_InitEmulatedCFOReadoutMode();
	}
	if(rc < 0)
		return rc;
	//-----------------------------------------------------------------------------
	// the DTC link mask could be reset by the previous DTC hard reset, so restore it
	// also, release all buffers from the previous read - this is the initialization
	//-----------------------------------------------------------------------------
	Ui_SetLinkMask();
	// this should do for now, later - set the partition ID
	// at begin run, for example, as follows

	uint8_t id           = fDtcID & 0xff;
	uint8_t event_mode   = fEventMode & 0xff;
	uint8_t partition_id = fPartitionID & 0xff;
	uint8_t mac_byte     = fMacAddrByte & 0xff;
	getDTC()->SetEVBInfo(id, event_mode, partition_id, mac_byte);

	Ui_InitRocReadoutMode();
	getDTC()->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);

	TLOG(TLVL_DEBUG) << "END" << std::endl;
	return rc;
} // end Ui_InitReadout()

//==============================================================================
/// this si fully tracker-specifi
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_InitRocReadoutMode()
{
	TLOG(TLVL_DEBUG) << Form("START : fRocReadoutMode=%i\n", fRocReadoutMode);
	//-----------------------------------------------------------------------------
	// this should be the only place where we reset the ROC
	// ROC readout mode (fixed_length << 4) | readout_mode
	//-----------------------------------------------------------------------------
	// Ui_ResetRoc();

	if(((fRocReadoutMode & 0xf) == 0) || ((fRocReadoutMode & 0xf) == 2))
	{
		Ui_MonicaVarPatternConfig();  // readout ROC patterns
	}
	else if((fRocReadoutMode & 0xf) == 1)
	{
		Ui_MonicaVarLinkConfig();  // readout ROC digis
		Ui_MonicaDigiClear();      //
	}
	else
	{
		TLOG(TLVL_DEBUG) << "unknown mode:" << fRocReadoutMode << "> BAIL OUT";
	}
	TLOG(TLVL_DEBUG) << Form("END   : fRocReadoutMode=%i\n", fRocReadoutMode);
} // end Ui_InitRocReadoutMode()

//==============================================================================
/// run plan already defined in InitEmulatedCFOReadoutMod
/// this function can be executed in a loop, after InitEmulatedCFOReadoutMod
/// has been executed onc
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_LaunchRunPlanEmulatedCfo(int EWLength, int NMarkers, int FirstEWTag)
{
	getDTC()->DisableCFOEmulation();
	getDTC()->SoftReset();  // write 0x9100:bit_31 = 1

	getDTC()->SetCFOEmulationEventWindowInterval(EWLength);
	getDTC()->SetCFOEmulationNumHeartbeats(NMarkers);

	uint64_t ew_mode = ((((int64_t)fOnSpill) << 32) | ((int64_t)fEventMode));  // this really is the event mode

	getDTC()->SetCFOEmulationEventMode(ew_mode);

	getDTC()->SetCFOEmulationTimestamp(DTCLib::DTC_EventWindowTag((uint64_t)FirstEWTag));

	// this command sends the EWM's
	getDTC()->EnableCFOEmulation();  // r_0x9100:bit_30 = 1

	TLOG(TLVL_DEBUG + 10) << Form(
	    "EWLength=%i NMarkers=%i FirstEWTag=%i EventMode=0x%08lx\n",
	    EWLength,
	    NMarkers,
	    FirstEWTag,
	    ew_mode);
} // end Ui_LaunchRunPlanEmulatedCfo()

//==============================================================================
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ConvertSpiData(const std::vector<uint16_t>& Data,
                                 trkdaq::TrkSpiData_t*                Spi,
                                 int                          PrintLevel)
{
	const char* keys[] = {"I3.3",
	                      "I2.5",
	                      "I1.8HV",
	                      "IHV5.0",
	                      "VDMBHV5.0",
	                      "V1.8HV",
	                      "V3.3HV",
	                      "V2.5",
	                      "A0",
	                      "A1",
	                      "A2",
	                      "A3",
	                      "I1.8CAL",
	                      "I1.2",
	                      "ICAL5.0",
	                      "ADCSPARE",
	                      "V3.3",
	                      "VCAL5.0",
	                      "V1.8CAL",
	                      "V1.0",
	                      "ROCPCBTEMP",
	                      "HVPCBTEMP",
	                      "CALPCBTEMP",
	                      "RTD",
	                      "ROC_RAIL_1V(mV)",
	                      "ROC_RAIL_1.8V(mV)",
	                      "ROC_RAIL_2.5V(mV)",
	                      "ROC_TEMP(CELSIUS)",
	                      "CAL_RAIL_1V(mV)",
	                      "CAL_RAIL_1.8V(mV)",
	                      "CAL_RAIL_2.5V(mV)",
	                      "CAL_TEMP(CELSIUS)",
	                      "HV_RAIL_1V(mV)",
	                      "HV_RAIL_1.8V(mV)",
	                      "HV_RAIL_2.5V(mV)",
	                      "HV_TEMP(CELSIUS)"};
	//-----------------------------------------------------------------------------
	// primary source :
	// https://github.com/bonventre/trackerScripts/blob/master/constants.py#L99
	//-----------------------------------------------------------------------------
	struct constants_t
	{
		float iconst  = 3.3 / (4096 * 0.006 * 20);
		float iconst5 = 3.25 / (4096 * 0.500 * 20);
		float iconst1 = 3.25 / (4096 * 0.005 * 20);
		float toffset = 0.509;
		float tslope  = 0.00645;
		float tconst  = 0.000806;
		float tlm45   = 0.080566;
	} constants;

	int nw = Data.size();

	float* val = (float*)Spi;

	for(int i = 0; i < nw; i++)
	{
		if(i == 20 or i == 21 or i == 22)
		{
			val[i] = Data[i] * constants.tlm45;
		}
		else if(i == 0 or i == 1 or i == 2 or i == 12 or i == 13)
		{
			val[i] = Data[i] * constants.iconst;
		}
		else if(i == 3 or i == 14)
		{
			val[i] = Data[i] * constants.iconst5;
		}
		else if(i == 4 or i == 5 or i == 6 or i == 7 or i == 16 or i == 17 or i == 18 or
		        i == 19)
		{
			val[i] = Data[i] * 3.3 * 2 / 4096;
		}
		else if(i == 15)
		{
			val[i] = Data[i] * 3.3 / 4096;
		}
		else if(i == 23)
		{
			val[i] = Data[i] * 3.3 / 4096;
		}
		else if(i == 8 or i == 9 or i == 10 or i == 11)
		{
			val[i] = Data[i];
		}
		else if(i > 23)
		{
			if((i % 4) < 3)
				val[i] = Data[i] / 8.;
			else
				val[i] = Data[i] / 16. - 273.15;
		}

		if(PrintLevel > 0)
		{
			printf("%-20s : %10.3f\n", keys[i], val[i]);
		}
	}

	return 0;
} // end Ui_ConvertSpiData()

//==============================================================================
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
uint32_t ROCTrackerInterface::Ui_ReadRegister(uint16_t Register)
{
	uint32_t data;
	int      timeout(150);

	mu2edev* dev = getDTC()->GetDevice();
	dev->read_register(Register, timeout, &data);

	return data;
} // end Ui_ReadRegister()

//==============================================================================
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ReadSpiData(int Link, std::vector<uint16_t>& SpiRawData, int PrintLevel)
{
	int rc(0);
	//-----------------------------------------------------------------------------
	// is this really needed ? - probably not
	//-----------------------------------------------------------------------------
	// Ui_MonicaVarLinkConfig();
	// Ui_MonicaDigiClear();
	//-----------------------------------------------------------------------------
	// after writing into reg 258, sleep for some time,
	// then wait till reg 128 returns non-zero
	//-----------------------------------------------------------------------------
	DTCLib::DTC_Link_ID rlink = DTCLib::DTC_ROC_Links[Link];

	getDTC()->WriteROCRegister(rlink, 258, 0x0000, false, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	uint16_t u;
	while((u = getDTC()->ReadROCRegister(rlink, 128, 100)) == 0) {};
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 128, u);
	//-----------------------------------------------------------------------------
	// register 129: number of words to read, currently-  (+ 4) (ask Monica)
	// 2024-05-10: is r129 now returning the number of bytes ?
	//-----------------------------------------------------------------------------
	int nb = getDTC()->ReadROCRegister(rlink, 129, 100);
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 129, nb);

	int nw = nb - 4;

	if(nw != trkdaq::TrkSpiDataNWords)
	{
		TLOG(TLVL_ERROR) << "expected N(words)=" << trkdaq::TrkSpiDataNWords
		                 << " , reported nw=" << nw;
		rc = -1;
	}

	getDTC()->ReadROCBlock(SpiRawData, rlink, 258, nw, false, 100);  //
	//-----------------------------------------------------------------------------
	// print SPI data in std::hex
	//-----------------------------------------------------------------------------
	if((PrintLevel & 0x1) != 0)
	{
		DTCLib::Utilities::PrintBuffer(SpiRawData.data(), nw);
	}
	//-----------------------------------------------------------------------------
	// parse SPI data and print them
	//-----------------------------------------------------------------------------
	if((PrintLevel & 0x2) != 0)
	{
		struct trkdaq::TrkSpiData_t spi;
		Ui_ConvertSpiData(SpiRawData, &spi, PrintLevel);  // &spi[0]
	}

	return rc;
} // end Ui_ReadSpiData()

//==============================================================================
/// preserve historic naming convention- Monica named her script 'var_pattern_config
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_RocConfigurePatternMode(int LinkMask)
{
	Ui_MonicaVarPatternConfig(LinkMask);
} // end Ui_RocConfigurePatternMode()

//==============================================================================
/// ROC reset : write 0x1 to R14 of each ROC specified as active by the mas
/// by default, don't redefine the link mas
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_ResetRoc(int LinkMask, int UpdateMask)
{
	if((LinkMask != 0) and (UpdateMask != 0))
		fLinkMask = LinkMask;

	int tmo_ms(100);
	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			getDTC()->WriteROCRegister(
			    DTCLib::DTC_Link_ID(i), 14, 1, false, tmo_ms);  // 1 --> r14: reset ROC
		}
	}

	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCReset));
} // end Ui_ResetRoc()

//==============================================================================
/// Version --> R2
/// as thre is no point inhaving different ROCs with different data versions, assum
/// that specifying the mask means that we want it to be redefine
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_RocSetDataVersion(int Version, int LinkMask)
{
	if(LinkMask != 0)
		fLinkMask = LinkMask;

	int tmo_ms(100);
	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 29, Version, false, tmo_ms);
		}
	}
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
} // end Ui_RocSetDataVersion()

//==============================================================================
/// ROC reset : write 0x1 to register 1
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_ReadSubevents(std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& VSub,
                                 ulong       FirstEWT,
                                 int         PrintLevel,
                                 int         Validate,
                                 const char* Fn)
{
	ulong ewt      = FirstEWT;
	bool  match_ts = false;
	int   nerr_tot(0);
	ulong nbytes_tot(0);
	ulong offset(0);  // used in validation mode
	int   nerr_roc[6], nerr_roc_tot[6];

	FILE* file(nullptr);
	if(Fn != nullptr)
	{
		//-----------------------------------------------------------------------------
		// check if Fn exists
		//-----------------------------------------------------------------------------
		if((file = fopen(Fn, "r")) != NULL)
		{
			// file exists
			fclose(file);
			std::cout << "ERROR in " << __func__ << " : file " << Fn
			     << " already exists, BAIL OUT" << std::endl;
			return;
		}
		else
		{
			//-----------------------------------------------------------------------------
			// Fn doesn't exist, open it
			//-----------------------------------------------------------------------------
			file = fopen(Fn, "w");
			if(file == nullptr)
			{
				std::cout << "ERROR in " << __func__ << " : failed to open " << Fn
				     << " , BAIL OUT" << std::endl;
				return;
			}
		}
	}
	//-----------------------------------------------------------------------------
	// reset per-roc error counters
	//-----------------------------------------------------------------------------
	for(int i = 0; i < 6; i++)
	{
		nerr_roc[i]     = 0;
		nerr_roc_tot[i] = 0;
	}
	//-----------------------------------------------------------------------------
	// always read an event into the same external buffer (VSub),
	// so no problem with the memory management
	//-----------------------------------------------------------------------------
	int header_printed = 0;
	while(1)
	{
		// sleep(1);
		DTCLib::DTC_EventWindowTag event_tag = DTCLib::DTC_EventWindowTag(ewt);
		try
		{
			if(PrintLevel > 0)
			{
				//-----------------------------------------------------------------------------
				// print header
				// if fValidate != 0, there is a lot of printout, so it is better to print
				// header for every event
				//-----------------------------------------------------------------------------
				if((Validate and PrintLevel > 1) or (header_printed == 0))
				{
					std::cout << Form(
					    "      event  DTC     EW Tag nbytes   nbytes_tot  link0   nb0  "
					    "link1   nb1  link2   nb2  link3   nb3  link4   nb4  link5   nb5 "
					    " nerr nerr_tot\n");
					std::cout << Form(
					    "----------------------------------------------------------------"
					    "----------------------------------------------------------------"
					    "------------\n");
					header_printed = 1;
				}
			}
			VSub   = getDTC()->GetSubEventData(event_tag, match_ts);
			int sz = VSub.size();
			if(sz == 0)
			{
				if(PrintLevel > 0)
				{
					std::cout << Form(
					    ">>>> ------- ewt = %5li NDTCs:%2i END_OF_DATA\n", ewt, sz);
				}
				break;
			}
			//-----------------------------------------------------------------------------
			// a subevent contains data of a single DTC
			//-----------------------------------------------------------------------------
			int                  rs[6];
			std::vector<uint8_t> dtc_block;

			for(int i = 0; i < sz; i++)
			{
				DTCLib::DTC_SubEvent* ev       = VSub[i].get();
				uint64_t      ew_tag   = ev->GetEventWindowTag().GetEventWindowTag(true);
				char*         raw_data = (char*)ev->GetRawBufferPointer();

				int nbytes = ev->GetSubEventByteCount();
				//-----------------------------------------------------------------------------
				// create a local copy of the DTC data block
				//-----------------------------------------------------------------------------
				dtc_block.reserve(nbytes);
				memcpy(dtc_block.data(), raw_data, nbytes);

				nbytes_tot += nbytes;

				int nerr(0);

				if(Validate > 0)
				{
					// different readout modes - different validation
					if((fRocReadoutMode & 0xf) == 0)
					{
						nerr = Ui_ValidateVarPatterns((ushort*)dtc_block.data(),
						                           ew_tag,
						                           &offset,
						                           PrintLevel,
						                           nerr_roc);
					}
					else if((fRocReadoutMode & 0xf) == 1)
					{
						nerr = Ui_ValidateDigiPatterns((ushort*)dtc_block.data(),
						                            ew_tag,
						                            &offset,
						                            PrintLevel,
						                            nerr_roc);
					}
					else if((fRocReadoutMode & 0xf) == 2)
					{
						nerr = Ui_ValidateFixedPatterns((ushort*)dtc_block.data(),
						                             ew_tag,
						                             &offset,
						                             PrintLevel,
						                             nerr_roc);
					}

					nerr_tot += nerr;
					for(int ir = 0; ir < 6; ir++)
						nerr_roc_tot[ir] += nerr_roc[ir];
				}

				uint8_t* roc_data = dtc_block.data() + 0x30;

				int nb_roc[6];
				for(int roc = 0; roc < 6; roc++)
				{
					nb_roc[roc] = *((ushort*)roc_data);
					rs[roc]     = *((ushort*)(roc_data + 0x0c));
					roc_data += nb_roc[roc];
				}

				if(PrintLevel > 0)
				{
					std::cout << Form(
					    " %10li  %2i  %10li %5i %13li 0x%04x %5i 0x%04x %5i 0x%04x %5i "
					    "0x%04x %5i 0x%04x %5i 0x%04x %5i %5i %8i %4i %4i %4i %4i %4i "
					    "%4i\n",
					    ewt,
					    i,
					    ew_tag,
					    nbytes,
					    nbytes_tot,
					    rs[0],
					    nb_roc[0],
					    rs[1],
					    nb_roc[1],
					    rs[2],
					    nb_roc[2],
					    rs[3],
					    nb_roc[3],
					    rs[4],
					    nb_roc[4],
					    rs[5],
					    nb_roc[5],
					    nerr,
					    nerr_tot,
					    nerr_roc[0],
					    nerr_roc[1],
					    nerr_roc[2],
					    nerr_roc[3],
					    nerr_roc[4],
					    nerr_roc[5]);
					if(((nerr > 0) and (PrintLevel > 1)) or (PrintLevel > 2))
					{
						DTCLib::Utilities::PrintBuffer(ev->GetRawBufferPointer(),
						            ev->GetSubEventByteCount() / 2);
					}
				}

				if(file)
				{
					//-----------------------------------------------------------------------------
					// write event to output file
					//-----------------------------------------------------------------------------
					int nbb = fwrite(dtc_block.data(), 1, nbytes, file);
					if(nbb == 0)
					{
						TLOG(TLVL_ERROR) << Form(
						    "failed to write event %10li , close file and BAIL OUT\n",
						    ew_tag);
						fclose(file);
						return;
					}
				}
			}

			ewt++;  // event in sequence
		}
		catch(...)
		{
			TLOG(TLVL_ERROR) << "ERROR reading event_tag:"
			                 << event_tag.GetEventWindowTag(true) << " ewt:" << ewt
			                 << std::endl;
			break;
		}
	}

	//    getDTC()->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
	//-----------------------------------------------------------------------------
	// print summary
	//-----------------------------------------------------------------------------
	ulong nev = ewt - FirstEWT;
	TLOG(TLVL_DEBUG) << Form(
	    "nevents: %10li nbytes_tot: %13li Validate:%i\n", nev, nbytes_tot, Validate);
	TLOG(TLVL_DEBUG) << Form("nerr_tot:%10i nerr_roc_tot: %8i %8i %8i %8i %8i %8i\n",
	                         nerr_tot,
	                         nerr_roc_tot[0],
	                         nerr_roc_tot[1],
	                         nerr_roc_tot[2],
	                         nerr_roc_tot[3],
	                         nerr_roc_tot[4],
	                         nerr_roc_tot[5]);
	//-----------------------------------------------------------------------------
	// to simplify first steps, assume that in a file writing mode all events
	// are read at once, so close the file on exit
	//-----------------------------------------------------------------------------
	if(file)
	{
		fclose(file);
	}
} // end Ui_ReadSubevents()

//==============================================================================
/// wrapper for DTCLib::DTC::ReadROCBloc
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<DTCLib::roc_data_t> ROCTrackerInterface::Ui_ReadROCBlockEnsured(const DTCLib::DTC_Link_ID&   Link,
                                                          const DTCLib::roc_address_t& address)
{
	// register 129: number of words to read
	size_t nwords = static_cast<size_t>(getDTC()->ReadROCRegister(Link, 129, 1000));
	nwords -= 4;  // account for low-level headers already consumed on-chip

	std::vector<DTCLib::roc_data_t> rv;
	bool                    increment_address = false;  // read via fifo
	getDTC()->ReadROCBlock(rv, Link, address, nwords, increment_address, 10000);
	if(rv.size() != nwords)
	{
		std::string msg = "Malformed block read";
		msg += " expected ";
		msg += std::to_string(nwords);
		msg += " words, received ";
		msg += std::to_string(rv.size());
		msg += " words";
		throw cet::exception("DtcInterface::Ui_ReadROCBlockEnsured") << msg;
	}

	// reset ddr memory
	getDTC()->WriteROCRegister(Link, 14, 0x01, false, 1000);

	// return
	return rv;
} // end Ui_ReadROCBlockEnsured()

//==============================================================================
/// assume that only one link is specified (not DTC_Link_ALL
/// read serial number and device inf
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<DTCLib::roc_data_t> ROCTrackerInterface::Ui_ReadDeviceID(const DTCLib::DTC_Link_ID& Link)
{
	std::vector<DTCLib::roc_data_t> rv;
	//-----------------------------------------------------------------------------
	// make sure the link is enabled, othrwise risk corrupting memory if Link value is
	// random
	//-----------------------------------------------------------------------------
	bool ok(false);
	for(int i = 0; i < 6; i++)
	{
		int enabled = (fLinkMask >> 4 * i) & 1;
		if(enabled and (i == Link))
		{
			ok = true;
		}
	}
	if(not ok)
	{
		TLOG(TLVL_ERROR) << "Link " << int(Link) << " is not enabled" << std::endl;
		return rv;
	}
	// reset only ROC in question
	// 2024-11-14: Monica tells reset is not needed
	//    this->Ui_ResetRoc(Link,0);
	// write nothing to trigger query
	std::vector<DTCLib::roc_data_t> empty;
	getDTC()->WriteROCBlock(Link, 260, empty, false, false, 1000);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	// read back payload
	rv = this->Ui_ReadROCBlockEnsured(Link, 260);

	// reset ddr memory
	getDTC()->WriteROCRegister(Link, 14, 0x01, false, 1000);

	// return
	return rv;
} // end Ui_ReadDeviceID()

//==============================================================================
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_ReadSerialNumber(const DTCLib::DTC_Link_ID& Link)
{
	bool ok(false);
	for(int i = 0; i < 6; i++)
	{
		int enabled = (fLinkMask >> 4 * i) & 1;
		if(enabled and (i == Link))
		{
			ok = true;
		}
	}
	if(not ok)
	{
		TLOG(TLVL_ERROR) << "Link " << int(Link) << " is not enabled" << std::endl;
		return "";
	}

	auto returned = this->Ui_ReadDeviceID(Link);

	std::stringstream ss;
	ss << "0x";

	// first 16 words are the serial number
	for(size_t i = 0; i < 16; i++)
	{
		ss << std::hex << returned[i];
	}

	auto rv = ss.str();
	return rv;
} // end Ui_ReadSerialNumber()

//==============================================================================
/// align ROC fpga/adc signals, and optionally print summary tabl
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
Alignment ROCTrackerInterface::Ui_FindAlignment(DTCLib::DTC_Link_ID Link)
{
	// write parameters into roc to initiate routine
	std::vector<DTCLib::roc_data_t> writeable = {
	    4,                          // eye-monitor width
	    0,                          // initial adc phase
	    1,                          // flag to check adc patterns
	    static_cast<uint16_t>(-1),  // for channel remapping; unused
	    static_cast<uint16_t>(-1),  // for channel remapping; unused
	    0xFFFF,                     // bitmask for channels  0 - 15
	    0xFFFF,                     // bitmask for channels 16 - 31
	    0xFFFF,                     // bitmask for channels 32 - 47
	    0xFFFF,                     // bitmask for channels 48 - 63
	    0xFFFF,                     // bitmask for channels 64 - 79
	    0xFFFF,                     // bitmask for channels 80 - 95
	};

	// register 264: find alignment routine
	bool increment_address = false;  // read via fifo
	getDTC()->WriteROCBlock(Link, 264, writeable, false, increment_address, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	// then, wait till reg 128 returns non-zero
	uint16_t u;
	while((u = getDTC()->ReadROCRegister(Link, 128, 100)) != 0x8000)
	{
		// idle
	}

	std::vector<DTCLib::roc_data_t> returned = this->Ui_ReadROCBlockEnsured(Link, 264);

	// return
	auto rv = Alignment(returned);
	return rv;
} // end Ui_FindAlignment()

//==============================================================================
/// align ROC fpga/adc signals, and optionally print summary tabl
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_FindAlignments(bool print, int LinkMask)
{
	// reset link mask if desired
	if(LinkMask != 0)
		fLinkMask = LinkMask;

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			auto link      = DTCLib::DTC_Link_ID(i);
			auto alignment = Ui_FindAlignment(link);
			if(print)
			{
				print_legacy_table(alignment);
			}
		}
	}
} // end Ui_FindAlignments()

//==============================================================================
/// configure itself to use a CF
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_SetBit(int Register, int Bit, int Value)
{
	int tmo_ms(100);

	uint32_t data;
	getDTC()->GetDevice()->read_register(Register, tmo_ms, &data);

	uint32_t w = (1 << Bit);

	data = (data ^ w) | (Value << Bit);
	getDTC()->GetDevice()->write_register(Register, tmo_ms, data);
} // end Ui_SetBit()

//==============================================================================
/// by default, enable/disable both TX and RX
/// DTC_LinkEnableMode() = DTC_LinkEnableMode(true,true
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_SetLinkMask(int Mask)
{
	if(Mask != 0)
		fLinkMask = Mask;

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used)
			getDTC()->EnableLink(DTCLib::DTC_Link_ID(i), DTCLib::DTC_LinkEnableMode());
		else
			getDTC()->DisableLink(DTCLib::DTC_Link_ID(i), DTCLib::DTC_LinkEnableMode());
	}
} // end Ui_SetLinkMask()

//==============================================================================
/// configure itself to use a CF
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_SetupCfoInterface(int CFOEmulationMode,
                                     int ForceCFOEdge,
                                     int EnableCFORxTx,
                                     int EnableAutogenDRP)
{
	// int tmo_ms(150);

	if(CFOEmulationMode == 0)
		getDTC()->ClearCFOEmulationMode();
	else
		getDTC()->SetCFOEmulationMode();

	// ForceCFOEdge: defines bit_6 and bit_5 of the control register 0x9100
	// bit_6: 1:force       0:auto
	// bit_5: 0:rising edge 1:falling edge
	// ForceCFOEdge = 0 : force use of the rising  edge
	//              = 1 : force use of the falling edge
	//              = 2 : auto

	getDTC()->SetExternalCFOSampleEdgeMode(ForceCFOEdge);

	if(EnableCFORxTx == 0)
	{
		getDTC()->DisableReceiveCFOLink();
		getDTC()->DisableTransmitCFOLink();
	}
	else
	{
		getDTC()->EnableReceiveCFOLink();
		getDTC()->EnableTransmitCFOLink();
	}

	if(EnableAutogenDRP == 0)
		getDTC()->DisableAutogenDRP();
	else
		getDTC()->EnableAutogenDRP();
} // end Ui_SetupCfoInterface()

//==============================================================================
/// validate data taken in the tracker ROC pattern generation mode, focus on payloa
/// returns number of found errors in the payload dat
/// assume ROC pattern generatio
/// 'Offset' is th
/// PrintLevel =  0: print nothin
///            =  1: print all about error
///            > 10: full printou
/// also returned NErrRoc[6]: number of errors per RO
/// returns nerrors, where does the error code goes 
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ValidateDigiPatterns(
    ushort* DtcData, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc)
{
	int n_adc_packets(1);
	int nerr = 0;

	RocData_t* roc = (RocData_t*)(DtcData + 0x18);  // 0x30 bytes
	for(int i = 0; i < 6; i++)
	{
		// nb_roc[i]    = roc->header.byteCount;
		// nb_rocs_tot += nb_roc[i];

		int nhits = roc->header.packetCount / (n_adc_packets + 1);

		short* first_address = (short*)roc;

		for(int ihit = 0; ihit < nhits; ihit++)
		{
			mu2e::TrackerDataDecoder::TrackerDataPacket* hit;
			int offset = ihit * (8 + 8 * n_adc_packets);  // in 2-byte words
			// int offset_in_bytes = offset*2;
			hit = (mu2e::TrackerDataDecoder::TrackerDataPacket*)(first_address + 0x08 +
			                                                     offset);
			if(hit->ErrorFlags != 0)
			{  // 4 bits
				nerr += 1;
			}
			//-----------------------------------------------------------------------------
			// check hit straaw ID
			//-----------------------------------------------------------------------------
			int ich = hit->StrawIndex;

			if(ich > 128)
				ich = ich - 128;

			if(ich > 95)
			{
				//-----------------------------------------------------------------------------
				// non existing channel ID : flag an error, don't save the hit, but
				// continue
				//-----------------------------------------------------------------------------
				nerr += 1;
			}
			if(hit->NumADCPackets != n_adc_packets)
			{
				nerr += 1;
				// assume errors are localized within the ROC payload
				break;
			}
		}

		roc = (RocData_t*)(((char*)roc) + roc->header.byteCount);
	}
	return 0;
} // end Ui_ValidateDigiPatterns()

//==============================================================================
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ValidateFixedPatterns(
    ushort* DtcData, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc)
{
	//  int ewt    = EwTag % 64 ;
	int nb_dtc = *DtcData;

	int nerr = 0;

	int nb_roc[6];
	int nb_rocs_tot = 0;
	int last_nb(-1);

	RocData_t* roc = (RocData_t*)(DtcData + 0x18);  // 0x30 bytes
	for(int i = 0; i < 6; i++)
	{
		nb_roc[i] = roc->header.byteCount;
		nb_rocs_tot += nb_roc[i];
		//-----------------------------------------------------------------------------
		// although some ROC may not respond,  all responding ones should report
		// the same number of bytes
		//-----------------------------------------------------------------------------
		if(roc->header.error_code() == 0)
		{
			if((last_nb > 0) and (nb_roc[i] != last_nb))
			{
				nerr += 1;
				if(PrintLevel > 1)
				{
					printf(
					    "ERROR: EWtag, nb_dtc, i, nb_roc[i-1], nb[roc] : %10lu 0x%04x %i "
					    "0x%04x 0x%04x\n",
					    EwTag,
					    nb_dtc,
					    i,
					    nb_roc[i - 1],
					    nb_roc[i]);
				}
			}
			last_nb = nb_roc[i];
		}
		roc = (RocData_t*)(((char*)roc) + roc->header.byteCount);
	}

	// DTC header is 0x30 bytes - 3 packets
	if(nb_dtc != nb_rocs_tot + 0x30)
	{
		if(PrintLevel > 1)
			printf("ERROR: EWtag, nb_dtc, nb_rocs_tot : %10lu 0x%04x 0x%04x\n",
			       EwTag,
			       nb_dtc,
			       nb_rocs_tot);
		nerr += 1;
	}

	return nerr;
} // end Ui_ValidateFixedPatterns()

//==============================================================================
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ValidateVarPatterns(
    ushort* DtcData, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc)
{
	int nhits[64] = {1,  2,  3,  0,  0,  0,  7,  8,  9,  10, 11, 12, 13, 14, 15, 16,
	                 0,  20, 21, 22, 12, 13, 11, 12, 0,  0,  8,  4,  12, 11, 12, 13,
	                 16, 6,  3,  1,  12, 0,  16, 17, 18, 19, 12, 1,  12, 12, 11, 11,
	                 0,  0,  0,  0,  13, 14, 10, 13, 11, 14, 14, 15, 8,  9,  10, 32};

	//-----------------------------------------------------------------------------
	// check consistency of the lengths
	// 1. total number of 2-byte words
	//
	//-----------------------------------------------------------------------------
	int ewt    = EwTag % 64;
	int nb_dtc = *DtcData;

	RocData_t* roc = (RocData_t*)(DtcData + 0x18);

	int nb_rocs = 0;
	for(int i = 0; i < 6; i++)
	{
		int nb = roc->header.byteCount;
		nb_rocs += nb;
		roc = (RocData_t*)(((char*)roc) + nb);
	}

	int nerr = 0;

	if(nb_dtc != nb_rocs + 0x30)
	{
		if(PrintLevel > 1)
			printf("ERROR: EWtag, nb_dtc, nb_rocs : %10lu 0x%04x 0x%04x\n",
			       EwTag,
			       nb_dtc,
			       nb_rocs);
		nerr += 1;
	}
	//-----------------------------------------------------------------------------
	// event length checks out, check ROC payload
	// check the ROC payload, assume a hit = 2 packets
	//-----------------------------------------------------------------------------
	roc = (RocData_t*)(DtcData + 0x18);
	for(int iroc = 0; iroc < 6; iroc++)
	{
		NErrRoc[iroc] = 0;
		if(PrintLevel > 10)
			printf("  ---- roc # %i\n", iroc);
		//-----------------------------------------------------------------------------
		// offsets are the same for all non-emty ROC's in the DTC data block
		//-----------------------------------------------------------------------------
		ulong offset = *Offset;
		//    int   nb     = roc->header.byteCount;
		//-----------------------------------------------------------------------------
		// validate ROC header
		//-----------------------------------------------------------------------------
		// ... TODO
		ulong ewtag_roc = roc->header.ewtag();

		if(ewtag_roc != EwTag)
		{
			if(PrintLevel > 1)
				printf("ERROR: EwTag ewtag_roc roc : 0x%08lx 0x%08lx %i\n",
				       EwTag,
				       ewtag_roc,
				       iroc);
			nerr += 1;
			NErrRoc[iroc] += 1;
		}

		if(roc->header.byteCount > 0x10)
		{
			//-----------------------------------------------------------------------------
			// non-zero payload
			//-----------------------------------------------------------------------------
			uint32_t* pattern = (uint32_t*)&roc->data[0];
			if(PrintLevel > 10)
				printf("data[0]  = nb = 0x%04x\n", pattern[0]);

			int npackets = roc->header.packetCount;
			int npackets_exp =
			    nhits[ewt] *
			    2;  // assume two packets per hit (this number is stored somewhere)

			if(npackets != npackets_exp)
			{
				if(PrintLevel > 1)
					printf("ERROR: EwTag roc npackets npackets_exp: 0x%08lx %i %5i %5i\n",
					       EwTag,
					       iroc,
					       npackets,
					       npackets_exp);
				nerr += 1;
				NErrRoc[iroc] += 1;
			}

			if(PrintLevel > 10)
			{
				printf(
				    "EwTag, ewt, roc, npackets, npackets_exp,  offset: %10lu %3i %i %2i "
				    "%2i %10lu\n",
				    EwTag,
				    ewt,
				    iroc,
				    npackets,
				    npackets_exp,
				    offset);
			}

			uint nw = npackets * 4;  // N 4-byte words

			for(uint iw = 0; iw < nw; iw++)
			{
				uint exp_pattern = (iw + offset) & 0xffffffff;

				if(pattern[iw] != exp_pattern)
				{
					nerr += 1;
					NErrRoc[iroc] += 1;
					if(PrintLevel > 1)
					{
						printf(
						    "ERROR: EwTag, ewt roc iw  offset payload[iw] exp_word: "
						    "%10lu %3i %i %3i %10li 0x%08x 0x%08x\n",
						    EwTag,
						    ewt,
						    iroc,
						    iw,
						    offset,
						    pattern[iw],
						    exp_pattern);
					}
				}
			}
		}
		roc = (RocData_t*)(((char*)roc) + roc->header.byteCount);
	}

	*Offset += 2 * 4 * nhits[ewt];

	if(PrintLevel > 10)
		printf("EwTag = %10lx, nb_dtc = %i nerr = %i nerr_roc: %5i %5i %5i %5i %5i %5i\n",
		       EwTag,
		       nb_dtc,
		       nerr,
		       NErrRoc[0],
		       NErrRoc[1],
		       NErrRoc[2],
		       NErrRoc[3],
		       NErrRoc[4],
		       NErrRoc[5]);

	return nerr;
} // end Ui_ValidateVarPatterns()

//==============================================================================
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_MonicaDigiClear(int LinkMask)
{
	if(LinkMask != 0)
		Ui_SetLinkMask(LinkMask);

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(not used)
			continue;
		//-----------------------------------------------------------------------------
		// link is active
		//-----------------------------------------------------------------------------
		auto link = DTCLib::DTC_Link_ID(i);

		// rocUtil write_register -l $LINK -a 28 -w 16 > /dev/null
		getDTC()->WriteROCRegister(link, 28, 0x10, false, 1000);  //

		// Writing 0 & 1 to  address=16 for HV DIGIs ???
		// rocUtil write_register -l $LINK -a 27 -w  0 > /dev/null # write 0
		// rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null ## toggle INIT
		// rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
		getDTC()->WriteROCRegister(link, 27, 0x00, false, 1000);  //
		getDTC()->WriteROCRegister(link, 26, 0x01, false, 1000);  // toggle INIT
		getDTC()->WriteROCRegister(link, 26, 0x00, false, 1000);  //

		// rocUtil write_register -l $LINK -a 27 -w  1 > /dev/null # write 1
		// rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null # toggle INIT
		// rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
		getDTC()->WriteROCRegister(link, 27, 0x01, false, 1000);  //
		getDTC()->WriteROCRegister(link, 26, 0x01, false, 1000);  //
		getDTC()->WriteROCRegister(link, 26, 0x00, false, 1000);  //

		// echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
		// rocUtil write_register -l $LINK -a 25 -w 16 > /dev/null
		getDTC()->WriteROCRegister(link, 25, 0x10, false, 1000);  //

		// rocUtil write_register -l $LINK -a 24 -w  0 > /dev/null # write 0
		// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
		// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
		getDTC()->WriteROCRegister(link, 24, 0x00, false, 1000);  //
		getDTC()->WriteROCRegister(link, 23, 0x01, false, 1000);  //
		getDTC()->WriteROCRegister(link, 23, 0x00, false, 1000);  //

		// rocUtil write_register -l $LINK -a 24 -w  1 > /dev/null # write 1
		// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
		// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
		getDTC()->WriteROCRegister(link, 24, 0x01, false, 1000);  //
		getDTC()->WriteROCRegister(link, 23, 0x01, false, 1000);  //
		getDTC()->WriteROCRegister(link, 23, 0x00, false, 1000);  //
	}
	return 0;
} // end Ui_MonicaDigiClear()

//==============================================================================
/// LaneMask bits
///           0x1 : CAL lane 
///           0x2 : HV  lane 
///           0x4 : CAL lane 
///           0x8 : HV  lane 
/// origin: ~mu2etrk/test_stand/monica_002/var_link_config.sh from Mar 12 202
///  -rwxr-xr-x  1 mu2etrk mu2e      1553 Mar 12 10:11 var_link_config.s
/// configure_ROC 'read' command should be followed by ROC rese
/// to be adde
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_MonicaVarLinkConfig(int LinkMask, int LaneMask)
{
	int rc(0);

	fRocReadoutMode = 1;  // 1: read digis
	if(LinkMask != 0)
		Ui_SetLinkMask(LinkMask);
	// bit 13 - disable reset of the counters by the HB next to the null HB
	// int lane_mask = 0x0300 | LaneMask;
	int lane_mask = 0x2300 | LaneMask;

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			getDTC()->WriteROCRegister(
			    DTCLib::DTC_Link_ID(i), 8, lane_mask, false, 1000);  // enable lanes
		}
	}

	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	int data_version = 1;
	Ui_RocSetDataVersion(data_version);  // Version --> R29

	Ui_ResetRoc();  // use fLinkMask
	//-----------------------------------------------------------------------------
	// according to Monica, this is the place for find_alignment and control_roc_read
	// check if all lanes are ready to be read
	//-----------------------------------------------------------------------------
	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			uint16_t u = getDTC()->ReadROCRegister(DTCLib::DTC_Link_ID(i), 18, 100);
			if((u >> 0x8) != LaneMask)
			{
				// try to recover
				getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 13, 0x1, false, 1000);
				// getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 13,0x0,false,1000);
				// and check again
				u = getDTC()->ReadROCRegister(DTCLib::DTC_Link_ID(i), 18, 100);
				if((u >> 0x8) != LaneMask)
				{
					// still in trouble
					TLOG(TLVL_ERROR) << Form(
					    "ROC on link %i is not ready to read the DIGIs  link mask is "
					    "0x%04x, call Monica and Richie\n",
					    i,
					    u);
					rc -= 1;
				}
			}
		}
	}
	//-----------------------------------------------------------------------------
	return rc;
} // end Ui_MonicaVarLinkConfig()

//==============================================================================
/// origin: test_stand/monica_002/var_pattern_config.sh from Feb 14 202
///  -rwxr-xr-x  1 mu2etrk mu2e      1820 Feb 14 15:00 var_pattern_config.s
/// adding 0x2000 prevents ROC from reinitializing the pattern, so two subsequen
/// buffer test runs would return different result
/// lane mask default: 0x
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Wed Dec  3 23:47:05 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_MonicaVarPatternConfig(int LinkMask, int LaneMask, int NHitsPerLane)
{
	Ui_ResetRoc();  // use fLinkMask
	if(LinkMask != 0)
		Ui_SetLinkMask(LinkMask);
	int version = 1;
	Ui_RocSetDataVersion(version);  // Version --> R29

	int ro_mode            = (fRocReadoutMode >> 0) & 0xf;
	int var_pattern_length = (fRocReadoutMode >> 4) & 0xf;

	if((ro_mode != 0) and (ro_mode != 2))
	{
		TLOG(TLVL_ERROR) << "unknown mode:" << fRocReadoutMode << " BAIL OUT";
		return -1;
	}

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			if(ro_mode == 0)
			{
				//-----------------------------------------------------------------------------
				// mask bit#04=1: variable length
				// mask bit#12=0: 'ROC counter;
				//-----------------------------------------------------------------------------
				getDTC()->WriteROCRegister(
				    DTCLib::DTC_Link_ID(i),
				    8,
				    0x2010,
				    false,
				    1000);  // configure ROC to send variable length patterns
			}
			else
			{
				//-----------------------------------------------------------------------------
				// can only be Mode == 2
				// set number of simulated hits per lane - where that number is coming
				// from? have only 10 bits for the number  of hits mask bit#04=1: variable
				// length mask bit#13=1: don't reset the conters when receiving a null HB
				// mask bit#12=1: 'ROC checkerboard'
				// mask bit#11=1: fixed length patters
				// NHits : 10 LS bits in reg@15
				//-----------------------------------------------------------------------------
				int lane_mask = LaneMask;
				if(lane_mask < 0)
					lane_mask = fRocLaneMask;
				uint16_t mask = 0x3800 | lane_mask;
				if(var_pattern_length == 1)
					mask = mask | 0x00000010;
				else
					mask = mask & 0xffffffef;
				getDTC()->WriteROCRegister(
				    DTCLib::DTC_Link_ID(i),
				    8,
				    mask,
				    false,
				    1000);  // configure ROC to send fixed length patterns

				int nhits = NHitsPerLane;
				if(nhits < 0)
					nhits = fRocNHitsPerLane;
				uint16_t w15 = (nhits & 0x3ff);
				getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 15, w15, false, 1000);

				TLOG(TLVL_DEBUG) << "var_pattern_length:" << var_pattern_length
				                 << " reg#08:0x" << std::hex << std::setw(4)
				                 << std::setfill('0') << mask << " reg#15:0x" << std::hex
				                 << std::setw(4) << std::setfill('0') << w15;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	return 0;
} // end Ui_MonicaVarPatternConfig()

