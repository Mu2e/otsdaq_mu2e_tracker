#include "otsdaq/MessageFacility/MessageFacility.h"

#include <dirent.h>
#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <fstream>

#include "otsdaq/Macros/StringMacros.h"

/// Extracts the function definitions in the tracker Ui folder into
///		the otsdaq-mu2e-tracker/otsdaq-mu2e-tracker/FEInterfaces/ folder (or any folder)
///
///	Warning! it will overwrite files in the output folder with the same names!
///
/// usage:
/// otsdaq_import_tracker_test_stand <import Ui path> <export FEInterface path>
///
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
		{"getDTC()ID", "fDtcID"}, //fix accidental replacement of member variable
		
		{"TrkSpiData_t", "trkdaq::TrkSpiData_t"},
		{"roc_data_t", "DTCLib::roc_data_t"},
		{"DTC_Link_ID", "DTCLib::DTC_Link_ID"},
		{"DTCLib::DTCLib::DTC_Link_ID", "DTCLib::DTC_Link_ID"},
		{"DTC_SubEvent", "DTCLib::DTC_SubEvent"},
		{"DTCLib::DTCLib::DTC_SubEvent", "DTCLib::DTC_SubEvent"},
		{"DTC_EventWindowTag", "DTCLib::DTC_EventWindowTag"},
		{"DTCLib::DTCLib::DTC_EventWindowTag", "DTCLib::DTC_EventWindowTag"},
		{"roc_address_t", "DTCLib::roc_address_t"},
		{"DTCLib::DTCLib::roc_address_t", "DTCLib::roc_address_t"},
		{"DTC_LinkEnableMode", "DTCLib::DTC_LinkEnableMode"},
		{"DTCLib::DTCLib::DTC_LinkEnableMode", "DTCLib::DTC_LinkEnableMode"},
		{"DTC_Link_ALL", "DTCLib::DTC_Link_ALL"},
		{"DTCLib::DTCLib::DTC_Link_ALL", "DTCLib::DTC_Link_ALL"},
		{"EventMode()", "((((int64_t)fOnSpill) << 32) | ((int64_t)fEventMode))"},
		{"DTC_ROC_Links", "DTCLib::DTC_ROC_Links"},
		{"DTCLib::DTCLib::DTC_ROC_Links", "DTCLib::DTC_ROC_Links"},
		{"TrkSpiDataNWords", "trkdaq::TrkSpiDataNWords"},
		{"PrintBuffer", "DTCLib::Utilities::PrintBuffer"},

		{"roc_serial_t", "std::string"},

		{"vector", "std::vector"},
		{"string", "std::string"},
		{"to_std::string", "std::to_string"}, //fix accidental replacement
		{"ifstream", "std::ifstream"},
		{"ofstream", "std::ofstream"},
		{"fstream", "std::fstream"},
		{"cout", "std::cout"},
		{"endl", "std::endl"},
		{"stringstream", "std::stringstream"},
		{"hex", "std::hex"},
		{"dec", "std::dec"},

		{"trkdaq::trkdaq::", "trkdaq::"},
		{"std::std::", "std::"},
	};
	std::vector<std::pair<std::string, std::string>> functionReplacements = {
		{"ConfigureJA", "Ui_ConfigureJA"},
		{"InitEmulatedCFOReadoutMode", "Ui_InitEmulatedCFOReadoutMode"},
		{"InitExternalCFOReadoutMode", "Ui_InitExternalCFOReadoutMode"},
		{"InitReadout", "Ui_InitReadout"},
		{"InitRocReadoutMode", "Ui_InitRocReadoutMode"},
		{"LaunchRunPlanEmulatedCfo", "Ui_LaunchRunPlanEmulatedCfo"},
		{"ConvertSpiData", "Ui_ConvertSpiData"},
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
		{"SetLinkMask", "Ui_SetLinkMask"},
		{"SetupCfoInterface", "Ui_SetupCfoInterface"},
		{"ValidateDigiPatterns", "Ui_ValidateDigiPatterns"},
		{"ValidateFixedPatterns", "Ui_ValidateFixedPatterns"},
		{"ValidateVarPatterns", "Ui_ValidateVarPatterns"},
		{"MonicaDigiClear", "Ui_MonicaDigiClear"},
		{"MonicaVarLinkConfig", "Ui_MonicaVarLinkConfig"},
		{"MonicaVarPatternConfig", "Ui_MonicaVarPatternConfig"}
	};
	
	for(const auto& [oldStr, newStr] : replacements)
	{
		size_t pos = 0;
		while ((pos = source.find(oldStr, pos)) != std::string::npos)
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
			while ((pos = source.find(oldStr, pos)) != std::string::npos)
			{
				source.replace(pos, oldStr.length(), newStr);
				pos += newStr.length();
			}
		}
	}
	return source;
} //end modifySource()

