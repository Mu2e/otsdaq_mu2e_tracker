
//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//------------------------------------------------------------------------


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"



//==============================================================================
/// FE Macro to call Ui_ConfigureJA()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ConfigureJA(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int ClockSource = __GET_ARG_IN__("ClockSource", int);
	int Reset = __GET_ARG_IN__("Reset (Default = 1)", int, 1);

	int Result;

	Result =
		Ui_ConfigureJA(ClockSource, Reset);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_ConfigureJA()

//==============================================================================
/// FE Macro to call Ui_InitEmulatedCFOReadoutMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_InitEmulatedCFOReadoutMode(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;


	int Result;

	Result =
		Ui_InitEmulatedCFOReadoutMode();

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_InitEmulatedCFOReadoutMode()

//==============================================================================
/// FE Macro to call Ui_InitExternalCFOReadoutMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_InitExternalCFOReadoutMode(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int SampleEdgeMode = __GET_ARG_IN__("SampleEdgeMode (Default = -1)", int, -1);

	int Result;

	Result =
		Ui_InitExternalCFOReadoutMode(SampleEdgeMode);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_InitExternalCFOReadoutMode()

//==============================================================================
/// FE Macro to call Ui_InitReadout()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_InitReadout(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int EmulateCfo = __GET_ARG_IN__("EmulateCfo (Default = -1)", int, -1);
	int RocReadoutMode = __GET_ARG_IN__("RocReadoutMode (Default = -1)", int, -1);

	int Result;

	Result =
		Ui_InitReadout(EmulateCfo, RocReadoutMode);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_InitReadout()

//==============================================================================
/// FE Macro to call Ui_InitRocReadoutMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_InitRocReadoutMode(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;



	Ui_InitRocReadoutMode();


} // end FEMacro_Ui_InitRocReadoutMode()

//==============================================================================
/// FE Macro to call Ui_LaunchRunPlanEmulatedCfo()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_LaunchRunPlanEmulatedCfo(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int EWLength = __GET_ARG_IN__("EWLength", int);
	int NMarkers = __GET_ARG_IN__("NMarkers", int);
	int FirstEWTag = __GET_ARG_IN__("FirstEWTag", int);


	Ui_LaunchRunPlanEmulatedCfo(EWLength, NMarkers, FirstEWTag);


} // end FEMacro_Ui_LaunchRunPlanEmulatedCfo()

//==============================================================================
/// FE Macro to call Ui_ReadRegister()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ReadRegister(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	uint16_t Register = __GET_ARG_IN__("Register", uint16_t);

	uint32_t Result;

	Result =
		Ui_ReadRegister(Register);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_ReadRegister()

//==============================================================================
/// FE Macro to call Ui_ReadSpiData()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ReadSpiData(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::vector<uint16_t> SpiRawData;

	Result =
		Ui_ReadSpiData(Link, SpiRawData, PrintLevel);

	__SET_ARG_OUT__("Result", Result);
	__SET_ARG_OUT__("SpiRawData", StringMacros::vectorToString(SpiRawData));

} // end FEMacro_Ui_ReadSpiData()

//==============================================================================
/// FE Macro to call Ui_RocConfigurePatternMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_RocConfigurePatternMode(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);


	Ui_RocConfigurePatternMode(LinkMask);


} // end FEMacro_Ui_RocConfigurePatternMode()

//==============================================================================
/// FE Macro to call Ui_ResetRoc()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ResetRoc(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);
	int SetNewMask = __GET_ARG_IN__("SetNewMask (Default = 0)", int, 0);


	Ui_ResetRoc(LinkMask, SetNewMask);


} // end FEMacro_Ui_ResetRoc()

//==============================================================================
/// FE Macro to call Ui_RocSetDataVersion()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_RocSetDataVersion(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Version = __GET_ARG_IN__("Version", int);
	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);


	Ui_RocSetDataVersion(Version, LinkMask);


} // end FEMacro_Ui_RocSetDataVersion()

//==============================================================================
/// FE Macro to call Ui_FindAlignments()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_FindAlignments(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	bool print = __GET_ARG_IN__("print (Default = false)", bool, false);
	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);


	Ui_FindAlignments(print, LinkMask);


} // end FEMacro_Ui_FindAlignments()

//==============================================================================
/// FE Macro to call Ui_SetBit()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_SetBit(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Register = __GET_ARG_IN__("Register", int);
	int Bit = __GET_ARG_IN__("Bit", int);
	int Value = __GET_ARG_IN__("Value", int);


	Ui_SetBit(Register, Bit, Value);


} // end FEMacro_Ui_SetBit()

//==============================================================================
/// FE Macro to call Ui_SetLinkMask()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_SetLinkMask(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Mask = __GET_ARG_IN__("Mask (Default = 0)", int, 0);


	Ui_SetLinkMask(Mask);


} // end FEMacro_Ui_SetLinkMask()

//==============================================================================
/// FE Macro to call Ui_SetupCfoInterface()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_SetupCfoInterface(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int CFOEmulationMode = __GET_ARG_IN__("CFOEmulationMode", int);
	int ForceCFOEdge = __GET_ARG_IN__("ForceCFOEdge", int);
	int EnableCFORxTx = __GET_ARG_IN__("EnableCFORxTx", int);
	int EnableAutogenDRP = __GET_ARG_IN__("EnableAutogenDRP", int);


	Ui_SetupCfoInterface(CFOEmulationMode, ForceCFOEdge, EnableCFORxTx, EnableAutogenDRP);


} // end FEMacro_Ui_SetupCfoInterface()

//==============================================================================
/// FE Macro to call Ui_MonicaDigiClear()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_MonicaDigiClear(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);

	int Result;

	Result =
		Ui_MonicaDigiClear(LinkMask);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_MonicaDigiClear()

//==============================================================================
/// FE Macro to call Ui_MonicaVarLinkConfig()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_MonicaVarLinkConfig(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);
	int LaneMask = __GET_ARG_IN__("LaneMask (Default = 0xf)", int, 0xf);

	int Result;

	Result =
		Ui_MonicaVarLinkConfig(LinkMask, LaneMask);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_MonicaVarLinkConfig()

//==============================================================================
/// FE Macro to call Ui_MonicaVarPatternConfig()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Thu Dec 11 19:43:23 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_MonicaVarPatternConfig(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int LinkMask = 1 << (ROCCoreVInterface::getLinkID() * 4);
	int LaneMask = __GET_ARG_IN__("LaneMask (Default = -1)", int, -1);
	int NHits = __GET_ARG_IN__("NHits (Default = -1)", int, -1);

	int Result;

	Result =
		Ui_MonicaVarPatternConfig(LinkMask, LaneMask, NHits);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_MonicaVarPatternConfig()
