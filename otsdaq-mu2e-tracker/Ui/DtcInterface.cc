//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_interface_cc__
#define __trkdaq_dtc_interface_cc__

#define __CLING__ 1

#include "iostream"
#include "vector"

#include "artdaq-core-mu2e/Overlays/Decoders/TrackerDataDecoder.hh"

#include "DtcInterface.hh"
#include "TString.h"  // includes ROOT's Form

#include "TRACE/tracemf.h"
#define TRACE_NAME "DtcInterface"

using namespace DTCLib;
using namespace std;

namespace trkdaq
{

const char* kSpiVarName[TrkSpiDataNWords] = {
    "I3_3",
    "I2_5",
    "I1_8HV",
    "IHV5_0",  //  0
    "VDMBHV5_0",
    "V1_8HV",
    "V3_3HV",
    "V2_5",  //  4
    "A0",
    "A1",
    "A2",
    "A3",  //  8
    "I1_8CAL",
    "I1_2",
    "ICAL5_0",   // 12
    "ADCSPARE",  // 15
    "V3_3",
    "VCAL5_0",
    "V1_8CAL",
    "V1_0",  // 16
    "ROCPCBTEMP",
    "HVPCBTEMP",
    "CALPCBTEMP",
    "RTD",  // 20
    "ROC_RAIL_1V",
    "ROC_RAIL_1_8V",
    "ROC_RAIL_2_5V",
    "ROC_TEMP",  // 24
    "CAL_RAIL_1V",
    "CAL_RAIL_1_8V",
    "CAL_RAIL_2_5V",
    "CAL_TEMP",  // 28
    "HV_RAIL_1V",
    "HV_RAIL_1_8V",
    "HV_RAIL_2_5V",
    "HV_TEMP"  // 32
};

DtcInterface* DtcInterface::fgInstance[2] = {nullptr, nullptr};

const char* DtcInterface::fgSpiVarName[TrkSpiDataNWords];

//-----------------------------------------------------------------------------
DtcInterface::DtcInterface(int PcieAddr, uint LinkMask, bool SkipInit)
{
	std::string expected_version("");  // dont check
	std::string sim_file("mu2esim.bin");
	std::string uid("");

	TLOG(TLVL_DEBUG) << "CONSTRUCT DTC: pcie_addr:" << PcieAddr << " LinkMask:0x"
	                 << std::hex << LinkMask << std::dec << " SkipInit:" << SkipInit
	                 << std::endl;
	fEnabled         = 1;  // default: enabled
	fPcieAddr        = PcieAddr;
	fRocReadoutMode  = 0;    // for now, assume patterns are the default
	fRocLaneMask     = 0xf;  // all lanes enabled
	fRocNHitsPerLane = 2;    // Monica's default for fRocReadoutMode=2
	fSampleEdgeMode  = 1;
	fEmulateCfo      = 0;
	fJAMode          = 0x11;  // by default, assume RTF clock and reset upon setting

	fDtcID       = 0;  // needed for multi-DTC DAQ, default:0
	fPartitionID = 0;  // use reasonable defaults, which would work for one DTC
	fMacAddrByte = 0;  //

	fOnSpill   = 0;  // together: 0x0100000001
	fEventMode = 1;

	fDtc = new DTC(
	    DTC_SimMode_NoCFO, PcieAddr, LinkMask, expected_version, SkipInit, sim_file, uid);
	//-----------------------------------------------------------------------------
	// constructor performs soft reset
	// if SkipInit = true, links are not initialized
	//-----------------------------------------------------------------------------
	if(SkipInit)
	{
		// get link mask from the DTC

		uint32_t link_mask = ReadRegister(0x9114);
		fLinkMask          = 0;
		for(int i = 0; i < 6; i++)
		{
			if((link_mask >> i) & 0x1)
			{
				fLinkMask |= (0x1 << 4 * i);
			}
		}
	}
	else
	{
		fLinkMask = LinkMask;
	}

	fDtc->SoftReset();

	fSleepTimeROCWrite = 2000;
	fSleepTimeROCReset = 4000;
	//    fDtc->ClearCFOEmulationMode();
	//    fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
}

//-----------------------------------------------------------------------------
DtcInterface::~DtcInterface() {}

//-----------------------------------------------------------------------------
// in many cases, want SkipInit=false
//-----------------------------------------------------------------------------
DtcInterface* DtcInterface::Instance(int PcieAddr, uint LinkMask, bool SkipInit)
{
	int pcie_addr = PcieAddr;
	if(pcie_addr < 0)
	{
		//-----------------------------------------------------------------------------
		// PCIE address is not specified, check environment
		//-----------------------------------------------------------------------------
		if(getenv("DTCLIB_DTC") != nullptr)
			pcie_addr = atoi(getenv("DTCLIB_DTC"));
		else
		{
			TLOG(TLVL_ERROR) << Form(
			    "PcieAddr < 0 and $DTCLIB_DTC is not defined. BAIL out\n");
			return nullptr;
		}
	}
	//-----------------------------------------------------------------------------
	// initialize the variable names just once
	//-----------------------------------------------------------------------------
	if((fgInstance[0] == nullptr) and (fgInstance[1] == nullptr))
	{
		for(int i = 0; i < TrkSpiDataNWords; i++)
		{
			fgSpiVarName[i] = kSpiVarName[i];
		}
	}

	TLOG(TLVL_DEBUG) << "pcie_addr:" << pcie_addr << " LinkMask:0x" << std::hex
	                 << LinkMask << std::dec << " SkipInit:" << SkipInit << std::endl;

	if(fgInstance[pcie_addr] == nullptr)
		fgInstance[pcie_addr] = new DtcInterface(pcie_addr, LinkMask, SkipInit);

	if(fgInstance[pcie_addr]->PcieAddr() != pcie_addr)
	{
		TLOG(TLVL_ERROR) << Form(
		    "DtcInterface::Instance has been already initialized with PcieAddress = %i. "
		    "BAIL out\n",
		    fgInstance[pcie_addr]->PcieAddr());
		return nullptr;
	}
	else
		return fgInstance[pcie_addr];
}

//-----------------------------------------------------------------------------
// Source=0: sync to internal clock ; =1: RTF
// on success, returns 1
//-----------------------------------------------------------------------------
int DtcInterface::ConfigureJA(int ClockSource, int Reset)
{
	int nmax_iter(10);

	fDtc->SetJitterAttenuatorSelect(ClockSource, Reset);  // 0:internal clock sync, 1:RTF
	usleep(100000);
	int ok(0);
	for(int i = 0; i < nmax_iter; i++)
	{
		ok = fDtc->ReadJitterAttenuatorLocked();  // in case of success, returns true
		usleep(100000);
		if(ok == 1)
			break;
	}

	// fDtc->FormatJitterAttenuatorCSR();

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
}

//-----------------------------------------------------------------------------
// according to Ryan, disabling the CFO emulation is critical, otherwise NMarkers
// would be cached for the next time
// EW length         : in units of 25 ns (clock)
// EWMOde            : 1 for buffer test
// EnableClockMarkers: set to 0
// EnableAutogenDRP  : set to 1
//-----------------------------------------------------------------------------
int DtcInterface::InitEmulatedCFOReadoutMode()
{
	//                                 int EWMode, int EnableClockMarkers, int
	//                                 EnableAutogenDRP) {
	int rc(0);

	TLOG(TLVL_DEBUG) << Form("START\n");

	fDtc->DisableCFOEmulation();
	fDtc->DisableReceiveCFOLink();  // r_0x9114:bit_14 = 0
	                                // this one doesn't take DTC_Link_ALL gently
	for(int i = 0; i < 6; i++)
	{
		fDtc->DisableLink(DTC_Link_ID(i), DTC_LinkEnableMode(true, true));
	}

	fDtc->DisableAutogenDRP();

	fDtc->SoftReset();  // write 0x9100:bit_31 = 1

	int clock_source = (fJAMode >> 4) & 0x1;
	int reset        = fJAMode & 0x1;

	rc = ConfigureJA(clock_source, reset);
	if(rc < 0)
		return rc;
	// this one is OK...
	int EnableClockMarkers = 0;
	fDtc->SetCFO40MHzClockMarkerEnable(DTC_Link_ALL, EnableClockMarkers);

	fDtc->EnableCFOEmulatorDRP();  // r_0x9100:bit_24 = 1
	fDtc->EnableAutogenDRP();      // r_0x9100:bit_23 = 1

	fDtc->SetCFOEmulationMode();  // r_0x9100:bit_15 = 1

	fDtc->EnableReceiveCFOLink();  // r_0x9114:bit_14 = 1

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
int DtcInterface::InitExternalCFOReadoutMode(int SampleEdgeMode)
{
	int rc(0);
	TLOG(TLVL_DEBUG) << Form("START SampleEdgeMode=%i\n", fSampleEdgeMode);

	if(SampleEdgeMode != -1)
		fSampleEdgeMode = SampleEdgeMode;

	// this one doesn't take DTC_Link_ALL gently
	for(int i = 0; i < 6; i++)
	{
		fDtc->DisableLink(DTC_Link_ID(i), DTC_LinkEnableMode(true, true));
	}

	// fDtc->HardReset();                  // write 0x9100:bit_00=1
	fDtc->SoftReset();  // write 0x9100:bit_31=1

	fDtc->DisableCFOEmulation();    // r_0x9100:bit_30 = 0
	fDtc->DisableCFOEmulatorDRP();  // r_0x9100:bit_24 = 0
	fDtc->DisableAutogenDRP();      // r_0x9100:bit_23 = 0

	// do it only when the bit is set ?
	fDtc->ClearCFOEmulationMode();  // r_0x9100:bit_15 = 0

	int clock_source = (fJAMode >> 4) & 0x1;
	int reset        = fJAMode & 0x1;

	rc = ConfigureJA(clock_source, reset);
	if(rc < 0)
		return rc;
	// which ROC links should be enabled ? - all active ?
	int EnableClockMarkers = 0;  // for now
	                             // this function handles DTC_Link_ALL correctly
	fDtc->SetCFO40MHzClockMarkerEnable(DTC_Link_ALL, EnableClockMarkers);

	fDtc->SetExternalCFOSampleEdgeMode(fSampleEdgeMode);

	fDtc->EnableAutogenDRP();  // r_0x9100:bit_23

	// dtc->SetCFOEmulationMode();      // r_0x9100:bit_15 = 1

	// dtc->EnableCFOEmulation();       // r_0x9100:bit_30 = 1

	fDtc->EnableReceiveCFOLink();  // r_0x9114:bit_14 = 1

	TLOG(TLVL_DEBUG) << Form("END\n");
	return rc;
}

//-----------------------------------------------------------------------------
// Init Readout
//-----------------------------------------------------------------------------
int DtcInterface::InitReadout(int EmulateCfo, int RocReadoutMode)
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
		rc = InitExternalCFOReadoutMode();
	}
	else
	{
		//-----------------------------------------------------------------------------
		// bit_30 will be restored on the 'emulated CFO side", in the call to
		// InitEmulatedCFOReadoutMode
		//-----------------------------------------------------------------------------
		rc = InitEmulatedCFOReadoutMode();
	}
	if(rc < 0)
		return rc;
	//-----------------------------------------------------------------------------
	// the DTC link mask could be reset by the previous DTC hard reset, so restore it
	// also, release all buffers from the previous read - this is the initialization
	//-----------------------------------------------------------------------------
	SetLinkMask();
	// this should do for now, later - set the partition ID
	// at begin run, for example, as follows

