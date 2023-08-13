
#include "artdaq/DAQdata/Globals.hh"
#define TRACE_NAME (app_name + "_TrackerVST").c_str()

// #include "otsdaq-mu2e-tracker/Generators/TrackerVST.hh"

#include "canvas/Utilities/Exception.h"

#include "artdaq-core/Utilities/SimpleLookupPolicy.hh"
#include "artdaq/Generators/GeneratorMacros.hh"
#include "cetlib_except/exception.h"
#include "fhiclcpp/ParameterSet.h"
#include "artdaq-core-mu2e/Overlays/FragmentType.hh"
#include "artdaq-core-mu2e/Data/ArtFragment.hh"
// #include "artdaq-core-mu2e/Overlays/mu2eFragmentWriter.hh"

#include <fstream>
#include <iomanip>
#include <iterator>

#include <unistd.h>

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
  class TrackerVST : public artdaq::CommandableFragmentGenerator {
  public:
    explicit TrackerVST(fhicl::ParameterSet const& ps);
    virtual ~TrackerVST();
    
  private:
    // The "getNext_" function is used to implement user-specific
    // functionality; it's a mandatory override of the pure virtual
    // getNext_ function declared in CommandableFragmentGenerator
    
    bool getNext_(artdaq::FragmentPtrs& output) override;

    bool sendEmpty_(artdaq::FragmentPtrs& output);

    void start() override {}

    void stopNoMutex() override {}
    
    void stop() override;
    
    void readSimFile_       (std::string sim_file);

    void print_dtc_registers(DTC* Dtc, const char* Header);
    void printBuffer        (const void* ptr, int sz);
//-----------------------------------------------------------------------------
// clones of Monica's scripts
//-----------------------------------------------------------------------------
    void monica_digi_clear         (DTC* Dtc);
    void monica_var_link_config    (DTC* Dtc);
    void monica_var_pattern_config (DTC* Dtc);
   
    // Like "getNext_", "fragmentIDs_" is a mandatory override; it
    // returns a vector of the fragment IDs an instance of this class
    // is responsible for (in the case of TrackerVST, this is just
    // the fragment_id_ variable declared in the parent
    // CommandableFragmentGenerator class)

    std::vector<artdaq::Fragment::fragment_id_t> fragmentIDs_() { return fragment_ids_; }

    // FHiCL-configurable variables. Note that the C++ variable names
    // are the FHiCL variable names with a "_" appended

    FragmentType const fragment_type_;  // Type of fragment (see FragmentType.hh)

    std::vector<artdaq::Fragment::fragment_id_t> fragment_ids_;

    // State
    size_t                                timestamps_read_;
    size_t                                highest_timestamp_seen_{0};
    size_t                                timestamp_loops_{0}; // For playback mode, so that we continually generate unique timestamps
    std::chrono::steady_clock::time_point lastReportTime_;
    std::chrono::steady_clock::time_point procStartTime_;
    DTCLib::DTC_SimMode                   _sim_mode;
    uint8_t                               board_id_;
    bool                                  simFileRead_;
    bool                                  _loadSimFile;
    std::string                           _simFileName;
    bool                                  _rawOutputEnable;
    std::string                           rawOutputFile_;
    std::ofstream                         rawOutputStream_;
    //    size_t                                _nSkip;
    bool                                  sendEmpties_;
    int                                   _debugLevel;
    size_t                                _nEventsDbg;
    size_t                                request_delay_;
    size_t                                _heartbeatsAfter;
		int                                   _heartbeatInterval;
    int                                   _dtcId;
    uint                                  _rocMask;
    int                                   _sleepTimeDMA;           // sleep time (us) after DMA release
    int                                   _sleepTimeDTC;           // sleep time (ms) after register writes
    int                                   _sleepTimeROC;           // sleep time (ms) after ROC register writes
    int                                   _sleepTimeROCReset;      // sleep time (ms) after ROC reset register write

    int                                   _resetROC;               // 1: reset ROC every event
    			                  
    DTCLib::DTC*                          _dtc;
    mu2edev*                              _device;
    DTCLib::DTCSoftwareCFO*               _cfo;
    
    double _timeSinceLastSend() {
      auto now    = std::chrono::steady_clock::now();
      auto deltaw = std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(now - lastReportTime_).count();
      lastReportTime_ = now;
      return deltaw;
    }
    
    void   _startProcTimer() { procStartTime_ = std::chrono::steady_clock::now(); }
    
    double _getProcTimerCount() {
      auto now = std::chrono::steady_clock::now();
      auto deltaw =
	std::chrono::duration_cast<std::chrono::duration<double, std::ratio<1>>>(now - procStartTime_).count();
      return deltaw;
    }
  };
}  // namespace mu2e

