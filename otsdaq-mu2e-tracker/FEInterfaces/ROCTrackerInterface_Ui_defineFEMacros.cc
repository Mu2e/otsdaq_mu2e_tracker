
// clang-format off

//------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
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
/// FE Macro to call Ui_ReadPanelID()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ReadPanelID(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;

	Result =
		trackerDTC_->ReadPanelID(Link, PrintLevel);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_ReadPanelID()

//==============================================================================
/// FE Macro to call Ui_WritePanelID()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_WritePanelID(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int PanelID = __GET_ARG_IN__("PanelID", int);
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	int Result;

	Result =
		trackerDTC_->WritePanelID(Link, PanelID, PrintLevel);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_WritePanelID()

//==============================================================================
/// FE Macro to call Ui_ReadSerialNumber()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ReadSerialNumber(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	DTCLib::DTC_Link_ID Link = ROCCoreVInterface::getLinkID();

	std::string Result;

	Result =
		trackerDTC_->ReadSerialNumber(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_ReadSerialNumber()

//==============================================================================
/// FE Macro to call Ui_RebootMcu()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_RebootMcu(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	int Result;

	Result =
		trackerDTC_->RebootMcu(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_RebootMcu()

//==============================================================================
/// FE Macro to call Ui_ResetLink()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ResetLink(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	int Result;

	Result =
		trackerDTC_->ResetLink(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_ResetLink()

//==============================================================================
/// FE Macro to call Ui_RocConfigurePatternMode()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
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


	int Result;

	Result =
		trackerDTC_->RocConfigurePatternMode();

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_RocConfigurePatternMode()

//==============================================================================
/// FE Macro to call Ui_RocSetDataVersion()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
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

	int Result;

	Result =
		trackerDTC_->RocSetDataVersion(Version, LinkMask);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_RocSetDataVersion()

//==============================================================================
/// FE Macro to call Ui_ReadDeviceID()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ReadDeviceID(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	DTCLib::DTC_Link_ID Link = ROCCoreVInterface::getLinkID();
	int PrintLevel = __GET_ARG_IN__("PrintLevel (Default = 0)", int, 0);

	std::vector<DTCLib::roc_data_t> Result;
	std::stringstream Stream;

	Result =
		trackerDTC_->ReadDeviceID(Link, PrintLevel, Stream);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ReadDeviceID()

//==============================================================================
/// FE Macro to call Ui_MonicaDigiClear()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
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


	int Result;

	Result =
		trackerDTC_->MonicaDigiClear();

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_MonicaDigiClear()

//==============================================================================
/// FE Macro to call Ui_MonicaVarPatternConfig()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
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

	int LaneMask = __GET_ARG_IN__("LaneMask (Default = -1)", int, -1);
	int NHits = __GET_ARG_IN__("NHits (Default = -1)", int, -1);

	int Result;

	Result =
		trackerDTC_->MonicaVarPatternConfig(LaneMask, NHits);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_MonicaVarPatternConfig()

//==============================================================================
/// FE Macro to call Ui_RocBlockRead()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_RocBlockRead(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int Reg = __GET_ARG_IN__("Reg", int);
	int NExpected = __GET_ARG_IN__("NExpected (Default = -1)", int, -1);

	int Result;
	std::vector<uint16_t> Res;

	Result =
		trackerDTC_->RocBlockRead(Link, Reg, Res, NExpected);

	__SET_ARG_OUT__("Result", Result);
	__SET_ARG_OUT__("Res", StringMacros::vectorToString(Res));

} // end FEMacro_Ui_RocBlockRead()

//==============================================================================
/// FE Macro to call Ui_ReadRocDDR()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ReadRocDDR(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int Block = __GET_ARG_IN__("Block", int);

	int Result;
	std::stringstream Stream;

	Result =
		trackerDTC_->ReadRocDDR(Link, Block, Stream);

	__SET_ARG_OUT__("Result", Result);
	__COUT_MULTI__(2, Stream.str());
	__SET_ARG_OUT__("Stream", Stream.str());

} // end FEMacro_Ui_ReadRocDDR()

//==============================================================================
/// FE Macro to call Ui_ReadROCBlockEnsured()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_ReadROCBlockEnsured(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	DTCLib::DTC_Link_ID Link = ROCCoreVInterface::getLinkID();
	DTCLib::roc_address_t address = __GET_ARG_IN__("address", DTCLib::roc_address_t);

	std::vector<DTCLib::roc_data_t> Result;

	Result =
		trackerDTC_->ReadROCBlockEnsured(Link, address);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));

} // end FEMacro_Ui_ReadROCBlockEnsured()

//==============================================================================
/// FE Macro to call Ui_GetRocRegistersNames()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_GetRocRegistersNames(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	bool history = __GET_ARG_IN__("history (Default = false)", bool, false);

	std::vector<std::string> Result;

	Result =
		trackerDTC_->GetRocRegistersNames(history);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));

} // end FEMacro_Ui_GetRocRegistersNames()

//==============================================================================
/// FE Macro to call Ui_GetRocRegisters()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_GetRocRegisters(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int ilink = __GET_ARG_IN__("ilink", int);
	bool history = __GET_ARG_IN__("history (Default = false)", bool, false);

	std::vector<uint32_t> Result;

	Result =
		trackerDTC_->GetRocRegisters(ilink, history);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));

} // end FEMacro_Ui_GetRocRegisters()

//==============================================================================
/// FE Macro to call Ui_GetConvertedRocRegisters()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_GetConvertedRocRegisters(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int ilink = __GET_ARG_IN__("ilink", int);
	bool history = __GET_ARG_IN__("history (Default = false)", bool, false);

	std::vector<float> Result;

	Result =
		trackerDTC_->GetConvertedRocRegisters(ilink, history);

	__SET_ARG_OUT__("Result", StringMacros::vectorToString(Result));

} // end FEMacro_Ui_GetConvertedRocRegisters()

//==============================================================================
/// FE Macro to call Ui_GetRocID()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_GetRocID(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	std::string Result;

	Result =
		trackerDTC_->GetRocID(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_GetRocID()

//==============================================================================
/// FE Macro to call Ui_GetRocDesignInfo()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_GetRocDesignInfo(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	std::string Result;

	Result =
		trackerDTC_->GetRocDesignInfo(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_GetRocDesignInfo()

//==============================================================================
/// FE Macro to call Ui_GetRocFwGitCommit()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_GetRocFwGitCommit(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();

	std::string Result;

	Result =
		trackerDTC_->GetRocFwGitCommit(Link);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_GetRocFwGitCommit()

//==============================================================================
/// FE Macro to call Ui_FindThreshold()
///
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::FEMacro_Ui_FindThreshold(__ARGS__)
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

	int Link = ROCCoreVInterface::getLinkID();
	int ChannelID = __GET_ARG_IN__("ChannelID", int);
	int PreampType = __GET_ARG_IN__("PreampType", int);
	float threshold_mv = __GET_ARG_IN__("threshold_mv", float);
	float tolerance_mv = __GET_ARG_IN__("tolerance_mv", float);

	bool Result;

	Result =
		trackerDTC_->FindThreshold(Link, ChannelID, PreampType, threshold_mv, tolerance_mv);

	__SET_ARG_OUT__("Result", Result);

} // end FEMacro_Ui_FindThreshold()
// clang-format on
