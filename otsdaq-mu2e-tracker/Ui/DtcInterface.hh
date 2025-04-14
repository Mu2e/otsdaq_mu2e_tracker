//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_interface_hh__
#define __trkdaq_dtc_interface_hh__

#define __CLING__ 1

#include <string>
#include <vector>
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"
#include "dtcInterfaceLib/DTC.h"
#include "iostream"

#include "otsdaq-mu2e-tracker/ParseAlignment/Alignment.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/PrintLegacyTable.hh"
#include "otsdaq-mu2e-tracker/Ui/BisectionSearch.hh"
#include "otsdaq-mu2e-tracker/Ui/PreampChannel.hh"
#include "otsdaq-mu2e-tracker/Ui/PreampThreshold.hh"
#include "otsdaq-mu2e-tracker/Ui/TrkSpiData.hh"
#include "otsdaq-mu2e-tracker/Ui/ControlRocTypes.hh"

namespace trkdaq
{
using roc_serial_t = std::string;

class DtcInterface
{
  public:
	static DtcInterface* fgInstance[2];

	DTCLib::DTC* fDtc;
	int          fEnabled;   // if comes from ODB, could be 0
	int          fPcieAddr;  //
	int          fLinkMask;  // int is OK, bit 31 is never used for arithmetics
	                         // for now assume that all ROCs are doing the same
	                         // fRocReadoutMode: (fixed_length << 4) | readout_mode
	int fRocReadoutMode;     // 0: 'counter patterns' 1:digis 2:checkerboard patterns
	int fRocLaneMask;        // 0xf : all of them
	int fRocNHitsPerLane;    // NHits per lane for Mode=2
	int fSampleEdgeMode;     // 0:force raising 1:force falling 2:auto
	int fEmulateCfo;         // 1: this DTC operated in the emulated CFO mode
	int fJAMode;             // clock_source << 4 | reset

	int fOnSpill;    // 1:on-spill, 0:off-spill
	int fEventMode;  // whatever it is, hopefully, together they make 5 bytes

	int fDtcID;  // unique DTC ID used by the DAQ (0x9154)
	int fPartitionID;
	int fMacAddrByte;

	int fSleepTimeROCWrite;  // the two are different
	int fSleepTimeROCReset;  //
	//    int                  fPrintLevel;                    //

	static const char* fgSpiVarName[TrkSpiDataNWords];  //
	//-----------------------------------------------------------------------------
	// functions
	//-----------------------------------------------------------------------------
  private:
	DtcInterface(int PcieAddr, uint LinkMask, bool SkipInit);

  public:
	virtual ~DtcInterface();

	static DtcInterface* Instance(int  PcieAddr,
	                              uint LinkMask = 0x11,
	                              bool SkipInit = false);

	int PcieAddr() { return fPcieAddr; }

	DTCLib::DTC* Dtc() { return fDtc; }

	static const char* SpiVarName(int I) { return fgSpiVarName[I]; }
	//-----------------------------------------------------------------------------
	// clock source= 0:internal, 1:RTF (RJ45)
	//-----------------------------------------------------------------------------
	int ConfigureJA(int ClockSource, int Reset = 1);
	//-----------------------------------------------------------------------------
	// generic interface to control_ROC.py commands.
	// When/if we figure how to do it better, we'll implement a better solution
	//-----------------------------------------------------------------------------
	int ControlRoc(const char* Command, void* Parameters);

	// need: digi_rw -h 0 -w 1 -a 0x82 -d 0x1388
	int ControlRoc_DigiRW(ControlRoc_DigiRW_Input_t*  Input,
	                      ControlRoc_DigiRW_Output_t* Output,
	                      int                         LinkMask   = -1,
	                      int                         PrintLevel = 0);

