#include <format>

#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"
#include "otsdaq/Macros/InterfacePluginMacros.h"

using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"

std::mutex ROCTrackerInterface::_json_filesystem_mutex;

ROCTrackerInterface::ROCTrackerInterface(
    const std::string&       rocUID,
    const ConfigurationTree& theXDAQContextConfigTree,
    const std::string&       theConfigurationPath)
    : ROCPolarFireCoreInterface(rocUID, theXDAQContextConfigTree, theConfigurationPath)
{
	INIT_MF("." /*directory used is USER_DATA/LOG/.*/);

	__COUT_INFO__ << "ROCTrackerInterface instantiated with link: " << linkID_
	              << " and EventWindowDelayOffset = " << delay_ << __E__;

	__CFG_COUT__ << "Constructor..." << __E__;

	registerFEMacroFunction("Read Register",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::ReadRegister),
	                        std::vector<std::string>{"Address"},
	                        std::vector<std::string>{"Value"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Reset Counters",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::ResetCounters),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Return Code"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Find Alignment",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::FindAlignment),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Success", "Summary"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction(
	    "Set Threshold",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::SetThreshold),
	    std::vector<std::string>{"Channel", "Preamp", "DAC value", "PrintLevel"},
	    std::vector<std::string>{"Success"},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction(
	    "Measure Threshold",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::MeasureThreshold),
	    std::vector<std::string>{"Channel"},
	    std::vector<std::string>{"Readback count", "Cal", "HV", "Total"},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction(
	    "Enable charge injection",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::EnableChargeInjection),
	    std::vector<std::string>{"First channel mask", "Duty cycle", "Delay"},
	    std::vector<std::string>{"Success"},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction("Disable charge injection",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::EnableChargeInjection),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Success"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Reset Digis",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::ResetDigis),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Return code"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Reboot MCU",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::RebootMCU),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Return code"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Set Event Window Delay",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::SetEventWindowDelay),
	                        std::vector<std::string>{"Delay (5 ns units)"},
	                        std::vector<std::string>{"Return code"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction(
	    "Set Digitization Window",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::SetDigitizationWindow),
	    std::vector<std::string>{"TStart (5 ns units)", "TStop (5 ns units)"},
	    std::vector<std::string>{"Return code"},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction("Digi Read",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::DigiRead),
	                        std::vector<std::string>{"Address", "HvCal"},
	                        std::vector<std::string>{"Return code", "Value"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Digi Write",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::DigiWrite),
	                        std::vector<std::string>{"Address", "HvCal", "Data"},
	                        std::vector<std::string>{"Return code"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Read Panel ID",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::ReadPanelID),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Panel ID", "Success"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Read Serial Number",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::ReadSerialNumber),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Serial Number"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Measure Thresholds",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::MeasureThresholds),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Return code", "Thresholds"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction(
	    "Find Threshold",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::FindThreshold),
	    std::vector<std::string>{"Channel", "Preamp", "Threshold (mV)", "Tolerance (mV)"},
	    std::vector<std::string>{"Success", "DAC value"},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction("Find Thresholds",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::FindThresholds),
	                        std::vector<std::string>{"Threshold (mV)", "Tolerance (mV)"},
	                        std::vector<std::string>{"Failed count", "DAC values"},
	                        1,
	                        "" /* tooltip info here */);

  registerFEMacroFunction("Find and serialize thresholds",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::FindAndSerializeThresholds),
	                        std::vector<std::string>{"Threshold (mV)", "Tolerance (mV)", "Filesystem path"},
	                        std::vector<std::string>{"Failed count", "DAC values", "Serialization successful"},
	                        1,
	                        "" /* tooltip info here */);

  registerFEMacroFunction("Deserialize and set thresholds",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::DeserializeAndSetThresholds),
	                        std::vector<std::string>{"Filesystem path"},
	                        std::vector<std::string>{"Failed count"},
	                        1,
	                        "" /* tooltip info here */);

  registerFEMacroFunction("Test JSON write",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::TestJSON),
	                        std::vector<std::string>{"Key", "Value", "Path"},
	                        std::vector<std::string>{"Serialization successful"},
	                        1,
	                        "" /* tooltip info here */);
}  // end constructor

