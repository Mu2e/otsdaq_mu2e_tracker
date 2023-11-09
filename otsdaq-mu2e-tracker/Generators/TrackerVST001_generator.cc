///////////////////////////////////////////////////////////////////////////////
// P.Murat: test version, implements multiple DTC read tests
// test2: test_buffer in async mode
// test3: test_buffer in sync  mode
// test4: read_digi
///////////////////////////////////////////////////////////////////////////////
#include "artdaq/DAQdata/Globals.hh"

#define TRACE_NAME (app_name + "_TrackerVST001").c_str()

#include "canvas/Utilities/Exception.h"

#include "artdaq-core/Utilities/SimpleLookupPolicy.hh"
#include "artdaq/Generators/GeneratorMacros.hh"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "artdaq-core-mu2e/Overlays/FragmentType.hh"
#include "artdaq-core-mu2e/Data/DTCDataDecoder.hh"
// #include "artdaq-core-mu2e/Overlays/mu2eFragmentWriter.hh"

#include <fstream>
#include <iomanip>
#include <iterator>
#include <thread>
#include <chrono>

#include <unistd.h>

// TrackerVST001 is designed to call the DTCInterfaceLibrary a certain number of times
// (set in the mu2eFragment header) and pack that data into DTCFragments contained
// in a single mu2eFragment object.

// Some C++ conventions used:

// -Append a "_" to every private member function and variable

#include "artdaq-core/Data/Fragment.hh"
#include "artdaq/Generators/CommandableFragmentGenerator.hh"
#include "fhiclcpp/fwd.h"
#include "artdaq-core-mu2e/Overlays/FragmentType.hh"

#include "dtcInterfaceLib/DTC.h"
#include "dtcInterfaceLib/DTCSoftwareCFO.h"

#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

using namespace DTCLib;

namespace mu2e {
  class TrackerVST001 : public artdaq::CommandableFragmentGenerator {
  public:
    explicit TrackerVST001(fhicl::ParameterSet const& ps);
    virtual ~TrackerVST001();
    
  private:
    // The "getNext_" function is used to implement user-specific
    // functionality; it's a mandatory override of the pure virtual
    // getNext_ function declared in CommandableFragmentGenerator
    
    bool getNext_  (artdaq::FragmentPtrs& output) override;

    bool sendEmpty_(artdaq::FragmentPtrs& output);

    void start      () override {}
    void stopNoMutex() override {}
    void stop       () override;

    void readSimFile_(std::string sim_file);
    mu2e_databuff_t* readDTCBuffer(mu2edev* device, bool& success, bool& timeout, size_t& sts, bool continuedMode);

    void print_roc_registers();
    void print_dtc_registers(DTC* Dtc, const char* Header);
    void printBuffer(const void* ptr, int sz);
//-----------------------------------------------------------------------------
// clones of Monica's scripts
//-----------------------------------------------------------------------------
    void monica_digi_clear         (DTC* Dtc);
    void monica_var_link_config    (DTC* Dtc);
    void monica_var_pattern_config (DTC* Dtc);

    void buffer_test_002    (artdaq::FragmentPtrs& frags);
    void buffer_test_003    (artdaq::FragmentPtrs& frags);
    void buffer_test_004    (artdaq::FragmentPtrs& frags);

					// different tests

    void test_002           (artdaq::FragmentPtrs& frags);
    void test_003           (artdaq::FragmentPtrs& frags);
    void test_004           (artdaq::FragmentPtrs& frags);

    // Like "getNext_", "fragmentIDs_" is a mandatory override; it
    // returns a vector of the fragment IDs an instance of this class
    // is responsible for (in the case of TrackerVST001, this is just
    // the fragment_id_ variable declared in the parent
    // CommandableFragmentGenerator class)

    std::vector<artdaq::Fragment::fragment_id_t> fragmentIDs_() { return fragment_ids_; }

    // FHiCL-configurable variables. Note that the C++ variable names
    // are the FHiCL variable names with a "_" appended

    FragmentType const fragment_type_;  // Type of fragment (see FragmentType.hh)
    
    std::vector<artdaq::Fragment::fragment_id_t> fragment_ids_;

    // State
    size_t          timestamps_read_;
    size_t          highest_timestamp_seen_{0};
    size_t          timestamp_loops_{0}; // For playback mode, so that we continually generate unique timestamps
    DTC_SimMode     _sim_mode; 
    uint8_t         board_id_;
    bool            simFileRead_;
    bool            rawOutput_;
    std::string     rawOutputFile_;
    std::ofstream   rawOutputStream_;
    size_t          nSkip_;
    bool            sendEmpties_;
    int             _debugLevel;
    size_t          _nEvents;              // n(events) to dump


    std::string     _test;
    size_t          request_delay_;
    size_t          _requestsAhead;
    size_t          _heartbeatsAfter;
		int             _heartbeatInterval;
    int             _dtc_id;
    uint            _roc_mask;
					// hardware
    DTC*            _dtc;
    mu2edev*        _device;
    DTCSoftwareCFO* _cfo;

    int             _firstTime;
    uint            _nbuffers;
    uint            _ievent;
    