//-----------------------------------------------------------------------------
// sim_mode="N" means real DTC 
//-----------------------------------------------------------------------------
mu2e::TrackerVST::TrackerVST(fhicl::ParameterSet const& ps) : 
  CommandableFragmentGenerator(ps)
  , fragment_type_    (toFragmentType("MU2E"))
  , fragment_ids_     {static_cast<artdaq::Fragment::fragment_id_t>(fragment_id())}
  , timestamps_read_  (0)
  , lastReportTime_   (std::chrono::steady_clock::now())
  , _sim_mode         (DTCLib::DTC_SimModeConverter::ConvertToSimMode(ps.get<std::string>("sim_mode", "N")))
  , board_id_         (static_cast<uint8_t>(ps.get<int>("board_id"                       , 0)))
  , _loadSimFile      (ps.get<bool>                    ("loadSimFile"                        ))
  , _simFileName      (ps.get<std::string>             ("simFileName"                        ))
  , _rawOutputEnable  (ps.get<bool>                    ("rawOutputEnable"                    ))
  , rawOutputFile_    (ps.get<std::string>             ("raw_output_file"             , "/tmp/TrackerVST.bin"))
  , sendEmpties_      (ps.get<bool>                    ("sendEmpties"                        ))
  , _debugLevel       (ps.get<int>                     ("debugLevel"                  ,     0))
  , _nEventsDbg       (ps.get<size_t>                  ("nEventsDbg"                  ,    20))
  , request_delay_    (ps.get<size_t>                  ("delay_between_requests_ticks", 20000))
  , _heartbeatsAfter  (ps.get<size_t>                  ("heartbeatsAfter"                    )) 
  , _heartbeatInterval(ps.get<int>                     ("heartbeatInterval"                  ))
  , _dtcId            (ps.get<int>                     ("dtcId"                 ,          -1)) 
  , _rocMask          (ps.get<int>                     ("rocMask"               ,         0x1)) 
  , _sleepTimeDMA     (ps.get<int>                     ("sleepTimeDMA"          ,         100))  // 100 microseconds
  , _sleepTimeDTC     (ps.get<int>                     ("sleepTimeDTC"          ,         300))  // 300 microseconds
  , _sleepTimeROC     (ps.get<int>                     ("sleepTimeROC"          ,        2500))  // 2.5 milliseconds
  , _sleepTimeROCReset(ps.get<int>                     ("sleepTimeROCReset"     ,        4000))  // 4.0 milliseconds
  , _resetROC         (ps.get<int>                     ("resetROC"              ,           1))  // 
  {
    
    TLOG(TLVL_INFO) << "TrackerVST_generator CONSTRUCTOR";
//-----------------------------------------------------------------------------
// _sim_mode can still be overridden by environment var $DTCLIB_SIM_ENABLE 
// the sim mode conversion is non-trivial
//-----------------------------------------------------------------------------
    _dtc      = new DTC(_sim_mode,_dtcId,_rocMask,"",false,_simFileName);
    _sim_mode = _dtc->ReadSimMode();

    _device = _dtc->GetDevice();
    
    TLOG(TLVL_INFO) << "The DTC Firmware version string is: " << _dtc->ReadDesignVersion();
    
    fhicl::ParameterSet cfoConfig = ps.get<fhicl::ParameterSet>("cfo_config", fhicl::ParameterSet());
    
    bool          use_dtc_cfo_emulator = cfoConfig.get<bool>       ("use_dtc_cfo_emulator", true );
    size_t        debug_packet_count   = cfoConfig.get<size_t>     ("debug_packet_count"  , 0);
    std::string   dbtype               = cfoConfig.get<std::string>("debug_type"          , "DTC_DebugType_SpecialSequence");   // default was set to "2"
    DTC_DebugType debug_type           = DTC_DebugTypeConverter::ConvertToDebugType(dbtype);
    bool          sticky_debug_type    = cfoConfig.get<bool>       ("sticky_debug_type"   , true );  // what is this ?
    bool          quiet                = cfoConfig.get<bool>       ("quiet"               , true );
    bool          asyncRR              = cfoConfig.get<bool>       ("asyncRR"             , true );  // default was false
    bool          force_no_debug_mode  = cfoConfig.get<bool>       ("force_no_debug_mode" , true );  // sets a bit written to the DTC register
    bool          useCFODRP            = cfoConfig.get<bool>       ("useCFODRP"           , false);  // defaults to false

    _cfo = new DTCSoftwareCFO(_dtc                ,
                              use_dtc_cfo_emulator,
                              debug_packet_count  ,
                              debug_type          ,
                              sticky_debug_type   ,
                              quiet               ,
                              asyncRR             ,
                              force_no_debug_mode ,
                              useCFODRP           );
//-----------------------------------------------------------------------------
// not sure I fully understand the logic below
//-----------------------------------------------------------------------------
    if (_loadSimFile) {
      _dtc->SetDetectorEmulatorInUse();
      _dtc->ResetDDR();
      _dtc->ResetDTC();
      
      if (_simFileName.size() > 0) {
				simFileRead_ = false;
				std::thread reader(&mu2e::TrackerVST::readSimFile_,this,_simFileName);
				reader.detach();
      }
    }
    else {
      _dtc->ClearDetectorEmulatorInUse();  // Needed if we're doing ROC Emulator...
			// make sure Detector Emulation is disabled
      simFileRead_ = true;
    }
    
    if (_rawOutputEnable) rawOutputStream_.open(rawOutputFile_, std::ios::out | std::ios::app | std::ios::binary);
  }