ROCTrackerInterface::~ROCTrackerInterface(void)
{
	// NOTE:: be careful not to call __FE_COUT__ decoration because it uses the
	// tree and it may already be destructed partially
	__COUT__ << FEVInterface::interfaceUID_ << " Destructor" << __E__;
}  // end destructor

void ROCTrackerInterface::onDTCReady()
{
	auto dtc = getDTC();
	_roc     = std::make_shared<trkdaq::ROC>(linkID_, dtc);
}

void ROCTrackerInterface::ReadRegister(__ARGS__)
{
	address_t address = __GET_ARG_IN__("Address", address_t, 0x0);
	uint32_t  rv      = _roc->ReadRegister(address);
	__SET_ARG_OUT__("Value", std::to_string(rv));
}

void ROCTrackerInterface::ResetCounters(__ARGS__)
{
	int rv = _roc->Reset();
	__SET_ARG_OUT__("Return code", std::to_string(rv));
}

void ROCTrackerInterface::FindAlignment(__ARGS__)
{
	auto rv = _roc->FindAlignment();

	std::stringstream stream;
	const auto&       alignment = _roc->LatestAlignment();
	print_legacy_table(alignment, stream);

	__SET_ARG_OUT__("Success", std::to_string(rv));
	__SET_ARG_OUT__("Summary", stream.str());
}

void ROCTrackerInterface::SetThreshold(__ARGS__)
{
	int channel     = __GET_ARG_IN__("Channel", int, -1);
	int preamp      = __GET_ARG_IN__("Preamp", int, -1);
	int threshold   = __GET_ARG_IN__("DAC value", int, -1);
	int print_level = __GET_ARG_IN__("PrintLevel", int, 0x2);
	// TODO throw on -1s
	__FE_COUT__ << "ejc: ROCTrackerInterface::SetThreshold" << __E__;
	auto rv = _roc->SetThreshold(channel, preamp, threshold, print_level);
	__SET_ARG_OUT__("Success", std::to_string(rv));
}

void ROCTrackerInterface::MeasureThreshold(__ARGS__)
{
	int channel = __GET_ARG_IN__("Channel", int, -1);

	std::vector<float> rvs;
	uint32_t           mask_lo = 0xFFFFFFFF;
	uint32_t           mask_md = 0xFFFFFFFF;
	uint32_t           mask_hi = 0xFFFFFFFF;
	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);
	_roc->ReadThresholds(rvs, mask_lo, mask_md, mask_hi, 0, stream);

	size_t idx;

	idx      = 3 * channel;
	auto rvh = rvs[idx + 0];
	auto rvc = rvs[idx + 1];
	auto rvt = rvs[idx + 2];

	std::string msg = "ejc: channel " + std::to_string(channel) +
	                  " thresholds = " + std::to_string(rvh) + ", " +
	                  std::to_string(rvc) + ", " + std::to_string(rvt);
	__FE_COUT__ << msg << __E__;
	__SET_ARG_OUT__("Readback count", std::to_string(rvs.size()));
	__SET_ARG_OUT__("HV", std::to_string(rvh));
	__SET_ARG_OUT__("Cal", std::to_string(rvc));
	__SET_ARG_OUT__("Total", std::to_string(rvt));
}

void ROCTrackerInterface::EnableChargeInjection(__ARGS__)
{
	int first_channel_mask = __GET_ARG_IN__("First channel mask", int, 0x10);
	int duty_cycle         = __GET_ARG_IN__("Duty cycle", int, 10);
	int delay              = __GET_ARG_IN__("Delay", int, 1000);
	int print_level        = 0;
	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	__FE_COUT__ << "ejc: ROCTrackerInterface::EnableChargeInjection" << __E__;
	auto rv = _roc->EnableChargeInjection(
	    first_channel_mask, duty_cycle, delay, print_level, stream);
	__SET_ARG_OUT__("Success", std::to_string(rv));
}

