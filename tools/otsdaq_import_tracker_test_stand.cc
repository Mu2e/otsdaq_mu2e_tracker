#include "otsdaq/MessageFacility/MessageFacility.h"

#include <dirent.h>
#include <cassert>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

#include "otsdaq/Macros/StringMacros.h"

// Shared test utilities
#include "otsdaq/Macros/TestUtilities.h"

///
/// @file otsdaq_import_tracker_test_stand.cc
/// @brief
/// Extracts the function definitions in the tracker Ui folder into
///		the otsdaq-mu2e-tracker/otsdaq-mu2e-tracker/FEInterfaces/ folder (or any folder)
///
///	Warning! it will overwrite files in the output folder with the same names!
///
/// usage:
/// otsdaq_import_tracker_test_stand <import Ui path> <export FEInterface path>
///
/// Created by rrivera, December 2025
///

using namespace ots;

//==============================================================================
/// @brief Modifies the source code string for the FEInterface as follows:
///		- Replaces instances of the fDtc with getDTC()
/// @param source the source code to modify
/// @param isHeader whether the source is a header file
/// @param outputFile
std::string modifySource(std::string& source, bool isHeader = false)
{
	std::vector<std::pair<std::string, std::string>> replacements = {
	    {"fDtc", "getDTC()"},
	    {"getDTC()ID", "fDtcID"},  // fix accidental replacement of member variable

	    {"TrkSpiData_t", "trkdaq::TrkSpiData_t"},
	    {"roc_data_t", "DTCLib::roc_data_t"},
	    {"DTC_Link_ID", "DTCLib::DTC_Link_ID"},
	    {"DTC_SubEvent", "DTCLib::DTC_SubEvent"},
	    {"DTC_EventWindowTag", "DTCLib::DTC_EventWindowTag"},
	    {"roc_address_t", "DTCLib::roc_address_t"},
	    {"DTC_LinkEnableMode", "DTCLib::DTC_LinkEnableMode"},
	    {"DTC_Link_ALL", "DTCLib::DTC_Link_ALL"},
	    {"EventMode()", "((((int64_t)fOnSpill) << 32) | ((int64_t)fEventMode))"},
	    {"DTC_ROC_Links", "DTCLib::DTC_ROC_Links"},
	    {"ControlRoc_DeviceID_t", "trkdaq::ControlRoc_DeviceID_t"},
	    {"TrkSpiDataNWords", "trkdaq::TrkSpiDataNWords"},

	    // {"RocRegisters", "trkdaq::RocRegisters"},
	    {"trkdaq::RocRegisters", "RocRegisters"},
	    {"Gettrkdaq::RocRegisters", "GetRocRegisters"},  // fix accidental replacement
	    {"GetConvertedtrkdaq::RocRegisters",
	     "GetConvertedRocRegisters"},  // fix accidental replacement

	    {"roc_fw_data_t", "trkdaq::roc_fw_data_t"},
	    {"roc_fw_version_t", "trkdaq::roc_fw_version_t"},
	    {"RocFwData_t", "trkdaq::RocFwData_t"},
	    {"ControlRoc_DigiRW_Input_t", "trkdaq::ControlRoc_DigiRW_Input_t"},
	    {"ControlRoc_DigiRW_Output_t", "trkdaq::ControlRoc_DigiRW_Output_t"},
	    {"ControlRoc_Read_Input_t0", "trkdaq::ControlRoc_Read_Input_t0"},
	    {"ControlRoc_Read_Output_t0", "trkdaq::ControlRoc_Read_Output_t0"},
	    {"ControlRoc_Rates_t", "trkdaq::ControlRoc_Rates_t"},
	    {"REG_", "trkdaq::REG_"},

	    {"trkdaq::REG_STATUS", "REG_STATUS"},

	    {"roc_serial_t", "std::string"},

	    {"vector", "std::vector"},
	    {"string", "std::string"},
	    {"to_std::string", "std::to_string"},  // fix accidental replacement
	    {"ifstream", "std::ifstream"},
	    {"ofstream", "std::ofstream"},
	    {"fstream", "std::fstream"},
	    {"std::istd::fstream", "std::ifstream"},  // fix accidental replacement
	    {"cout", "std::cout"},
	    {"endl", "std::endl"},
	    {"stringstream", "std::stringstream"},
	    {"hex", "std::hex"},
	    {"dec", "std::dec"},

	    {"override", ""},

	    {"trkdaq::trkdaq::", "trkdaq::"},
	    {"std::std::std::", "std::"},
	    {"std::std::", "std::"},
	    {"DTCLib::DTCLib::", "DTCLib::"},
	};
	std::vector<std::pair<std::string, std::string>> functionReplacements = {
	    {"ConfigureJA", "Ui_base_ConfigureJA"},
	    {"InitEmulatedCFOReadoutMode", "Ui_base_InitEmulatedCFOReadoutMode"},
	    {"InitExternalCFOReadoutMode", "Ui_base_InitExternalCFOReadoutMode"},
	    {"ResetLinks", "Ui_base_ResetLinks"},
	    {"SetLinkMask", "Ui_base_SetLinkMask"},

	    {"ResetLink", "Ui_ResetLink"},

	    {"Ui_base_Ui_ResetLinks", "Ui_base_ResetLinks"},  // fix accidental replacement

	    {"InitReadout", "Ui_InitReadout"},
	    {"InitRocReadoutMode", "Ui_InitRocReadoutMode"},
	    {"LaunchRunPlanEmulatedCfo", "Ui_LaunchRunPlanEmulatedCfo"},
	    {"ReadRegister", "Ui_ReadRegister"},
	    {"ReadSpiData", "Ui_ReadSpiData"},
	    {"RocConfigurePatternMode", "Ui_RocConfigurePatternMode"},
	    {"ResetRoc", "Ui_ResetRoc"},
	    {"RocSetDataVersion", "Ui_RocSetDataVersion"},
	    {"ReadSubevents", "Ui_ReadSubevents"},
	    {"ReadROCBlockEnsured", "Ui_ReadROCBlockEnsured"},
	    {"ReadDeviceID", "Ui_ReadDeviceID"},
	    {"ReadSerialNumber", "Ui_ReadSerialNumber"},
	    {"FindAlignment", "Ui_FindAlignment"},
	    {"FindAlignments", "Ui_FindAlignments"},
	    {"SetBit", "Ui_SetBit"},
	    {"SetupCfoInterface", "Ui_SetupCfoInterface"},
	    {"ValidateDigiPatterns", "Ui_ValidateDigiPatterns"},
	    {"ValidateFixedPatterns", "Ui_ValidateFixedPatterns"},
	    {"ValidateVarPatterns", "Ui_ValidateVarPatterns"},
	    {"MonicaDigiClear", "Ui_MonicaDigiClear"},
	    {"MonicaVarLinkConfig", "Ui_MonicaVarLinkConfig"},
	    {"MonicaVarPatternConfig", "Ui_MonicaVarPatternConfig"},
	    {"GetRocRegisters", "Ui_GetRocRegisters"},
	    {"ProgramAndQueryThreshold", "Ui_ProgramAndQueryThreshold"},
	    {"FindThreshold", "Ui_FindThreshold"},
	    {"RocBlockRead", "Ui_RocBlockRead"},

	    {"PrintBuffer", "Ui_print_PrintBuffer"},
	    {"PrintRocRegister", "Ui_print_PrintRocRegister"},
	    {"PrintRocRegister2", "Ui_print_PrintRocRegister2"},
	    {"Ui_print_Ui_print_", "Ui_print_"},  // fix accidental replacement

	    {"ControlRoc_ReadSpi", "Ui_ControlRoc_ControlRoc_ReadSpi"},
	    {"ConvertSpiData", "Ui_ControlRoc_ConvertSpiData"},
	    {"ControlRoc_Ui_ReadDeviceID", "Ui_ControlRoc_ControlRoc_ReadDeviceID"},
	    {"ControlRoc_ReadGitCommit", "Ui_ControlRoc_ControlRoc_ReadGitCommit"},
	    {"ControlRoc_SetThreshold", "Ui_ControlRoc_ControlRoc_SetThreshold"},
	    {"ControlRoc_ReadThresholds", "Ui_ControlRoc_ControlRoc_ReadThresholds"},
	    {"ControlRoc_ReadSettings", "Ui_ControlRoc_ControlRoc_ReadSettings"},
	    {"ControlRoc_PrintThresholds", "Ui_ControlRoc_ControlRoc_PrintThresholds"},

	    {"SpiWriteRecord", "Ui_ProgramRoc_SpiWriteRecord"},
	    {"SpiLoadImage", "Ui_ProgramRoc_SpiLoadImage"},
	    {"SpiIapIndex", "Ui_ProgramRoc_SpiIapIndex"},
	    {"SpiWriteDirectory", "Ui_ProgramRoc_SpiWriteDirectory"},
	    {"SpiClearMemory", "Ui_ProgramRoc_SpiClearMemory"},

	};

	for(const auto& [oldStr, newStr] : replacements)
	{
		size_t pos = 0;
		while((pos = source.find(oldStr, pos)) != std::string::npos)
		{
			source.replace(pos, oldStr.length(), newStr);
			pos += newStr.length();
		}
	}
	if(!isHeader)
	{
		for(const auto& [oldStr, newStr] : functionReplacements)
		{
			size_t pos = 0;
			while((pos = source.find(oldStr, pos)) != std::string::npos)
			{
				source.replace(pos, oldStr.length(), newStr);
				pos += newStr.length();
			}
		}
	}
	return source;
}  // end modifySource()

