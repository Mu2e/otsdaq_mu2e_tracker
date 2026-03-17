#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"
#include "otsdaq/Macros/InterfacePluginMacros.h"

using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"

//=========================================================================================
ROCTrackerInterface::ROCTrackerInterface(
    const std::string&       rocUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string&       theConfigurationPath)
    : ROCPolarFireCoreInterface(rocUID, theXDAQContextConfigTree, theConfigurationPath)
    , trackerDTC_(std::make_unique<trkdaq::DtcInterface>(getDTC()))
{
	INIT_MF("." /*directory used is USER_DATA/LOG/.*/);

	__COUT_INFO__ << "ROCTrackerInterface instantiated with link: " << linkID_
	              << " and EventWindowDelayOffset = " << delay_ << __E__;

	__CFG_COUT__ << "Constructor..." << __E__;

// ejc: block out macros for now
/*
	registerFEMacroFunction("ROC Status",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::GetStatus),
	                        std::vector<std::string>{},          // inputs parameters
	                        std::vector<std::string>{"Status"},  // output parameters
	                        1);  // requiredUserPermissions

	registerFEMacroFunction(
	    "Read ROC Error Counter",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::ReadROCErrorCounter),
	    std::vector<std::string>{"Address to read, Default := 0]"},  // inputs parameters
	    std::vector<std::string>{"Status"},                          // output parameters
	    1);  // requiredUserPermissions

	registerFEMacroFunction("Setup for Digi Data Taking",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::SetupForDigiDataTaking),
	                        std::vector<std::string>{},          // inputs parameters
	                        std::vector<std::string>{"Result"},  // output parameters
	                        1);  // requiredUserPermissions

	registerFEMacroFunction("Find Alignment",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::FindAlignment),
	                        std::vector<std::string>{},          // inputs parameters
	                        std::vector<std::string>{"Result"},  // output parameters
	                        1);  // requiredUserPermissions

	registerFEMacroFunction(
	    "Get UI Parameters",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::GetUIParameters),
	    std::vector<std::string>{},  // inputs parameters
	    std::vector<std::string>{"fEnabled",
	                             "fPcieAddr",
	                             "fLinkMask",
	                             "fRocReadoutMode",
	                             "fRocLaneMask",
	                             "fRocNHitsPerLane",
	                             "fSampleEdgeMode",
	                             "fEmulateCfo",
	                             "fJAMode",
	                             "fOnSpill",
	                             "fEventMode",
	                             "fDtcID",
	                             "fPartitionID",
	                             "fMacAddrByte",
	                             "fSleepTimeROCWrite",
	                             "fSleepTimeROCReset"},  // output parameters
	    1);                                              // requiredUserPermissions

	registerFEMacroFunction(
	    "Set UI Parameters",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::SetUIParameters),
	    std::vector<std::string>{
	        "fEnabled (Default := unchanged)",
	        "fPcieAddr (Default := unchanged)",
	        "fLinkMask (Default := unchanged)",
	        "fRocReadoutMode (Default := unchanged)",
	        "fRocLaneMask (Default := unchanged)",
	        "fRocNHitsPerLane (Default := unchanged)",
	        "fSampleEdgeMode (Default := unchanged)",
	        "fEmulateCfo (Default := unchanged)",
	        "fJAMode (Default := unchanged)",
	        "fOnSpill (Default := unchanged)",
	        "fEventMode (Default := unchanged)",
	        "fDtcID (Default := unchanged)",
	        "fPartitionID (Default := unchanged)",
	        "fMacAddrByte (Default := unchanged)",
	        "fSleepTimeROCWrite (Default := unchanged)",
	        "fSleepTimeROCReset (Default := unchanged)"},  // inputs parameters
	    std::vector<std::string>{"fEnabled",
	                             "fPcieAddr",
	                             "fLinkMask",
	                             "fRocReadoutMode",
	                             "fRocLaneMask",
	                             "fRocNHitsPerLane",
	                             "fSampleEdgeMode",
	                             "fEmulateCfo",
	                             "fJAMode",
	                             "fOnSpill",
	                             "fEventMode",
	                             "fDtcID",
	                             "fPartitionID",
	                             "fMacAddrByte",
	                             "fSleepTimeROCWrite",
	                             "fSleepTimeROCReset"},  // output parameters
	    1);                                              // requiredUserPermissions
*/

}  // end constructor