//==============================================================================
/// @brief Extracts the declared function header from the full function header
/// @param functionHeader the full function header
/// @param sourceFile the source file the function is from
std::string getDeclaredFunctionHeader(const std::string& functionHeader, const std::string& sourceFile)
{		
	//general case
	size_t parenPos = functionHeader.find('(');
	if(parenPos == std::string::npos)
	{
		__SS__ << "ERROR: could not find '(' in function header: " << functionHeader << __E__;
		__SS_THROW__;
	}
	std::string beforeParen = functionHeader.substr(0, parenPos);

	//find in file the function name
	std::ifstream in(sourceFile);
	if (!in)
	{
		__SS__ << "\n\nERROR! Could not open file at " << sourceFile
						<< " - error: " << errno << " - " << strerror(errno) << __E__;
		__SS_THROW__;
	}
	std::string fileContents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
	// fileContents now contains the full contents of sourceFile	
	size_t funcNamePos = fileContents.find(beforeParen);
	if(funcNamePos == std::string::npos)
	{
		__SS__ << "ERROR: could not find function name in source file: " << beforeParen << " in file " << sourceFile << __E__;
		__SS_THROW__;
	}
	//find semicolon after function name
	size_t semicolonPos = funcNamePos;
	while(1)
	{
		semicolonPos = fileContents.find(';', semicolonPos + 1);
		if(semicolonPos == std::string::npos)
		{
			__SS__ << "ERROR: could not find ';' after function name in source file: " << beforeParen << " in file " << sourceFile << __E__;
			__SS_THROW__;
		}
		
		//ignore semicolons in comments
		size_t prevNewLine = fileContents.rfind("\n", semicolonPos);
		size_t prevComment = fileContents.rfind("//", semicolonPos);
		if(prevComment == std::string::npos || (prevNewLine != std::string::npos && prevComment < prevNewLine))			
			break; //then ; is not in comment
		//else is in comment, so continue searching
	} //end search for semicolon loop

	//extract declared function header
	std::string declaredFunctionHeader = fileContents.substr(funcNamePos, semicolonPos - funcNamePos);
	__COUTTV__(declaredFunctionHeader);
	return declaredFunctionHeader;
} //end getDeclaredFunctionHeader()

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

	__COUT_INFO__ << "\n\nusage: two argumenst:\n\t <import Ui path> <export FEInterface path> \n\n" << std::endl;

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

	std::vector<std::pair<std::string /* file */, 
		std::string /* error message */>> importErrors;

	//usually at otsdaq-mu2e-tracker/otsdaq-mu2e-tracker/Ui/DtcInterface.cc
	std::vector<std::string> filesToImport = {
	    "DtcInterface.cc", "DtcInterface_ProgramRoc.cc", "DtcInterface_ControlRoc.cc"};	
	std::vector<std::string> headerFilesToImport = {
	    "DtcInterface.hh", "DtcInterface.hh", "DtcInterface.hh"};	 
	std::vector<std::string> prepend = {
	    "Ui", "Ui_ProgramRoc", "Ui_ControlRoc"};	 
	for(size_t i = 0; i < filesToImport.size(); i++)
	{
		std::string importFile = importPath + "/" + filesToImport[i];
		__COUTV__(importFile);
		__COUTV__(prepend[i]);
		try
		{		
			std::ifstream in(importFile);
			if (!in)
			{
				__SS__ << "\n\nERROR! Could not open file at " << importFile
								<< ". Error: " << errno << " - " << strerror(errno) << __E__;
				__SS_THROW__;
			}
			std::string fileContents((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
			// fileContents now contains the full contents of importFile

			std::string outputFilePath = exportPath + "/ROCTrackerInterface_" + prepend[i] + ".cc";
			std::ofstream outputFile(outputFilePath);
			if (!outputFile.is_open())
			{
			  __SS__ << "\n\nERROR! Could not open output file at " << outputFilePath << __E__;
			  __SS_THROW__;
			}
			std::string outputHeaderPath = exportPath + "/ROCTrackerInterface_" + prepend[i] + ".h";
			std::ofstream outputHeaderFile(outputHeaderPath);
			if (!outputHeaderFile.is_open())
			{
			  __SS__ << "\n\nERROR! Could not open header output file at " << outputHeaderPath << __E__;
			  __SS_THROW__;
			}

			std::stringstream headerInstructionsSs;
			
			//////////////// source file header
			outputFile << "\n//-----------------------------------------------------------------------------\n";
			headerInstructionsSs << "/// This file was auto-generated from " << importFile << " on " << StringMacros::getTimestampString() << "\n";
			headerInstructionsSs << "/// Do not modify this file directly.\n";
			headerInstructionsSs << "///\n";
			headerInstructionsSs << "/// To modify, edit " << importFile << " and re-run the import tool:\n";
			headerInstructionsSs << "///\n";
			headerInstructionsSs << "///   otsdaq_import_tracker_test_stand   " << importPath << "   " << exportPath << "\n";
			headerInstructionsSs << "///\n";
			outputFile << headerInstructionsSs.str();
			outputFile << "//-----------------------------------------------------------------------------\n";
			
			outputFile << "\n\n#include \"otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h\"\n\n";
			outputFile << "\n#include <TString.h>  // includes ROOT's Form\n";

			outputFile << "\n\nusing namespace ots;\n";
			outputFile << "\n#undef __MF_SUBJECT__\n";
			outputFile << "#define __MF_SUBJECT__ \"FE-ROCTrackerInterface\"\n";

			outputFile << "\n\n";

			//////////////// header file header
			outputHeaderFile << "\n//------------------------------------------------------------------------\n";
			outputHeaderFile << headerInstructionsSs.str();
			outputHeaderFile << "//------------------------------------------------------------------------\n";

			outputHeaderFile << "\n\n";

			const std::string functionNeedle = " DtcInterface::";
			//-----------------------------------------------------------------------------
			// search for each function definition
			size_t pos = 0;
			while (true)
			{
				// find the next function definition
				size_t funcPos = fileContents.find(" DtcInterface::", pos);
				pos = funcPos + functionNeedle.size(); // move past this point for next search
				if (funcPos == std::string::npos)
				{
					__COUTT__ << "No more functions found at " << funcPos << std::endl;
					break; // no more functions found
				}

				__COUTV__(fileContents.substr(funcPos, 50));
								
				size_t endFuncPos = fileContents.find(")", funcPos);
				if (endFuncPos == std::string::npos)
				{
					__COUTT__ << "Malformed function found at " << funcPos << std::endl;
					continue; // malformed function definition, so skip			
				}

				__COUTV__(fileContents.substr(endFuncPos, 50));	
				
				// find the opening brace of the function body
				size_t bracePos = fileContents.find("{", endFuncPos);
				if (bracePos == std::string::npos)
				{
					__COUTT__ << "Malformed function found at " << funcPos << std::endl;
					continue; // malformed function definition, so skip			
				}
				
				__COUTV__(fileContents.substr(bracePos, 50));	

				std::string functionHeader = fileContents.substr(funcPos + functionNeedle.size(), bracePos - (funcPos + functionNeedle.size()) - 1 /* remove new line */);

				//enforce that there are no special characters in function header between funcPos and bracePos				
				if (functionHeader.find("{") != std::string::npos || 
					functionHeader.find("}") != std::string::npos || 
					functionHeader.find("[") != std::string::npos || 
					functionHeader.find("]") != std::string::npos || 
					functionHeader.find(";") != std::string::npos )				
				{
					__COUTT__ << "Malformed function found at " << funcPos << std::endl;
					continue; // malformed function definition, so skip			
				}
				__COUTV__(functionHeader);

				// find the matching closing brace
				int braceCount = 1;
				size_t endPos = bracePos + 1;
				bool inComment = false;
				bool inString = false;
				while (endPos < fileContents.size() && braceCount > 0)
				{
					if (fileContents[endPos] == '{' && !inComment && !inString)
					{
						braceCount++;
						__COUTT__ << "braceCount=" << braceCount << fileContents.substr(endPos-49, 50);
					}
					else if (fileContents[endPos] == '}' && !inComment && !inString)
					{
						braceCount--;
						__COUTT__ << "braceCount=" << braceCount << fileContents.substr(endPos-49, 50);
					}
					else if (fileContents[endPos] == '/' && fileContents[endPos+1] == '/')
					{
						inComment = true;
						__COUTT__ << "inComment start" << fileContents.substr(endPos-49, 50);
					}
					else if (fileContents[endPos] == '\n' && inComment)
					{
						inComment = false;
						__COUTT__ << "inComment end" << fileContents.substr(endPos-49, 50);
					}
					else if (fileContents[endPos] == '"' && !inComment)
					{
						inString = !inString; // toggle string state
						__COUTT__ << "inString toggle to " << inString << fileContents.substr(endPos-49, 50);
					}
					endPos++;
				}
				__COUTV__(fileContents.substr(endPos-50, 50));
				if (braceCount != 0)
				{
					__COUTT__ << "Malformed function found at " << funcPos << " to " << endPos << std::endl;
					continue; // malformed function definition, so skip			
				}

				// extract the function definition
				std::string functionDef = fileContents.substr(bracePos, endPos - bracePos);				

				// update position for next search
				pos = endPos;

				// eliminate special functions that should not be imported
				if (functionDef.find("fgInstance") != std::string::npos)
				{
					__COUTT__ << "Eliminated function def found at " << funcPos << " to " << endPos << std::endl;
					continue; // malformed function definition, so skip			
				}

				// prepend the specified string
				__COUT__ << "// Found: " + prepend[i] + "_" + functionHeader;

				size_t startFunc = fileContents.rfind('\n',funcPos);
				if(startFunc == std::string::npos)
				{
					__COUTT__ << "Malformed function found at startFunc=" << startFunc << " from " << funcPos << std::endl;
					continue; // malformed function definition, so skip			
				}
				std::string functionReturnVal = fileContents.substr(startFunc + 1, funcPos - (startFunc + 1));
				__COUTV__(functionReturnVal);
				__COUTTV__(startFunc);

				//accept as comment lines with "// "
				--startFunc; //move before newline of function definition
				std::string functionComment = "";
				while(true)
				{
					size_t startComment1 = fileContents.rfind("//",startFunc);
					size_t startComment2 = fileContents.rfind("\n",startFunc);
					__COUTTV__(startComment1);
					__COUTTV__(startComment2);

					if(startComment1 == std::string::npos || startComment1 < startComment2)
					{
						__COUTT__ << "No more comments found at " << startComment1 << " " << fileContents.substr(startFunc-48,50) << std::endl;
						break; //no more comments found
					}
					__COUTT__ << "Comment found at " << startComment1 << " " << fileContents.substr(startFunc-48,50) << std::endl;

					if(fileContents[startComment1 + 2] == ' ') //is a comment line
					{
						functionComment = "/" + fileContents.substr(startComment1, startFunc - startComment1) + "\n" + functionComment;						
					}
					//else //is a comment line to ignore
					
					startFunc = startComment1 - 2; //move before this comment line
				} //end comment extraction loop

				__COUTTV__(functionComment);
				__COUTTV__(functionDef);

				// output the function prototype to header file
				std::string declaredFunctionHeader = getDeclaredFunctionHeader(functionHeader, importPath + "/" + headerFilesToImport[i]);	
				outputHeaderFile << modifySource(functionReturnVal) << " " << prepend[i] << "_" << modifySource(declaredFunctionHeader, true /* isHeader */) << ";\n";

				// output the extracted function to source file
				outputFile << "//==============================================================================\n";
				outputFile << functionComment;
				outputFile << headerInstructionsSs.str();
				outputFile << functionReturnVal << " ROCTrackerInterface" << "::" << prepend[i] << "_" << modifySource(functionHeader, true /* isHeader */) << "\n";
				outputFile << modifySource(functionDef);
				outputFile << " // end " << prepend[i] << "_" << functionHeader.substr(0, functionHeader.find('(')) << "()\n\n";

				// break; //for debugging, only do one function per file
			} //end main function search loop

			outputFile.close();
			outputHeaderFile.close();
			__COUT_INFO__ << "Successfully wrote output file at " << outputFilePath << std::endl;
			__COUT_INFO__ << "Successfully wrote header file at " << outputHeaderPath << std::endl;

		}
		catch(const std::runtime_error& e)
		{
			importErrors.push_back(std::make_pair(
			    importFile, prepend[i] + std::string(" std::runtime_error: ") + e.what()));
		}
		catch(const std::exception& e)
		{
			importErrors.push_back(std::make_pair(
			    importFile, prepend[i] + std::string(" std::exception: ") + e.what()));
		}
		catch(...)
		{
			importErrors.push_back(
			    std::make_pair(importFile, prepend[i] + " Unknown exception occurred."));
		}
	} //end main import loop


	if(importErrors.size())
	{
		__COUT_ERR__ << "There were " << importErrors.size()
		             << " errors found while importing. See below:\n"
		             << std::endl;
		for(auto& importErr : importErrors)
			__COUT_ERR__ << "\t" << importErr.first << ": " << importErr.second << std::endl;
		__COUT_ERR__ << "End of errors.\n\n" << std::endl;
	}
	else
		__COUT_INFO__ << "There were NO ERRORS found while loading and exporting groups."
		              << __E__;
	std::cout << "Import of Tracker Test Stand complete!\n";

}  //end ImportTrackerTestStand()

//==============================================================================
int main(int argc, char* argv[])
{
	if(getenv("OTSDAQ_LOG_FHICL") == NULL)
		setenv("OTSDAQ_LOG_FHICL",
		       (std::string(__ENV__("USER_DATA")) +
		        "/MessageFacilityConfigurations/MessageFacilityWithCout.fcl")
		           .c_str(),
		       1);

	if(getenv("OTSDAQ_LOG_ROOT") == NULL)
		setenv(
		    "OTSDAQ_LOG_ROOT", (std::string(__ENV__("USER_DATA")) + "/Logs").c_str(), 1);

	INIT_MF("ImportTrackerTestStand");
	ImportTrackerTestStand(argc, argv);
	return 0;
} //end main()
// BOOST_AUTO_TEST_SUITE_END()
