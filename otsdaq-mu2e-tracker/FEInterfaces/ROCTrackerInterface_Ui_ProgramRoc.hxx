
// clang-format off

//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------


int Ui_ProgramRoc_SpiClearMemory    (int Link, const trkdaq::roc_fw_data_t* Dir, int PrintLevel=0, std::ostream& Stream = std::cout);
int Ui_ProgramRoc_SpiIapIndex       (int Link, const trkdaq::roc_fw_data_t* Dir, int PrintLevel=0, std::ostream& Stream = std::cout);
int Ui_ProgramRoc_SpiIapAddress     (int Link, const trkdaq::roc_fw_data_t* Dir, int PrintLevel=0, std::ostream& Stream = std::cout);
int Ui_ProgramRoc_SpiLoadImage      (int Link, const trkdaq::roc_fw_data_t* Dir, int TestMode, int NWrites=-1, int PrintLevel=0, std::ostream& Stream = std::cout);
int Ui_ProgramRoc_SpiReadFlash      (int Link, int Address, int NWords, std::vector<uint16_t>* Res,
                                    int PrintLevel=0, std::ostream& Stream = std::cout);
int Ui_ProgramRoc_SpiWriteDirectory (int Link, const trkdaq::roc_fw_data_t* Dir, int PrintLevel=0, std::ostream& Stream = std::cout);
int Ui_ProgramRoc_SpiWriteRecord    (int Link, int FirstAddr, int NWords, const uint16_t* Data,
                                    int PrintLevel=0, std::ostream& Stream = std::cout);
int Ui_ProgramRoc_ProgramRoc        (int Link, const trkdaq::RocFwData_t* FwData, const char* Version, int Doit=0, int PrintLevel=0, std::ostream& Stream = std::cout);
// clang-format on