    std::chrono::steady_clock::time_point lastReportTime_;
    std::chrono::steady_clock::time_point procStartTime_;

    double _timeSinceLastSend() {
      auto now        = std::chrono::steady_clock::now();
      auto deltaw     = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(now - lastReportTime_).count();
      lastReportTime_ = now;
      return deltaw;
    }
    
    void   _startProcTimer() { procStartTime_ = std::chrono::steady_clock::now(); }
    
    double _getProcTimerCount() {
      auto now    = std::chrono::steady_clock::now();
      auto deltaw = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(now - procStartTime_).count();
      return deltaw;
    }
  };
}  // namespace mu2e

//-----------------------------------------------------------------------------
// the FHICL structure passed is fragment_receiver: {}
// no defaults in FCL
//-----------------------------------------------------------------------------
mu2e::TrackerVST001::TrackerVST001(fhicl::ParameterSet const& ps) : 
  CommandableFragmentGenerator(ps)
  , fragment_type_    (toFragmentType("MU2E"))
  , fragment_ids_     {static_cast<artdaq::Fragment::fragment_id_t>(fragment_id())}
  , timestamps_read_  (0)
  , _sim_mode         (DTC_SimModeConverter::ConvertToSimMode(ps.get<std::string>("sim_mode", "Disabled")))
  , board_id_         (static_cast<uint8_t>(ps.get<int>("board_id", 0)))
  , rawOutput_        (ps.get<bool>        ("rawOutput"      ))
  , rawOutputFile_    (ps.get<std::string> ("rawOutputFile"  ))
  , nSkip_            (ps.get<size_t>      ("nSkip"          ))
  , sendEmpties_      (ps.get<bool>        ("sendEmpties"    ))
  , _debugLevel       (ps.get<int>         ("debugLevel"     ))
  , _nEvents          (ps.get<size_t>      ("nEvents"        ))                 // default:  1
  , _test             (ps.get<std::string> ("test"           ))                 // type of the test, default : "test2"
  , request_delay_    (ps.get<size_t>      ("request_delay"  ))
  , _requestsAhead    (ps.get<size_t>      ("requestsAhead"  ))                 // default:  1
  , _heartbeatsAfter  (ps.get<size_t>      ("heartbeatsAfter"))                 // default: 16
  , _heartbeatInterval(ps.get<int>         ("heartbeatInterval"))
  , _dtc_id           (ps.get<int>         ("dtc_id"         ))                 // default: -1
  , _roc_mask         (ps.get<int>         ("roc_mask"       ))                 // default: 0x1
  , lastReportTime_   (std::chrono::steady_clock::now()       ) 
  {
    
    TLOG(TLVL_DEBUG) << "TrackerVST001_generator CONSTRUCTOR";
    // _sim_mode can still be overridden by environment!
    
    _ievent   = 0;
//-----------------------------------------------------------------------------
// _dtc_id = -1 : use $DTCLIB_DTC to determine the DTC's PCI ID
//-----------------------------------------------------------------------------
    _dtc      = new DTC(_sim_mode,_dtc_id,_roc_mask,"");
    _device   = _dtc->GetDevice();

    // 			false, 
    // 			ps.get<std::string>("simulator_memory_file_name","mu2esim.bin"));
    _dtc->SetSequenceNumberDisable();


    bool          useCFOEmulator (true);
    unsigned      packetCount    (0);
    DTC_DebugType debugType      (DTC_DebugType_SpecialSequence);
    bool          stickyDebugType(true);
    bool          quiet          (true);  // was false
    bool          forceNoDebug   (true) ; // (false);
    bool          useCFODRP      (false); // was (true);

    TLOG(TLVL_DEBUG) << "useCFOEmulator, packetCount, debugType, stickyDebugType, quiet, forceNoDebug, useCFODRP:" 
		     << useCFOEmulator << " " << packetCount << " " << debugType << " " << stickyDebugType 
		     << " " << quiet << " " << forceNoDebug << " " << useCFODRP ;

    print_dtc_registers(_dtc,"buffer_test 000");

    _cfo = new DTCSoftwareCFO(_dtc, useCFOEmulator, packetCount, debugType, stickyDebugType, quiet, false, forceNoDebug, useCFODRP);

    TLOG(TLVL_INFO) << "P,Murat: VST board reader created" ;
  }

//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::readSimFile_(std::string sim_file) {
  TLOG(TLVL_INFO) << "Starting read of simulation file " << sim_file << "."
		  << " Please wait to start the run until finished.";
  _dtc->WriteSimFileToDTC(sim_file, true, true);
  simFileRead_ = true;
  TLOG(TLVL_INFO) << "Done reading simulation file into DTC memory.";
}