//==============================================================================
/// @brief Returns true if the function arguments in the function header are simple enough
/// (i.e. int, string) for converting to FE macros
///		Also fills in the argTypes, argNames, argDefaults, outputTypes, outputNames
/// vectors.
/// @param functionHeader the full function header
bool validateArgsForFEMacro(const std::string&        functionHeader,
                            const std::string&        returnVal,
                            std::vector<std::string>& argTypes,
                            std::vector<std::string>& argNames,
                            std::vector<std::string>& argDefaults,
                            std::vector<std::string>& outputTypes,
                            std::vector<std::string>& outputNames,
                            bool                      ignoreAllowedTypes = false)
{
	size_t argTypePos = functionHeader.find('(');
	if(argTypePos == std::string::npos)
	{
		__SS__ << "ERROR: could not find '(' in function header: " << functionHeader
		       << __E__;
		__SS_THROW__;
	}
	size_t argEndPos = functionHeader.find(')', argTypePos);
	if(argEndPos == std::string::npos)
	{
		__SS__ << "ERROR: could not find ')' in function header: " << functionHeader
		       << __E__;
		__SS_THROW__;
	}

	// decide if there is an output return valu
	// for now, only handle void return type with no outputs
	if(!ignoreAllowedTypes && returnVal != "void" && returnVal != "int" &&
	   returnVal != "unsigned int" && returnVal != "std::string" && returnVal != "bool" &&
	   returnVal != "double" && returnVal != "float" &&
	   returnVal != "std::vector<DTCLib::roc_data_t>" &&
	   returnVal != "std::vector<std::string>" && returnVal != "std::vector<uint32_t>" &&
	   returnVal != "std::vector<float>" && returnVal != "size_t" &&
	   returnVal != "uint16_t" && returnVal != "uint32_t")
	{
		__COUT_ERR__ << "- Return type '" << returnVal
		             << "' is not supported for FE Macro conversion in function header: "
		             << functionHeader << std::endl;
		return false;
	}
	if(returnVal != "void")
	{
		outputTypes.push_back(returnVal);
		outputNames.push_back("Result");
	}
	else  // indicate no return value
	{
		outputTypes.push_back("void");
		outputNames.push_back("");
	}

	std::string argList =
	    functionHeader.substr(argTypePos + 1, argEndPos - argTypePos - 1);
	__COUTTV__(argList);

	// remove any comments from argList
	while(1)
	{
		size_t commentPos = argList.find("//");
		if(commentPos == std::string::npos)
			break;
		size_t endOfLinePos = argList.find('\n', commentPos);
		if(endOfLinePos == std::string::npos)
			endOfLinePos = argList.size();
		argList.erase(commentPos, endOfLinePos - commentPos);
	}  // end comment removal loop

	// split by commas
	std::vector<std::string> args = StringMacros::getVectorFromString(argList, {','});
	for(const auto& arg : args)
	{
		__COUTTV__(arg);

		if(!arg.size())
			break;  // skip empty args (possible for no args)

		std::vector<std::string> argParts = StringMacros::getVectorFromString(
		    arg, {' ', '\t', '\n'}, {} /* whitespace */);
		if(argParts.size() < 2)
		{
			__COUT_ERR__ << "Could not parse argument '"
			             << StringMacros::vectorToString(argParts)
			             << "' in function header: " << functionHeader << std::endl;
			return false;
		}

		size_t      i                   = 0;
		bool        foundEquals         = false;
		bool        isConst             = false;
		bool        isOutputByReference = false;
		std::string argType;
		for(const auto& part : argParts)
		{
			__COUTT__ << "pre-part " << part << " i=" << i << std::endl;

			if(part.size() == 0)
				continue;  // skip empty parts

			if(part.size() && (part == " " || part == "\t" || part == "\n"))
				continue;  // skip whitespace parts

			__COUTT__ << "part " << part << " i=" << i << std::endl;

			if(i == 0)
			{
				argType = part;  // first part is type
				__COUTTV__(argType);
				if(argType == "const")
				{
					isConst = true;
					continue;
				}
				__COUTTV__(argType);

				// check if argType is simple enough
				if(!ignoreAllowedTypes && argType != "int" && argType != "uint" &&
				   argType != "unsigned int" && argType != "std::string" &&
				   argType != "bool" && argType != "double" && argType != "float" &&
				   argType != "DTCLib::DTC_Link_ID" &&
				   (argType != "DTCLib::DTC_Link_ID&" || !isConst) &&
				   argType != "DTCLib::roc_address_t&" && argType != "std::ostream&" &&
				   (argType != "std::vector<DTCLib::roc_data_t>&" || isConst) &&
				   (argType != "std::vector<uint16_t>&" || isConst) &&
				   (argType != "std::vector<uint16_t>*" || isConst) &&
				   argType != "size_t" && argType != "uint16_t" && argType != "uint32_t")
				{
					__COUT_ERR__ << "- Argument type '" << (isConst ? "const " : "")
					             << argType
					             << "' is not supported for FE Macro conversion in "
					                "function header: "
					             << functionHeader << std::endl;
					return false;
				}
				__COUTTV__(argType);

				// do not add "const," since FE Macro must set parameter to pass (and
				// argType search cannot overload const/no-const anyway)
				//  if(isConst)
				//  	argType = "const " + argType;

				if(argType == "std::vector<uint16_t>&" ||
				   argType == "std::vector<uint16_t>*" || argType == "std::ostream&" ||
				   argType == "std::vector<roc_data_t>&")
				{
					__COUTT__ << " -- Treating " << argType << " as output parameter."
					          << std::endl;
					isOutputByReference = true;
				}

				if(isOutputByReference)
					argTypes.push_back(
					    std::string("out") +
					    (argType[argType.size() - 1] == '*'
					         ? "*"
					         : ""));  // mark as output by reference or by pointer
				else
					argTypes.push_back(argType);
				__COUTTV__(argTypes.back());
			}
			else if(i == 1)
			{
				if(part.find('=') != std::string::npos)
				{
					foundEquals = true;

					__COUTT__ << "Found default value in name part: " << part
					          << std::endl;
					auto equalParts = StringMacros::getVectorFromString(part, {'='});
					if(equalParts.size() != 2)
					{
						__COUT_ERR__ << "Could not parse argument with = in '"
						             << StringMacros::vectorToString(argParts)
						             << "' in function header: " << functionHeader
						             << std::endl;
						return false;
					}
					argNames.push_back(equalParts[0]);  // second part is name

					if(equalParts[1].size() == 0)  // case is 'name= Default'
					{
						__COUTT__ << "No default value YET after = in name part: " << part
						          << std::endl;
					}
					else
					{
						__COUTT__ << "Found default value after = in name part: " << part
						          << std::endl;
						argDefaults.push_back(equalParts[1]);  // default value
						__COUTTV__(argDefaults.back());
					}
				}
				else
				{
					__COUTT__ << "No default value in name part: " << part << std::endl;
					argNames.push_back(part);  // second part is name
				}
				__COUTTV__(argNames.back());

				if(isOutputByReference)
				{
					outputTypes.push_back(argType);
					outputNames.push_back(argNames.back());
				}
			}
			else if(i > 1 && !foundEquals)  // possible 'name =Default' here
			{
				if(part.size() > 1 && part[0] == '=')  // handle case of no spaces around
				                                       // =
				{
					argDefaults.push_back(part.substr(1));  // fourth part is default
					                                        // value
					__COUTTV__(argDefaults.back());
				}
				else if(part == "=")
				{
					foundEquals = true;  // next part is default value
					__COUTTV__(foundEquals);
				}
				else
				{
					__COUT_ERR__ << "Could not parse argument missing = in '"
					             << StringMacros::vectorToString(argParts)
					             << "' in function header: " << functionHeader
					             << std::endl;
					return false;
				}
			}
			else if(i > 1 && foundEquals)  // fourth part is default value
			{
				if(argDefaults.size() < argNames.size())
				{
					argDefaults.push_back(part);
					__COUTTV__(argDefaults.back());
				}
				else
				{
					__COUT_ERR__
					    << "Could not parse argument with multiple default values in '"
					    << StringMacros::vectorToString(argParts)
					    << "' in function header: " << functionHeader << std::endl;
					return false;
				}
			}

			++i;
		}  // end art parts loop

		if(argDefaults.size() < argNames.size())
		{
			argDefaults.push_back("");  // no default value
			__COUTTV__(argDefaults.back());
		}
		else if(argDefaults.size() > argNames.size())
		{
			__COUT_ERR__ << "Could not parse argument with multiple default values in '"
			             << StringMacros::vectorToString(argParts)
			             << "' in function header: " << functionHeader << std::endl;
			return false;
		}
	}  // end main arg loop

	// if here, all args are simple enough
	__COUTTV__(StringMacros::vectorToString(argTypes));
	__COUTTV__(StringMacros::vectorToString(argNames));
	__COUTTV__(StringMacros::vectorToString(argDefaults));
	__COUTTV__(StringMacros::vectorToString(outputTypes));
	__COUTTV__(StringMacros::vectorToString(outputNames));

	return true;
}  // end validateArgsForFEMacro()