//==========================================================================================
ROCTrackerInterface::~ROCTrackerInterface(void)
{
	// NOTE:: be careful not to call __FE_COUT__ decoration because it uses the
	// tree and it may already be destructed partially
	__COUT__ << FEVInterface::interfaceUID_ << " Destructor" << __E__;

	// trackerDTC_ is automatically deleted by unique_ptr
}  // end destructor

//==================================================================================================
void ROCTrackerInterface::writeEmulatorRegister(uint16_t address, uint16_t data_to_write)
{
	__FE_COUT__ << "Calling Tracker write ROC Emulator register: link number " << std::dec
	            << linkID_ << ", address = " << address
	            << ", write data = " << data_to_write << __E__;

	return;

}  // end writeEmulatorRegister()

//==================================================================================================
uint16_t ROCTrackerInterface::readEmulatorRegister(uint16_t address)
{
	__CFG_COUT__ << "Tracker emulator read" << __E__;

	// if(address == 6 || address == 7)
	return ROCPolarFireCoreInterface::readEmulatorRegister(address);
	// if(address == ADDRESS_FIRMWARE_VERSION)
	// 	return 0x5;
	// else if(address == ADDRESS_MYREGISTER)
	//  	return temp1_.GetBoardTempC();
	// else
	return 0xBAFD;

}  // end readEmulatorRegister()

//==================================================================================================
void ROCTrackerInterface::readEmulatorBlock(std::vector<uint16_t>& data,
                                            uint16_t               address,
                                            uint16_t               wordCount,
                                            bool                   incrementAddress)
{
	__CFG_COUT__ << "Tracker emulator block read "
	             << "wordCount= " << wordCount << __E__;

	// make up some bogus data. Right now hardwired, could be read in as a parameter...
	double input_data = 15;

	for(unsigned int i = 0; i < wordCount; ++i)
	{
		double rand_data =
		    input_data + 0.5 * (input_data * (((double)rand() / (RAND_MAX)) - 0.5));
		__CFG_COUT__ << "rand_data= " << rand_data << __E__;
		data.push_back(rand_data);
		//		data.push_back(address + (incrementAddress?i:0));
	}
}  // end readEmulatorBlock()

//==================================================================================================
void ROCTrackerInterface::configure(void)
{
	try
	{
		__CFG_COUT__
				<< "Tracker configure, first configure back-end communication with DTC... "
				<< __E__;
		ROCPolarFireCoreInterface::configure();

		//__COUT_INFO__ << "Tracker configure, next configure front-end... " << __E__;
		//__COUT_INFO__ << "..... write parameter 1 = " << TrackerParameter_1_ << __E__;
		//__COUT_INFO__ << "..... followed by parameter 2 = " << TrackerParameter_2_ << __E__;
	}
	catch(const std::runtime_error& e)
	{
		__FE_COUT__ << "Error caught: " << e.what() << __E__;
		throw;
	}
	catch(...)
	{
		__FE_SS__ << "Unknown error caught. Check printouts!" << __E__;
		try
		{
			throw;
		}  // one more try to printout extra info
		catch(const std::exception& e)
		{
			ss << "Exception message: " << e.what();
		}
		catch(...)
		{
		}
		__FE_SS_THROW__;
	}
}

//==============================================================================
void ROCTrackerInterface::start(std::string runNumber)
{

	//	DataProducerBase::registerToBuffer();

	return;
}

//==============================================================================
bool ROCTrackerInterface::running(void)
{
	return true;
}

void ROCTrackerInterface::stop()  // runNumber)
{
	return;
}

//==================================================================================================
// return false to stop workloop thread
bool ROCTrackerInterface::emulatorWorkLoop(void)
{
	//__CFG_COUT__ << "emulator working..." << __E__;
	return true;  // true to keep workloop going
}  // end emulatorWorkLoop()

DEFINE_OTS_INTERFACE(ROCTrackerInterface)
