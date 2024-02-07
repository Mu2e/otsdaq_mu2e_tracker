// For an explanation of this class, look at its header,
// ToySim.hh, as well as
// https://cdcvs.fnal.gov/redmine/projects/artdaq-demo/wiki/Fragments_and_FragmentGenerators_w_Toy_Fragments_as_Examples

#include "otsdaq-mu2e-tracker/Generators/ToySim.hh"
#include "artdaq/DAQdata/Globals.hh"

#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"

#include "artdaq-core-demo/Overlays/FragmentType.hh"
#include "artdaq-core-demo/Overlays/ToyFragment.hh"
#include "artdaq-core/Utilities/SimpleLookupPolicy.hh"
#include "artdaq/Generators/GeneratorMacros.hh"

#define TRACE_NAME "ToySim"
#include "TRACE/tracemf.h"  // TRACE, TLOG*

#include <unistd.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>

demo::ToySim::ToySim(fhicl::ParameterSet const& ps)
    : CommandableFragmentGenerator(ps)
    , hardware_interface_(new ToyHW(ps))
    , timestamp_(0)
    , starting_timestamp_(0)
    , timestampScale_(ps.get<int>("timestamp_scale_factor", 1))
    , sequence_id_scale_(ps.get<size_t>("sequence_scale_factor", 1))
    , initial_sequence_id_(ps.get<size_t>("initial_sequence_id", 1))
    , rollover_subrun_interval_(ps.get<int>("rollover_subrun_interval", 0))
    , metadata_({0, 0, 0})
    , readout_buffer_(nullptr)
    , fragment_type_(static_cast<decltype(fragment_type_)>(artdaq::Fragment::InvalidFragmentType))
    , distribution_type_(static_cast<ToyHW::DistributionType>(ps.get<int>("distribution_type")))
    , generated_fragments_per_event_(ps.get<int>("generated_fragments_per_event", 1))
    , exception_on_config_(ps.get<bool>("exception_on_config", false))
    , dies_on_config_(ps.get<bool>("dies_on_config", false))
    , lazy_mode_(ps.get<bool>("lazy_mode", false))

{
	hardware_interface_->AllocateReadoutBuffer(&readout_buffer_);

	auto ts = ps.get<int>("starting_timestamp", 0);
	if (ts < 0) { starting_timestamp_ = artdaq::Fragment::InvalidTimestamp; }
	else
	{
		starting_timestamp_ = static_cast<artdaq::Fragment::timestamp_t>(ts);
	}
	timestamp_ = starting_timestamp_;

	if (exception_on_config_)
	{
		throw cet::exception("ToySim") << "This is an engineered exception designed for testing purposes, set "  // NOLINT(cert-err60-cpp)
		                                        "by the exception_on_config FHiCL variable";
	}
	if (dies_on_config_)
	{
		TLOG(TLVL_ERROR) << "This is an engineered process death, set by the dies_on_config FHiCL variable";
		std::exit(1);
	}

	metadata_.board_serial_number = hardware_interface_->SerialNumber() & 0xFFFF;
	metadata_.num_adc_bits = hardware_interface_->NumADCBits();
	TLOG(TLVL_INFO) << "Constructor: metadata_.unused = 0x" << std::hex << metadata_.unused
	                << " sizeof(metadata_) = " << std::dec << sizeof(metadata_);

	switch (hardware_interface_->BoardType())
	{
		case 1002:
			fragment_type_ = toFragmentType("TOY1");
			break;
		case 1003:
			fragment_type_ = toFragmentType("TOY2");
			break;
		default:
			throw cet::exception("ToySim") << "Unable to determine board type supplied by hardware";  // NOLINT(cert-err60-cpp)
	}
}

demo::ToySim::~ToySim() { hardware_interface_->FreeReadoutBuffer(readout_buffer_); }