//-----------------------------------------------------------------------------
mu2e::TrackerVST001::~TrackerVST001() {
  rawOutputStream_.close();
  delete _cfo;
  delete _dtc;
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::stop() {
  // _dtc->DisableDetectorEmulator();
  // _dtc->DisableCFOEmulation();
}

//-----------------------------------------------------------------------------
bool mu2e::TrackerVST001::getNext_(artdaq::FragmentPtrs& Frags) {
  bool rc(true);

  printf("----------------------- mu2e::TrackerVST001::%s : event : %10i\n",__func__,_ievent);

  if (should_stop()) return false;

  // _startProcTimer();

  if (_test == "test_002") { 
//-----------------------------------------------------------------------------
// this is a syncronous read, need only one call
// it also looks that by using _nEvents I'm duplicating the existing functionality,
// if so, need to get rid of that
//-----------------------------------------------------------------------------
    test_002(Frags);
    _ievent += 1;
    rc       = false;
  }
  else if (_test == "test_003") { 
    test_003(Frags);
    _ievent += 1;
    // rc       = (_nEvents > 0) and (_ievent < _nEvents);
  }
  else if (_test == "test_004") { 
    test_004(Frags);
    _ievent += 1;
    // rc       = (_nEvents > 0) and (_ievent < _nEvents);
  }
  else {
    printf("ERROR: undefined test mode: %s, STOP\n",_test.data());
    rc = false;
  }

  ev_counter_inc();

  TLOG(TLVL_DEBUG) << __func__ << ": end of getNext_, return " << rc;
  return rc;
}

//-----------------------------------------------------------------------------
bool mu2e::TrackerVST001::sendEmpty_(artdaq::FragmentPtrs& frags) {
  frags.emplace_back(new artdaq::Fragment());
  frags.back()->setSystemType(artdaq::Fragment::EmptyFragmentType);
  frags.back()->setSequenceID(ev_counter());
  frags.back()->setFragmentID(fragment_ids_[0]);
  ev_counter_inc();
  return true;
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::print_dtc_registers(DTC* Dtc, const char* Header) {
  printf("---------------------- %s : DTC status :\n",Header);
  uint32_t res;
  int      rc;
  rc = Dtc->GetDevice()->read_register(0x9100,100,&res); printf("  0x9100: status           : 0x%08x rc:%i\n",res,rc);
  rc = Dtc->GetDevice()->read_register(0x91c8,100,&res); printf("  0x91c8: debug packet type: 0x%08x rc:%i\n",res,rc);
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::monica_digi_clear(DTCLib::DTC* Dtc) {
//-----------------------------------------------------------------------------
//  Monica's digi_clear
//  this will proceed in 3 steps each for HV and CAL DIGIs:
// 1) pass TWI address and data toTWI controller (fiber is enabled by default)
// 2) write TWI INIT high
// 3) write TWI INIT low
//-----------------------------------------------------------------------------
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,28,0x10,false,1000); // 

  // Writing 0 & 1 to  address=16 for HV DIGIs ??? 

  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,27,0x00,false,1000); // write 0 
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x01,false,1000); // toggle INIT 
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x00,false,1000); // 

  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,27,0x01,false,1000); //  write 1 
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x01,false,1000); //  toggle INIT
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,26,0x00,false,1000); // 

  // echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,25,0x10,false,1000); // 

  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,24,0x00,false,1000); // write 0
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x01,false,1000); // toggle INIT
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x00,false,1000); // 

  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,24,0x01,false,1000); // write 1
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x01,false,1000); // toggle INIT
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,23,0x00,false,1000); // 
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::monica_var_link_config(DTCLib::DTC* Dtc) {
  mu2edev* dev = Dtc->GetDevice();

  dev->write_register(0x91a8,100,0);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,14,     1,false,1000); // reset ROC
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  Dtc->WriteROCRegister(DTCLib::DTC_Link_0, 8,0x030f,false,1000); // configure ROC to read all 4 lanes
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}


//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::monica_var_pattern_config(DTC* Dtc) {
  TLOG(TLVL_DEBUG) << "---------------------------------- operation \"var_patern_config\"" << std::endl;

  //  auto startTime = std::chrono::steady_clock::now();

  print_dtc_registers(Dtc,"var_pattern_config 001");  // debug

  mu2edev* dev = Dtc->GetDevice();

  dev->ResetDeviceTime();

  dev->write_register(0x91a8,100,0); // _CFOEmulation_HeartbeatInterval);
  sleep(1);

  int tmo_ms    (1500);  // 1500 is OK
  int sleep_time( 100);  //  300 is OK

  Dtc->WriteROCRegister(DTC_Link_0,14,0x01,false,tmo_ms);  // this seems to be a reset
  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

  Dtc->WriteROCRegister(DTC_Link_0, 8,0x10,false,tmo_ms);
  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

  Dtc->WriteROCRegister(DTC_Link_0,30,0x00,false,tmo_ms);
  std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

  print_dtc_registers(Dtc,"var_pattern_config 002");  // debug
}


