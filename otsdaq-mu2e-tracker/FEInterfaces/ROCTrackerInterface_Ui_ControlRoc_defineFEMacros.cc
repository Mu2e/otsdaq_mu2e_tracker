
//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
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
/// FE Macro to call Ui_ControlRoc_ControlRoc_DumpSettings()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_DumpSettings(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int Channel = __GET_ARG_IN__("Channel (Default = -1)", int, -1);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_DumpSettings(Link, Channel, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_DumpSettings()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_PulserOn()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_PulserOn(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int FirstChannelMask = __GET_ARG_IN__("FirstChannelMask (Default = 0x10)", int, 0x10);
	int DutyCycle = __GET_ARG_IN__("DutyCycle (Default = 10)", int, 10);
	int PulserDelay = __GET_ARG_IN__("PulserDelay (Default = 1000)", int, 1000);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0x2)", int, 0x2);

	int Result;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_PulserOn(Link, FirstChannelMask, DutyCycle, PulserDelay, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_PulserOn()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_PulserOff()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_PulserOff(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_PulserOff(Link, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_PulserOff()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_ReadSettings()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_ReadSettings(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int Channel = __GET_ARG_IN__("Channel", int);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::vector<uint16_t> Data;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_ReadSettings(Link, Channel, Data, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__SET_ARG_OUT__("Data", StringMacros::vectorToString(Data));
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_ReadSettings()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_SetCalDac()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_SetCalDac(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int FirstChannelMask = __GET_ARG_IN__("FirstChannelMask", int);
	int PulseHeight = __GET_ARG_IN__("PulseHeight", int);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_SetCalDac(Link, FirstChannelMask, PulseHeight, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_SetCalDac()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_SetGain()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_SetGain(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int ChannelID = __GET_ARG_IN__("ChannelID", int);
	int PreampType = __GET_ARG_IN__("PreampType", int);
	int Gain = __GET_ARG_IN__("Gain", int);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;

	Result =
		Ui_ControlRoc_ControlRoc_SetGain(Link, ChannelID, PreampType, Gain, PrintLevel);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_ControlRoc_ControlRoc_SetGain()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_SetThreshold()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_SetThreshold(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int ChannelID = __GET_ARG_IN__("ChannelID", int);
	int PreampType = __GET_ARG_IN__("PreampType", int);
	int Threshold = __GET_ARG_IN__("Threshold", int);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;

	Result =
		Ui_ControlRoc_ControlRoc_SetThreshold(Link, ChannelID, PreampType, Threshold, PrintLevel);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_ControlRoc_ControlRoc_SetThreshold()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_MeasureThresholds()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_MeasureThresholds(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	uint32_t MaskC = __GET_ARG_IN__("MaskC (Default = 0xFFFFFFFF)", uint32_t, 0xFFFFFFFF);
	uint32_t MaskD = __GET_ARG_IN__("MaskD (Default = 0xFFFFFFFF)", uint32_t, 0xFFFFFFFF);
	uint32_t MaskE = __GET_ARG_IN__("MaskE (Default = 0xFFFFFFFF)", uint32_t, 0xFFFFFFFF);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0x2)", int, 0x2);

	int Result;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_MeasureThresholds(Link, MaskC, MaskD, MaskE, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_MeasureThresholds()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_ReadSpi()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_ReadSpi(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::vector<uint16_t> SpiRawData;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_ReadSpi(SpiRawData, Link, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__SET_ARG_OUT__("SpiRawData", StringMacros::vectorToString(SpiRawData));
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_ReadSpi()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_ReadIlp()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_ReadIlp(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::vector<uint16_t> RawData;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_ReadIlp(RawData, Link, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__SET_ARG_OUT__("RawData", StringMacros::vectorToString(RawData));
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_ReadIlp()

//==============================================================================
/// FE Macro to call Ui_ControlRoc_ControlRoc_GetKey ()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc on Tue Dec 16 12:02:00 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ControlRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ControlRoc_ControlRoc_GetKey (__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;
	std::vector<uint16_t> RawData;
	std::stringstream Stream;

	Result =
		Ui_ControlRoc_ControlRoc_GetKey (RawData, Link, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", Result);
	__SET_ARG_OUT__("RawData", StringMacros::vectorToString(RawData));
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ControlRoc_ControlRoc_GetKey ()