bool demo::ToySim::getNext_(artdaq::FragmentPtrs& frags)
{
	if (should_stop())
	{
		return false;
	}


	TLOG(TLVL_DEBUG + 3) << "getNext_: Calling ToyHW::FillBuffer";
	std::size_t bytes_read = 0;
	hardware_interface_->FillBuffer(readout_buffer_, &bytes_read);
	TLOG(TLVL_DEBUG + 3) << "getNext_: Done with FillBuffer";

	// We'll use the static factory function

	// artdaq::Fragment::FragmentBytes(std::size_t payload_size_in_bytes, sequence_id_t sequence_id,
	//  fragment_id_t fragment_id, type_t type, const T & metadata)

	// which will then return a unique_ptr to an artdaq::Fragment
	// object.

	TLOG(TLVL_DEBUG + 3) << "getNext_: Creating Fragments for configured Fragment IDs";
	for (auto& id : fragmentIDs()) {
		// The offset logic below is designed to both ensure
		// backwards compatibility and to (help) avoid collisions
		// with fragment_ids from other boardreaders if more than
		// one fragment is generated per event

		std::unique_ptr<artdaq::Fragment> fragptr(
		    artdaq::Fragment::FragmentBytes(bytes_read, ev_counter(), id, fragment_type_, metadata_, timestamp_));
		frags.emplace_back(std::move(fragptr));

		TLOG(TLVL_DEBUG + 4) << "getNext_: Before memcpy";
		if (distribution_type_ != ToyHW::DistributionType::uninitialized) {
			memcpy(frags.back()->dataBeginBytes(), readout_buffer_, bytes_read);
		}
		else {
			// Must preserve the Header!
			memcpy(frags.back()->dataBeginBytes(), readout_buffer_, sizeof(ToyFragment::Header));
		}

		TLOG(TLVL_DEBUG + 4) << "getNext_ after memcpy " << bytes_read
		                     << " bytes and std::move dataSizeBytes()=" << frags.back()->sizeBytes()
		                     << " metabytes=" << sizeof(metadata_);
	}

	if (metricMan != nullptr) {
		metricMan->sendMetric("Fragments Sent", ev_counter(), "Events", 3, artdaq::MetricMode::LastPoint);
	}

	TLOG(TLVL_DEBUG + 3) << "getNext_: Checking for subrun rollover";
	if (rollover_subrun_interval_ > 0 && ev_counter() % rollover_subrun_interval_ == 0 && fragment_id() == 0)	{
		bool fragmentIdZero = false;
		for (auto& id : fragmentIDs())
		{
			if (id == 0)
			{
				fragmentIdZero = true;
			}
		}
		if (fragmentIdZero)
		{
			artdaq::FragmentPtr endOfSubrunFrag(new artdaq::Fragment(static_cast<size_t>(
			    ceil(sizeof(my_rank) / static_cast<double>(sizeof(artdaq::Fragment::value_type))))));
			endOfSubrunFrag->setSystemType(artdaq::Fragment::EndOfSubrunFragmentType);

			endOfSubrunFrag->setSequenceID(ev_counter() + 1);
			endOfSubrunFrag->setTimestamp(1 + (ev_counter() / rollover_subrun_interval_));

			*endOfSubrunFrag->dataBegin() = my_rank;
			frags.emplace_back(std::move(endOfSubrunFrag));
		}
	}

	ev_counter_inc(sequence_id_scale_);
	timestamp_ += timestampScale_;

	TLOG(TLVL_DEBUG + 3) << "getNext_: DONE";
	return true;
}

void demo::ToySim::start()
{
	hardware_interface_->StartDatataking();
	while (ev_counter() < initial_sequence_id_)
	{
		ev_counter_inc();
	}
	timestamp_ = starting_timestamp_;
	lazily_handled_requests_.clear();
}

void demo::ToySim::stop() { hardware_interface_->StopDatataking(); }

// The following macro is defined in artdaq's GeneratorMacros.hh header
DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(demo::ToySim)