	uint8_t id           = fDtcID & 0xff;
	uint8_t event_mode   = fEventMode & 0xff;
	uint8_t partition_id = fPartitionID & 0xff;
	uint8_t mac_byte     = fMacAddrByte & 0xff;
	fDtc->SetEVBInfo(id, event_mode, partition_id, mac_byte);

	InitRocReadoutMode();
	fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);

	TLOG(TLVL_DEBUG) << "END" << std::endl;
	return rc;
}

//-----------------------------------------------------------------------------
// fRocReadoutMode is supposed to be already set, don't reinitialize
// fRocReadoutMode = 0: read ROC-renerated patterns
//                 = 1: read digis

//-----------------------------------------------------------------------------
// this si fully tracker-specific
//-----------------------------------------------------------------------------
void DtcInterface::InitRocReadoutMode()
{
	TLOG(TLVL_DEBUG) << Form("START : fRocReadoutMode=%i\n", fRocReadoutMode);
	//-----------------------------------------------------------------------------
	// this should be the only place where we reset the ROC
	// ROC readout mode (fixed_length << 4) | readout_mode
	//-----------------------------------------------------------------------------
	// ResetRoc();

	if(((fRocReadoutMode & 0xf) == 0) || ((fRocReadoutMode & 0xf) == 2))
	{
		MonicaVarPatternConfig();  // readout ROC patterns
	}
	else if((fRocReadoutMode & 0xf) == 1)
	{
		MonicaVarLinkConfig();  // readout ROC digis
		MonicaDigiClear();      //
	}
	else
	{
		TLOG(TLVL_DEBUG) << "unknown mode:" << fRocReadoutMode << "> BAIL OUT";
	}
	TLOG(TLVL_DEBUG) << Form("END   : fRocReadoutMode=%i\n", fRocReadoutMode);
}