void ROCTrackerInterface::DisableChargeInjection(__ARGS__)
{
	int                print_level = 0;
	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	__FE_COUT__ << "ejc: ROCTrackerInterface::DisableChargeInjection" << __E__;
	auto rv = _roc->DisableChargeInjection(print_level, stream);
	__SET_ARG_OUT__("Success", std::to_string(rv));
}

void ROCTrackerInterface::ResetDigis(__ARGS__)
{
	__FE_COUT__ << "ROCTrackerInterface::ResetDigis" << __E__;
	auto rv = _roc->ResetDigis();
	__SET_ARG_OUT__("Return code", std::to_string(rv));
}

void ROCTrackerInterface::RebootMCU(__ARGS__)
{
	__FE_COUT__ << "ROCTrackerInterface::RebootMCU" << __E__;
	auto rv = _roc->RebootMCU();
	__SET_ARG_OUT__("Return code", std::to_string(rv));
}

void ROCTrackerInterface::SetEventWindowDelay(__ARGS__)
{
	int delay_5ns = __GET_ARG_IN__("Delay (5 ns units)", int, 0);
	if(delay_5ns < 0 || delay_5ns > 0xFFFF)
	{
		__FE_SS__ << "Delay (5 ns units) out of range [0, 65535]: " << delay_5ns << __E__;
		__FE_SS_THROW__;
	}

	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	__FE_COUT__ << "ROCTrackerInterface::SetEventWindowDelay delay_5ns=" << delay_5ns
	            << __E__;
	auto rv = _roc->SetEventWindowDelay(static_cast<uint16_t>(delay_5ns), stream);
	__SET_ARG_OUT__("Return code", std::to_string(rv));
}

void ROCTrackerInterface::SetDigitizationWindow(__ARGS__)
{
	int t_start     = __GET_ARG_IN__("TStart (5 ns units)", int, -1);
	int t_stop      = __GET_ARG_IN__("TStop (5 ns units)", int, -1);
	int print_level = 0;

	if(t_start < 0 || t_start > 0xFFFF)
	{
		__FE_SS__ << "TStart (5 ns units) out of range [0, 65535]: " << t_start << __E__;
		__FE_SS_THROW__;
	}
	if(t_stop < 0 || t_stop > 0xFFFF)
	{
		__FE_SS__ << "TStop (5 ns units) out of range [0, 65535]: " << t_stop << __E__;
		__FE_SS_THROW__;
	}
	if(t_stop <= t_start)
	{
		__FE_SS__ << "TStop must exceed TStart: TStart=" << t_start << " TStop=" << t_stop
		          << __E__;
		__FE_SS_THROW__;
	}

	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	__FE_COUT__ << "ROCTrackerInterface::SetDigitizationWindow TStart=" << t_start
	            << " TStop=" << t_stop << __E__;
	auto rv = _roc->SetDigitizationWindow(static_cast<uint16_t>(t_start),
	                                      static_cast<uint16_t>(t_stop),
	                                      print_level,
	                                      stream);
	__SET_ARG_OUT__("Return code", std::to_string(rv));
}

void ROCTrackerInterface::DigiRead(__ARGS__)
{
	int addr        = __GET_ARG_IN__("Address", int, -1);
	int hv_cal      = __GET_ARG_IN__("HvCal", int, -1);
	int print_level = 0;

	if(addr < 0)
	{
		__FE_SS__ << "Address must be non-negative: " << addr << __E__;
		__FE_SS_THROW__;
	}
	if(hv_cal != 0 && hv_cal != 1)
	{
		__FE_SS__ << "HvCal must be 0 or 1: " << hv_cal << __E__;
		__FE_SS_THROW__;
	}

	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	uint32_t value = 0;
	__FE_COUT__ << "ROCTrackerInterface::DigiRead addr=" << addr << " hv_cal=" << hv_cal
	            << __E__;
	auto rv = _roc->DigiRead(addr, hv_cal, value, print_level, stream);
	__SET_ARG_OUT__("Return code", std::to_string(rv));
	__SET_ARG_OUT__("Value", std::to_string(value));
}