//==============================================================================
/// @brief Extracts the declared function header from the full function header
/// @param functionHeader the full function header
/// @param sourceFile the source file the function is from
std::string getDeclaredFunctionHeader(const std::string& functionHeader,
                                      const std::string& sourceFile)
{
	// general case
	size_t parenPos = functionHeader.find('(');
	if(parenPos == std::string::npos)
	{
		__SS__ << "ERROR: could not find '(' in function header: " << functionHeader
		       << __E__;
		__SS_THROW__;
	}
	std::string beforeParen = functionHeader.substr(0, parenPos);
	__COUTTV__(beforeParen);

	// get arg list to match when finding declaration in header file
	std::vector<std::string> argTypes, argNames, argDefaults, outputTypes, outputNames;
	{  // extract arg types (do not modify types, since comparing against unmodified
	   // header)
		if(!validateArgsForFEMacro(functionHeader,
		                           "void" /* dummy return val arg */,
		                           argTypes,
		                           argNames,
		                           argDefaults,
		                           outputTypes,
		                           outputNames,
		                           true /* ignoreAllowedTypes */))
		{
			__SS__ << "ERROR: could not validate args for function header: "
			       << functionHeader << __E__;
			__SS_THROW__;
		}
	}

	// find in file the function name
	std::ifstream in(sourceFile);
	if(!in)
	{
		__SS__ << "\n\nERROR! Could not open file at " << sourceFile
		       << " - error: " << errno << " - " << strerror(errno) << __E__;
		__SS_THROW__;
	}
	std::string fileContents((std::istreambuf_iterator<char>(in)),
	                         std::istreambuf_iterator<char>());
	// fileContents now contains the full contents of sourceFile
	size_t funcNamePos = 0;
	while(1)  // search for function name loop in header
	{
		funcNamePos = fileContents.find(beforeParen, funcNamePos + 1);
		if(funcNamePos == std::string::npos)
		{
			__SS__ << "\nERROR: could not find function name '" << beforeParen
			       << "' in source file: " << sourceFile << __E__;
			__SS_THROW__;
		}
		__COUTTV__(fileContents.substr(funcNamePos, 50));
		// make sure found instance is not a comment or header with similar name
		size_t prevNewLine = fileContents.rfind("\n", funcNamePos);
		size_t prevComment = fileContents.rfind("//", funcNamePos);
		if(prevComment != std::string::npos && prevNewLine != std::string::npos &&
		   prevComment > prevNewLine)
		{
			__COUTT__ << "Found function name in comment, continuing search..."
			          << std::endl;
			continue;  // then found function name is in comment
		}

		// make sure next character is whitespace or '(' for arg list
		size_t charAfterFuncNamePos = funcNamePos + beforeParen.size();
		if(charAfterFuncNamePos >= fileContents.size() ||
		   (fileContents[charAfterFuncNamePos] != ' ' &&
		    fileContents[charAfterFuncNamePos] != '\t' &&
		    fileContents[charAfterFuncNamePos] != '\n' &&
		    fileContents[charAfterFuncNamePos] != '('))
		{
			__COUTT__
			    << "Found function name as part of another word, continuing search..."
			    << std::endl;
			continue;  // then found function name is part of another word
		}

		// make sure there is an arg list after function name
		size_t nextNewline     = fileContents.find("\n", funcNamePos);
		size_t nextParenthesis = fileContents.find("(", funcNamePos);

		if(nextParenthesis == std::string::npos ||
		   (nextNewline != std::string::npos && nextParenthesis > nextNewline))
		{
			__COUTT__ << "Found function name without arg list, continuing search..."
			          << std::endl;
			continue;  // then found function name is not a function definition
		}

		// make sure all arg types exist to match function definition (Note: arg name
		// mismatch is allowed in C++)
		size_t argEndPos = fileContents.find(')', nextParenthesis);
		if(argEndPos == std::string::npos)
		{
			__COUTT__ << "Found function name without arg list end, continuing search..."
			          << std::endl;
			continue;  // then found function name is not a function definition
		}
		size_t argTypePos    = nextParenthesis;
		size_t maxArgTypePos = nextParenthesis;
		bool   mismatch      = false;
		for(const auto& argType : argTypes)  // input arg types confirmation loop
		{
			if(argType.find("out") == 0)  // skip output by reference args
				continue;
			size_t foundArgTypePos = argTypePos;
			while(1)
			{
				foundArgTypePos = fileContents.find(argType, argTypePos);
				if(foundArgTypePos == std::string::npos || foundArgTypePos > argEndPos)
				{
					__COUTT__ << "Could not find input arg type '" << argType
					          << "' in function definition, continuing search..."
					          << std::endl;
					mismatch = true;
					break;  // then found function name is not the correct function
					        // definition
				}
				__COUTTV__(argType);
				size_t charAfterPos = foundArgTypePos + argType.size();
				// make sure it is clean argType match (i.e. not part of another word)
				//	must be , or whitespace or end before and after
				if(foundArgTypePos != nextParenthesis)
				{
					char charBefore = fileContents[foundArgTypePos - 1];
					if(charBefore != ',' && charBefore != ' ' && charBefore != '\t' &&
					   charBefore != '\n' && charBefore != '(' && charBefore != ':')
					{
						__COUTT__ << "Found input arg type '" << argType
						          << "' as part of another word, continuing search..."
						          << fileContents.substr(foundArgTypePos - 1, 20)
						          << std::endl;
						argTypePos = charAfterPos;
						continue;  // then keep looking for the argType
					}
				}

				if(charAfterPos < fileContents.size())
				{
					char charAfter = fileContents[charAfterPos];
					if(charAfter != ',' && charAfter != ' ' && charAfter != '\t' &&
					   charAfter != '\n' && charAfter != ')')
					{
						__COUTT__ << "Found input arg type '" << argType
						          << "' as part of another word, continuing search..."
						          << fileContents.substr(foundArgTypePos - 1, 20)
						          << std::endl;
						argTypePos = charAfterPos;
						continue;  // then keep looking for the argType
					}
				}
				break;  // argType found cleanly
			}           // end argType find loop
			argTypePos = fileContents.find(',', foundArgTypePos + argType.size());
			__COUTTV__(argTypePos);
			if(argTypePos == std::string::npos || argTypePos > argEndPos)
				argTypePos = argEndPos;  // move to end of arg list if no more
			else
				__COUTTV__(fileContents.substr(argTypePos, 20));
			maxArgTypePos = argTypePos;
			__COUTTV__(maxArgTypePos);
			__COUTTV__(fileContents.substr(maxArgTypePos, 20));
		}  // end input arg types confirmation loop
		if(mismatch)
			continue;

		argTypePos   = nextParenthesis;
		mismatch     = false;
		bool isFirst = true;
		for(const auto& argType : outputTypes)  // output arg types confirmation loop
		{
			if(isFirst)
			{
				isFirst = false;
				continue;  // skip return value
			}
			size_t foundArgTypePos = argTypePos;
			while(1)
			{
				foundArgTypePos = fileContents.find(argType, argTypePos);
				if(foundArgTypePos == std::string::npos || foundArgTypePos > argEndPos)
				{
					__COUTT__ << "Could not find output arg type '" << argType
					          << "' in function definition, continuing search..."
					          << std::endl;
					mismatch = true;
					break;  // then found function name is not the correct function
					        // definition
				}
				__COUTTV__(argType);

				size_t charAfterPos = foundArgTypePos + argType.size();
				// make sure it is clean argType match (i.e. not part of another word)
				//	must be , or whitespace or end before and after
				if(foundArgTypePos != nextParenthesis)
				{
					char charBefore = fileContents[foundArgTypePos - 1];
					if(charBefore != ',' && charBefore != ' ' && charBefore != '\t' &&
					   charBefore != '\n' && charBefore != '(')
					{
						__COUTT__ << "Found input arg type '" << argType
						          << "' as part of another word, continuing search..."
						          << fileContents.substr(foundArgTypePos - 1, 20)
						          << std::endl;
						argTypePos = charAfterPos;
						continue;  // then keep looking for the argType
					}
				}
				if(charAfterPos < fileContents.size())
				{
					char charAfter = fileContents[charAfterPos];
					if(charAfter != ',' && charAfter != ' ' && charAfter != '\t' &&
					   charAfter != '\n' && charAfter != ')')
					{
						__COUTT__ << "Found input arg type '" << argType
						          << "' as part of another word, continuing search..."
						          << fileContents.substr(foundArgTypePos - 1, 20)
						          << std::endl;
						argTypePos = charAfterPos;
						continue;  // then keep looking for the argType
					}
				}
				break;  // argType found cleanly
			}           // end argType find loop
			argTypePos = fileContents.find(',', foundArgTypePos + argType.size());
			__COUTTV__(argTypePos);
			if(argTypePos == std::string::npos || argTypePos > argEndPos)
				argTypePos = argEndPos;  // move to end of arg list if no more
			else
				__COUTTV__(fileContents.substr(argTypePos, 20));
			if(argTypePos > maxArgTypePos)
			{
				maxArgTypePos = argTypePos;
				__COUTTV__(maxArgTypePos);
				__COUTTV__(fileContents.substr(maxArgTypePos, 20));
			}
		}  // end output arg types confirmation loop
		if(mismatch)
			continue;

		__COUTTV__(maxArgTypePos);
		__COUTTV__(fileContents.substr(maxArgTypePos, 20));

		// confirm no more parameters exist that would indicate overload function mismatch
		size_t nextComma = fileContents.find(',', maxArgTypePos);
		if(nextComma != std::string::npos && nextComma < argEndPos)
		{
			__COUTT__ << "Found extra arg type after expected types, continuing search..."
			          << std::endl;
			continue;  // then found function name is not the correct function definition
		}

		// else found function name is valid, so break out of search loop
		break;
	}  // end search for function name loop

	// find semicolon after function name
	size_t semicolonPos = funcNamePos;
	while(1)
	{
		semicolonPos = fileContents.find(';', semicolonPos + 1);
		if(semicolonPos == std::string::npos)
		{
			__SS__ << "ERROR: could not find ';' after function name in source file: "
			       << beforeParen << " in file " << sourceFile << __E__;
			__SS_THROW__;
		}

		// ignore semicolons in comments
		size_t prevNewLine = fileContents.rfind("\n", semicolonPos);
		size_t prevComment = fileContents.rfind("//", semicolonPos);
		if(prevComment == std::string::npos ||
		   (prevNewLine != std::string::npos && prevComment < prevNewLine))
			break;  // then ; is not in comment
			        // else is in comment, so continue searching
	}               // end search for semicolon loop

	// extract declared function header
	std::string declaredFunctionHeader =
	    fileContents.substr(funcNamePos, semicolonPos - funcNamePos);
	__COUTTV__(declaredFunctionHeader);
	return declaredFunctionHeader;
}  // end getDeclaredFunctionHeader()