//-----------------------------------------------------------------------------
// run plan already defined in InitEmulatedCFOReadoutMode
// this function can be executed in a loop, after InitEmulatedCFOReadoutMode
// has been executed once
//-----------------------------------------------------------------------------
void DtcInterface::LaunchRunPlanEmulatedCfo(int EWLength, int NMarkers, int FirstEWTag)
{
	fDtc->DisableCFOEmulation();
	fDtc->SoftReset();  // write 0x9100:bit_31 = 1

	fDtc->SetCFOEmulationEventWindowInterval(EWLength);
	fDtc->SetCFOEmulationNumHeartbeats(NMarkers);

	uint64_t ew_mode = EventMode();  // this really is the event mode

	fDtc->SetCFOEmulationEventMode(ew_mode);

	fDtc->SetCFOEmulationTimestamp(DTC_EventWindowTag((uint64_t)FirstEWTag));

	// this command sends the EWM's
	fDtc->EnableCFOEmulation();  // r_0x9100:bit_30 = 1

	TLOG(TLVL_DEBUG + 10) << Form(
	    "EWLength=%i NMarkers=%i FirstEWTag=%i EventMode=0x%08lx\n",
	    EWLength,
	    NMarkers,
	    FirstEWTag,
	    ew_mode);
}

//-----------------------------------------------------------------------------
int DtcInterface::ConvertSpiData(const std::vector<uint16_t>& Data,
                                 TrkSpiData_t*                Spi,
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
}

