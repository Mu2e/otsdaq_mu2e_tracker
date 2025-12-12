
//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------


int Ui_ConfigureJA(int ClockSource, int Reset = 1);
int Ui_InitEmulatedCFOReadoutMode();
int Ui_InitExternalCFOReadoutMode(int SampleEdgeMode = -1);
int Ui_InitReadout(int EmulateCfo = -1, int RocReadoutMode = -1);
void Ui_InitRocReadoutMode();
void Ui_LaunchRunPlanEmulatedCfo(int EWLength, int NMarkers, int FirstEWTag);
int Ui_ConvertSpiData(const std::vector<uint16_t>& RawData,
	                   trkdaq::TrkSpiData_t*                Data,
	                   int                          PrintLevel = 0);
uint32_t Ui_ReadRegister(uint16_t Register);
int Ui_ReadSpiData(int Link, std::vector<uint16_t>& SpiRawData, int PrintLevel = 0);
void Ui_RocConfigurePatternMode(int LinkMask = 0);
void Ui_ResetRoc(int LinkMask = 0, int SetNewMask = 0);
void Ui_RocSetDataVersion(int Version, int LinkMask = 0);
void Ui_ReadSubevents(std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev,
	                   ulong                                               FirstTS,
	                   int                                                 PrintData,
	                   int                                                 Validate = 0,
	                   const char* OutputFn = nullptr);
std::vector<DTCLib::roc_data_t> Ui_ReadROCBlockEnsured(
	    const DTCLib::DTC_Link_ID& Link, const DTCLib::roc_address_t& address);
std::vector<DTCLib::roc_data_t> Ui_ReadDeviceID(const DTCLib::DTC_Link_ID& Link);
std::string Ui_ReadSerialNumber(const DTCLib::DTC_Link_ID& Link);
Alignment Ui_FindAlignment(DTCLib::DTC_Link_ID Link);
void Ui_FindAlignments(bool print = false, int LinkMask = 0);
void Ui_SetBit(int Register, int Bit, int Value);
void Ui_SetLinkMask(int Mask = 0);
void Ui_SetupCfoInterface(int CFOEmulationMode,
	                       int ForceCFOEdge,
	                       int EnableCFORxTx,
	                       int EnableAutogenDRP);
int Ui_ValidateDigiPatterns(
	    ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
int Ui_ValidateFixedPatterns(
	    ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
int Ui_ValidateVarPatterns(
	    ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
int Ui_MonicaDigiClear(int LinkMask = 0);
int Ui_MonicaVarLinkConfig(int LinkMask = 0, int LaneMask = 0xf);
int Ui_MonicaVarPatternConfig(int LinkMask = 0, int LaneMask = -1, int NHits = -1);