//-----------------------------------------------------------------------------
// this is a clone of Monica's var_read_all.sh script
//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::print_roc_registers() {
  TLOG(TLVL_DEBUG) << "---------------------------------- operation \"print_roc_registers\"" << std::endl;

  // auto startTime = std::chrono::steady_clock::now();

  // Monica starts from disabling EWM's : my_cntl write 0x91a8 0x0

  uint32_t hb_interval;
  int rc = _device->read_register(0x91a8,100,&hb_interval); // DTC_Register_CFOEmulation_HeartbeatInterval

  TLOG(TLVL_DEBUG) << "-------------- mu2e::TrackerVst::" << __func__ 
		   << "  hb_interval=" << hb_interval << "rc:" << rc;

  // Try to comment out  _dtc->WriteRegister_(0,DTC_Register_CFOEmulation_HeartbeatInterval); // 0x91a8

  // step 1 : read everything : registers 0,8,18,23-59,64,65

  roc_data_t r[256]; 
  
  int tmo_ms(10);
  
  r[ 0]  = _dtc->ReadROCRegister(DTC_Link_0, 0,tmo_ms);
  r[ 8]  = _dtc->ReadROCRegister(DTC_Link_0, 8,tmo_ms);
  r[18]  = _dtc->ReadROCRegister(DTC_Link_0,18,tmo_ms);

  for (int i=23; i<60; i++) {
    r[i] = _dtc->ReadROCRegister(DTC_Link_0, i,tmo_ms);
  }

  r[64]  = _dtc->ReadROCRegister(DTC_Link_0,64,tmo_ms);
  r[65]  = _dtc->ReadROCRegister(DTC_Link_0,65,tmo_ms);