//-----------------------------------------------------------------------------
uint32_t DtcInterface::ReadRegister(uint16_t Register)
{
	uint32_t data;
	int      timeout(150);

	mu2edev* dev = fDtc->GetDevice();
	dev->read_register(Register, timeout, &data);

	return data;
}

//-----------------------------------------------------------------------------
int DtcInterface::ReadSpiData(int Link, vector<uint16_t>& SpiRawData, int PrintLevel)
{
	int rc(0);
	//-----------------------------------------------------------------------------
	// is this really needed ? - probably not
	//-----------------------------------------------------------------------------
	// MonicaVarLinkConfig();
	// MonicaDigiClear();
	//-----------------------------------------------------------------------------
	// after writing into reg 258, sleep for some time,
	// then wait till reg 128 returns non-zero
	//-----------------------------------------------------------------------------
	DTC_Link_ID rlink = DTC_ROC_Links[Link];

	fDtc->WriteROCRegister(rlink, 258, 0x0000, false, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	uint16_t u;
	while((u = fDtc->ReadROCRegister(rlink, 128, 100)) == 0) {};
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 128, u);
	//-----------------------------------------------------------------------------
	// register 129: number of words to read, currently-  (+ 4) (ask Monica)
	// 2024-05-10: is r129 now returning the number of bytes ?
	//-----------------------------------------------------------------------------
	int nb = fDtc->ReadROCRegister(rlink, 129, 100);
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 129, nb);

	int nw = nb - 4;

	if(nw != TrkSpiDataNWords)
	{
		TLOG(TLVL_ERROR) << "expected N(words)=" << TrkSpiDataNWords
		                 << " , reported nw=" << nw;
		rc = -1;
	}

	fDtc->ReadROCBlock(SpiRawData, rlink, 258, nw, false, 100);  //
	//-----------------------------------------------------------------------------
	// print SPI data in hex
	//-----------------------------------------------------------------------------
	if((PrintLevel & 0x1) != 0)
	{
		PrintBuffer(SpiRawData.data(), nw);
	}
	//-----------------------------------------------------------------------------
	// parse SPI data and print them
	//-----------------------------------------------------------------------------
	if((PrintLevel & 0x2) != 0)
	{
		struct TrkSpiData_t spi;
		ConvertSpiData(SpiRawData, &spi, PrintLevel);  // &spi[0]
	}

	return rc;
}

//-----------------------------------------------------------------------------
// preserve historic naming convention- Monica named her script 'var_pattern_config'
//-----------------------------------------------------------------------------
void DtcInterface::RocConfigurePatternMode(int LinkMask)
{
	MonicaVarPatternConfig(LinkMask);
}

