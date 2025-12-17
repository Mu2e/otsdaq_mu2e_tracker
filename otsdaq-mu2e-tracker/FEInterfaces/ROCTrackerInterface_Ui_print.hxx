
// clang-format off

//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------


void Ui_print_PrintRocRegister  (uint Reg, std::string& Desc, int Format = 1, int LinkMask = -1, std::ostream& Stream = std::cout);
void Ui_print_PrintRocRegister2 (uint Reg, std::string& Desc, int Format = 1, int LinkMask = -1, std::ostream& Stream = std::cout);
void Ui_print_PrintRocStatus    (uint32_t Format = 1, int Link = -1, std::ostream& Stream = std::cout) ;
void Ui_print_PrintBuffer        (const void* ptr, int nw, int Offset = 0, std::ostream* Stream = nullptr);
void Ui_print_PrintRatesSingleRoc(std::vector<uint16_t>* Rates, std::vector<int>* ChMask = nullptr, std::ostream& Stream = std::cout);
void Ui_print_PrintRatesAllRocs  (std::vector<uint16_t>* Rates, std::vector<int>* ChMask, std::ostream& Stream = std::cout);
void Ui_print_PrintSpiAll       (trkdaq::TrkSpiData_t* Spi, std::ostream& Stream = std::cout);
// clang-format on
