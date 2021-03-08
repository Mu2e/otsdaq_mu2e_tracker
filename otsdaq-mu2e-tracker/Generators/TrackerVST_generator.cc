
#include "artdaq/DAQdata/Globals.hh"
#define TRACE_NAME (app_name + "_TrackerVST").c_str()

#include "otsdaq-mu2e-tracker/Generators/TrackerVST.hh"

#include "canvas/Utilities/Exception.h"

#include "artdaq-core/Utilities/SimpleLookupPolicy.hh"
#include "artdaq/Generators/GeneratorMacros.hh"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "mu2e-artdaq-core/Overlays/FragmentType.hh"
#include "mu2e-artdaq-core/Overlays/mu2eFragment.hh"
#include "mu2e-artdaq-core/Overlays/mu2eFragmentWriter.hh"

#include <fstream>
#include <iomanip>
#include <iterator>

#include <unistd.h>

//-----------------------------------------------------------------------------
mu2e::TrackerVST::TrackerVST(fhicl::ParameterSet const& ps) : 
	CommandableFragmentGenerator(ps)
	, fragment_type_   (toFragmentType("MU2E"))
	, fragment_ids_    {static_cast<artdaq::Fragment::fragment_id_t>(fragment_id())}
	, timestamps_read_ (0)
	, lastReportTime_  (std::chrono::steady_clock::now())
	, mode_            (DTCLib::DTC_SimModeConverter::ConvertToSimMode(ps.get<std::string>("sim_mode", "Disabled")))
	, board_id _       (static_cast<uint8_t>(ps.get<int>("board_id", 0)))
	, rawOutput_       (ps.get<bool>("raw_output_enable", false))
	, rawOutputFile_   (ps.get<std::string>("raw_output_file", "/tmp/TrackerVST.bin"))
	, nSkip_           (ps.get<size_t>("fragment_receiver_count", 1))
	, sendEmpties_     (ps.get<bool>("send_empty_fragments", false))
	, verbose_         (ps.get<bool>("verbose", false))
	, nEvents_         (ps.get<size_t>("number_of_events_to_generate"  ,    -1))
	, request_delay_   (ps.get<size_t>("delay_between_requests_ticks"  , 20000))
	, heartbeats_after_(ps.get<size_t>("null_heartbeats_after_requests",    16)) 
	, dtc_id_          (ps.get<int>   ("dtc_id"                        ,    -1)) 
	, roc_mask_        (ps.get<int>   ("roc_mask"                      ,     0)) 
	{

	TLOG(TLVL_DEBUG) << "TrackerVST_generator CONSTRUCTOR";
		// mode_ can still be overridden by environment!

	theInterface_ = new DTCLib::DTC(mode_,dtc_id_,roc_mask_,
																	"", 
																	false, 
																	ps.get<std::string>("simulator_memory_file_name","mu2esim.bin"));

	fhicl::ParameterSet cfoConfig = ps.get<fhicl::ParameterSet>("cfo_config", fhicl::ParameterSet());

	theCFO_ = new DTCLib::DTCSoftwareCFO(theInterface_, 
																			 cfoConfig.get<bool>("use_dtc_cfo_emulator", true), 
																			 cfoConfig.get<size_t>("debug_packet_count", 0), 
																			 DTCLib::DTC_DebugTypeConverter::ConvertToDebugType(cfoConfig.get<std::string>("debug_type", "2")), 
																			 cfoConfig.get<bool>("sticky_debug_type", false), 
																			 cfoConfig.get<bool>("quiet", false), 
																			 cfoConfig.get<bool>("asyncRR", false), 
																			 cfoConfig.get<bool>("force_no_debug_mode", false), 
																			 cfoConfig.get<bool>("useCFODRP", false));
	mode_ = theInterface_->ReadSimMode();

	TLOG(TLVL_INFO) << "The DTC Firmware version string is: " << theInterface_->ReadDesignVersion();

	if (ps.get<bool>("load_sim_file", false)) {
		theInterface_->SetDetectorEmulatorInUse();
		theInterface_->ResetDDR();

		resetDTC();

		char* file_c = getenv("DTCLIB_SIM_FILE");

		auto sim_file = ps.get<std::string>("sim_file", "");
		if (file_c != nullptr) {
			sim_file = std::string(file_c);
		}
		if (sim_file.size() > 0) {
			simFileRead_ = false;
			std::thread reader(&mu2e::TrackerVST::readSimFile_, this, sim_file);
			reader.detach();
		}
	}
	else {
		theInterface_->ClearDetectorEmulatorInUse();  // Needed if we're doing ROC Emulator...make sure Detector Emulation
													  // is disabled
		simFileRead_ = true;
	}

	if (rawOutput_) rawOutputStream_.open(rawOutputFile_, std::ios::out | std::ios::app | std::ios::binary);
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST::readSimFile_(std::string sim_file) {
	TLOG(TLVL_INFO) << "Starting read of simulation file " << sim_file << "."
							  << " Please wait to start the run until finished.";
	theInterface_->WriteSimFileToDTC(sim_file, true, true);
	simFileRead_ = true;
	TLOG(TLVL_INFO) << "Done reading simulation file into DTC memory.";
}

//-----------------------------------------------------------------------------
mu2e::TrackerVST::~TrackerVST() {
	rawOutputStream_.close();
	delete theCFO_;
	delete theInterface_;
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST::stop() {
	theInterface_->DisableDetectorEmulator();
	theInterface_->DisableCFOEmulation();
}

//-----------------------------------------------------------------------------
bool mu2e::TrackerVST::getNext_(artdaq::FragmentPtrs& frags) {
	const char* oname = "mu2e::TrackerVST::getNext_: ";

	TLOG(TLVL_DEBUG) << oname << "STARTING";

	while (!simFileRead_ and !should_stop()) {
		usleep(5000);
	}

	if (should_stop() or ev_counter() > nEvents_) return false;

	if (sendEmpties_) {
		int mod = ev_counter() % nSkip_;
		if (mod == board_id_ || (mod == 0 && board_id_ == nSkip_)) {
			// TLOG(TLVL_DEBUG) << "Sending Data  Fragment for sequence id " << ev_counter() << " (board_id " <<
			// std::to_string(board_id_) << ")" ;
		}
		else {
			// TLOG(TLVL_DEBUG) << "Sending Empty Fragment for sequence id " << ev_counter() << " (board_id " <<
			// std::to_string(board_id_) << ")" ;
			return sendEmpty_(frags);
		}
	}

	_startProcTimer();

	TLOG(TLVL_DEBUG) << oname << "after startProcTimer";

	TLOG(TLVL_TRACE + 5) << oname << "Starting CFO thread";
	uint64_t z = 0;
	DTCLib::DTC_EventWindowTag zero(z);
	if (mode_ != 0) {
#if 0
		//theInterface_->ReleaseAllBuffers();
		TLOG(TLVL_DEBUG) << "Sending requests for " << mu2e::BLOCK_COUNT_MAX << " timestamps, starting at " << mu2e::BLOCK_COUNT_MAX * (ev_counter() - 1);
		theCFO_->SendRequestsForRange(mu2e::BLOCK_COUNT_MAX, DTCLib::DTC_EventWindowTag(mu2e::BLOCK_COUNT_MAX * (ev_counter() - 1)));
#else
		int loc = mu2e::BLOCK_COUNT_MAX * (ev_counter() - 1);
		TLOG(TLVL_DEBUG) << "pasha Sending requests for " << mu2e::BLOCK_COUNT_MAX 
										 << " timestamps, starting at loc=" << loc;

		theCFO_->SendRequestsForRange(-1, 
																	DTCLib::DTC_EventWindowTag(loc), 
																	true, request_delay_, 1, heartbeats_after_);
#endif
	}

	theInterface_->WriteROCRegister(DTCLib::DTC_Link_2,11,1,true);

	DTCLib::roc_data_t r11 = theInterface_->ReadROCRegister(DTCLib::DTC_Link_2, 11);
	TLOG(TLVL_DEBUG) << "pasha reading regster 11: value=" << r11 ; 
	

	TLOG(TLVL_TRACE + 5) << oname << "Initializing mu2eFragment metadata";
	mu2eFragment::Metadata metadata;
	metadata.sim_mode   = static_cast<int>(mode_);
	metadata.run_number = run_number();
	metadata.board_id   = board_id_;
//-----------------------------------------------------------------------------
// And use it, along with the artdaq::Fragment header information
// (fragment id, sequence id, and user type) to create a fragment
//-----------------------------------------------------------------------------
	TLOG(TLVL_TRACE + 5) << oname << "Creating new mu2eFragment!";
	frags.emplace_back(new artdaq::Fragment(0, ev_counter(), fragment_ids_[0], fragment_type_, metadata));

	// Now we make an instance of the overlay to put the data into...
	TLOG(TLVL_TRACE + 5) << oname << "Making mu2eFragmentWriter";
	mu2eFragmentWriter newfrag(*frags.back());

	TLOG(TLVL_TRACE + 5) << oname << "Reserving space for 16 * 201 * BLOCK_COUNT_MAX bytes";
	newfrag.addSpace(mu2e::BLOCK_COUNT_MAX * 16 * 201);

	// Get data from Mu2eReceiver
	TLOG(TLVL_TRACE + 5) << oname << "Starting DTCFragment Loop";
	theInterface_->GetDevice()->ResetDeviceTime();
	size_t totalSize = 0;
	bool first = true;
	while (newfrag.hdr_block_count() < mu2e::BLOCK_COUNT_MAX) {
		if (should_stop()) {
			break;
		}

		TLOG(TLVL_TRACE + 5) << oname << "Getting DTC Data for block " << newfrag.hdr_block_count() << "/" << mu2e::BLOCK_COUNT_MAX
							 << ", sz=" << totalSize;
		std::vector<std::unique_ptr<DTCLib::DTC_Event>> data;
		int retryCount = 5;

		while (data.size() == 0 && retryCount >= 0) {
			try {
				//				TLOG(TLVL_TRACE + 10) << oname << "Calling theInterface->GetData(zero)";
				TLOG(TLVL_DEBUG) << oname << "Calling theInterface->GetData(zero)";
				data = theInterface_->GetData(zero);
				//				TLOG(TLVL_TRACE + 10) << oname << "Done calling theInterface->GetData(zero)";
				TLOG(TLVL_DEBUG) << oname << "Done calling theInterface->GetData(zero)";
			}
			catch (std::exception const& ex) {
				TLOG(TLVL_ERROR) << "There was an error in the DTC Library: " << ex.what();
			}
			retryCount--;
			// if (data.size() == 0){usleep(10000);}
		}

		if (retryCount < 0 && data.size() == 0) {
			TLOG(TLVL_DEBUG) << oname << "Retry count exceeded. Something is very wrong indeed";
			TLOG(TLVL_ERROR) << oname << "Had an error with block " << newfrag.hdr_block_count() << " of event "
							 << ev_counter();
			if (newfrag.hdr_block_count() == 0) {
				throw cet::exception("DTC Retry count exceeded in first block of Event. Probably something is very wrong, aborting");
			}
			break;
		}

		TLOG(TLVL_TRACE + 6) << "Allocating space in Mu2eFragment for DTC packets";
		totalSize = 0;
		for (size_t i = 0; i < data.size(); ++i) {
			totalSize += data[i]->GetEventByteCount();
		}

		int64_t diff = totalSize + newfrag.dataEndBytes() - newfrag.dataSize();
		TLOG(TLVL_TRACE + 7) << "diff=" << diff << ", totalSize=" << totalSize << ", dataSize=" << newfrag.dataEndBytes()
							 << ", fragSize=" << newfrag.dataSize();
		if (diff > 0) {
			auto currSize = newfrag.dataSize();
			auto remaining = 1.0 - (newfrag.hdr_block_count() / static_cast<double>(BLOCK_COUNT_MAX));

			auto newSize = static_cast<size_t>(currSize * remaining);
			TLOG(TLVL_TRACE + 8) << "mu2eReceiver::getNext: " << totalSize << " + " << newfrag.dataEndBytes() << " > "
								 << newfrag.dataSize() << ", allocating space for " << newSize + diff << " more bytes";
			newfrag.addSpace(diff + newSize);
		}

		auto tag = data[0]->GetEventWindowTag();

		auto fragment_timestamp = tag.GetEventWindowTag(true);
		if (fragment_timestamp > highest_timestamp_seen_) {
			highest_timestamp_seen_ = fragment_timestamp;
		}

		if (first) {
			first = false;
			if (fragment_timestamp < highest_timestamp_seen_) {
				if (fragment_timestamp == 0) { timestamp_loops_++; }
				// Timestamps start at 0, so make sure to offset by one so we don't repeat highest_timestamp_seen_
				fragment_timestamp += timestamp_loops_ * (highest_timestamp_seen_ + 1);
			}
			frags.back()->setTimestamp(fragment_timestamp);
		}

		TLOG(TLVL_TRACE + 9) << "Copying DTC packets into Mu2eFragment";
		// auto offset = newfrag.dataBegin() + newfrag.blockSizeBytes();
		size_t offset = newfrag.dataEndBytes();
		for (size_t i = 0; i < data.size(); ++i) {
			auto begin = reinterpret_cast<const uint8_t*>(data[i]->GetRawBufferPointer());
			auto size = data[i]->GetEventByteCount();

			while (data[i + 1]->GetRawBufferPointer() == static_cast<const void*>(begin + size)) {
				size += data[++i]->GetEventByteCount();
			}

			TLOG(TLVL_TRACE + 11) << "Copying data from " << begin << " to "
								  << reinterpret_cast<void*>(newfrag.dataAtBytes(offset)) << " (sz=" << size << ")";
			// memcpy(reinterpret_cast<void*>(offset + intraBlockOffset), data[i].blockPointer, data[i].byteSize);
			memcpy(newfrag.dataAtBytes(offset), begin, size);
			TLOG(TLVL_TRACE + 11) << "Done with copy";
			//std::copy(begin, reinterpret_cast<const uint8_t*>(begin) + size, newfrag.dataAtBytes(offset));
			if (rawOutput_) rawOutputStream_.write((char*)begin, size);
			offset += size;
		}

		TLOG(TLVL_TRACE + 12) << oname << "Ending SubEvt " << newfrag.hdr_block_count();
		newfrag.endSubEvt(offset - newfrag.dataEndBytes());
	}

	TLOG(TLVL_TRACE + 5) << oname << "Incrementing event counter";
	ev_counter_inc();

	TLOG(TLVL_TRACE + 5) << oname << "Reporting Metrics";
	timestamps_read_ += newfrag.hdr_block_count();
	auto hwTime = theInterface_->GetDevice()->GetDeviceTime();

	double processing_rate = newfrag.hdr_block_count() / _getProcTimerCount();
	double timestamp_rate = newfrag.hdr_block_count() / _timeSinceLastSend();
	double hw_timestamp_rate = newfrag.hdr_block_count() / hwTime;
	double hw_data_rate = newfrag.dataEndBytes() / hwTime;

	metricMan->sendMetric("Timestamp Count", timestamps_read_, "timestamps", 1, artdaq::MetricMode::LastPoint);
	metricMan->sendMetric("Timestamp Rate", timestamp_rate, "timestamps/s", 1, artdaq::MetricMode::Average);
	metricMan->sendMetric("Generator Timestamp Rate", processing_rate, "timestamps/s", 1, artdaq::MetricMode::Average);
	metricMan->sendMetric("HW Timestamp Rate", hw_timestamp_rate, "timestamps/s", 1, artdaq::MetricMode::Average);
	metricMan->sendMetric("PCIe Transfer Rate", hw_data_rate, "B/s", 1, artdaq::MetricMode::Average);

	TLOG(TLVL_TRACE + 5) << oname << "Returning true";
	return true;
}
//-----------------------------------------------------------------------------
// old version
//-----------------------------------------------------------------------------
// 	while (!simFileRead_ && !should_stop()) {
// 		usleep(5000);
// 	}

// 	if (should_stop() or (ev_counter() > nEvents_)) return false;

// 	if (sendEmpties_) {
// 		int mod = ev_counter() % nSkip_;
// 		if (mod == board_id_ or (mod == 0 and board_id_ == nSkip_)) {
// 			// TLOG(TLVL_DEBUG) << "Sending Data  Fragment for sequence id " << ev_counter() << " (board_id " <<
// 			// std::to_string(board_id_) << ")" ;
// 		}
// 		else {
// 			// TLOG(TLVL_DEBUG) << "Sending Empty Fragment for sequence id " << ev_counter() << " (board_id " <<
// 			// std::to_string(board_id_) << ")" ;
// 			return sendEmpty_(frags);
// 		}
// 	}

// 	_startProcTimer();
// 	TLOG(TLVL_TRACE +5) << "mu2eReceiver::getNext: Starting CFO thread";
// 	uint64_t z = 0;
// 	DTCLib::DTC_EventWindowTag zero(z);
// 	if (mode_ != 0) {
// #if 0
// 		//theInterface_->ReleaseAllBuffers();
// 		TLOG(TLVL_DEBUG) << "Sending requests for " << mu2e::BLOCK_COUNT_MAX << " timestamps, starting at " << mu2e::BLOCK_COUNT_MAX * (ev_counter() - 1);
// 		theCFO_->SendRequestsForRange(mu2e::BLOCK_COUNT_MAX, DTCLib::DTC_EventWindowTag(mu2e::BLOCK_COUNT_MAX * (ev_counter() - 1)));
// #else
// 		theCFO_->SendRequestsForRange(-1, DTCLib::DTC_EventWindowTag(mu2e::BLOCK_COUNT_MAX * (ev_counter() - 1)), true, request_delay_, 1, heartbeats_after_);
// #endif
// 	}

// 	TLOG(TLVL_TRACE +5) << "mu2eReceiver::getNext: Initializing mu2eFragment metadata";
// 	mu2eFragment::Metadata metadata;
// 	metadata.sim_mode   = static_cast<int>(mode_);
// 	metadata.run_number = run_number();
// 	metadata.board_id   = board_id_;

// 	// And use it, along with the artdaq::Fragment header information
// 	// (fragment id, sequence id, and user type) to create a fragment
// 	TLOG(TLVL_TRACE +5) << "mu2eReceiver::getNext: Creating new mu2eFragment!";
// 	frags.emplace_back(new artdaq::Fragment(0, ev_counter(), fragment_ids_[0], fragment_type_, metadata));
// 	// Now we make an instance of the overlay to put the data into...
// 	TLOG(TLVL_TRACE +5) << "mu2eReceiver::getNext: Making mu2eFragmentWriter";
// 	mu2eFragmentWriter newfrag(*frags.back());

// 	TLOG(TLVL_TRACE +5) << "mu2eReceiver::getNext: Reserving space for 16 * 201 * BLOCK_COUNT_MAX bytes";
// 	newfrag.addSpace(mu2e::BLOCK_COUNT_MAX * 16 * 201);

// 	// Get data from TrackerVST
// 	TLOG(TLVL_TRACE +5) << "mu2eReceiver::getNext: Starting DTCFragment Loop";
// 	theInterface_->GetDevice()->ResetDeviceTime();
// 	size_t totalSize = 0;
// 	bool first = true;
// 	while (newfrag.hdr_block_count() < mu2e::BLOCK_COUNT_MAX) {
// 		if (should_stop()) {
// 			break;
// 		}

// 		TLOG(TLVL_TRACE +5) << "Getting DTC Data for block " << newfrag.hdr_block_count()   
// 												<< "/"                           << mu2e::BLOCK_COUNT_MAX
// 												<< ", sz="                       << totalSize;

// 		std::vector<std::unique_ptr<DTCLib::DTC_Event>> data;
// 		int retryCount = 5;
// 		while (data.size() == 0 && retryCount >= 0) {
// 			try {
// 				TLOG(TLVL_TRACE +10) << "Calling theInterface->GetData(zero)";
// 				data = theInterface_->GetData(zero);
// 				TLOG(TLVL_TRACE +10) << "Done calling theInterface->GetData(zero)";
// 			}
// 			catch (std::exception const& ex) {
// 				TLOG(TLVL_ERROR) << "There was an error in the DTC Library: " << ex.what();
// 			}
// 			retryCount--;
// 			// if (data.size() == 0){usleep(10000);}
// 		}
// 		if (retryCount < 0 && data.size() == 0) {
// 			TLOG(TLVL_DEBUG) << "Retry count exceeded. Something is very wrong indeed";
// 			TLOG(TLVL_ERROR) << "Had an error with block " << newfrag.hdr_block_count() << " of event " << ev_counter();
// 			if (newfrag.hdr_block_count() == 0) {
// 				throw cet::exception("DTC Retry count exceeded in first block of Event. Probably something is very wrong, aborting");
// 			}
// 			break;
// 		}

// 		TLOG(TLVL_TRACE +6) << "Allocating space in Mu2eFragment for DTC packets";
// 		totalSize = 0;
// 		for (size_t i = 0; i < data.size(); ++i) {
// 		  totalSize += data[i]->GetEventByteCount() ; // byteSize;
// 		}

// 		int64_t diff = totalSize + newfrag.dataEndBytes() - newfrag.dataSize();
// 		TLOG(TLVL_TRACE +7) << "diff=" << diff << ", totalSize=" << totalSize << ", dataSize=" << newfrag.dataEndBytes()
// 				 << ", fragSize=" << newfrag.dataSize();
// 		if (diff > 0) {
// 			auto currSize  = newfrag.dataSize();
// 			auto remaining = 1.0 - (newfrag.hdr_block_count() / static_cast<double>(BLOCK_COUNT_MAX));

// 			auto newSize   = static_cast<size_t>(currSize * remaining);
// 			TLOG(TLVL_TRACE +8) << "mu2eReceiver::getNext: " << totalSize << " + " << newfrag.dataEndBytes() << " > "
// 													<< newfrag.dataSize() << ", allocating space for " << newSize + diff << " more bytes";
// 			newfrag.addSpace(diff + newSize);
// 		}

// 		TLOG(TLVL_TRACE +9) << "Copying DTC packets into Mu2eFragment";
// 		// auto offset = newfrag.dataBegin() + newfrag.blockSizeBytes();
// 		size_t offset = newfrag.dataEndBytes();
// 		for (size_t i = 0; i < data.size(); ++i) {
// 			auto dp  = DTCLib::DTC_DataPacket(data[i]->GetRawBufferPointer());
// 			auto dhp = DTCLib::DTC_DataHeaderPacket(dp);
// 			TLOG(TLVL_TRACE +12) << "Placing DataBlock with timestamp "
// 					 << static_cast<double>(dhp.GetEventWindowTag()) << " into Mu2eFragment";

// 			auto fragment_timestamp = dhp.GetEventWindowTag();
// 			if (fragment_timestamp > highest_timestamp_seen_) {
// 				highest_timestamp_seen_ = fragment_timestamp;
// 			}

// 			if (first) {
// 				first = false;
// 				if (fragment_timestamp < highest_timestamp_seen_) {
// 					if (fragment_timestamp == 0) { timestamp_loops_++; }
// 					// Timestamps start at 0, so make sure to offset by one so we don't repeat highest_timestamp_seen_
// 					fragment_timestamp += timestamp_loops_ * (highest_timestamp_seen_ + 1);
// 				}
// 				frags.back()->setTimestamp(fragment_timestamp);
// 			}

// 			auto begin = reinterpret_cast<const uint8_t*>(data[i]->GetRawBufferPointer());
// 			auto size = data[i]->GetEventByteCount();

// 			while (data[i + 1]->GetRawBufferPointer() == static_cast<const void*>(begin + size)) {
// 				size += data[++i]->GetEventByteCount();
// 			}

// 			TLOG(TLVL_TRACE +11) << "Copying data from " << begin << " to "
// 					 << reinterpret_cast<void*>(newfrag.dataAtBytes(offset)) << " (sz=" << size << ")";
// 			// memcpy(reinterpret_cast<void*>(offset + intraBlockOffset), data[i].blockPointer, data[i].byteSize);
// 			memcpy(newfrag.dataAtBytes(offset), begin, size);
// 			TLOG(TLVL_TRACE +11) << "Done with copy";
// 			//std::copy(begin, reinterpret_cast<const uint8_t*>(begin) + size, newfrag.dataAtBytes(offset));
// 			if (rawOutput_) rawOutputStream_.write((char*)begin, size);
// 			offset += size;
// 		}

// 		TLOG(TLVL_TRACE +12) << "Ending SubEvt " << newfrag.hdr_block_count();
// 		newfrag.endSubEvt(offset - newfrag.dataEndBytes());
// 	}
// 	TLOG(TLVL_TRACE +5) << "Incrementing event counter";
// 	ev_counter_inc();

// 	TLOG(TLVL_TRACE +5) << "Reporting Metrics";
// 	timestamps_read_ += newfrag.hdr_block_count();
// 	auto hwTime = theInterface_->GetDevice()->GetDeviceTime();

// 	double processing_rate = newfrag.hdr_block_count() / _getProcTimerCount();
// 	double timestamp_rate = newfrag.hdr_block_count() / _timeSinceLastSend();
// 	double hw_timestamp_rate = newfrag.hdr_block_count() / hwTime;
// 	double hw_data_rate = newfrag.dataEndBytes() / hwTime;

// 	metricMan->sendMetric("Timestamp Count", timestamps_read_, "timestamps", 1, artdaq::MetricMode::LastPoint);
// 	metricMan->sendMetric("Timestamp Rate", timestamp_rate, "timestamps/s", 1, artdaq::MetricMode::Average);
// 	metricMan->sendMetric("Generator Timestamp Rate", processing_rate, "timestamps/s", 1, artdaq::MetricMode::Average);
// 	metricMan->sendMetric("HW Timestamp Rate", hw_timestamp_rate, "timestamps/s", 1, artdaq::MetricMode::Average);
// 	metricMan->sendMetric("PCIe Transfer Rate", hw_data_rate, "B/s", 1, artdaq::MetricMode::Average);

// 	TLOG(TLVL_TRACE +5) << "Returning true";
// 	return true;
//}

//-----------------------------------------------------------------------------
 bool mu2e::TrackerVST::sendEmpty_(artdaq::FragmentPtrs& frags) {
	frags.emplace_back(new artdaq::Fragment());
	frags.back()->setSystemType(artdaq::Fragment::EmptyFragmentType);
	frags.back()->setSequenceID(ev_counter());
	frags.back()->setFragmentID(fragment_ids_[0]);
	ev_counter_inc();
	return true;
}

// The following macro is defined in artdaq's GeneratorMacros.hh header
DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(mu2e::TrackerVST)
