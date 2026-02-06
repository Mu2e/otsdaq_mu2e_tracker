
// clang-format off

//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
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
/// FE Macro to call Ui_base_InitReadout()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_InitReadout(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int EmulateCfo = __GET_ARG_IN__("EmulateCfo (Default = -1)", int, -1);
	int RocReadoutMode = __GET_ARG_IN__("RocReadoutMode (Default = -1)", int, -1);

	int Result;

	Result = trackerDTC_->InitReadout(EmulateCfo, RocReadoutMode);
		// Ui_base_InitReadout(EmulateCfo, RocReadoutMode);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_InitReadout()

//==============================================================================
/// FE Macro to call Ui_base_InitRocReadoutMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_InitRocReadoutMode(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;


	int Result;

	Result =
		Ui_base_InitRocReadoutMode();

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_InitRocReadoutMode()

//==============================================================================
/// FE Macro to call Ui_base_ConfigureJA()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_ConfigureJA(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int ClockSource = __GET_ARG_IN__("ClockSource (Default = -1)", int, -1);
	int Reset = __GET_ARG_IN__("Reset (Default = -1)", int, -1);

	int Result;

	Result =
		Ui_base_ConfigureJA(ClockSource, Reset);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_ConfigureJA()

//==============================================================================
/// FE Macro to call Ui_base_InitEmulatedCFOReadoutMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_InitEmulatedCFOReadoutMode(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;


	int Result;

	Result =
		Ui_base_InitEmulatedCFOReadoutMode();

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_InitEmulatedCFOReadoutMode()

//==============================================================================
/// FE Macro to call Ui_base_InitExternalCFOReadoutMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_InitExternalCFOReadoutMode(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int SampleEdgeMode = __GET_ARG_IN__("SampleEdgeMode (Default = -1)", int, -1);

	int Result;

	Result =
		Ui_base_InitExternalCFOReadoutMode(SampleEdgeMode);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_InitExternalCFOReadoutMode()

//==============================================================================
/// FE Macro to call Ui_base_LaunchRunPlanEmulatedCfo()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_LaunchRunPlanEmulatedCfo(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int EWLength = __GET_ARG_IN__("EWLength", int);
	int NMarkers = __GET_ARG_IN__("NMarkers", int);
	int FirstEWTag = __GET_ARG_IN__("FirstEWTag", int);


	Ui_base_LaunchRunPlanEmulatedCfo(EWLength, NMarkers, FirstEWTag);


} // end FEMacro_Ui_base_LaunchRunPlanEmulatedCfo()

//==============================================================================
/// FE Macro to call Ui_base_ReadRegister()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_ReadRegister(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	uint16_t Register = __GET_ARG_IN__("Register", uint16_t);

	uint32_t Result;

	Result =
		Ui_base_ReadRegister(Register);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_ReadRegister()

//==============================================================================
/// FE Macro to call Ui_base_ResetLinks()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_ResetLinks(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);
	int SetNewMask = __GET_ARG_IN__("SetNewMask (Default = 0)", int, 0);

	int Result;

	Result =
		Ui_base_ResetLinks(LinkMask, SetNewMask);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_ResetLinks()

//==============================================================================
/// FE Macro to call Ui_base_ResetLink()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_ResetLink(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	int Result;

	Result =
		Ui_base_ResetLink(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_ResetLink()

//==============================================================================
/// FE Macro to call Ui_base_SetBit()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_SetBit(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Register = __GET_ARG_IN__("Register", int);
	int Bit = __GET_ARG_IN__("Bit", int);
	int Value = __GET_ARG_IN__("Value", int);


	Ui_base_SetBit(Register, Bit, Value);


} // end FEMacro_Ui_base_SetBit()

//==============================================================================
/// FE Macro to call Ui_base_SetLinkMask()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_SetLinkMask(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Mask = __GET_ARG_IN__("Mask (Default = 0)", int, 0);


	Ui_base_SetLinkMask(Mask);


} // end FEMacro_Ui_base_SetLinkMask()

//==============================================================================
/// FE Macro to call Ui_base_SetupCfoInterface()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_SetupCfoInterface(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int CFOEmulationMode = __GET_ARG_IN__("CFOEmulationMode", int);
	int ForceCFOEdge = __GET_ARG_IN__("ForceCFOEdge", int);
	int EnableCFORxTx = __GET_ARG_IN__("EnableCFORxTx", int);
	int EnableAutogenDRP = __GET_ARG_IN__("EnableAutogenDRP", int);


	Ui_base_SetupCfoInterface(CFOEmulationMode, ForceCFOEdge, EnableCFORxTx, EnableAutogenDRP);


} // end FEMacro_Ui_base_SetupCfoInterface()

//==============================================================================
/// FE Macro to call Ui_base_GetRocRegistersNames()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_GetRocRegistersNames(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	bool history = __GET_ARG_IN__("history (Default = false)", bool, false);

	std::vector<std::string> Result;

	Result =
		Ui_base_GetRocRegistersNames(history);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));

} // end FEMacro_Ui_base_GetRocRegistersNames()

//==============================================================================
/// FE Macro to call Ui_base_GetRocRegisters()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_GetRocRegisters(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int ilink = __GET_ARG_IN__("ilink", int);
	bool history = __GET_ARG_IN__("history (Default = false)", bool, false);

	std::vector<uint32_t> Result;

	Result =
		Ui_base_GetRocRegisters(ilink, history);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));

} // end FEMacro_Ui_base_GetRocRegisters()

//==============================================================================
/// FE Macro to call Ui_base_GetConvertedRocRegisters()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_GetConvertedRocRegisters(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int ilink = __GET_ARG_IN__("ilink", int);
	bool history = __GET_ARG_IN__("history (Default = false)", bool, false);

	std::vector<float> Result;

	Result =
		Ui_base_GetConvertedRocRegisters(ilink, history);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));

} // end FEMacro_Ui_base_GetConvertedRocRegisters()

//==============================================================================
/// FE Macro to call Ui_base_GetRocID         ()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_GetRocID         (__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	std::string Result;

	Result =
		Ui_base_GetRocID         (Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_GetRocID         ()

//==============================================================================
/// FE Macro to call Ui_base_GetRocDesignInfo ()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_GetRocDesignInfo (__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	std::string Result;

	Result =
		Ui_base_GetRocDesignInfo (Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_GetRocDesignInfo ()

//==============================================================================
/// FE Macro to call Ui_base_GetRocFwGitCommit()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterfaceBase.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_base_GetRocFwGitCommit(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	std::string Result;

	Result =
		Ui_base_GetRocFwGitCommit(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_base_GetRocFwGitCommit()
// clang-format on