//-----------------------------------------------------------------------------
void mu2e::TrackerVST::readSimFile_(std::string sim_file) {
  TLOG(TLVL_INFO) << "Starting read of simulation file " << sim_file << "."
		  << " Please wait to start the run until finished.";

  _dtc->WriteSimFileToDTC(sim_file, true, true);
  simFileRead_ = true;
  TLOG(TLVL_INFO) << "Done reading simulation file into DTC memory.";
}

//-----------------------------------------------------------------------------
mu2e::TrackerVST::~TrackerVST() {
  rawOutputStream_.close();
  delete _cfo;
  delete _dtc;
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST::stop() {
  _dtc->DisableDetectorEmulator();
  _dtc->DisableCFOEmulation    ();
}

//-----------------------------------------------------------------------------
// a virtual function called from the outside world
//-----------------------------------------------------------------------------
bool mu2e::TrackerVST::getNext_(artdaq::FragmentPtrs& Frags) {
  const char* oname = "mu2e::TrackerVST::getNext_: ";

  TLOG(TLVL_DEBUG) << oname << "STARTING";

  if (should_stop()) return false;

//-----------------------------------------------------------------------------
// P.Murat: don't know what it is, comment out
//-----------------------------------------------------------------------------
  // if (sendEmpties_) {
  //   int mod = ev_counter() % _nSkip;
  //   if (mod == board_id_ || (mod == 0 && board_id_ == _nSkip)) {
  //     // TLOG(TLVL_DEBUG) << "Sending Data  Fragment for sequence id " << ev_counter() << " (board_id " <<
  //     // std::to_string(board_id_) << ")" ;
  //   }
  //   else {
  //     // TLOG(TLVL_DEBUG) << "Sending Empty Fragment for sequence id " << ev_counter() << " (board_id " <<
  //     // std::to_string(board_id_) << ")" ;
  //     return sendEmpty_(Frags);
  //   }
  // }

  _startProcTimer();

  TLOG(TLVL_DEBUG) << oname << "after startProcTimer";
//-----------------------------------------------------------------------------
// Monica's way of resetting the DTC and the ROC - so far, assume just one ROC
// having reset the ROC, go back to the requested time window 
//-----------------------------------------------------------------------------
  if (_resetROC) {
    monica_digi_clear     (_dtc);
    monica_var_link_config(_dtc);
    _dtc->GetDevice()->write_register(0x91a8,100,_heartbeatInterval);
  }
//-----------------------------------------------------------------------------
// send a request for one event, what is the role of requestsAhead ? 
//-----------------------------------------------------------------------------
  uint64_t             z(0);
  DTC_EventWindowTag   zero(z);
  bool                 incrementTimestamp(true);
  int                  requestsAhead     (  0);
  // unsigned long timestampOffset   (  1);

  int nev = 1;
//-----------------------------------------------------------------------------
// duplicating setting the distance btw teh two event window markers
//-----------------------------------------------------------------------------
  _cfo->SendRequestsForRange(nev,DTC_EventWindowTag(ev_counter()),
                             incrementTimestamp,
                             _heartbeatInterval,
                             requestsAhead,
                             _heartbeatsAfter);

  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeDTC));

  _dtc->GetDevice()->ResetDeviceTime();

    // print_roc_registers(&_dtc,DTCLib::DTC_Link_0,"001 [after cfo.SendRequestForTimestamp]");

  printf(" ------------------------------------------------------- reading event %li",ev_counter());