//-----------------------------------------------------------------------------
// ROC reset : write 0x1 to R14 of each ROC specified as active by the mask
// by default, don't redefine the link mask
//-----------------------------------------------------------------------------
void DtcInterface::ResetRoc(int LinkMask, int UpdateMask)
{
	if((LinkMask != 0) and (UpdateMask != 0))
		fLinkMask = LinkMask;

	int tmo_ms(100);
	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			fDtc->WriteROCRegister(
			    DTC_Link_ID(i), 14, 1, false, tmo_ms);  // 1 --> r14: reset ROC
		}
	}

	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCReset));
}

//-----------------------------------------------------------------------------
// Version --> R29
// as thre is no point inhaving different ROCs with different data versions, assume
// that specifying the mask means that we want it to be redefined
//-----------------------------------------------------------------------------
void DtcInterface::RocSetDataVersion(int Version, int LinkMask)
{
	if(LinkMask != 0)
		fLinkMask = LinkMask;

	int tmo_ms(100);
	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			fDtc->WriteROCRegister(DTC_Link_ID(i), 29, Version, false, tmo_ms);
		}
	}
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
}

//-----------------------------------------------------------------------------
// ROC reset : write 0x1 to register 14
//-----------------------------------------------------------------------------
void DtcInterface::ReadSubevents(std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& VSub,
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
			cout << "ERROR in " << __func__ << " : file " << Fn
			     << " already exists, BAIL OUT" << endl;
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
				cout << "ERROR in " << __func__ << " : failed to open " << Fn
				     << " , BAIL OUT" << endl;
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
		DTC_EventWindowTag event_tag = DTC_EventWindowTag(ewt);
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
					cout << Form(
					    "      event  DTC     EW Tag nbytes   nbytes_tot  link0   nb0  "
					    "link1   nb1  link2   nb2  link3   nb3  link4   nb4  link5   nb5 "
					    " nerr nerr_tot\n");
					cout << Form(
					    "----------------------------------------------------------------"
					    "----------------------------------------------------------------"
					    "------------\n");
					header_printed = 1;
				}
			}
			VSub   = fDtc->GetSubEventData(event_tag, match_ts);
			int sz = VSub.size();
			if(sz == 0)
			{
				if(PrintLevel > 0)
				{
					cout << Form(
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
				DTC_SubEvent* ev       = VSub[i].get();
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
						nerr = ValidateVarPatterns((ushort*)dtc_block.data(),
						                           ew_tag,
						                           &offset,
						                           PrintLevel,
						                           nerr_roc);
					}
					else if((fRocReadoutMode & 0xf) == 1)
					{
						nerr = ValidateDigiPatterns((ushort*)dtc_block.data(),
						                            ew_tag,
						                            &offset,
						                            PrintLevel,
						                            nerr_roc);
					}
					else if((fRocReadoutMode & 0xf) == 2)
					{
						nerr = ValidateFixedPatterns((ushort*)dtc_block.data(),
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
					cout << Form(
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
						PrintBuffer(ev->GetRawBufferPointer(),
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

	//    fDtc->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
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
}

//-----------------------------------------------------------------------------
// wrapper for DTCLib::DTC::ReadROCBlock
//-----------------------------------------------------------------------------
std::vector<roc_data_t> DtcInterface::ReadROCBlockEnsured(const DTC_Link_ID&   Link,
                                                          const roc_address_t& address)
{
	// register 129: number of words to read
	size_t nwords = static_cast<size_t>(fDtc->ReadROCRegister(Link, 129, 1000));
	nwords -= 4;  // account for low-level headers already consumed on-chip

	std::vector<roc_data_t> rv;
	bool                    increment_address = false;  // read via fifo
	fDtc->ReadROCBlock(rv, Link, address, nwords, increment_address, 10000);
	if(rv.size() != nwords)
	{
		std::string msg = "Malformed block read";
		msg += " expected ";
		msg += std::to_string(nwords);
		msg += " words, received ";
		msg += std::to_string(rv.size());
		msg += " words";
		throw cet::exception("DtcInterface::ReadROCBlockEnsured") << msg;
	}

	// reset ddr memory
	fDtc->WriteROCRegister(Link, 14, 0x01, false, 1000);

	// return
	return rv;
}

//-----------------------------------------------------------------------------
// assume that only one link is specified (not DTC_Link_ALL)
// read serial number and device info
//-----------------------------------------------------------------------------
vector<roc_data_t> DtcInterface::ReadDeviceID(const DTCLib::DTC_Link_ID& Link)
{
	vector<roc_data_t> rv;
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
	//    this->ResetRoc(Link,0);
	// write nothing to trigger query
	vector<roc_data_t> empty;
	fDtc->WriteROCBlock(Link, 260, empty, false, false, 1000);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	// read back payload
	rv = this->ReadROCBlockEnsured(Link, 260);

	// reset ddr memory
	fDtc->WriteROCRegister(Link, 14, 0x01, false, 1000);

	// return
	return rv;
}

//-----------------------------------------------------------------------------
roc_serial_t DtcInterface::ReadSerialNumber(const DTCLib::DTC_Link_ID& Link)
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

	auto returned = this->ReadDeviceID(Link);

	stringstream ss;
	ss << "0x";

	// first 16 words are the serial number
	for(size_t i = 0; i < 16; i++)
	{
		ss << hex << returned[i];
	}

	auto rv = ss.str();
	return rv;
}

//-----------------------------------------------------------------------------
// align ROC fpga/adc signals, and optionally print summary table
//-----------------------------------------------------------------------------
Alignment DtcInterface::FindAlignment(DTC_Link_ID Link)
{
	// write parameters into roc to initiate routine
	vector<roc_data_t> writeable = {
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
	fDtc->WriteROCBlock(Link, 264, writeable, false, increment_address, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	// then, wait till reg 128 returns non-zero
	uint16_t u;
	while((u = fDtc->ReadROCRegister(Link, 128, 100)) != 0x8000)
	{
		// idle
	}

	vector<roc_data_t> returned = this->ReadROCBlockEnsured(Link, 264);

	// return
	auto rv = Alignment(returned);
	return rv;
}

//-----------------------------------------------------------------------------
// align ROC fpga/adc signals, and optionally print summary table
//-----------------------------------------------------------------------------
void DtcInterface::FindAlignments(bool print, int LinkMask)
{
	// reset link mask if desired
	if(LinkMask != 0)
		fLinkMask = LinkMask;

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			auto link      = DTC_Link_ID(i);
			auto alignment = FindAlignment(link);
			if(print)
			{
				print_legacy_table(alignment);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// configure itself to use a CFO
//-----------------------------------------------------------------------------
void DtcInterface::SetBit(int Register, int Bit, int Value)
{
	int tmo_ms(100);

	uint32_t data;
	fDtc->GetDevice()->read_register(Register, tmo_ms, &data);

	uint32_t w = (1 << Bit);

	data = (data ^ w) | (Value << Bit);
	fDtc->GetDevice()->write_register(Register, tmo_ms, data);
}

//-----------------------------------------------------------------------------
// by default, enable/disable both TX and RX:
// DTC_LinkEnableMode() = DTC_LinkEnableMode(true,true)
//-----------------------------------------------------------------------------
void DtcInterface::SetLinkMask(int Mask)
{
	if(Mask != 0)
		fLinkMask = Mask;

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used)
			fDtc->EnableLink(DTC_Link_ID(i), DTC_LinkEnableMode());
		else
			fDtc->DisableLink(DTC_Link_ID(i), DTC_LinkEnableMode());
	}
}

//-----------------------------------------------------------------------------
// configure itself to use a CFO
//-----------------------------------------------------------------------------
void DtcInterface::SetupCfoInterface(int CFOEmulationMode,
                                     int ForceCFOEdge,
                                     int EnableCFORxTx,
                                     int EnableAutogenDRP)
{
	// int tmo_ms(150);

	if(CFOEmulationMode == 0)
		fDtc->ClearCFOEmulationMode();
	else
		fDtc->SetCFOEmulationMode();

	// ForceCFOEdge: defines bit_6 and bit_5 of the control register 0x9100
	// bit_6: 1:force       0:auto
	// bit_5: 0:rising edge 1:falling edge
	// ForceCFOEdge = 0 : force use of the rising  edge
	//              = 1 : force use of the falling edge
	//              = 2 : auto

	fDtc->SetExternalCFOSampleEdgeMode(ForceCFOEdge);

	if(EnableCFORxTx == 0)
	{
		fDtc->DisableReceiveCFOLink();
		fDtc->DisableTransmitCFOLink();
	}
	else
	{
		fDtc->EnableReceiveCFOLink();
		fDtc->EnableTransmitCFOLink();
	}

	if(EnableAutogenDRP == 0)
		fDtc->DisableAutogenDRP();
	else
		fDtc->EnableAutogenDRP();
}

struct RocDataHeaderPacket_t
{  // 8 16-byte words in total
   // 16-bit word 0
	uint16_t byteCount : 16;
	// 16-bit word 1
	uint16_t unused : 4;
	uint16_t packetType : 4;
	uint16_t linkID : 3;
	uint16_t DtcErrors : 4;
	uint16_t valid : 1;
	// 16-bit word 2
	uint16_t packetCount : 11;
	uint16_t unused2 : 2;
	uint16_t subsystemID : 3;
	// 16-bit words 3-5
	uint16_t eventTag[3];
	// 16-bit word 6
	uint8_t status : 8;
	uint8_t version : 8;
	// 16-bit word 7
	uint8_t dtcID : 8;
	uint8_t onSpill : 1;
	uint8_t subrun : 2;
	uint8_t eventMode : 5;

	ulong ewtag()
	{
		ulong x1  = eventTag[0];
		ulong x2  = eventTag[1];
		ulong x3  = eventTag[2];
		ulong ewt = x1 | (x2 << 16) | (x3 << 32);
		return ewt;
	}

	// decoding status

	int empty() { return (status & 0x01) == 0; }
	int invalid_dr() { return (status & 0x02); }
	int corrupt() { return (status & 0x04); }
	int timeout() { return (status & 0x08); }
	int overflow() { return (status & 0x10); }

	int error_code() { return (status & 0x1e); }
};

struct RocData_t
{  // 8 16-byte words in total
	RocDataHeaderPacket_t header;
	uint16_t              data[1];
};

// struct RocData_t {
// ushort  nb;
// ushort  header;
// ushort  n_data_packets;  // n data packets, 16 bytes each
// ushort  ewt[3];
// ushort  status;
// ushort  xxx2;
// ushort  data; // array, use it juxsst for memory mapping

// int                 empty     () { return (status & 0x01) == 0; }
// int                 invalid_dr() { return (status & 0x02); }
// int                 corrupt   () { return (status & 0x04); }
// int                 timeout   () { return (status & 0x08); }
// int                 overflow  () { return (status & 0x10); }

// int                 error_code() { return (status & 0x1e); }
//};

//-----------------------------------------------------------------------------
// validate data taken in the tracker ROC pattern generation mode, focus on payload
// returns number of found errors in the payload data
// assume ROC pattern generation
// 'Offset' is the
// PrintLevel =  0: print nothing
//            =  1: print all about errors
//            > 10: full printout
// also returned NErrRoc[6]: number of errors per ROC
// returns nerrors, where does the error code goes ?
//-----------------------------------------------------------------------------
int DtcInterface::ValidateDigiPatterns(
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
}

//-----------------------------------------------------------------------------
int DtcInterface::ValidateFixedPatterns(
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
}

//-----------------------------------------------------------------------------
int DtcInterface::ValidateVarPatterns(
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
}

//-----------------------------------------------------------------------------
int DtcInterface::MonicaDigiClear(int LinkMask)
{
	if(LinkMask != 0)
		SetLinkMask(LinkMask);

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
		fDtc->WriteROCRegister(link, 28, 0x10, false, 1000);  //

		// Writing 0 & 1 to  address=16 for HV DIGIs ???
		// rocUtil write_register -l $LINK -a 27 -w  0 > /dev/null # write 0
		// rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null ## toggle INIT
		// rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
		fDtc->WriteROCRegister(link, 27, 0x00, false, 1000);  //
		fDtc->WriteROCRegister(link, 26, 0x01, false, 1000);  // toggle INIT
		fDtc->WriteROCRegister(link, 26, 0x00, false, 1000);  //

		// rocUtil write_register -l $LINK -a 27 -w  1 > /dev/null # write 1
		// rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null # toggle INIT
		// rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
		fDtc->WriteROCRegister(link, 27, 0x01, false, 1000);  //
		fDtc->WriteROCRegister(link, 26, 0x01, false, 1000);  //
		fDtc->WriteROCRegister(link, 26, 0x00, false, 1000);  //

		// echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
		// rocUtil write_register -l $LINK -a 25 -w 16 > /dev/null
		fDtc->WriteROCRegister(link, 25, 0x10, false, 1000);  //

		// rocUtil write_register -l $LINK -a 24 -w  0 > /dev/null # write 0
		// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
		// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
		fDtc->WriteROCRegister(link, 24, 0x00, false, 1000);  //
		fDtc->WriteROCRegister(link, 23, 0x01, false, 1000);  //
		fDtc->WriteROCRegister(link, 23, 0x00, false, 1000);  //

		// rocUtil write_register -l $LINK -a 24 -w  1 > /dev/null # write 1
		// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
		// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
		fDtc->WriteROCRegister(link, 24, 0x01, false, 1000);  //
		fDtc->WriteROCRegister(link, 23, 0x01, false, 1000);  //
		fDtc->WriteROCRegister(link, 23, 0x00, false, 1000);  //
	}
	return 0;
}

//-----------------------------------------------------------------------------
// LaneMask bits:
//           0x1 : CAL lane 0
//           0x2 : HV  lane 0
//           0x4 : CAL lane 1
//           0x8 : HV  lane 1
//
// origin: ~mu2etrk/test_stand/monica_002/var_link_config.sh from Mar 12 2024
//
//  -rwxr-xr-x  1 mu2etrk mu2e      1553 Mar 12 10:11 var_link_config.sh
//-----------------------------------------------------------------------------
// configure_ROC 'read' command should be followed by ROC reset
//-----------------------------------------------------------------------------
// to be added
//-----------------------------------------------------------------------------
int DtcInterface::MonicaVarLinkConfig(int LinkMask, int LaneMask)
{
	int rc(0);

	fRocReadoutMode = 1;  // 1: read digis
	if(LinkMask != 0)
		SetLinkMask(LinkMask);
	// bit 13 - disable reset of the counters by the HB next to the null HB
	// int lane_mask = 0x0300 | LaneMask;
	int lane_mask = 0x2300 | LaneMask;

	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			fDtc->WriteROCRegister(
			    DTC_Link_ID(i), 8, lane_mask, false, 1000);  // enable lanes
		}
	}

	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	int data_version = 1;
	RocSetDataVersion(data_version);  // Version --> R29

	ResetRoc();  // use fLinkMask
	//-----------------------------------------------------------------------------
	// according to Monica, this is the place for find_alignment and control_roc_read
	// check if all lanes are ready to be read
	//-----------------------------------------------------------------------------
	for(int i = 0; i < 6; i++)
	{
		int used = (fLinkMask >> 4 * i) & 0x1;
		if(used != 0)
		{
			uint16_t u = fDtc->ReadROCRegister(DTC_Link_ID(i), 18, 100);
			if((u >> 0x8) != LaneMask)
			{
				// try to recover
				fDtc->WriteROCRegister(DTC_Link_ID(i), 13, 0x1, false, 1000);
				// fDtc->WriteROCRegister(DTC_Link_ID(i), 13,0x0,false,1000);
				// and check again
				u = fDtc->ReadROCRegister(DTC_Link_ID(i), 18, 100);
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
}

//-----------------------------------------------------------------------------
// origin: test_stand/monica_002/var_pattern_config.sh from Feb 14 2024
//
//  -rwxr-xr-x  1 mu2etrk mu2e      1820 Feb 14 15:00 var_pattern_config.sh
// adding 0x2000 prevents ROC from reinitializing the pattern, so two subsequent
// buffer test runs would return different results
// lane mask default: 0xf
//-----------------------------------------------------------------------------
int DtcInterface::MonicaVarPatternConfig(int LinkMask, int LaneMask, int NHitsPerLane)
{
	ResetRoc();  // use fLinkMask
	if(LinkMask != 0)
		SetLinkMask(LinkMask);
	int version = 1;
	RocSetDataVersion(version);  // Version --> R29

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
				fDtc->WriteROCRegister(
				    DTC_Link_ID(i),
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
				fDtc->WriteROCRegister(
				    DTC_Link_ID(i),
				    8,
				    mask,
				    false,
				    1000);  // configure ROC to send fixed length patterns

				int nhits = NHitsPerLane;
				if(nhits < 0)
					nhits = fRocNHitsPerLane;
				uint16_t w15 = (nhits & 0x3ff);
				fDtc->WriteROCRegister(DTC_Link_ID(i), 15, w15, false, 1000);

				TLOG(TLVL_DEBUG) << "var_pattern_length:" << var_pattern_length
				                 << " reg#08:0x" << std::hex << std::setw(4)
				                 << std::setfill('0') << mask << " reg#15:0x" << std::hex
				                 << std::setw(4) << std::setfill('0') << w15;
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	return 0;
}

};  // namespace trkdaq

#endif