void ROCTrackerInterface::DigiWrite(__ARGS__)
{
	int addr        = __GET_ARG_IN__("Address", int, -1);
	int hv_cal      = __GET_ARG_IN__("HvCal", int, -1);
	int data        = __GET_ARG_IN__("Data", int, -1);
	int print_level = 0;

	if(addr < 0)
	{
		__FE_SS__ << "Address must be non-negative: " << addr << __E__;
		__FE_SS_THROW__;
	}
	if(hv_cal != 0 && hv_cal != 1)
	{
		__FE_SS__ << "HvCal must be 0 or 1: " << hv_cal << __E__;
		__FE_SS_THROW__;
	}
	if(data < 0 || data > 0xFFFF)
	{
		__FE_SS__ << "Data out of range [0, 65535]: " << data << __E__;
		__FE_SS_THROW__;
	}

	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	__FE_COUT__ << "ROCTrackerInterface::DigiWrite addr=" << addr << " hv_cal=" << hv_cal
	            << " data=" << data << __E__;
	auto rv =
	    _roc->DigiWrite(addr, hv_cal, static_cast<uint16_t>(data), print_level, stream);
	__SET_ARG_OUT__("Return code", std::to_string(rv));
}

void ROCTrackerInterface::ReadPanelID(__ARGS__)
{
	int print_level = 0;

	__FE_COUT__ << "ROCTrackerInterface::ReadPanelID" << __E__;
	auto rv = _roc->ReadPanelID(print_level);

	// ReadPanelID returns the panel ID on success, negative error code on failure
	if(rv < 0)
	{
		__SET_ARG_OUT__("Panel ID", std::string(""));
		__SET_ARG_OUT__("Success", std::to_string(rv));
	}
	else
	{
		__SET_ARG_OUT__("Panel ID", std::to_string(rv));
		__SET_ARG_OUT__("Success", std::to_string(0));
	}
}

void ROCTrackerInterface::ReadSerialNumber(__ARGS__)
{
	__FE_COUT__ << "ROCTrackerInterface::ReadSerialNumber" << __E__;
	auto rv = _roc->ReadSerialNumber();
	__SET_ARG_OUT__("Serial Number", rv);
}

std::string ROCTrackerInterface::FormatThresholdTable(
    const std::vector<float>& thresholds)
{
	std::stringstream table;
	table << std::endl;
	table << std::format(" {:>7} {:>11} {:>11} {:>11}\n", "Channel", "Cal", "HV", "Sum");
	table << "--------------------------------------------\n";
	for(int channel = 0; channel < 96; ++channel)
	{
		size_t idx = 3 * static_cast<size_t>(channel);
		float  hv  = thresholds.at(idx + 0);
		float  cal = thresholds.at(idx + 1);
		float  tot = thresholds.at(idx + 2);
		table << std::format(
		    " {:4d} {:11.3f} {:11.3f} {:11.3f}\n", channel, cal, hv, tot);
	}
	return table.str();
}

void ROCTrackerInterface::MeasureThresholds(__ARGS__)
{
	// measure thresholds for all channels (all masks fully enabled)
	uint32_t           mask_lo     = 0xFFFFFFFF;
	uint32_t           mask_md     = 0xFFFFFFFF;
	uint32_t           mask_hi     = 0xFFFFFFFF;
	int                print_level = 0;
	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	std::vector<float> thresholds;
	__FE_COUT__ << "ROCTrackerInterface::MeasureThresholds" << __E__;
	auto rv =
	    _roc->ReadThresholds(thresholds, mask_lo, mask_md, mask_hi, print_level, stream);

	__SET_ARG_OUT__("Return code", std::to_string(rv));
	__SET_ARG_OUT__("Thresholds", FormatThresholdTable(thresholds));
}