//-----------------------------------------------------------------------------
// the body of the readDTCBuffer
//-----------------------------------------------------------------------------
  size_t nbytes(0);
  bool   timeout(false);
  bool   readSuccess(false);
  auto   tmo_ms(1500);

  mu2e_databuff_t* buffer; //  = readDTCBuffer( _dtc->GetDevice(), readSuccess, timeout, nbytes, false);

  nbytes = _dtc->GetDevice()->read_data(DTC_DMA_Engine_DAQ, reinterpret_cast<void**>(&buffer), tmo_ms);
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeDTC));

  if (nbytes > 0)    {
    readSuccess      = true;
    void* readPtr    = &buffer[0];
    uint16_t bufSize = static_cast<uint16_t>(*static_cast<uint64_t*>(readPtr));
    readPtr          = static_cast<uint8_t*>(readPtr) + 8;

    TLOG(TLVL_DEBUG) << "Buffer reports DMA size of " 
		     << std::dec << bufSize << " bytes. Device driver reports read of "
		     << nbytes << " bytes," << std::endl;
    TLOG(TLVL_DEBUG) << "mu2e::TrackerVST::readDTCBuffer: bufSize is " << bufSize;

    timeout = false;

    if (nbytes > sizeof(DTC_EventHeader) + sizeof(DTC_SubEventHeader) + 8) {
//-----------------------------------------------------------------------------
// I suspect this is a check of any useful data being present. although not sure
// what 8 stands for. Also not sure about the meaning of the checks performed
// Check for dead or cafe in first packet
//-----------------------------------------------------------------------------
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
														 << word 
														 << (word == 1 ? "st" : word == 2 ? "nd" : word == 3 ? "rd" : "th")
														 << " word of buffer is 0x" << std::hex << *wordPtr;
					DTCLib::Utilities::PrintBuffer(readPtr, 16, 0, TLVL_DEBUG);
					timeout = true;
					break;
				}
      }
    }
  }

  printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,nbytes);

  if ((_debugLevel > 0) and (ev_counter() < _nEventsDbg)) { 
    // print_roc_registers(&dtc,DTCLib::DTC_Link_0,"002 [after readDTCBuffer]");
    DTCLib::Utilities::PrintBuffer(buffer, nbytes, 0);
  
    // printBuffer(buffer, (int) nbytes);
  }
//-----------------------------------------------------------------------------
// first 0x40 bytes seem to be useless, they are followed by the data header packer,
// offline starts from there
// suspect the offset is the size of the (DTC_EventHeader + DTC_SubEventHeader)
//-----------------------------------------------------------------------------
  int offset = 0x40;

  double fragment_timestamp = ev_counter();
  artdaq::Fragment* frag    = new artdaq::Fragment(ev_counter(), fragment_id(), FragmentType::TRK, fragment_timestamp);
  frag->resizeBytes(nbytes-offset);

  Frags.emplace_back(frag);
//-----------------------------------------------------------------------------
// copy data and patch format version - set it to 1
// don't copy 0x40 bytes - event header and subevent header (??) - to the artdaq fragment
//-----------------------------------------------------------------------------
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
  void* afd  = frag->dataBegin();
  memcpy(afd, cbuf+offset,nbytes-offset);
//-----------------------------------------------------------------------------
// artdaq fragment starts from the data header packet
//-----------------------------------------------------------------------------
  DataHeaderPacket_t* dp = (DataHeaderPacket_t*) afd;
  dp->setVersion(1);
//-----------------------------------------------------------------------------
// now print the block saved in the file for 10 events
//-----------------------------------------------------------------------------
  // if ((_debugLevel > 0) and (ev_counter() < _nEventsDbg)) { 
  //   printf(" ----------------------------------------------------------------------- saved Fragment %lu\n",ev_counter());
  //   printf(" readSuccess:%i timeout:%i nbytes: %5lu\n",readSuccess,timeout,nbytes);
  //   printBuffer(af, (int) nbytes-offset);
  // }
