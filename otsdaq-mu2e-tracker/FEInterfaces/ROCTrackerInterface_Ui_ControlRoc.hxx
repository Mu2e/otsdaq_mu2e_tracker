
//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 14:12:08 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------

// clang-format off


int Ui_ControlRoc_ControlRoc(const char* Command, void* Parameters);
int Ui_ControlRoc_ControlRoc_DigiRW (trkdaq::ControlRoc_DigiRW_Input_t*  Input          ,
                                    trkdaq::ControlRoc_DigiRW_Output_t* Output         ,
                                    int                         LinkMask   = -1,
                                    int                         PrintLevel =  0,
                                    std::ostream&               Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_DumpSettings(int                   Link           ,
                                         int                   Channel    = -1,
                                         int                   PrintLevel =  0,
                                         std::ostream&         Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_Read   (trkdaq::ControlRoc_Read_Input_t0* Par        = nullptr,
                                    int                       Link       = 0    ,
                                    int                       PrintLevel = 0    ,
                                    std::ostream&             Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_PulserOn (int Link              ,
                                      int FirstChannelMask = 0x10,      // first channel:- #4
                                      int DutyCycle        = 10  ,
                                      int PulserDelay      = 1000,
                                      int PrintLevel       = 0x2,
                                      std::ostream& Stream = std::cout);
int Ui_ControlRoc_ControlRoc_PulserOff(int Link, int PrintLevel = 0, std::ostream& Stream= std::cout);
int Ui_ControlRoc_ControlRoc_ReadSettings(int                    Link           ,
                                         int                    Channel        ,
                                         std::vector<uint16_t>& Data           ,
                                         int                    PrintLevel =  0,
                                         std::ostream&          Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_SetCalDac        (int Link, int FirstChannelMask, int PulseHeight,
                                              int PrintLevel = 0, std::ostream& Stream = std::cout);
int Ui_ControlRoc_ControlRoc_SetGain      (int Link, int ChannelID, int PreampType, int Gain     , int PrintLevel = 0);
int Ui_ControlRoc_ControlRoc_SetThreshold (int Link, int ChannelID, int PreampType, int Threshold, int PrintLevel = 0);
int Ui_ControlRoc_ControlRoc_SetThresholds(int Link, uint16_t* TG, int PrintLevel = 0, std::ostream& Stream = std::cout);
int Ui_ControlRoc_ControlRoc_MeasureThresholds(int           Link                   ,
                                              uint32_t      MaskC      = 0xFFFFFFFF,
                                              uint32_t      MaskD      = 0xFFFFFFFF,
                                              uint32_t      MaskE      = 0xFFFFFFFF,
                                              int           PrintLevel = 0x2       ,
                                              std::ostream& Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_PrintThresholds  (int                        Link      ,
                                              std::vector<float>&        Thr       ,
                                              uint32_t      MaskC      = 0xFFFFFFFF,
                                              uint32_t      MaskD      = 0xFFFFFFFF,
                                              uint32_t      MaskE      = 0xFFFFFFFF,
                                              int           PrintLevel = 0x2       ,
                                              std::ostream& Stream     = std::cout );
int Ui_ControlRoc_ControlRoc_ReadThresholds   (int                        Link      ,
                                              std::vector<float>&        Thr       ,
                                              uint32_t      MaskC      = 0xFFFFFFFF,
                                              uint32_t      MaskD      = 0xFFFFFFFF,
                                              uint32_t      MaskE      = 0xFFFFFFFF,
                                              int           PrintLevel = 0x2       ,
                                              std::ostream& Stream     = std::cout );
int Ui_ControlRoc_ConvertSpiData(const std::vector<uint16_t>& RawData,
                                trkdaq::TrkSpiData_t*                Data   ,
                                int                          PrintLevel = 0,
                                std::ostream&                Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_ReadSpi(std::vector<uint16_t>&   SpiRawData     ,
                                    int                      Link       = -1,
                                    int                      PrintLevel = 0 ,
                                    std::ostream&            Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_ReadSpi_1(trkdaq::TrkSpiData_t*          Spi,
                                      int                    Link       = -1,
                                      int                    PrintLevel = 0 ,
                                      std::ostream&          Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_ReadGitCommit(std::string&       GitCommit       ,
                                          int                Link       = -1 ,
                                          int                PrintLevel = 0  ,
                                          std::ostream&      Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_ReadIlp(std::vector<uint16_t>&   RawData         ,
                                    int                      Link       = -1 ,
                                    int                      PrintLevel = 0  ,
                                    std::ostream&            Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_GetKey (std::vector<uint16_t>&   RawData         ,
                                    int                      Link       = -1 ,
                                    int                      PrintLevel = 0  ,
                                    std::ostream&            Stream     = std::cout);
int Ui_ControlRoc_ControlRoc_Rates    (int                     Link            ,
                                      std::vector<uint16_t>*  Output          ,
                                      int                     PrintLevel = 0x2,
                                      trkdaq::ControlRoc_Rates_t*     Par        = nullptr,
                                      std::ostream*           Stream     = nullptr);
int Ui_ControlRoc_ControlRoc_ReadDeviceID(int                    Link,
                                         trkdaq::ControlRoc_DeviceID_t& DevId,
                                         int                    PrintLevel = 0,
                                         std::ostream&          Stream     = std::cout);

// clang-format on
