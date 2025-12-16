
//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------


std::string Ui_ReadSerialNumber(const DTCLib::DTC_Link_ID& Link);
int Ui_InitRocReadoutMode() ;
int Ui_RebootMcu          (int Link);
int Ui_ResetLink         (int Link) ;
void Ui_RocConfigurePatternMode();
void Ui_RocSetDataVersion      (int Version, int LinkMask=0);
std::vector<DTCLib::roc_data_t> Ui_ReadDeviceID    (DTCLib::DTC_Link_ID Link,
                                                     int                 PrintLevel = 0,
                                                     std::ostream&       Stream     = std::cout);
Alignment Ui_FindAlignment (DTCLib::DTC_Link_ID Link);
int Ui_FindAlignments(int PrintLevel=1, int Link=-1, std::ostream& Stream = std::cout);
int Ui_ValidateDigiPatterns (ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
int Ui_ValidateFixedPatterns(ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
int Ui_ValidateVarPatterns  (ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
int Ui_MonicaDigiClear();
int Ui_MonicaVarLinkConfig   ();
int Ui_MonicaVarPatternConfig(int LaneMask = -1, int NHits = -1);
void Ui_ReadSubevents     (std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev, 
                                    ulong       FirstTS,
                                    int         PrintData,
                                    int         Validate = 0      , 
                                    const char* OutputFn = nullptr);
int Ui_RocBlockRead      (int Link, int Reg, std::vector<uint16_t>& Res, int NExpected = -1);
int Ui_ReadRocDDR        (int Link, int Block, std::ostream& Stream = std::cout);
std::vector<DTCLib::roc_data_t> Ui_ReadROCBlockEnsured(const DTCLib::DTC_Link_ID& Link,
                                                        const DTCLib::roc_address_t& address);
std::vector<std::string> Ui_GetRocRegistersNames     (bool history = false)            ;
std::vector<uint32_t> Ui_GetRocRegisters          (int ilink, bool history = false) ;
std::vector<float> Ui_GetConvertedRocRegisters (int ilink, bool history = false) ;
std::string Ui_GetRocID         (int Link) ;
std::string Ui_GetRocDesignInfo (int Link) ;
std::string Ui_GetRocFwGitCommit(int Link) ;
float Ui_ProgramAndQueryThreshold(const int Link,
                                   const int ChannelID,
                                   const int PreampType,
                                   const DTCLib::roc_data_t dac);
bool Ui_FindThreshold(const int           Link        ,
                       const int           ChannelID   ,
                       const int           PreampType  ,
                       const float         threshold_mv,
                       const float         tolerance_mv,
                       DTCLib::roc_data_t& out         );
bool Ui_FindThreshold(const int   Link,
                       const int   ChannelID,
                       const int   PreampType,
                       const float threshold_mv,
                       const float tolerance_mv);