//-----------------------------------------------------------------------------
// release the DMA channel
//-----------------------------------------------------------------------------
  _dtc->GetDevice()->read_release(DTC_DMA_Engine_DAQ, 1);

  if (_sleepTimeDMA > 0) {
    std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeDMA));
  }

  if ((_debugLevel > 0) and (ev_counter() < _nEventsDbg)) {
    int      rc;
    uint32_t res;
    rc = _dtc->GetDevice()->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x rc:%i\n",res,rc); // expect: 0x40808404
    rc = _dtc->GetDevice()->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x rc:%i\n",res,rc); // expect: 0x00000000
  }

  _dtc->GetDevice()->release_all(DTC_DMA_Engine_DAQ);
//-----------------------------------------------------------------------------
// increment the number of generated events
//-----------------------------------------------------------------------------
  ev_counter_inc();

  return true;
}


//-----------------------------------------------------------------------------
bool mu2e::TrackerVST::sendEmpty_(artdaq::FragmentPtrs& Frags) {
  Frags.emplace_back(new artdaq::Fragment());
  Frags.back()->setSystemType(artdaq::Fragment::EmptyFragmentType);
  Frags.back()->setSequenceID(ev_counter());
  Frags.back()->setFragmentID(fragment_ids_[0]);
  ev_counter_inc();
  return true;
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST::print_dtc_registers(DTC* Dtc, const char* Header) {
  printf("---------------------- %s : DTC status :\n",Header);
  uint32_t res; 
  int      rc;
  rc = _device->read_register(0x9100,100,&res); printf("DTC status       : 0x%08x rc:%i\n",res,rc); // expect: 0x40808404
  rc = _device->read_register(0x91c8,100,&res); printf("debug packet type: 0x%08x rc:%i\n",res,rc); // expect: 0x00000000
}

//-----------------------------------------------------------------------------
void mu2e::TrackerVST::monica_digi_clear(DTCLib::DTC* Dtc) {
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
void mu2e::TrackerVST::monica_var_link_config(DTCLib::DTC* Dtc) {
  mu2edev* dev = Dtc->GetDevice();

  dev->write_register(0x91a8,100,0);                                     // disable event window marker - set deltaT = 0
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeDTC));

  Dtc->WriteROCRegister(DTCLib::DTC_Link_0,14,     1,false,1000);        // reset ROC
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeROCReset));

  Dtc->WriteROCRegister(DTCLib::DTC_Link_0, 8,0x030f,false,1000);        // configure ROC to read all 4 lanes
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeROC));
}


//-----------------------------------------------------------------------------
void mu2e::TrackerVST::monica_var_pattern_config(DTC* Dtc) {
  TLOG(TLVL_DEBUG) << "---------------------------------- operation \"var_patern_config\"" << std::endl;

  //  auto startTime = std::chrono::steady_clock::now();

  if (_debugLevel > 0) print_dtc_registers(Dtc,"var_pattern_config 001");  // debug

  mu2edev* dev = Dtc->GetDevice();

  dev->ResetDeviceTime();

  dev->write_register(0x91a8,100,0);                            // disable event window marker - set deltaT = 0
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeDTC));

  int tmo_ms    (1500);  // 1500 is OK

  Dtc->WriteROCRegister(DTC_Link_0,14,0x01,false,tmo_ms);  // this seems to be a reset
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeROCReset));

  Dtc->WriteROCRegister(DTC_Link_0, 8,0x10,false,tmo_ms);
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeROC));

  Dtc->WriteROCRegister(DTC_Link_0,30,0x00,false,tmo_ms);
  std::this_thread::sleep_for(std::chrono::microseconds(_sleepTimeROC));

   if (_debugLevel > 0) print_dtc_registers(Dtc,"var_pattern_config 002");  // debug
}
//-----------------------------------------------------------------------------
// print 16 bytes per line
// size - number of bytes to print, even
//-----------------------------------------------------------------------------
void mu2e::TrackerVST::printBuffer(const void* ptr, int sz) {

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


// The following macro is defined in artdaq's GeneratorMacros.hh header
DEFINE_ARTDAQ_COMMANDABLE_GENERATOR(mu2e::TrackerVST)
