
//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc on Tue Dec 16 14:12:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------

// clang-format off


int Ui_base_InitReadout        (int EmulateCfo = -1, int RocReadoutMode = -1);
int Ui_base_InitRocReadoutMode();
int Ui_base_ConfigureJA(int ClockSource = -1, int Reset = -1);
int Ui_base_InitEmulatedCFOReadoutMode();
int Ui_base_InitExternalCFOReadoutMode(int SampleEdgeMode = -1);
void Ui_base_LaunchRunPlanEmulatedCfo  (int EWLength, int NMarkers, int FirstEWTag);
uint32_t Ui_base_ReadRegister         (uint16_t Register);
int Ui_base_ResetLinks             (int LinkMask = 0, int SetNewMask = 0);
int Ui_base_ResetLink              (int Link);
void Ui_base_SetBit       (int Register, int Bit, int Value);
void Ui_base_SetLinkMask  (int Mask = 0);
void Ui_base_SetupCfoInterface(int CFOEmulationMode, 
                                   int ForceCFOEdge    , 
                                   int EnableCFORxTx   , 
                                   int EnableAutogenDRP);
std::vector<std::string> Ui_base_GetRocRegistersNames     (           bool history = false);
std::vector<uint32_t> Ui_base_GetRocRegisters          (int ilink, bool history = false);
std::vector<float> Ui_base_GetConvertedRocRegisters (int ilink, bool history = false);
std::string Ui_base_GetRocID         (int Link);
std::string Ui_base_GetRocDesignInfo (int Link);
std::string Ui_base_GetRocFwGitCommit(int Link);

// clang-format on