	int ControlRoc_Read(ControlRoc_Read_Input_t* Par,
	                    int                      LinkMask   = 0,
	                    bool                     UpdateMask = false,
	                    int                      PrintLevel = 0);
	//-----------------------------------------------------------------------------
	// measure thresholds returns an array of thresholds, which needs to be parsed
	// so far, do it internally
	//-----------------------------------------------------------------------------
	int ControlRoc_MeasureThresholds(int      Link,
	                                 uint32_t MaskC = 0xFFFFFFFF,
	                                 uint32_t MaskD = 0xFFFFFFFF,
	                                 uint32_t MaskE = 0xFFFFFFFF);
	//-----------------------------------------------------------------------------
	// PreampType: 0:HV 1:CAL, or vice versa
	// do one channel at a time
	// shall we think of a block operation ? or not ? - channels could be masked OFFx
	//-----------------------------------------------------------------------------
	int ControlRoc_SetGain(int Link, int ChannelID, int PreampType, int Gain);
	int ControlRoc_SetThreshold(int Link, int ChannelID, int PreampType, int Threshold);

	int ConvertSpiData(const std::vector<uint16_t>& RawData,
	                   TrkSpiData_t*                Data,
	                   int                          PrintLevel = 0);

	int Enabled() { return fEnabled; }
	int EmulateCfo() { return fEmulateCfo; }

	int64_t EventMode() { return (((int64_t)fOnSpill) << 32) | ((int64_t)fEventMode); }

	int DtcID() { return fDtcID; }

	int InitEmulatedCFOReadoutMode();

	// EWLength - in 25 ns ticks
	// to be executed on the emulated CFO side

	void LaunchRunPlanEmulatedCfo(int EWLength, int NMarkers, int FirstEWTag);

	int LinkEnabled(int Link) { return (fLinkMask >> 4 * Link) & 0xf; }

    Alignment FindAlignment(DTCLib::DTC_Link_ID Link);
    void      FindAlignments(bool print=false, int LinkMask=0);

    // TODO revisit these signatures --- what should be vectorized, what not...
    void      ProgramThreshold(const DTCLib::DTC_Link_ID& Link,
                               const PreampChannel& channel,
                               const DTCLib::roc_data_t dac);
    std::vector<PreampThreshold>
              QueryThresholds(const DTCLib::DTC_Link_ID& Link);
    double    ProgramAndQueryThreshold(const DTCLib::DTC_Link_ID& Link,
                                       const PreampChannel& channel,
                                       const DTCLib::roc_data_t dac);
    bool      SetThreshold(const DTCLib::DTC_Link_ID& Link,
                           const PreampChannel& channel,
                           const double threshold,
                           const double tolerance);
    bool      SetThresholds(const DTCLib::DTC_Link_ID& Link,
                            const std::vector<PreampChannel>& channels,
                            std::vector<double>& thresholds,
                            const double tolerance);
 
//-----------------------------------------------------------------------------
// ROC functions
// if LinkMask=0, use fLinkMask
//-----------------------------------------------------------------------------
    void         ResetRoc               (int LinkMask = 0, int SetNewMask = 0);
	// SampleEdgeMode=0: force rising  edge
	//                1: force falling edge
	//                2: auto
	// -1 means use the pre-fetched one
	// success: returns rc=0
	// if rc < 0, can't continue
	int InitExternalCFOReadoutMode(int SampleEdgeMode = -1);

	int  InitReadout(int EmulateCfo = -1, int RocReadoutMode = -1);
	void InitRocReadoutMode();

	int GetLinkMask() { return fLinkMask; }
	//-----------------------------------------------------------------------------
	// assume that to be printed are 'nw' uint16_t words , in hex
	//-----------------------------------------------------------------------------
	void PrintBuffer(const void* ptr, int nw);
	void PrintFireflyTemp();

	void PrintDtcLinkRegisters(uint FirstReg, const char* Desc);
	void PrintRegister(uint16_t Register, const char* Title = "");
	//-----------------------------------------------------------------------------
	// Format = 0 : for each register, print a register and its value
	// Format = 1 : add short description of each register
	// if Link = -1, print a line per register for each ROC
	//-----------------------------------------------------------------------------
	void PrintRocRegister(uint Reg, std::string& Desc, int Format = 1, int LinkMask = -1);
	void PrintRocRegister2(uint         Reg,
	                       std::string& Desc,
	                       int          Format   = 1,
	                       int          LinkMask = -1);
	void PrintRocStatus(int Format = 1, int LinkMask = -1);
	void PrintStatus();

