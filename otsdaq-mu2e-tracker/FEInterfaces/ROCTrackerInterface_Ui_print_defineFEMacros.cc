
//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 14:12:04 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------

// clang-format off


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form

#include <filesystem>


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"



//==============================================================================
/// FE Macro to call Ui_print_PrintRocStatus()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 14:12:04 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_print_PrintRocStatus(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	uint32_t Format = __GET_ARG_IN__("Format (Default = 1)", uint32_t, 1);
	int Link = ROCCoreVInterface::getLinkID();

	std::stringstream Stream;

	Ui_print_PrintRocStatus(Format, Link, Stream);

	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_print_PrintRocStatus()

// clang-format on
