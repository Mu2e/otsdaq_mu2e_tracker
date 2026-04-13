#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"
#include "otsdaq/Macros/InterfacePluginMacros.h"

using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"

ROCTrackerInterface::ROCTrackerInterface(
    const std::string& rocUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string& theConfigurationPath)
			: ROCPolarFireCoreInterface(rocUID,
																	theXDAQContextConfigTree,
																	theConfigurationPath)
{
	INIT_MF("." /*directory used is USER_DATA/LOG/.*/);

	__COUT_INFO__ << "ROCTrackerInterface instantiated with link: " << linkID_
	              << " and EventWindowDelayOffset = " << delay_ << __E__;

	__CFG_COUT__ << "Constructor..." << __E__;

	registerFEMacroFunction("Read Register",
													static_cast<FEVInterface::frontEndMacroFunction_t>(
														&ROCTrackerInterface::ReadRegister
													),
													std::vector<std::string>{"Address"},
													std::vector<std::string>{"Value"},
													1, "" /* tooltip info here */);

	registerFEMacroFunction("Find Alignment",
													static_cast<FEVInterface::frontEndMacroFunction_t>(
														&ROCTrackerInterface::FindAlignment
													),
													std::vector<std::string>{},
													std::vector<std::string>{"Success", "Summary"},
													1, "" /* tooltip info here */);

	registerFEMacroFunction("Set Threshold",
													static_cast<FEVInterface::frontEndMacroFunction_t>(
														&ROCTrackerInterface::SetThreshold
													),
													std::vector<std::string>{"Channel", "Preamp",
																									 "DAC value", "PrintLevel"},
													std::vector<std::string>{"Success"},
													1, "" /* tooltip info here */);

	registerFEMacroFunction("Measure Threshold",
													static_cast<FEVInterface::frontEndMacroFunction_t>(
														&ROCTrackerInterface::MeasureThreshold
													),
													std::vector<std::string>{"Channel"},
													std::vector<std::string>{"Readback count", "Cal", "HV", "Total"},
													1, "" /* tooltip info here */);
}  // end constructor

ROCTrackerInterface::~ROCTrackerInterface(void)
{
	// NOTE:: be careful not to call __FE_COUT__ decoration because it uses the
	// tree and it may already be destructed partially
	__COUT__ << FEVInterface::interfaceUID_ << " Destructor" << __E__;
}  // end destructor

void ROCTrackerInterface::onDTCReady(){
	auto dtc = getDTC();
	_roc = std::make_shared<trkdaq::ROC>(linkID_, dtc);
}

void ROCTrackerInterface::ReadRegister(__ARGS__){
	address_t address = __GET_ARG_IN__("Address", address_t, 0x0);
	uint32_t rv = _roc->ReadRegister(address);
	__SET_ARG_OUT__("Value", std::to_string(rv));
}

void ROCTrackerInterface::FindAlignment(__ARGS__){
	Alignment alignment;
	auto rv = _roc->FindAlignment(alignment);

	std::stringstream stream;
	print_legacy_table(alignment, stream);

	__SET_ARG_OUT__("Success", std::to_string(rv));
	__SET_ARG_OUT__("Summary", stream.str());
}

void ROCTrackerInterface::SetThreshold(__ARGS__){
	int channel			= __GET_ARG_IN__("Channel", int, -1);
	int preamp 			= __GET_ARG_IN__("Preamp", int, -1);
	int threshold   = __GET_ARG_IN__("DAC value", int, -1);
	int print_level = __GET_ARG_IN__("PrintLevel", int, 0x2);
	// TODO throw on -1s
	__FE_COUT__ << "ejc: ROCTrackerInterface::SetThreshold" << __E__;
	auto rv = _roc->SetThreshold(channel, preamp, threshold, print_level);
	__SET_ARG_OUT__("Success", std::to_string(rv));
}

void ROCTrackerInterface::MeasureThreshold(__ARGS__){
	int channel			= __GET_ARG_IN__("Channel", int, -1);

	std::vector<float> rvs;
	uint32_t mask_lo = 0xFFFFFFFF;
	uint32_t mask_md = 0xFFFFFFFF;
	uint32_t mask_hi = 0xFFFFFFFF;
	trkdaq::NullStream null;
	auto stream = std::ostream(&null);
	_roc->ReadThresholds(rvs, mask_lo, mask_md, mask_hi, 0, stream);

	size_t idx;

	idx = 3*channel;
	auto rvh = rvs[idx + 0];
	auto rvc = rvs[idx + 1];
	auto rvt = rvs[idx + 2];

	std::string msg = "ejc: channel "
								  + std::to_string(channel)
									+ " thresholds = "
	                + std::to_string(rvh)
									+ ", "
	                + std::to_string(rvc)
									+ ", "
	                + std::to_string(rvt);
	__FE_COUT__ << msg << __E__;
	__SET_ARG_OUT__("Readback count", std::to_string(rvs.size()));
	__SET_ARG_OUT__("HV", std::to_string(rvh));
	__SET_ARG_OUT__("Cal", std::to_string(rvc));
	__SET_ARG_OUT__("Total", std::to_string(rvt));
}

void ROCTrackerInterface::writeEmulatorRegister(uint16_t address,
																								uint16_t data_to_write)
{
	__FE_COUT__ << "Calling Tracker write ROC Emulator register: link number " << std::dec
	            << linkID_ << ", address = " << address
	            << ", write data = " << data_to_write << __E__;

	return;

}  // end writeEmulatorRegister()

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

void ROCTrackerInterface::readEmulatorBlock(std::vector<uint16_t>& data,
                                            uint16_t address,
                                            uint16_t wordCount,
                                            bool incrementAddress)
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

void ROCTrackerInterface::start(std::string runNumber)
{

	//	DataProducerBase::registerToBuffer();

	return;
}

bool ROCTrackerInterface::running(void)
{
	return true;
}

void ROCTrackerInterface::stop()  // runNumber)
{
	return;
}

// return false to stop workloop thread
bool ROCTrackerInterface::emulatorWorkLoop(void)
{
	//__CFG_COUT__ << "emulator working..." << __E__;
	return true;  // true to keep workloop going
}  // end emulatorWorkLoop()

DEFINE_OTS_INTERFACE(ROCTrackerInterface)