void ROCTrackerInterface::FindThreshold(__ARGS__)
{
	int   channel      = __GET_ARG_IN__("Channel", int, -1);
	int   preamp       = __GET_ARG_IN__("Preamp", int, -1);
	float threshold_mv = __GET_ARG_IN__("Threshold (mV)", float, 0.0f);
	float tolerance_mv = __GET_ARG_IN__("Tolerance (mV)", float, 0.0f);

	if(channel < 0 || channel > 95)
	{
		__FE_SS__ << "Channel out of range [0, 95]: " << channel << __E__;
		__FE_SS_THROW__;
	}
	if(preamp != 0 && preamp != 1)
	{
		__FE_SS__ << "Preamp must be 0 or 1: " << preamp << __E__;
		__FE_SS_THROW__;
	}
	if(tolerance_mv <= 0.0f)
	{
		__FE_SS__ << "Tolerance (mV) must be positive: " << tolerance_mv << __E__;
		__FE_SS_THROW__;
	}

	DTCLib::roc_data_t dac = 0;
	__FE_COUT__ << "ROCTrackerInterface::FindThreshold channel=" << channel
	            << " preamp=" << preamp << " threshold_mv=" << threshold_mv
	            << " tolerance_mv=" << tolerance_mv << __E__;
	auto success = _roc->FindThreshold(channel, preamp, threshold_mv, tolerance_mv, dac);

	__SET_ARG_OUT__("Success", std::to_string(success));
	__SET_ARG_OUT__("DAC value", std::to_string(dac));
}

std::string ROCTrackerInterface::FormatDacTable(
    const std::vector<DTCLib::roc_data_t>& dacs)
{
	std::stringstream table;
	table << std::endl;
	table << std::format(" {:>7} {:>11} {:>11}\n", "Channel", "Cal", "HV");
	table << "--------------------------------\n";
	for(int channel = 0; channel < 96; ++channel)
	{
		size_t idx = 2 * static_cast<size_t>(channel);
		auto   cal = dacs.at(idx + 0);
		auto   hv  = dacs.at(idx + 1);
		table << std::format(" {:7d} {:11d} {:11d}\n", channel, cal, hv);
	}
	return table.str();
}

void ROCTrackerInterface::FindThresholds(__ARGS__)
{
	float threshold_mv = __GET_ARG_IN__("Threshold (mV)", float, 0.0f);
	float tolerance_mv = __GET_ARG_IN__("Tolerance (mV)", float, 0.0f);

	if(tolerance_mv <= 0.0f)
	{
		__FE_SS__ << "Tolerance (mV) must be positive: " << tolerance_mv << __E__;
		__FE_SS_THROW__;
	}

	std::vector<DTCLib::roc_data_t> dacs;
	__FE_COUT__ << "ROCTrackerInterface::FindThresholds threshold_mv=" << threshold_mv
	            << " tolerance_mv=" << tolerance_mv << __E__;
	auto n_failed = _roc->FindThresholds(threshold_mv, tolerance_mv, dacs);

	__SET_ARG_OUT__("Failed count", std::to_string(n_failed));
	__SET_ARG_OUT__("DAC values", FormatDacTable(dacs));
}

void ROCTrackerInterface::writeEmulatorRegister(uint16_t address, uint16_t data_to_write)
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

