#include <format>

#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"
#include "otsdaq/Macros/InterfacePluginMacros.h"

using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"

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
	                        std::vector<std::string>{},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction("Reset and configure",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::ResetAndConfigure),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{},
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

	registerFEMacroFunction(
	    "Set Channel Mask",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::SetChannelMask),
	    std::vector<std::string>{"Mask channels 0-31", "Mask channels 32-63", "Mask channels 64-95"},
	    std::vector<std::string>{"Return code"},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction(
	    "\"Notorious Read\"",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::NotoriousRead),
	    std::vector<std::string>{"ADC Mode",
	                             "TDC Mode",
	                             "Waveform delay (lookback)",
	                             "Additional Sample Packets",
	                             "Trigger count",
	                             "Channel mask lo",
	                             "Channel mask md",
	                             "Channel mask hi",
	                             "Enable pulser",
	                             "Fiber clock mask",
	                             "\"Mode\" (deprecated)",
	                             "\"Clock\" (deprecated)"},
	    std::vector<std::string>{"ADC Mode",
	                             "TDC Mode",
	                             "Waveform delay (lookback)",
	                             "Trigger count",
	                             "Channel masks",
	                             "Additional Sample Packets",
	                             "Enable pulser",
	                             "Fiber clock mask",
	                             "\"Mode\" (deprecated)",
	                             "\"Clock\" (deprecated)",
	                             "\"digi_read_0xb\"",
	                             "\"digi_read_0xe\"",
	                             "\"digi_read_0xd\"",
	                             "\"digi_read_0xc\""},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction(
	    "Configure Digis",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::ConfigureDigis),
	    std::vector<std::string>{"TDC Mode",
	                             "Waveform delay (lookback)",
	                             "Additional Sample Packets",
	                             "Channel mask lo",
	                             "Channel mask md",
	                             "Channel mask hi"},
	    std::vector<std::string>{},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction("Initialize Digis",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::InitializeDigis),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Output"},
	                        1,
	                        "" /* tooltip info here */);

	registerFEMacroFunction(
	    "Digi Read/Write",
	    static_cast<FEVInterface::frontEndMacroFunction_t>(
	        &ROCTrackerInterface::DigiRW),
	    std::vector<std::string>{"Read/Write", "HvCal", "Address", "Data"},
	    std::vector<std::string>{"Output"},
	    1,
	    "" /* tooltip info here */);

	registerFEMacroFunction("Print Status",
	                        static_cast<FEVInterface::frontEndMacroFunction_t>(
	                            &ROCTrackerInterface::PrintStatus),
	                        std::vector<std::string>{},
	                        std::vector<std::string>{"Output"},
	                        1,
	                        "" /* tooltip info here */);

  registerFEMacroFunction("Measure Channel Rates",
                          static_cast<FEVInterface::frontEndMacroFunction_t>(
                              &ROCTrackerInterface::MeasureChannelRates),
                          std::vector<std::string>{"TDC Mode"},
                          std::vector<std::string>{"Return Code", "Rates"},
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
	__SET_ARG_OUT__("Return Code", std::to_string(rv));
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
	_roc->ResetDigis();
}