//-----------------------------------------------------------------------------
// now the hard part - formatted printout
//-----------------------------------------------------------------------------
  int w1                  = (r[24]<<16) | r[23] ;
  int w2                  = (r[26]<<16) | r[25] ;
  int n_evm_seen          = (r[65]<<16) | r[64] ;
  int n_hbt_seen          = (r[28]<<16) | r[27] ;
  int n_null_hbt          = (r[30]<<16) | r[29] ;
  int n_hbt_hold          = (r[32]<<16) | r[31] ;
  int n_prefetch          = (r[34]<<16) | r[33] ;
  int n_data_req          = (r[36]<<16) | r[35] ;
  int n_data_req_read_ddr = (r[38]<<16) | r[37] ;
  int n_data_req_sent_dtc = (r[40]<<16) | r[39] ;
  int n_data_req_null_dat = (r[42]<<16) | r[41] ;
  int last_spill_tag      = (r[44]<<16) | r[43] ;
  int last_hb_tag         = (r[46]<<16) | r[45] ;
  int last_prefetch_tag   = (r[49]<<16) | r[48] ;
  int last_fetched_tag    = (r[52]<<16) | r[51] ;
  int last_data_req_tag   = (r[55]<<16) | r[54] ;
  int last_offset_tag     = (r[58]<<16) | r[57] ;

  TLOG(TLVL_DEBUG) << "--------------------------------" << std::endl 
		   << "ROC registers:" 
		   << " reg[ 0]: 0x" << std::hex << r[ 0] 
		   << " reg[ 8]: 0x" << std::hex << r[ 8] 
		   << " reg[18]: 0x" << std::hex << r[18]
		   << std::endl 
                   << "SIZE_FIFO_FULL [28]+STORE_POS[25:24]+STORE_CNT[19:0]: 0x" << std::hex << w1
		   << std::endl 
                   << "SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]: 0x" << std::hex << w2
		   << std::endl 
                   << "N(EWM)      seen    : " << std::dec << n_evm_seen
		   << std::endl 
                   << "N(HBT)      seen    : " << n_hbt_seen
		   << std::endl 
                   << "N(null HBT) seen    : " << n_null_hbt
		   << std::endl 
                   << "N(HBT on hold)      : " << n_hbt_hold
		   << std::endl 
                   << "N(prefetch)         : " << n_prefetch
		   << std::endl 
                   << "N(data req)         : " << n_data_req
		   << std::endl 
                   << "N(data req read DDR): " << n_data_req_read_ddr
		   << std::endl 
                   << "N(data req sent DTC): " << n_data_req_sent_dtc
		   << std::endl 
                   << "N(data req null dat): " << n_data_req_null_dat
		   << std::endl 
                   << "Last spill tag      : 0x" << std::hex << last_spill_tag
		   << std::endl 
                   << "Last HB    tag      : 0x" << std::hex << last_hb_tag
		   << std::endl 
                   << "Last prefetch tag   : 0x" << std::hex << last_prefetch_tag
		   << std::endl 
                   << "Last fetched tag    : 0x" << std::hex << last_fetched_tag
		   << std::endl 
                   << "Last data req tag   : 0x" << std::hex << last_data_req_tag
		   << std::endl 
                   << "Last offset tag     : 0x" << std::hex << last_offset_tag
    ;

}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::buffer_test_002(artdaq::FragmentPtrs& frags) {

  TLOG(TLVL_DEBUG) << "---------------------------------- operation \"buffer_test\"" << std::endl;
  auto startTime = std::chrono::steady_clock::now();

  auto initTime = _device->GetDeviceTime();
  _device->ResetDeviceTime();
  auto afterInit = std::chrono::steady_clock::now();
  
  print_dtc_registers(_dtc,"buffer_test 001");

  std::string   timestampFile = "";

  bool          incrementTimestamp(true);
  unsigned long timestampOffset = 1;
  // unsigned      cfodelay       (200);
  int           _requestsAhead  ( 0);
  bool          syncRequests = false;

  TLOG(TLVL_DEBUG) << "_nEvents, timestampOffset, incrementTimestamp,_heartbeatInterval,_requestsAhead,_heartbeatsAfter:" 
		   << _nEvents << " " << timestampOffset << " " << incrementTimestamp << " " << _heartbeatInterval << " " 
		   << _requestsAhead << " " << _heartbeatsAfter;
  TLOG(TLVL_DEBUG) << "syncRequests:" <<  syncRequests;
  
  _cfo->SendRequestsForRange(_nEvents, 
			     DTC_EventWindowTag(timestampOffset), 
			     incrementTimestamp, 
			     _heartbeatInterval, 
			     _requestsAhead, 
			     _heartbeatsAfter);
  
  auto readoutRequestTime = _device->GetDeviceTime();
  _device->ResetDeviceTime();

  auto afterRequests = std::chrono::steady_clock::now();

  print_dtc_registers(_dtc,"buffer_test 002");

  // unsigned      quietCount     (5);     // was 0
  //   bool          reallyQuiet    (false);

  bool          stopOnTimeout  (false);
  bool          checkSERDES    (false);
  unsigned      extraReads     (1);

  for (unsigned ii = 0; ii < _nEvents + extraReads; ++ii) {
    if (syncRequests && ii < _nEvents) {
      auto startRequest   = std::chrono::steady_clock::now();
      _cfo->SendRequestForTimestamp(DTC_EventWindowTag(timestampOffset + (incrementTimestamp ? ii : 0), _heartbeatsAfter));
      auto endRequest     = std::chrono::steady_clock::now();
      readoutRequestTime += std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(endRequest - startRequest).count();
    }
    TLOG(TLVL_DEBUG) << "Reading buffer " << std::dec << ii << std::endl;
    
    bool readSuccess = false;
    bool timeout     = false;
    size_t sts       = 0;

    print_dtc_registers(_dtc,"buffer_test 003");

    mu2e_databuff_t* buffer = readDTCBuffer(_device, readSuccess, timeout, sts, false);

    printf("BUFFER %5i read, sts=%5li readSuccess=%i timeout=%i\n",ii,sts,readSuccess,timeout);
    
    print_dtc_registers(_dtc,"buffer_test 004");

    if      (!readSuccess && checkSERDES) break;
    else if (!readSuccess)                continue;
    
    if (stopOnTimeout && timeout) {
      TLOG(TLVL_ERROR) << "Timeout detected and stop-on-timeout mode enabled. Stopping after " << ii << " events!";
      break;
    }
    
    DTCLib::Utilities::PrintBuffer(buffer, sts, 0);
    
    _device->read_release(DTC_DMA_Engine_DAQ, 1);
    usleep(200);

    print_dtc_registers(_dtc,"buffer_test 005");
  }

  _device->release_all(DTC_DMA_Engine_DAQ);
  
  print_dtc_registers(_dtc,"buffer_test 006");

  auto totalBytesRead    = _device->GetReadSize();
  auto totalBytesWritten = _device->GetWriteSize();

  auto readDevTime       = _device->GetDeviceTime();
  auto doneTime          = std::chrono::steady_clock::now();
  auto totalTime         = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(doneTime - startTime).count();
  auto totalInitTime     = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(afterInit - startTime).count();
  auto totalRequestTime  = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(afterRequests - afterInit).count();
  auto totalReadTime     = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(doneTime - afterRequests).count();
  
  TLOG(TLVL_INFO) << "Total Elapsed Time: " << Utilities::FormatTimeString(totalTime) << "." << std::endl
		  << "Total Init Time: " << Utilities::FormatTimeString(totalInitTime) << "." << std::endl
		  << "Total Readout Request Time: " << Utilities::FormatTimeString(totalRequestTime) << "." << std::endl
		  << "Total Read Time: " << Utilities::FormatTimeString(totalReadTime) << "." << std::endl;

  TLOG(TLVL_INFO) << "Device Init Time: "    << Utilities::FormatTimeString(initTime) << "." << std::endl
		  << "Device Request Time: " << Utilities::FormatTimeString(readoutRequestTime) << "." << std::endl
		  << "Device Read Time: "    << Utilities::FormatTimeString(readDevTime) << "." << std::endl;

  TLOG(TLVL_INFO) << "Total Bytes Written: " << Utilities::FormatByteString(static_cast<double>(totalBytesWritten), "")
		  << "." << std::endl
		  << "Total Bytes Read: " << Utilities::FormatByteString(static_cast<double>(totalBytesRead), "") << "."
		  << std::endl;

  TLOG(TLVL_INFO) << "Total PCIe Rate: "
		  << Utilities::FormatByteString((totalBytesWritten + totalBytesRead) / totalTime, "/s") << std::endl
		  << "Read Rate: " << Utilities::FormatByteString(totalBytesRead / totalReadTime, "/s") << std::endl
		  << "Device Read Rate: " << Utilities::FormatByteString(totalBytesRead / readDevTime, "/s") << std::endl;
}