void ROCTrackerInterface::configure(void)
{
  auto config = nlohmann::basic_json(NULL);
	try
	{
		__CFG_COUT__
		    << "Tracker configure, first configure back-end communication with DTC... "
		    << __E__;
		ROCPolarFireCoreInterface::configure();

		//__COUT_INFO__ << "Tracker configure, next configure front-end... " << __E__;
		//__COUT_INFO__ << "..... write parameter 1 = " << TrackerParameter_1_ << __E__;
		//__COUT_INFO__ << "..... followed by parameter 2 = " << TrackerParameter_2_ <<
		//__E__;
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

bool ROCTrackerInterface::running(void) { return true; }

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


void ROCTrackerInterface::FindAndSerializeThresholds(__ARGS__)
{
	float threshold_mv = __GET_ARG_IN__("Threshold (mV)", float, 0.0f);
	float tolerance_mv = __GET_ARG_IN__("Tolerance (mV)", float, 0.0f);
  std::string path   = __GET_ARG_IN__("Filesystem path", std::string, "");

	if(tolerance_mv <= 0.0f)
	{
		__FE_SS__ << "Tolerance (mV) must be positive: " << tolerance_mv << __E__;
		__FE_SS_THROW__;
	}

	std::vector<DTCLib::roc_data_t> dacs;
	__FE_COUT__ << "ROCTrackerInterface::FindThresholds threshold_mv=" << threshold_mv
	            << " tolerance_mv=" << tolerance_mv << __E__;
	auto n_failed = _roc->FindThresholds(threshold_mv, tolerance_mv, dacs);

  // build key from minnesota id
  auto minnesota = _roc->ReadPanelID();
  std::ostringstream ss;
  ss << std::setw(3) << std::setfill('0') << minnesota;
  auto key = "MN" + ss.str();
  // cast into json
  nlohmann::json value;
  for (size_t i = 0 ; i < 96 ; i++){
    value[std::to_string(i)] = {
      {"Cal", dacs[2*i + 0]},
      {"HV",  dacs[2*i + 1]}
    };
  }
  // write to disk
  auto written = ROCTrackerInterface::SafeSerialize(path, key, value);

	__SET_ARG_OUT__("Failed count", std::to_string(n_failed));
	__SET_ARG_OUT__("DAC values", FormatDacTable(dacs));
  __SET_ARG_OUT__("Serialization successful", std::to_string(written));
}

void ROCTrackerInterface::DeserializeAndSetThresholds(__ARGS__)
{
  std::string path   = __GET_ARG_IN__("Filesystem path", std::string, "");

  // build key from minnesota id
  auto minnesota = _roc->ReadPanelID();
  std::ostringstream ss;
  ss << std::setw(3) << std::setfill('0') << minnesota;
  auto key = "MN" + ss.str();

  // read from disk
  auto json = ROCTrackerInterface::SafeDeserialize(path, key);

  // set thresholds
	size_t n_failed = 0;
  for (size_t i = 0 ; i < 96 ; i++){
    int rc;
    int dac;
    auto dacs = json[std::to_string(i)];

    dac = static_cast<int>(dacs["Cal"]);
    rc = _roc->SetThreshold(i, 0, dac);
    if (rc != 0){
        n_failed++;
    }

    dac = static_cast<int>(dacs["HV"]);
    _roc->SetThreshold(i, 1, dac);
    if (rc != 0){
        n_failed++;
    }
  }

	__SET_ARG_OUT__("Failed count", std::to_string(n_failed));
}

void ROCTrackerInterface::TestJSON(__ARGS__){
  std::string key   = __GET_ARG_IN__("Key", std::string, "");
  std::string value = __GET_ARG_IN__("Value", std::string, "");
  std::string path  = __GET_ARG_IN__("Path", std::string, "");

  auto written = ROCTrackerInterface::SafeSerialize(path, key, value);

  __SET_ARG_OUT__("Serialization successful", std::to_string(written));
}

bool ROCTrackerInterface::SafeSerialize(std::string path,
                                        std::string key,
                                        nlohmann::json value){
  // lock on filesystem access read/write
  std::lock_guard lock(ROCTrackerInterface::_json_filesystem_mutex);
  nlohmann::json json;

  // read preexisting mappings from disk
  std::ifstream fi(path.c_str());
  if (!fi){
    return false;
  }
  fi >> json;
  fi.close();

  // add new mapping
  json[key] = value;

  // write back to disk
  std::ofstream fo(path.c_str());
  if (!fo){
    return false;
  }
  fo << json << std::endl;
  fo.close();

  return true;
}

nlohmann::json ROCTrackerInterface::SafeDeserialize(std::string path,
                                                    std::string key){
  // lock on filesystem access read/write
  std::lock_guard lock(ROCTrackerInterface::_json_filesystem_mutex);
  nlohmann::json json;

  // read preexisting mappings from disk
  std::ifstream fi(path.c_str());
  if (!fi){
    return false;
  }
  fi >> json;
  fi.close();

  auto rv = json[key];
  return rv;
}

DEFINE_OTS_INTERFACE(ROCTrackerInterface)