void ROCTrackerInterface::ResetAndConfigure(__ARGS__)
{
	uint16_t tdc_mode     = __GET_ARG_IN__("TDC Mode", uint16_t, 0);
	uint16_t num_lookback = __GET_ARG_IN__("Waveform delay (lookback)", uint16_t, 8);
	uint16_t num_samples  = __GET_ARG_IN__("Additional Sample Packets", uint16_t, 1);
	uint32_t mask_lo      = __GET_ARG_IN__("Channel mask lo", uint32_t, 0xFFFFFFFF);
	uint32_t mask_md      = __GET_ARG_IN__("Channel mask md", uint32_t, 0xFFFFFFFF);
	uint32_t mask_hi      = __GET_ARG_IN__("Channel mask hi", uint32_t, 0xFFFFFFFF);
	int t_start           = __GET_ARG_IN__("TStart (5 ns units)", int, -1);
	int t_stop            = __GET_ARG_IN__("TStop (5 ns units)", int, -1);

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
	__FE_COUT__ << "ROCTrackerInterface::ResetAndConfigure" << __E__;
	auto rv = _roc->ResetAndConfigure(tdc_mode, num_lookback, num_samples,
                                    mask_lo, mask_md, mask_hi,
                                    t_start, t_stop);
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

void ROCTrackerInterface::SetChannelMask(__ARGS__)
{
	uint32_t mask_lo = __GET_ARG_IN__("Mask channels 0-31", uint32_t, 0xFFFFFFFF);
	uint32_t mask_md = __GET_ARG_IN__("Mask channels 32-63", uint32_t, 0xFFFFFFFF);
	uint32_t mask_hi = __GET_ARG_IN__("Mask channels 64-95", uint32_t, 0xFFFFFFFF);

	__FE_COUT__ << std::format(
	    "ROCTrackerInterface::SetChannelMask mask_lo=0x{:08x} mask_md=0x{:08x} mask_hi=0x{:08x}",
	    mask_lo, mask_md, mask_hi) << __E__;
	auto rv = _roc->SetChannelMask(mask_lo, mask_md, mask_hi);
	__SET_ARG_OUT__("Return code", std::to_string(rv));
}

std::map<std::string, std::string> ROCTrackerInterface::ParseNotoriousReadOutput(
    const std::string& text)
{
	auto trim = [](const std::string& s) -> std::string {
		size_t b = s.find_first_not_of(" \t\r\n");
		if(b == std::string::npos)
			return std::string();
		size_t e = s.find_last_not_of(" \t\r\n");
		return s.substr(b, e - b + 1);
	};

	std::map<std::string, std::string> fields;
	std::stringstream                  in(text);
	std::string                        line;
	while(std::getline(in, line))
	{
		// each parsed field is printed as "label : value(s)"
		size_t colon = line.find(':');
		if(colon == std::string::npos)
			continue;
		std::string key   = trim(line.substr(0, colon));
		std::string value = trim(line.substr(colon + 1));
		if(!key.empty())
			fields[key] = value;
	}
	return fields;
}

void ROCTrackerInterface::NotoriousRead(__ARGS__)
{
	uint16_t adc_mode      = __GET_ARG_IN__("ADC Mode", uint16_t, 0);
	uint16_t tdc_mode      = __GET_ARG_IN__("TDC Mode", uint16_t, 0);
	uint16_t num_lookback  = __GET_ARG_IN__("Waveform delay (lookback)", uint16_t, 8);
	uint16_t num_samples   = __GET_ARG_IN__("Additional Sample Packets", uint16_t, 1);
	uint32_t num_triggers  = __GET_ARG_IN__("Trigger count", uint32_t, 0);
	uint32_t mask_lo       = __GET_ARG_IN__("Channel mask lo", uint32_t, 0xFFFFFFFF);
	uint32_t mask_md       = __GET_ARG_IN__("Channel mask md", uint32_t, 0xFFFFFFFF);
	uint32_t mask_hi       = __GET_ARG_IN__("Channel mask hi", uint32_t, 0xFFFFFFFF);
	uint16_t enable_pulser = __GET_ARG_IN__("Enable pulser", uint16_t, 0);
	uint16_t marker_clock  = __GET_ARG_IN__("Fiber clock mask", uint16_t, 3);
	uint16_t mode          = __GET_ARG_IN__("\"Mode\" (deprecated)", uint16_t, 0);
	uint16_t clock         = __GET_ARG_IN__("\"Clock\" (deprecated)", uint16_t, 99);

	std::stringstream stream;
	__FE_COUT__ << "ROCTrackerInterface::NotoriousRead" << __E__;
	_roc->NotoriousRead(adc_mode,
	                    tdc_mode,
	                    num_lookback,
	                    num_samples,
	                    num_triggers,
	                    mask_lo,
	                    mask_md,
	                    mask_hi,
	                    enable_pulser,
	                    marker_clock,
	                    mode,
	                    clock,
	                    stream);

	auto fields = ParseNotoriousReadOutput(stream.str());

	__SET_ARG_OUT__("ADC Mode", fields["adc_mode"]);
	__SET_ARG_OUT__("TDC Mode", fields["tdc_mode"]);
	__SET_ARG_OUT__("Waveform delay (lookback)", fields["num_lookback"]);
	__SET_ARG_OUT__("Trigger count", fields["num_triggers"]);
	__SET_ARG_OUT__("Channel masks", fields["ch_mask"]);
	__SET_ARG_OUT__("Additional Sample Packets", fields["num_samples"]);
	__SET_ARG_OUT__("Enable pulser", fields["enable_pulser"]);
	__SET_ARG_OUT__("Fiber clock mask", fields["marker_clock"]);
	__SET_ARG_OUT__("\"Mode\" (deprecated)", fields["mode"]);
	__SET_ARG_OUT__("\"Clock\" (deprecated)", fields["clock"]);
	__SET_ARG_OUT__("\"digi_read_0xb\"", fields["digi_read_0xb"]);
	__SET_ARG_OUT__("\"digi_read_0xe\"", fields["digi_read_0xe"]);
	__SET_ARG_OUT__("\"digi_read_0xd\"", fields["digi_read_0xd"]);
	__SET_ARG_OUT__("\"digi_read_0xc\"", fields["digi_read_0xc"]);
}

void ROCTrackerInterface::ConfigureDigis(__ARGS__)
{
	uint16_t tdc_mode     = __GET_ARG_IN__("TDC Mode", uint16_t, 0);
	uint16_t num_lookback = __GET_ARG_IN__("Waveform delay (lookback)", uint16_t, 8);
	uint16_t num_samples  = __GET_ARG_IN__("Additional Sample Packets", uint16_t, 1);
	uint32_t mask_lo      = __GET_ARG_IN__("Channel mask lo", uint32_t, 0xFFFFFFFF);
	uint32_t mask_md      = __GET_ARG_IN__("Channel mask md", uint32_t, 0xFFFFFFFF);
	uint32_t mask_hi      = __GET_ARG_IN__("Channel mask hi", uint32_t, 0xFFFFFFFF);

	trkdaq::NullStream null;
	auto               stream = std::ostream(&null);

	__FE_COUT__ << "ROCTrackerInterface::ConfigureDigis" << __E__;
	_roc->ConfigureDigis(tdc_mode, num_lookback, num_samples, mask_lo, mask_md, mask_hi, stream);
}

void ROCTrackerInterface::InitializeDigis(__ARGS__)
{
	__FE_COUT__ << "ROCTrackerInterface::InitializeDigis" << __E__;
	auto words = _roc->InitializeDigis();

	std::stringstream stream;
	for(size_t i = 0; i < words.size(); ++i)
		stream << std::format("0x{:04x}\n", words[i]);
	__SET_ARG_OUT__("Output", stream.str());
}

void ROCTrackerInterface::DigiRW(__ARGS__)
{
	uint16_t rw      = __GET_ARG_IN__("Read/Write", uint16_t, 0);
	uint16_t hv_cal  = __GET_ARG_IN__("HvCal", uint16_t, 0);
	uint16_t address = __GET_ARG_IN__("Address", uint16_t, 0);
	uint32_t data    = __GET_ARG_IN__("Data", uint32_t, 0);

	std::stringstream stream;
	__FE_COUT__ << "ROCTrackerInterface::DigiRW" << __E__;
	_roc->DigiRW(rw, hv_cal, address, data, stream);
	__SET_ARG_OUT__("Output", stream.str());
}

void ROCTrackerInterface::PrintStatus(__ARGS__)
{
	std::stringstream stream;
	__FE_COUT__ << "ROCTrackerInterface::PrintStatus" << __E__;
	_roc->PrintStatus(1, stream);
	__SET_ARG_OUT__("Output", stream.str());
}

std::string ROCTrackerInterface::FormatRatesTable(
    const std::vector<trkdaq::ROC::rates_t>& rates)
{
	std::stringstream table;
	table << std::endl;
	table << std::format(" {:>7} {:>11} {:>11} {:>11}\n", "Channel", "Cal", "HV", "Coinc.");
	table << "--------------------------------------------\n";
	for(int channel = 0; channel < 96; ++channel)
	{
    auto channel_rates = rates[channel];
		float hiv = std::get<0>(channel_rates);
		float cal = std::get<1>(channel_rates);
		float coi = std::get<2>(channel_rates);
		table << std::format(
		    " {:4d} {:11.3f} {:11.3f} {:11.3f}\n", channel, cal, hiv, coi);
	}
	return table.str();
}

void ROCTrackerInterface::MeasureChannelRates(__ARGS__)
{
  uint16_t tdc_mode = __GET_ARG_IN__("TDC Mode", uint16_t, 0);

  __FE_COUT__ << "ROCTrackerInterface::MeasureChannelRates" << __E__;
  std::vector<trkdaq::ROC::rates_t> rates;
  auto rv = _roc->ChannelRates(tdc_mode, rates);
  __SET_ARG_OUT__("Return Code", std::to_string(rv));
  __SET_ARG_OUT__("Rates", FormatRatesTable(rates));
}

/* --- */

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

DEFINE_OTS_INTERFACE(ROCTrackerInterface)