//-----------------------------------------------------------------------------
// buffer_test in sync mode : syncRequests = true
//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::buffer_test_003(artdaq::FragmentPtrs& frags) {
  TLOG(TLVL_DEBUG) << "---------------------------------- operation \"__func__\"" << std::endl;
  auto startTime = std::chrono::steady_clock::now();

  auto initTime = _device->GetDeviceTime();
  _device->ResetDeviceTime();
  auto afterInit = std::chrono::steady_clock::now();
  
  // bool          useCFOEmulator (true);
  // unsigned      packetCount    (0);
  // DTC_DebugType debugType      (DTC_DebugType_SpecialSequence);
  // bool          stickyDebugType(true);
  // bool          quiet          (true);  // was false
  unsigned      quietCount     (5);     // was 0
  // bool          reallyQuiet    (false);
  // bool          forceNoDebug   (true ); // (false);
  // bool          useCFODRP      (false); // was (true);

  uint          number         (1    ); // for test3 this is essential
  bool          stopOnTimeout  (false);
  bool          checkSERDES    (false);

  // TLOG(TLVL_DEBUG) << "useCFOEmulator, packetCount, debugType, stickyDebugType, quiet, forceNoDebug, useCFODRP:" 
  // 		   << useCFOEmulator << " " << packetCount << " " << debugType << " " << stickyDebugType 
  // 		   << " " << quiet << " " << forceNoDebug << " " << useCFODRP ;

  print_dtc_registers(_dtc,"buffer_test_003 000");

  std::string   timestampFile = "";

  bool          incrementTimestamp(true);
  unsigned long timestampOffset = 1;
  unsigned      cfodelay       (200);
  int           _requestsAhead  ( 0);
  unsigned      _heartbeatsAfter(16);
  bool          syncRequests = true;

  TLOG(TLVL_DEBUG) << "number, timestampOffset, incrementTimestamp,cfodelay,_requestsAhead,_heartbeatsAfter:" 
		   << number << " " << timestampOffset << " " << incrementTimestamp << " " << cfodelay << " " 
		   << _requestsAhead << " " << _heartbeatsAfter;
  TLOG(TLVL_DEBUG) << "syncRequests:" <<  syncRequests;
  

  auto readoutRequestTime = _device->GetDeviceTime();
  _device->ResetDeviceTime();

  auto afterRequests = std::chrono::steady_clock::now();

  print_dtc_registers(_dtc,"buffer_test_003 002");

  unsigned      extraReads     (1);
  for (unsigned ii = 0; ii < number + extraReads; ++ii) {
    if (syncRequests && ii < number) {
      auto startRequest   = std::chrono::steady_clock::now();
      _cfo->SendRequestForTimestamp(DTC_EventWindowTag(timestampOffset + (incrementTimestamp ? ii : 0), _heartbeatsAfter));
      auto endRequest     = std::chrono::steady_clock::now();
      readoutRequestTime += std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(endRequest - startRequest).count();
    }
    TLOG(TLVL_DEBUG) << "Reading buffer " << std::dec << ii << std::endl;
    
    bool readSuccess = false;
    bool timeout     = false;
    size_t sts       = 0;

    print_dtc_registers(_dtc,"buffer_test_003 003");

    mu2e_databuff_t* buffer = readDTCBuffer(_device, readSuccess, timeout, sts, false);

    printf("BUFFER %5i read, sts=%5li readSuccess=%i timeout=%i\n",ii,sts,readSuccess,timeout);
    
    print_dtc_registers(_dtc,"buffer_test_003 004");

    if      (!readSuccess && checkSERDES) break;
    else if (!readSuccess)                continue;
    
    if (stopOnTimeout && timeout) {
      TLOG(TLVL_ERROR) << "Timeout detected and stop-on-timeout mode enabled. Stopping after " << ii << " events!";
      break;
    }
    
    DTCLib::Utilities::PrintBuffer(buffer, sts, quietCount);
    
    _device->read_release(DTC_DMA_Engine_DAQ, 1);
    usleep(200);

    print_dtc_registers(_dtc,"buffer_test_003 005");
  }

  _device->release_all(DTC_DMA_Engine_DAQ);
  
  print_dtc_registers(_dtc,"buffer_test3 006");

  auto totalBytesRead    = _device->GetReadSize();
  auto totalBytesWritten = _device->GetWriteSize();

  auto readDevTime       = _device->GetDeviceTime();
  auto doneTime          = std::chrono::steady_clock::now();
  auto totalTime         = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(doneTime - startTime).count();
  auto totalInitTime     = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(afterInit - startTime).count();
  auto totalRequestTime  = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(afterRequests - afterInit).count();
  auto totalReadTime     = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(doneTime - afterRequests).count();
  
  TLOG(TLVL_INFO) << "Total Elapsed Time: " << Utilities::FormatTimeString(totalTime) << "." << std::endl
		  << "Total Init Time: " << Utilities::FormatTimeString(totalInitTime) << "." << std::endl
		  << "Total Readout Request Time: " << Utilities::FormatTimeString(totalRequestTime) << "." << std::endl
		  << "Total Read Time: " << Utilities::FormatTimeString(totalReadTime) << "." << std::endl;

  TLOG(TLVL_INFO) << "Device Init Time: "    << Utilities::FormatTimeString(initTime) << "." << std::endl
		  << "Device Request Time: " << Utilities::FormatTimeString(readoutRequestTime) << "." << std::endl
		  << "Device Read Time: "    << Utilities::FormatTimeString(readDevTime) << "." << std::endl;

  TLOG(TLVL_INFO) << "Total Bytes Written: " << Utilities::FormatByteString(static_cast<double>(totalBytesWritten), "")
		  << "." << std::endl
		  << "Total Bytes Read: " << Utilities::FormatByteString(static_cast<double>(totalBytesRead), "") << "."
		  << std::endl;

  TLOG(TLVL_INFO) << "Total PCIe Rate: "
		  << Utilities::FormatByteString((totalBytesWritten + totalBytesRead) / totalTime, "/s") << std::endl
		  << "Read Rate: " << Utilities::FormatByteString(totalBytesRead / totalReadTime, "/s") << std::endl
		  << "Device Read Rate: " << Utilities::FormatByteString(totalBytesRead / readDevTime, "/s") << std::endl;
}