//==============================================================================
/// @brief Imports the tracker test stand UI files and modifies them for the FEInterface
/// @param argc
/// @param argv
void ImportTrackerTestStand(int argc, char* argv[])
{
	std::cout << "=================================================\n";
	std::cout << "=======     ImportTrackerTestStand    ===========\n";
	std::cout << "=================================================\n";
	__COUT__ << "\nImporting Tracker Test Stand!" << std::endl;

	__COUT_INFO__
	    << "\n\nusage: two argumenst:\n\t <import Ui path> <export FEInterface path> \n\n"
	    << std::endl;

	__COUT_INFO__ << "argc = " << argc << std::endl;
	for(int i = 0; i < argc; i++)
		__COUT_INFO__ << "argv[" << i << "] = " << argv[i] << std::endl;

	if(argc != 3)
	{
		__COUT_ERR__ << "Error! Must provide two parameters.\n\n" << std::endl;
		return;
	}

	std::string importPath = argv[1];
	__COUTV__(importPath);
	std::string exportPath = argv[2];
	__COUTV__(exportPath);

	DIR* dpi;
	if((dpi = opendir(importPath.c_str())) == 0)
	{
		__COUT_ERR__ << "ERROR:(" << errno
		             << ").  Can't open Ui directory for import: " << importPath
		             << std::endl;
		exit(0);
	}
	closedir(dpi);

	DIR* dpo;
	if((dpo = opendir(exportPath.c_str())) == 0)
	{
		__COUT_ERR__ << "ERROR:(" << errno
		             << ").  Can't open FEInterface directory for export: " << exportPath
		             << std::endl;
		exit(0);
	}
	closedir(dpo);

	// return;

	std::vector<std::pair<std::string /* file */, std::string /* error message */>>
	    importErrors;

	// usually at otsdaq-mu2e-tracker/otsdaq-mu2e-tracker/Ui/DtcInterface.cc
	std::vector<std::string> filesToImport       = {"DtcInterfaceBase.cc",
	                                                "DtcInterface.cc",
	                                                "DtcInterface_print.cc",
	                                                "DtcInterface_ProgramRoc.cc",
	                                                "DtcInterface_ControlRoc.cc"};
	std::vector<std::string> headerFilesToImport = {"DtcInterfaceBase.hh",
	                                                "DtcInterface.hh",
	                                                "DtcInterface.hh",
	                                                "DtcInterface.hh",
	                                                "DtcInterface.hh"};
	std::vector<std::string> prepend             = {
        "Ui_base", "Ui", "Ui_print", "Ui_ProgramRoc", "Ui_ControlRoc"};
	for(size_t i = 0; i < filesToImport.size(); i++)
	{
		std::string importFile = importPath + "/" + filesToImport[i];
		__COUTV__(importFile);
		__COUTV__(prepend[i]);
		try
		{
			std::ifstream in(importFile);
			if(!in)
			{
				__SS__ << "\n\nERROR! Could not open file at " << importFile
				       << ". Error: " << errno << " - " << strerror(errno) << __E__;
				__SS_THROW__;
			}
			//modify import file to be consistent by path location (inserted in comments and printouts, which can cause git diffs), starting from otsdaq-mu2e-tracker/
			size_t tpos = importFile.rfind("otsdaq-mu2e-tracker"); //find last instance
			if(tpos == std::string::npos)
				importFile = "otsdaq-mu2e-tracker/" + importFile;
			else
				importFile = importFile.substr(tpos);

			std::string fileContents((std::istreambuf_iterator<char>(in)),
			                         std::istreambuf_iterator<char>());
			// fileContents now contains the full contents of importFile

			std::string outputFilePath =
			    exportPath + "/ROCTrackerInterface_" + prepend[i] + ".cc";
			std::ofstream outputFile(outputFilePath);
			if(!outputFile.is_open())
			{
				__SS__ << "\n\nERROR! Could not open output file at " << outputFilePath
				       << __E__;
				__SS_THROW__;
			}
			std::string outputHeaderPath =
			    exportPath + "/ROCTrackerInterface_" + prepend[i] + ".hxx";
			std::ofstream outputHeaderFile(outputHeaderPath);
			if(!outputHeaderFile.is_open())
			{
				__SS__ << "\n\nERROR! Could not open header output file at "
				       << outputHeaderPath << __E__;
				__SS_THROW__;
			}
			std::string outputFeMacroDeclarePath = exportPath + "/ROCTrackerInterface_" +
			                                       prepend[i] + "_declareFEMacros.hxx";
			std::ofstream outputFeMacroDeclareFile(outputFeMacroDeclarePath);
			if(!outputFeMacroDeclareFile.is_open())
			{
				__SS__ << "\n\nERROR! Could not open FE Macro declaration output file at "
				       << outputFeMacroDeclarePath << __E__;
				__SS_THROW__;
			}
			std::string outputFeMacroPath = exportPath + "/ROCTrackerInterface_" +
			                                prepend[i] + "_registerFEMacros.icc";
			std::ofstream outputFeMacroRegisterFile(outputFeMacroPath);
			if(!outputFeMacroRegisterFile.is_open())
			{
				__SS__
				    << "\n\nERROR! Could not open FE Macro registration output file at "
				    << outputFeMacroPath << __E__;
				__SS_THROW__;
			}
			std::string outputFeMacroDefinePath =
			    exportPath + "/ROCTrackerInterface_" + prepend[i] + "_defineFEMacros.cc";
			std::ofstream outputFeMacroDefineFile(outputFeMacroDefinePath);
			if(!outputFeMacroDefineFile.is_open())
			{
				__SS__
				    << "\n\nERROR! Could not open FE Macro registration output file at "
				    << outputFeMacroDefinePath << __E__;
				__SS_THROW__;
			}

			std::stringstream headerInstructionsSs, headerIncludes;

			//////////////// source file header
			outputFile << "\n// clang-format off\n";
			outputFile << "\n//"
			              "--------------------------------------------------------------"
			              "---------------\n";
			headerInstructionsSs << "/// This file was auto-generated from " << importFile
			                     << "\n";
			headerInstructionsSs << "/// Do not modify this file directly.\n";
			headerInstructionsSs << "///\n";
			headerInstructionsSs << "/// To modify, edit " << importFile
			                     << " and re-run the import tool:\n";
			headerInstructionsSs << "///\n";
			headerInstructionsSs << "///   otsdaq_import_tracker_test_stand   "
			                     << importPath << "   " << exportPath << "\n";
			headerInstructionsSs << "///\n";
			outputFile << headerInstructionsSs.str();
			outputFile << "//"
			              "--------------------------------------------------------------"
			              "---------------\n";

			headerIncludes
			    << "\n\n#include "
			       "\"otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h\"\n\n";
			headerIncludes << "\n#include <TString.h>  // includes ROOT's Form\n";
			headerIncludes << "\n#include <filesystem>\n";

			headerIncludes << "\n\nusing namespace ots;\n";
			headerIncludes << "\n#undef __MF_SUBJECT__\n";
			headerIncludes << "#define __MF_SUBJECT__ \"FE-ROCTrackerInterface\"\n";

			headerIncludes << "\n\n";
			outputFile << headerIncludes.str();

			//////////////// header file header
			outputHeaderFile << "\n// clang-format off\n";
			outputHeaderFile << "\n//"
			                    "--------------------------------------------------------"
			                    "----------------\n";
			outputHeaderFile << headerInstructionsSs.str();
			outputHeaderFile << "//"
			                    "--------------------------------------------------------"
			                    "----------------\n";
			outputHeaderFile << "\n\n";

			//////////////// FE macro declare file header
			outputFeMacroDeclareFile << "\n// clang-format off\n";
			outputFeMacroDeclareFile << "\n//"
			                            "------------------------------------------------"
			                            "------------------------\n";
			outputFeMacroDeclareFile << headerInstructionsSs.str();
			outputFeMacroDeclareFile << "//"
			                            "------------------------------------------------"
			                            "------------------------\n";
			outputFeMacroDeclareFile << "\n\n";

			//////////////// FE macro register source file header
			outputFeMacroRegisterFile << "\n// clang-format off\n";
			outputFeMacroRegisterFile << "\n//"
			                             "-----------------------------------------------"
			                             "-------------------------\n";
			outputFeMacroRegisterFile << headerInstructionsSs.str();
			outputFeMacroRegisterFile << "//"
			                             "-----------------------------------------------"
			                             "-------------------------\n";
			outputFeMacroRegisterFile << "\n\n";

			//////////////// FE macro deinition source file header
			outputFeMacroDefineFile << "\n// clang-format off\n";
			outputFeMacroDefineFile << "\n//"
			                           "-------------------------------------------------"
			                           "-----------------------\n";
			outputFeMacroDefineFile << headerInstructionsSs.str();
			outputFeMacroDefineFile << "//"
			                           "-------------------------------------------------"
			                           "-----------------------\n";
			outputFeMacroDefineFile << headerIncludes.str();

			const std::string functionNeedle = " DtcInterface::";
			//-----------------------------------------------------------------------------
			// search for each function definition
			size_t pos = 0;
			while(true)
			{
				// find the next function definition
				size_t funcPos = fileContents.find(" DtcInterface::", pos);
				pos            = funcPos +
				      functionNeedle.size();  // move past this point for next search
				if(funcPos == std::string::npos)
				{
					__COUTT__ << "No more functions found at " << funcPos << std::endl;
					break;  // no more functions found
				}

				__COUT__ << "===========> Considering next DtcInterface:: pos = "
				         << (fileContents.substr(funcPos, 50)) << __E__;

				size_t endFuncPos = fileContents.find(")", funcPos);
				if(endFuncPos == std::string::npos)
				{
					__COUTT__ << "Malformed function found at " << funcPos << std::endl;
					continue;  // malformed function definition, so skip
				}

				__COUTV__(fileContents.substr(endFuncPos, 50));

				// find the opening brace of the function body
				size_t bracePos = fileContents.find("{", endFuncPos);
				if(bracePos == std::string::npos)
				{
					__COUTT__ << "Malformed function found at " << funcPos << std::endl;
					continue;  // malformed function definition, so skip
				}

				__COUTV__(fileContents.substr(bracePos, 50));

				std::string functionHeader =
				    fileContents.substr(funcPos + functionNeedle.size(),
				                        bracePos - (funcPos + functionNeedle.size()));

				// enforce that there are no special characters in function header between
				// funcPos and bracePos
				if(functionHeader.find("{") != std::string::npos ||
				   functionHeader.find("}") != std::string::npos ||
				   functionHeader.find("[") != std::string::npos ||
				   functionHeader.find("]") != std::string::npos ||
				   functionHeader.find(";") != std::string::npos)
				{
					__COUTT__ << "Malformed function found at " << funcPos << std::endl;
					continue;  // malformed function definition, so skip
				}
				__COUTV__(functionHeader);

				// find the matching closing brace
				int    braceCount = 1;
				size_t endPos     = bracePos + 1;
				bool   inComment  = false;
				bool   inString   = false;
				while(endPos < fileContents.size() && braceCount > 0)
				{
					if(fileContents[endPos] == '{' && !inComment && !inString)
					{
						braceCount++;
						__COUTT__ << "braceCount=" << braceCount
						          << fileContents.substr(endPos - 49, 50);
					}
					else if(fileContents[endPos] == '}' && !inComment && !inString)
					{
						braceCount--;
						__COUTT__ << "braceCount=" << braceCount
						          << fileContents.substr(endPos - 49, 50);
					}
					else if(fileContents[endPos] == '/' &&
					        fileContents[endPos + 1] == '/')
					{
						inComment = true;
						__COUTT__ << "inComment start"
						          << fileContents.substr(endPos - 49, 50);
					}
					else if(fileContents[endPos] == '\n' && inComment)
					{
						inComment = false;
						__COUTT__ << "inComment end"
						          << fileContents.substr(endPos - 49, 50);
					}
					else if(fileContents[endPos] == '"' && !inComment)
					{
						inString = !inString;  // toggle string state
						__COUTT__ << "inString toggle to " << inString
						          << fileContents.substr(endPos - 49, 50);
					}
					endPos++;
				}
				__COUTV__(fileContents.substr(endPos - 50, 50));
				if(braceCount != 0)
				{
					__COUTT__ << "Malformed function found at " << funcPos << " to "
					          << endPos << std::endl;
					continue;  // malformed function definition, so skip
				}

				// extract the function definition
				std::string functionDef =
				    fileContents.substr(bracePos, endPos - bracePos);

				// update position for next search
				pos = endPos;

				// eliminate special functions that should not be imported
				if(functionDef.find("fgInstance") != std::string::npos)
				{
					__COUTT__ << "Eliminated function def found at " << funcPos << " to "
					          << endPos << std::endl;
					continue;  // malformed function definition, so skip
				}

				// prepend the specified string
				__COUT__ << "Found: " + prepend[i] + "_" + functionHeader;

				size_t startFunc = fileContents.rfind('\n', funcPos);
				if(startFunc == std::string::npos)
				{
					__COUTT__ << "Malformed function found at startFunc=" << startFunc
					          << " from " << funcPos << std::endl;
					continue;  // malformed function definition, so skip
				}
				std::string functionReturnVal =
				    fileContents.substr(startFunc + 1, funcPos - (startFunc + 1));
				__COUTTV__(functionReturnVal);
				functionReturnVal = StringMacros::trim(functionReturnVal);
				__COUTV__(functionReturnVal);
				if(functionReturnVal.size() == 0)
				{
					__COUTT__ << "Ignoring constructors/destructors functions."
					          << std::endl;
					continue;  // malformed function definition, so skip
				}
				__COUTTV__(startFunc);

				// accept as comment lines with "// "
				--startFunc;  // move before newline of function definition
				std::string functionComment = "";
				while(true)
				{
					size_t startComment1 = fileContents.rfind("//", startFunc);
					size_t startComment2 = fileContents.rfind("\n", startFunc);
					__COUTTV__(startComment1);
					__COUTTV__(startComment2);

					if(startComment1 == std::string::npos ||
					   startComment1 < startComment2)
					{
						__COUTT__ << "No more comments found at " << startComment1 << " "
						          << fileContents.substr(startFunc - 48, 50) << std::endl;
						break;  // no more comments found
					}
					__COUTT__ << "Comment found at " << startComment1 << " "
					          << fileContents.substr(startFunc - 48, 50) << std::endl;

					if(fileContents[startComment1 + 2] == ' ')  // is a comment line
					{
						//remove excess white space (from double comment)
						while(fileContents[startFunc] == ' ') --startFunc;

						functionComment =
						    "/" +
						    fileContents.substr(startComment1,
						                        startFunc - startComment1 + 1) +
						    "\n" + functionComment;
					}
					// else //is a comment line to ignore

					startFunc = startComment1 - 2;  // move before this comment line
				}                                   // end comment extraction loop

				__COUTTV__(functionComment);
				__COUTTV__(functionDef);

				// output the function prototype to header file
				std::string declaredFunctionHeader = getDeclaredFunctionHeader(
				    functionHeader, importPath + "/" + headerFilesToImport[i]);
				outputHeaderFile
				    << modifySource(functionReturnVal) << " " << prepend[i] << "_"
				    << modifySource(declaredFunctionHeader, true /* isHeader */) << ";\n";

				// output the extracted function to source file
				outputFile << "//"
				              "=========================================================="
				              "====================\n";
				modifySource(functionHeader, true /* isHeader */);
				outputFile << "///\t" << prepend[i] << "_"
				           << functionHeader.substr(0, functionHeader.find('('))
				           << "()\n";
				outputFile << functionComment;
				outputFile << headerInstructionsSs.str();
				outputFile << functionReturnVal << " ROCTrackerInterface"
				           << "::" << prepend[i] << "_" <<
						   functionHeader.substr(0,functionHeader.size()-1 /* removing trailing white space */) << "\n";
				outputFile << modifySource(functionDef);
				outputFile << " // end " << prepend[i] << "_"
				           << functionHeader.substr(0, functionHeader.find('('))
				           << "()\n\n";

				// if args are simple (int, string), then output the FE Macro declaration,
				// registration, and definition
				std::vector<std::string> argTypes, argNames, argDefaults, outputTypes,
				    outputNames;
				__COUT__ << "Validating args for FE Macro... " << declaredFunctionHeader
				         << std::endl;
				if(validateArgsForFEMacro(declaredFunctionHeader,
				                          functionReturnVal,
				                          argTypes,
				                          argNames,
				                          argDefaults,
				                          outputTypes,
				                          outputNames))
				{
					__COUT__ << "Valid FE Macro! " << prepend[i] << "_"
					         << functionHeader.substr(0, functionHeader.find('('))
					         << std::endl;
					// --------------------------------------------
					// FE Macro declaration ----------------
					outputFeMacroDeclareFile
					    << "void FEMacro_" << prepend[i] << "_"
					    << functionHeader.substr(0, functionHeader.find('('))
					    << "(__ARGS__);\n";

					// --------------------------------------------
					// FE Macro registration ----------------
					outputFeMacroRegisterFile << "\n";
					outputFeMacroRegisterFile
					    << "    registerFEMacroFunction(\"" << prepend[i] << " "
					    << functionHeader.substr(0, functionHeader.find('(')) << "\",\n";
					outputFeMacroRegisterFile
					    << "                            "
					       "static_cast<FEVInterface::frontEndMacroFunction_t>(\n";
					outputFeMacroRegisterFile
					    << "                                "
					       "&ROCTrackerInterface::FEMacro_"
					    << prepend[i] << "_"
					    << functionHeader.substr(0, functionHeader.find('(')) << "),\n";
					outputFeMacroRegisterFile
					    << "                            std::vector<std::string>{";
					for(size_t o = 0, ocount = 0; o < argNames.size();
					    o++)  // input args loop
					{
						if((argTypes[o].size() == 3 || argTypes[o].size() == 4) &&
						   argTypes[o].find("out") == 0)
							continue;  // skip output by reference/pointer args

						if(argNames[o] == "Link" || argNames[o] == "LinkMask")
							continue;  // skip link args (take from ROC member variable)

						if(ocount > 0)
							outputFeMacroRegisterFile << ", ";
						outputFeMacroRegisterFile << "\"" << argNames[o];

						if(argDefaults[o].size())
							outputFeMacroRegisterFile << " (Default = " << argDefaults[o]
							                          << ")";
						outputFeMacroRegisterFile << "\"";
						++ocount;
					}  // end input args loop
					outputFeMacroRegisterFile << "},          // inputs parameters\n";
					outputFeMacroRegisterFile
					    << "                            std::vector<std::string>{";
					for(size_t o = 0, ocount = 0; o < outputNames.size();
					    o++)  // output args loop
					{
						if(outputTypes[o] == "void")
							continue;  // no output to declare

						if(ocount > 0)
							outputFeMacroRegisterFile << ", ";
						outputFeMacroRegisterFile << "\"" << outputNames[o] << "\"";
						++ocount;
					}  // end output args loop
					outputFeMacroRegisterFile << "},  // output parameters\n";
					outputFeMacroRegisterFile << "                            1);  // "
					                             "requiredUserPermissions\n";

					// --------------------------------------------
					// FE Macro definition ----------------
					outputFeMacroDefineFile << "\n";
					outputFeMacroDefineFile << "//"
					                           "========================================="
					                           "=====================================\n";
					outputFeMacroDefineFile
					    << "/// FE Macro to call " << prepend[i] << "_"
					    << functionHeader.substr(0, functionHeader.find('(')) << "()\n";
					outputFeMacroDefineFile << "///\n";
					outputFeMacroDefineFile << headerInstructionsSs.str();
					outputFeMacroDefineFile
					    << "void ROCTrackerInterface::FEMacro_" << prepend[i] << "_"
					    << functionHeader.substr(0, functionHeader.find('('))
					    << "(__ARGS__)";
					outputFeMacroDefineFile <<
					    R"(
{
	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
	for(auto& argIn : argsIn)
		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

)";

					for(size_t o = 0; o < argNames.size(); o++)  // declare input args
					{
						if((argTypes[o].size() == 3 || argTypes[o].size() == 4) &&
						   argTypes[o].find("out") == 0)
							continue;  // skip output by reference/pointer args

						if(argTypes[o].find('&') !=
						   std::string::npos)  // remove the & from output by reference
						                       // args
							outputFeMacroDefineFile
							    << "\t" << argTypes[o].substr(0, argTypes[o].size() - 1)
							    << " " << argNames[o] << " = ";
						else
							outputFeMacroDefineFile << "\t" << argTypes[o] << " "
							                        << argNames[o] << " = ";

						if(argNames[o] ==
						   "Link")  // for link args, take from ROC member variable
						{
							outputFeMacroDefineFile
							    << "ROCCoreVInterface::getLinkID();\n";
						}
						else if(argNames[o] == "LinkMask")  // for link mask args, take
						                                    // from ROC member variable
						{
							// nibble based link mask
							outputFeMacroDefineFile
							    << "1 << (ROCCoreVInterface::getLinkID() * 4);\n";
						}
						else  // normal input arg
						{
							outputFeMacroDefineFile << "__GET_ARG_IN__"
							                        << "(\"" << argNames[o];

							if(argDefaults[o].size())
								outputFeMacroDefineFile
								    << " (Default = " << argDefaults[o] << ")";

							if(argTypes[o].find('&') !=
							   std::string::npos)  // remove the & from output by
							                       // reference args
								outputFeMacroDefineFile
								    << "\""
								    << ", "
								    << argTypes[o].substr(0, argTypes[o].size() - 1);
							else
								outputFeMacroDefineFile << "\""
								                        << ", " << argTypes[o];

							if(argDefaults[o].size())
								outputFeMacroDefineFile << ", " << argDefaults[o];

							outputFeMacroDefineFile << ");\n";
						}
					}  // end declare input args loop

					outputFeMacroDefineFile << "\n";
					for(size_t o = 0; o < outputNames.size(); o++)  // declare output args
					{
						if(outputTypes[o] == "void")
							continue;  // no output to declare

						if(outputTypes[o].find('&') !=
						   std::string::npos)  // remove the & from output by reference
						                       // args
						{
							if(outputTypes[o] ==
							   "std::ostream&")  // special case for ostream
								outputFeMacroDefineFile << "\t"
								                        << "std::stringstream"
								                        << " " << outputNames[o] << ";\n";
							else
								outputFeMacroDefineFile
								    << "\t"
								    << outputTypes[o].substr(0, outputTypes[o].size() - 1)
								    << " " << outputNames[o] << ";\n";
						}
						else if(outputTypes[o].find('*') !=
						        std::string::npos)  // remove the * from output by
						                            // reference args
							outputFeMacroDefineFile
							    << "\t"
							    << outputTypes[o].substr(0, outputTypes[o].size() - 1)
							    << " " << outputNames[o] << ";\n";
						else
							outputFeMacroDefineFile << "\t" << outputTypes[o] << " "
							                        << outputNames[o] << ";\n";
					}  // end declare output args loop

					outputFeMacroDefineFile << "\n";
					if(outputNames.size() && functionReturnVal != "void")
						outputFeMacroDefineFile << "\t" << outputNames[0] << " =\n\t";
					outputFeMacroDefineFile << "\t" << "trackerDTC_->"
					    // << "\t" << prepend[i] << "_"
					    << functionHeader.substr(0, functionHeader.find('(')) << "(";

					for(size_t o = 0; o < argNames.size(); o++)
					{
						if(o > 0)
							outputFeMacroDefineFile << ", ";
						if(argTypes[o] ==
						   "out*")  // needed to pass as pointer for output result
							outputFeMacroDefineFile << "&";
						outputFeMacroDefineFile << argNames[o];
					}
					outputFeMacroDefineFile << ");\n\n";

					for(size_t o = 0; o < outputNames.size(); o++)  // set output args
					{
						if(outputTypes[o] == "void")
							continue;  // no output to set

						// handle special output types
						if(outputTypes[o].find("std::vector") != std::string::npos)
							outputFeMacroDefineFile
							    << "\t__SET_ARG_OUT__(\"" << outputNames[o]
							    << "\", StringMacros::vectorToString(" << outputNames[o]
							    << "));\n";
						else if(outputTypes[o] ==
						        "std::ostream&")  // special case for ostream
						{
							outputFeMacroDefineFile << "\t__COUT_MULTI__(2, "
							                        << outputNames[o] << ".str());\n";
							outputFeMacroDefineFile << "\t__SET_ARG_OUT__(\""
							                        << outputNames[o] << "\", "
							                        << outputNames[o] << ".str());\n";
						}
						else  // normal output type
							outputFeMacroDefineFile << "\t__SET_ARG_OUT__(\""
							                        << outputNames[o] << "\", "
							                        << outputNames[o] << ");\n";
					}  // end set output args loop
					outputFeMacroDefineFile << "\n";

					outputFeMacroDefineFile
					    << "} // end FEMacro_" << prepend[i] << "_"
					    << functionHeader.substr(0, functionHeader.find('(')) << "()\n";
				}  // end outputing FE Macro definition, registration, declaration

				// flush output files so progress can be debugged if needed
				outputFile.flush();
				outputHeaderFile.flush();
				outputFeMacroDeclareFile.flush();
				outputFeMacroRegisterFile.flush();
				outputFeMacroDefineFile.flush();

				// break; //for debugging, only do one function per file
			}  // end main function search loop

			outputFile << "// clang-format on\n";
			outputHeaderFile << "// clang-format on\n";
			outputFeMacroDeclareFile << "// clang-format on\n";
			outputFeMacroRegisterFile << "// clang-format on\n";
			outputFeMacroDefineFile << "// clang-format on\n";

			outputFile.close();
			outputHeaderFile.close();
			outputFeMacroDeclareFile.close();
			outputFeMacroRegisterFile.close();
			outputFeMacroDefineFile.close();

			__COUT_INFO__ << "Successfully wrote output file at " << outputFilePath
			              << std::endl;
			__COUT_INFO__ << "Successfully wrote header file at " << outputHeaderPath
			              << std::endl;
			__COUT_INFO__ << "Successfully wrote FE Macro declaration file at "
			              << outputFeMacroDeclarePath << std::endl;
			__COUT_INFO__ << "Successfully wrote FE Macro registration file at "
			              << outputFeMacroPath << std::endl;
			__COUT_INFO__ << "Successfully wrote FE Macro define file at "
			              << outputFeMacroDefinePath << std::endl;
		}
		catch(const std::runtime_error& e)
		{
			importErrors.push_back(
			    std::make_pair(importFile,
			                   prepend[i] + " " + importFile +
			                       std::string(" std::runtime_error: ") + e.what()));
		}
		catch(const std::exception& e)
		{
			importErrors.push_back(std::make_pair(importFile,
			                                      prepend[i] + " " + importFile +
			                                          std::string(" std::exception: ") +
			                                          e.what()));
		}
		catch(...)
		{
			importErrors.push_back(std::make_pair(
			    importFile,
			    prepend[i] + " " + importFile + " Unknown exception occurred."));
		}
	}  // end main import loop

	if(importErrors.size())
	{
		__COUT_ERR__ << "There were " << importErrors.size()
		             << " errors found while importing. See below:\n"
		             << std::endl;
		for(auto& importErr : importErrors)
			__COUT_ERR__ << "\t" << importErr.first << ": " << importErr.second
			             << std::endl;
		__COUT_ERR__ << "End of errors.\n\n" << std::endl;
	}
	else
		__COUT_INFO__ << "There were NO ERRORS found while loading and exporting groups."
		              << __E__;
	std::cout << "Import of Tracker Test Stand complete!\n";

}  // end ImportTrackerTestStand()

//==============================================================================
int main(int argc, char* argv[])
{
	//==============================================================================
	// Define environment variables
	//	Note: normally these environment variables are set by ots script

	test::util::check_and_make_envs();

	////////////////////////////////////////////////////

	INIT_MF("ImportTrackerTestStand");
	ImportTrackerTestStand(argc, argv);
	return 0;
}  // end main()
// BOOST_AUTO_TEST_SUITE_END()