	uint32_t ReadRegister(uint16_t Register);

	int ReadSpiData(int Link, std::vector<uint16_t>& SpiRawData, int PrintLevel = 0);

	void ReadSubevents(std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev,
	                   ulong                                               FirstTS,
	                   int                                                 PrintData,
	                   int                                                 Validate = 0,
	                   const char* OutputFn = nullptr);

	std::vector<DTCLib::roc_data_t> ReadROCBlockEnsured(
	    const DTCLib::DTC_Link_ID& Link, const DTCLib::roc_address_t& address);
	std::vector<DTCLib::roc_data_t> ReadDeviceID(const DTCLib::DTC_Link_ID& Link);
	roc_serial_t                    ReadSerialNumber(const DTCLib::DTC_Link_ID& Link);

	Alignment FindAlignment(DTCLib::DTC_Link_ID Link);
	void      FindAlignments(bool print = false, int LinkMask = 0);
	//-----------------------------------------------------------------------------
	// ROC functions
	// if LinkMask=0, use fLinkMask
	//-----------------------------------------------------------------------------
	void ResetRoc(int LinkMask = 0, int SetNewMask = 0);

	int RocReadoutMode() { return fRocReadoutMode; }

	void RocConfigurePatternMode(int LinkMask = 0);
	void RocSetDataVersion(int Version, int LinkMask = 0);

	void SetOnSpill(int OnSpill) { fOnSpill = OnSpill; }

	// 'Value' : 0 or 1
	void SetBit(int Register, int Bit, int Value);

	void SetEmulateCfo(int EmulateCfo) { fEmulateCfo = EmulateCfo; }
	//-----------------------------------------------------------------------------
	// event mode is specified in the heartbeat packet, non-zero
	// event mode=0 is reserved, last packet of the train
	//-----------------------------------------------------------------------------
	void SetEventMode(int Mode) { fEventMode = Mode; }

	// just cache the DTC ID for future, to evolve

	void SetJAMode(int Mode) { fJAMode = Mode; }

	void SetLinkMask(int Mask = 0);

	void SetRocLaneMask(int Mask) { fRocLaneMask = Mask; }
	void SetRocNHitsPerLane(int NHits) { fRocNHitsPerLane = NHits; }
	void SetRocReadoutMode(int Mode) { fRocReadoutMode = Mode; }
	//-----------------------------------------------------------------------------
	// ForceCFOEdge: bit_6 and bit_5 of the control register 0x9100
	// bit_6: 1:force       0:auto
	// bit_5: 0:rising edge 1:falling edge
	//-----------------------------------------------------------------------------
	void SetupCfoInterface(int CFOEmulationMode,
	                       int ForceCFOEdge,
	                       int EnableCFORxTx,
	                       int EnableAutogenDRP);
	//-----------------------------------------------------------------------------
	// return number of found errors
	//-----------------------------------------------------------------------------
	int ValidateDigiPatterns(
	    ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
	int ValidateFixedPatterns(
	    ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
	int ValidateVarPatterns(
	    ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
	//-----------------------------------------------------------------------------
	// reset digitizers .. to be called in the beginning of each event
	//-----------------------------------------------------------------------------
	int MonicaDigiClear(int LinkMask = 0);
	//-----------------------------------------------------------------------------
	// ROC has 4 lanes: 2 CAL lanes (0x5) and 2 HV lanes (0xa)
	//-----------------------------------------------------------------------------
	int MonicaVarLinkConfig(int LinkMask = 0, int LaneMask = 0xf);
	//-----------------------------------------------------------------------------
	// VarPatternConfig = RocConfigurePatternMode
	//-----------------------------------------------------------------------------
	int MonicaVarPatternConfig(int LinkMask = 0, int LaneMask = -1, int NHits = -1);
};

};  // namespace trkdaq

#endif