//-----------------------------------------------------------------------------
// buffer_test_004: read data in sync mode
//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::buffer_test_004(artdaq::FragmentPtrs& Frags) {
  uint32_t res; 
  int      rc;

  TLOG(TLVL_DEBUG) << "---------------------------------- operation \"__func__\"" << std::endl;

  _device->ResetDeviceTime();

  if (_debugLevel > 0) print_dtc_registers(_dtc,"buffer_test_003 000");

  std::string   timestampFile = "";

  bool          incrementTimestamp(true);
  unsigned long timestampOffset = 1;
  unsigned      cfodelay       (200);
  int           delay         = 200;         // 500 was OK;

  TLOG(TLVL_DEBUG) << "timestampOffset, incrementTimestamp,cfodelay,_requestsAhead,_heartbeatsAfter:" 
		   << " " << timestampOffset << " " << incrementTimestamp << " " << cfodelay << " " 
		   << _requestsAhead << " " << _heartbeatsAfter;

  _device->ResetDeviceTime();

  if (_debugLevel > 0) print_dtc_registers(_dtc,"buffer_test_003 002");
//-----------------------------------------------------------------------------
// each time read just one event
//-----------------------------------------------------------------------------
  // for (int i=0; i<1; i++) {
  monica_digi_clear     (_dtc);
  monica_var_link_config(_dtc);
//-----------------------------------------------------------------------------
// back to the time window of 25.6 us 0x400 = 1024 (x25 ns)
//-----------------------------------------------------------------------------
  _device->write_register(0x91a8,100,_heartbeatInterval);

  int nev = 1;
  _cfo->SendRequestsForRange(nev,DTC_EventWindowTag(uint64_t(_ievent)),
			     incrementTimestamp,cfodelay,
			     _requestsAhead,_heartbeatsAfter);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  _device->ResetDeviceTime();

    // print_roc_registers(&_dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

  size_t nbytes(0);
  bool   timeout(false);
  bool   readSuccess(false);
   
  mu2e_databuff_t* buffer = readDTCBuffer(_device, readSuccess, timeout, nbytes, false);
  //    int nbytes = dev->read_data(DTC_DMA_Engine_DAQ, (void**) (&buffer), tmo_ms);

  // print_roc_registers(&dtc,DTCLib::DTC_Link_0,"002 [after readDTCBuffer]");

  // if (_debugLevel > 0) DTCLib::Utilities::PrintBuffer(buffer, nbytes, 0);
  
  if (ev_counter() < _nEvents) {
    printf(" ----------------------------------------------------------------------- reading event %i\n",_ievent);
    printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,nbytes);
    printBuffer(buffer, (int) nbytes);
  }
//-----------------------------------------------------------------------------
// first 0x40 bytes seem to be useless, they are followed by the data header packer,
// offline starts from there
//-----------------------------------------------------------------------------
  int offset = 0x40;
  double fragment_timestamp = _ievent;
  artdaq::Fragment* frag    = new artdaq::Fragment(ev_counter(), fragment_id(), FragmentType::TRK, fragment_timestamp);
  frag->resizeBytes(nbytes-offset);
  void* af = frag->dataBegin();
  Frags.emplace_back(frag);

  struct DataHeaderPacket_t {
    uint16_t  nBytes;
    uint16_t  w2;
    uint16_t  nPackets;
    uint16_t  evtWindowTag[3];
    uint16_t  w7;
    uint16_t  w8;

    void setVersion(int version) { w7 = (w7 & 0x00ff) + ((version & 0xff) << 8) ; }
    void setStatus (int status ) { w7 = (w7 & 0xff00) + (status & 0xff)         ; }
  };

  char* cbuf = (char*) buffer;
  memcpy(af, cbuf+offset,nbytes-offset);

  DataHeaderPacket_t* dp = (DataHeaderPacket_t*) af;
  dp->setVersion(1);
//-----------------------------------------------------------------------------
// now print the block saved in the file
//-----------------------------------------------------------------------------
  if (ev_counter() < _nEvents) {
    printf(" ----------------------------------------------------------------------- saved Fragment %i\n",_ievent);
    printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,nbytes);
    printBuffer(af, (int) nbytes-offset);
  }
