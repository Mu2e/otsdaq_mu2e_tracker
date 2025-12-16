
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


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form

#include <filesystem>


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"



//==============================================================================
/// FE Macro to call Ui_ProgramRoc_SpiReadFlash()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ProgramRoc_SpiReadFlash(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int Address = __GET_ARG_IN__("Address", int);
	int NWords = __GET_ARG_IN__("NWords", int);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::vector<uint16_t> Res;
	std::stringstream Stream;

	Result =
		Ui_ProgramRoc_SpiReadFlash(Link, Address, NWords, &Res, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__SET_ARG_OUT__("Res", StringMacros::vectorToString(Res));
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ProgramRoc_SpiReadFlash()

// clang-format on