//-----------------------------------------------------------------------------
// release the DMA channel
//-----------------------------------------------------------------------------
  _device->read_release(DTC_DMA_Engine_DAQ, 1);

  if (delay > 0) usleep(delay);

  if (_debugLevel > 0) {
    rc = _device->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x rc:%i\n",res,rc); // expect: 0x40808404
    rc = _device->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x rc:%i\n",res,rc); // expect: 0x00000000
  }

  _device->release_all(DTC_DMA_Engine_DAQ);

}


//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::test_002(artdaq::FragmentPtrs& Frags) {
  monica_var_pattern_config (_dtc);

  print_dtc_registers(_dtc,"test_002 001");
  print_roc_registers();

  buffer_test_002    (Frags);

  print_dtc_registers(_dtc,"test_002 002");
  print_roc_registers();
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::test_003(artdaq::FragmentPtrs& Frags) {
  monica_var_pattern_config (_dtc);

  print_dtc_registers(_dtc,"test_003 001");
  print_roc_registers();

  buffer_test_003    (Frags);

  print_dtc_registers(_dtc,"test3 002");
  print_roc_registers();
}


//-----------------------------------------------------------------------------
// read data
//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::test_004(artdaq::FragmentPtrs& Frags) {

  monica_digi_clear     (_dtc);
  monica_var_link_config(_dtc);

  if (_debugLevel > 0) print_dtc_registers(_dtc,"test_004 001");
  //  print_roc_registers();

  buffer_test_004(Frags);

  if (_debugLevel > 0) print_dtc_registers(_dtc,"test4 002");
  // print_roc_registers();
}

//-----------------------------------------------------------------------------
// this is to make an organized transition
//-----------------------------------------------------------------------------
mu2e_databuff_t* mu2e::TrackerVST001::readDTCBuffer(mu2edev* Device, bool& readSuccess, bool& timeout, 
						    size_t& sts, 
						    bool continuedMode) {
  mu2e_databuff_t* buffer;
  auto tmo_ms = 1500;
  readSuccess = false;
  TLOG(TLVL_DEBUG) << "mu2e::TrackerVST001::readDTCBuffer:  START : before read_data";
  sts = Device->read_data(DTC_DMA_Engine_DAQ, reinterpret_cast<void**>(&buffer), tmo_ms);

  TLOG(TLVL_DEBUG) << "mu2e::TrackerVST001::readDTCBuffer: after read for DAQ sts=" << sts << ", buffer=" << (void*)buffer;
  
  if (sts > 0)    {
    readSuccess = true;
    void* readPtr = &buffer[0];
    uint16_t bufSize = static_cast<uint16_t>(*static_cast<uint64_t*>(readPtr));
    readPtr = static_cast<uint8_t*>(readPtr) + 8;

    TLOG(TLVL_DEBUG) << "Buffer reports DMA size of " 
		     << std::dec << bufSize << " bytes. Device driver reports read of "
		     << sts << " bytes," << std::endl;

    TLOG(TLVL_DEBUG) << "mu2e::TrackerVST001::readDTCBuffer: bufSize is " << bufSize;

    timeout = false;
    if (!continuedMode && sts > sizeof(DTC_EventHeader) + sizeof(DTC_SubEventHeader) + 8) {
      // Check for dead or cafe in first packet
      readPtr = static_cast<uint8_t*>(readPtr) + sizeof(DTC_EventHeader) + sizeof(DTC_SubEventHeader);
      std::vector<size_t> wordsToCheck{ 1, 2, 3, 7, 8 };
      for (auto& word : wordsToCheck) 	{
	auto wordPtr = static_cast<uint16_t*>(readPtr) + (word - 1);
	TLOG(TLVL_DEBUG) << word << (word == 1 ? "st" : word == 2 ? "nd"
				     : word == 3 ? "rd"
				     : "th")
			 << " word of buffer: " << *wordPtr;

	if (*wordPtr == 0xcafe || *wordPtr == 0xdead) 	  {
	  TLOG(TLVL_WARNING) << "Buffer Timeout detected! " 
			     << word << (word == 1 ? "st" : word == 2 ? "nd"
					 : word == 3 ? "rd" : "th")
			     << " word of buffer is 0x" << std::hex << *wordPtr;
	  DTCLib::Utilities::PrintBuffer(readPtr, 16, 0, TLVL_DEBUG);
	  timeout = true;
	  break;
	}
      }
    }
  }

  printf("mu2e::TrackerVST001::readDTCBuffer: END\n") ;

  return buffer;
}

//-----------------------------------------------------------------------------
// print 16 bytes per line
// size - number of bytes to print, even
//-----------------------------------------------------------------------------
void mu2e::TrackerVST001::printBuffer(const void* ptr, int sz) {

  int     nw  = sz/2;
  ushort* p16 = (ushort*) ptr;
  int     n   = 0;

  for (int i=0; i<nw; i++) {
    if (n == 0) printf(" 0x%08x: ",i*2);

    ushort  word = p16[i];
    printf("0x%04x ",word);

    n   += 1;
    if (n == 8) {
      printf("\n");
      n = 0;
    }
  }

  if (n != 0) printf("\n");
}
//-----------------------------------------------------------------------------
// The following macro is defined in artdaq's GeneratorMacros.hh header
//-----------------------------------------------------------------------------
DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(mu2e::TrackerVST001)
