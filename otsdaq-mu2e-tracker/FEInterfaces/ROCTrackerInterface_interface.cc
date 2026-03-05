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
    // , trackerDTC_(std::make_unique<trkdaq::DtcInterface>(getDTC()))
{
	INIT_MF("." /*directory used is USER_DATA/LOG/.*/);

	__COUT_INFO__ << "ROCTrackerInterface instantiated with link: " << linkID_
	              << " and EventWindowDelayOffset = " << delay_ << __E__;

	__CFG_COUT__ << "Constructor..." << __E__;

	try
	{
		ConfigurationTree rocTypeLink =
		    Configurable::getSelfNode().getNode("ROCTypeLinkTable");

		TrackerParameter_1_ = rocTypeLink.getNode("NumberParam1").getValue<int>();

		TrackerParameter_2_ = rocTypeLink.getNode("TrueFalseParam2").getValue<bool>();

		__FE_COUTV__(TrackerParameter_1_);
		__FE_COUTV__(TrackerParameter_2_);
	}
	catch(const std::runtime_error& e)
	{
		__COUT__ << "Ignoring error testing Tracker subsystem parameters: " << e.what()
		         << __E__;
	}

	try
	{
		inputTemp_ = getSelfNode().getNode("inputTemperature").getValue<double>();
	}
	catch(...)
	{
		__CFG_COUT__ << "inputTemperature field not defined. Defaulting..." << __E__;
		inputTemp_ = 15.;
	}

	temp1_.noiseTemp(inputTemp_);

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
	         // registerFEMacroFunction(
	         //     "ReadROCTrackerFIFO",
	         //     static_cast<FEVInterface::frontEndMacroFunction_t>(
	         //         &ROCTrackerInterface::ReadTrackerFIFO),
	//     std::vector<std::string>{"NumberOfTimesToReadFIFO"},  // inputs parameters
	//     std::vector<std::string>{},                           // output parameters
	//     1);                                                   //
	//     requiredUserPermissions

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

#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface_Ui_ControlRoc_registerFEMacros.icc"
#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface_Ui_ProgramRoc_registerFEMacros.icc"
#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface_Ui_base_registerFEMacros.icc"
#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface_Ui_print_registerFEMacros.icc"
#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface_Ui_registerFEMacros.icc"

}  // end constructor

// void ROCTrackerInterface::ReadTrackerFIFO(__ARGS__)
// {
// 	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
// 	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;
// 	for(auto& argIn : argsIn)
// 		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

// 	// macro commands section

// 	__FE_COUT__ << "# of input args = " << argsIn.size() << __E__;
// 	__FE_COUT__ << "# of output args = " << argsOut.size() << __E__;

// 	for(auto& argIn : argsIn)
// 		__FE_COUT__ << argIn.first << ": " << argIn.second << __E__;

// 	uint8_t NumberOfTimesToReadFIFO = __GET_ARG_IN__("NumberOfTimesToReadFIFO", uint8_t);

// 	__FE_COUTV__(NumberOfTimesToReadFIFO);

// 	for(unsigned i = 0; i < NumberOfTimesToReadFIFO; i++)
// 	{

// 		unsigned FIFOdepth = 0;
// 		unsigned counter   = 0;  // don't wait forever

// 		while(FIFOdepth <= 0 && counter < 1000)
// 		{
// 			if(counter % 100 == 0)
// 				__FE_COUT__ << "... waiting for non-zero depth" << __E__;
// 			FIFOdepth = readRegister(0x35);
// 			counter++;
// 		}

// 	}

// 	for(auto& argOut : argsOut)
// 		__FE_COUT__ << argOut.first << ": " << argOut.second << __E__;
// }

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

//==============================================================================
void ROCTrackerInterface::start(std::string runNumber)
{
	std::cout << "TRK Inside start, opening data file " << std::endl;
	std::stringstream filename;
	filename << "/home/mu2etrk/test_stand/ots/Run" << runNumber << ".txt";
	std::string filenamestring = filename.str();
	datafile_.open(filenamestring);
	std::cout << "TRK Data file opened " << filenamestring << std::endl;

	number_of_good_events_  = 0;
	number_of_bad_events_   = 0;
	number_of_empty_events_ = 0;
	event_number_           = 0;

	//	DataProducerBase::registerToBuffer();

	return;
}

//==============================================================================
bool ROCTrackerInterface::running(void)
{
	event_number_++;

	if(event_number_ % 1000 == 0)
	{
		__FE_COUT_INFO__ << "Running event number " << std::dec << event_number_ << __E__;
	}

	// make sure we have an empty buffer to put data in.
	// we may want to try a few times, then return an error

	//	if(DataProducerBase::attachToEmptySubBuffer(dataP_, headerP_) < 0)
	//{
	//__CFG_COUT__ << "There are no available buffers! Retrying...after "
	//			                "waiting 10 milliseconds!"
	//			             << std::endl;
	// usleep(10000);
	// return true;
	// }

	//
	std::vector<uint16_t> val;

	// is this really what we want? How do we know how much to read in readblock?
	unsigned FIFOdepth = 0;
	FIFOdepth          = readRegister(35);

	std::cout << "TRK FIFOdepth " << FIFOdepth << " Event number " << event_number_
	          << std::endl;

	unsigned counter = 0;  // don't wait forever

	while((FIFOdepth == 65535 || FIFOdepth == 0) && counter < 1000)
	{
		readRegister(6);
		if(counter % 100 == 0)
		{
			__FE_COUT__ << "... waiting for non-zero depth" << __E__;
		}
		FIFOdepth = readRegister(35);
		counter++;
	}

	if(FIFOdepth > 0 && FIFOdepth != 65535)
	{
		// this is actually DCS values, I think. I'm not sure.

		//
		//		readBlock(val, 42 , FIFOdepth, 0);
		//
		//		std::string* buffer;
		//		buffer = FEVInterface::getNextBuffer();
		//
		//		//std::string        buffer;
		//		buffer->resize(8);  // NOTE: this is inexpensive according to
		//		                   // Lorenzo/documentation in C++11 (only increases size
		// once
		//		                   // and doesn't decrease size)
		//		memcpy((void*)buffer /*dest*/, (void*)&val /*src*/, FIFOdepth
		///*numOfBytes*/);
		//
		//	    	// size() and length() are equivalent
		//		__FE_COUT__ << "Writing to buffer " << buffer->size() << " bytes!" <<
		//__E__;
		//		__FE_COUT__ << "Writing to buffer length " << buffer->length() << "
		// bytes!"
		//		            << __E__;
		//
		//		__FE_COUT__ << "Buffer Data: "
		//		            << BinaryStringMacros::binaryNumberToHexString(*buffer) <<
		//__E__;
		//
		//		FEVInterface::copyToNextBuffer(*buffer);
	}
	else
	{
		number_of_empty_events_++;
	}

	if(0)
	{
		unsigned data_to_check = readRegister(0x6);

		while(data_to_check != 4860)
		{
			data_to_check = readRegister(0x6);
		}

		data_to_check = readRegister(0x7);

		while(data_to_check != delay_)
		{
			data_to_check = readRegister(0x7);
		}
	}

	return false;
}

void ROCTrackerInterface::stop()  // runNumber)
{
	//  __FE_COUTV__(number_of_good_events_);
	//  __FE_COUTV__(number_of_bad_events_);

	__COUT__ << "RUN END" << __E__;
	__COUT__ << "--> number of good events = " << number_of_good_events_ << __E__;
	__COUT__ << "--> number of bad events = " << number_of_bad_events_ << __E__;
	__COUT__ << "--> number of empty events = " << number_of_empty_events_ << __E__;
	// int startIndex = getIterationIndex();

	// indicateIterationWork();  // I still need to be touched

	datafile_ << "RUN END" << std::endl;
	datafile_ << "--> number of good events = " << number_of_good_events_ << std::endl;
	datafile_ << "--> number of bad events = " << number_of_bad_events_ << std::endl;
	datafile_ << "--> number of empty events = " << number_of_empty_events_ << std::endl;

	datafile_.close();

	return;
}

//==================================================================================================
// return false to stop workloop thread
bool ROCTrackerInterface::emulatorWorkLoop(void)
{
	//__CFG_COUT__ << "emulator working..." << __E__;

	temp1_.noiseTemp(inputTemp_);
	return true;  // true to keep workloop going

	//	float input, inputTemp;
	//	int addBoard, a;
	//	//
	//	addBoard = 105;
	//	inputTemp = 15.;
	//	a = 0;
	//	while( a < 20 ) {
	//		temp1.noiseTemp(inputTemp);
	//		temperature = temp1.GetBoardTempC();
	//		a++;
	//		return temperature;
	//		usleep(1000000);
	//		return true;
	//	}
}  // end emulatorWorkLoop()

//==================================================================================================
// Copied from Calorimter on
void ROCTrackerInterface::ReadROCErrorCounter(__ARGS__)
{
	__COUT_INFO__ << "ReadROCErrorCounter()" << __E__;

	unsigned int errAddr = __GET_ARG_IN__("Address to read, Default := 0]", uint16_t, 0);
	__FE_COUTV__(errAddr);

	writeRegister(ROC_ADDRESS_ERRCNT, errAddr);
	writeRegister(ROC_ADDRESS_IS_PATTERN, 64);

	std::stringstream  os;
	DTCLib::roc_data_t readVal;
	readVal = readRegister(ROC_ADDRESS_ERRCNT);

	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << errAddr
	   << " (" << std::dec << errAddr << std::hex << "): data 0x" << readVal << " ("
	   << std::dec << readVal << ")\n"
	   << __E__;

	writeRegister(ROC_ADDRESS_IS_PATTERN, 0);

	__COUT_INFO__ << "end ReadROCErrorCounter()" << __E__;

	__SET_ARG_OUT__("Status", os.str());

}  // end ReadROCErrorCounter()

//==================================================================================================
void ROCTrackerInterface::GetStatus(__ARGS__)
{
	// copied from Monica's va_read_all.sh

	DTCLib::roc_data_t readVal;

	std::stringstream     os;
	DTCLib::roc_address_t address;

	address = 0x0;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << address
	   << " (" << std::dec << address << std::hex << "): data 0x" << readVal << " ("
	   << std::dec << readVal << ")\n"
	   << __E__;

	address = 0x8;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << address
	   << " (" << std::dec << address << std::hex << "): data 0x" << readVal << " ("
	   << std::dec << readVal << ")" << __E__;
	os << "\t\t"
	   << "bit[9:8]=[enable_marker,enable_clock]"
	      "\n\t\t bit[7:4]=[en_int_ewm,en_free_ewm,error_en,pattern_en]"
	      "\n\t\t bit[3:0]=en_lanes[HV1,HV0,CAl1,CAL0]\n"
	   << __E__;

	address = 18;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << address
	   << " (" << std::dec << address << std::hex << "): data 0x" << readVal << " ("
	   << std::dec << readVal << ")" << __E__;
	os << "\t\t"
	   << "bit[9:8]=[enable_marker,enable_clock]"
	      "\n\t\t bit[7:4]=[en_int_ewm,en_free_ewm,error_en,pattern_en]"
	      "\n\t\t bit[3:0]=en_lanes[HV1,HV0,CAl1,CAL0]\n"
	   << __E__;

	address = 72;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << address
	   << " (" << std::dec << address << std::hex << "): hbtag error 0x" << readVal
	   << " (" << std::dec << readVal << ") \n"
	   << __E__;

	address = 73;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << address
	   << " (" << std::dec << address << std::hex << "): dreq error 0x" << readVal << " ("
	   << std::dec << readVal << ") \n"
	   << __E__;

	address = 74;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << address
	   << " (" << std::dec << address << std::hex << "): hblost 0x" << readVal << " ("
	   << std::dec << readVal << ") \n"
	   << __E__;

	address = 75;
	readVal = readRegister(address);
	os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x" << address
	   << " (" << std::dec << address << std::hex << "): evm lost 0x" << readVal << " ("
	   << std::dec << readVal << ") \n"
	   << __E__;

	uint32_t doubleRegVal = 0;

	std::vector<DTCLib::roc_address_t> doubleReads = {
	    23, 25, 64, 27, 29, 31, 33, 35, 37, 39, 41, 43, 45, 48, 51, 54, 57};

	std::vector<std::string> doubleReadCaptions = {
	    "SIZE_FIFO_FULL[28]+STORE_POS[25:24]+STORE_CNT[19:0]",   // 23,
	    "SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]",  // 25,
	    "no. EVM seen",                                          // 64,
	    "no. HB seen",                                           // 27,
	    "no. null HB seen:",                                     // 29,
	    "no. HB on hold",                                        // 31,
	    "no. PREFETCH seen",                                     // 33,
	    "no. DATA REQ seen",                                     // 35,
	    "no. DATA REQ read from DDR",                            // 37,
	    "no. DATA REQ sent to DTC",                              // 39,
	    "no. DATA REQ with null data",                           // 41,
	    "last SPILL TAG",                                        // 43,
	    "last HB tag",                                           // 45,
	    "last PREFETCH tag",                                     // 48,
	    "last FETCHED tag",                                      // 51,
	    "last DATA REQ tag",                                     // 54,
	    "OFFSET tag",                                            // 57
	};

	for(size_t i = 0; i < doubleReads.size(); ++i)
	{
		address      = doubleReads[i];
		readVal      = readRegister(address);
		doubleRegVal = readVal;
		readVal      = readRegister(++address);
		doubleRegVal |= readVal << 16;

		os << std::hex << std::setprecision(4) << std::setfill('0') << "address 0x"
		   << address - 1 << " (" << std::dec << address - 1 << std::setprecision(8)
		   << std::hex << "): data 0x" << doubleRegVal << " (" << std::dec << doubleRegVal
		   << ")" << __E__;
		os << "\t\t" << doubleReadCaptions[i] << "\n" << __E__;
	}  // end double read register loop

	__SET_ARG_OUT__("Status", os.str());

}  // end GetStatus()

//==================================================================================================
void ROCTrackerInterface::dtc_control_roc_read(int      LinkMask,
                                               int      AdcMode,
                                               int      TdcMode,
                                               int      NumLookback,
                                               int      EnablePulser,
                                               int      MarkerClock,
                                               int      NumSamples,
                                               uint32_t MaskC,
                                               uint32_t MaskD,
                                               uint32_t MaskE,
                                               int      PcieAddr)
{
	// DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

	ControlRoc_Read_Input_t0 par;

	par.adc_mode     = AdcMode;      // -a
	par.tdc_mode     = TdcMode;      // -t
	par.num_lookback = NumLookback;  //

	par.num_samples     = NumSamples;  // -s
	par.num_triggers[0] = 10;          // -T 10
	par.num_triggers[1] = 0;           // -T (high bytes)

	par.ch_mask[0] = (MaskC >> 0) & 0xffff;
	par.ch_mask[1] = (MaskC >> 16) & 0xffff;
	par.ch_mask[2] = (MaskD >> 0) & 0xffff;
	par.ch_mask[3] = (MaskD >> 16) & 0xffff;
	par.ch_mask[4] = (MaskE >> 0) & 0xffff;
	par.ch_mask[5] = (MaskE >> 16) & 0xffff;

	par.enable_pulser = EnablePulser;  // -p 1
	par.marker_clock  = MarkerClock;   // -m 3
	par.mode          = 0;             //
	par.clock         = 99;            //

	// printf("dtc_i->fLinkMask: 0x%04x\n",dtc_i->fLinkMask);
	int print_level(3);

	// dtc_i->ControlRoc_Read(&par,LinkMask,print_level);
	ControlRoc_Read(&par, LinkMask, print_level);
}  // end dtc_control_roc_read()

void ROCTrackerInterface::ControlRoc_Read(ControlRoc_Read_Input_t0* Par,
                                          int                       Link,
                                          int                       PrintLevel,
                                          std::ostream&             Stream)
{
	//-----------------------------------------------------------------------------
	// write parameters into reg 266 (via block write), sleep for some time,
	// then wait till reg 128 returns 0x8000
	/*
	            adc_mode = dtcbuffer[0];                                  // -a
	            tdc_mode = dtcbuffer[1];                                  // -t
	            num_lookback = dtcbuffer[2];                              // -l
	            num_triggers = (dtcbuffer[4] << 16) + dtcbuffer[3];         // -T
	            channel_mask[0] = (dtcbuffer[6] << 16) + dtcbuffer[5];    // -C
	            channel_mask[1] = (dtcbuffer[8] << 16) + dtcbuffer[7];    // -D
	            channel_mask[2] = (dtcbuffer[10] << 16) + dtcbuffer[9];    // -E
	            num_samples = dtcbuffer[11];                     // -s
	            enable_pulser = (uint8_t) dtcbuffer[12];         // -p
	            max_total_delay = dtcbuffer[13];                 // -d (def 1)
	            marker_clock = (uint8_t) dtcbuffer[14];          // -m
	*/
	//-----------------------------------------------------------------------------
	//    const int  reg (265);  // for control_ROC.py(read)
	std::vector<uint16_t> vec;

	TLOG(TLVL_DEBUG) << "Link: 0x" << std::hex << Link << std::dec
	                 << " PrintLevel:" << PrintLevel;

	if(Par == nullptr)
	{
		// reasonable defaults, to run w/o passing anything
		uint16_t adc_mode(0), tdc_mode(0), num_lookback(0), num_samples(1);
		uint16_t num_triggers[2] = {10, 0};
		uint16_t ch_mask[6]      = {0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff};
		uint16_t enable_pulser(0), marker_clock(3), mode(0), clock(99);

		vec.push_back(adc_mode);
		vec.push_back(tdc_mode);
		vec.push_back(num_lookback);
		vec.push_back(num_samples);
		vec.push_back(num_triggers[0]);
		vec.push_back(num_triggers[1]);
		for(int i = 0; i < 6; i++)
			vec.push_back(ch_mask[i]);
		vec.push_back(enable_pulser);
		vec.push_back(marker_clock);
		vec.push_back(mode);
		vec.push_back(clock);
	}
	else
	{
		vec.push_back(Par->adc_mode);
		vec.push_back(Par->tdc_mode);
		vec.push_back(Par->num_lookback);

		if(Par->num_samples > 63)
		{
			TLOG(TLVL_WARNING) << "num_samples:" << Par->num_samples
			                   << " gt 63, truncate to 63";
			Par->num_samples = 63;
		}

		vec.push_back(Par->num_samples);

		uint16_t w1 = Par->num_triggers[0];
		uint16_t w2 = Par->num_triggers[1];

		vec.push_back(w1);
		vec.push_back(w2);

		for(int i = 0; i < 6; i++)
			vec.push_back(Par->ch_mask[i]);

		vec.push_back(Par->enable_pulser);
		vec.push_back(Par->marker_clock);
		vec.push_back(Par->mode);
		vec.push_back(Par->clock);
	}

	bool increment_address(false);

	// // //-----------------------------------------------------------------------------
	// // // if LinkMask != -1, use it
	// // // in addition, if UpdateMask=true, update the DTC link mask (fLinkMask)
	// // //-----------------------------------------------------------------------------
	// // int link_mask = fLinkMask;
	// // if (Link != -1) {
	// //   	link_mask = (1 << 4*Link);
	// // }

	// // for (int i=0; i<6; i++)
	// {
	// 	int used = (link_mask >> 4*i) & 0x1;
	// 	if (not used)
	// 	  continue;

	{
		// auto roc  = DTC_Link_ID(i);
		// fDtc->WriteROCBlock   (roc,REG_READ,vec,false,increment_address,100);
		writeBlock(vec, REG_READ, increment_address);
		// std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

		// 0x86 = 0x82 + 4
		uint16_t u;
		// while ((u = fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {};
		while((u = readRegister(REG_UP_DONE)) != 0x8000)
		{
			usleep(10);
		};

		// TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
		__FE_COUT__ << std::hex << "Reg: 0x" << REG_UP_DONE << " Value: 0x" << u << __E__;

		//-----------------------------------------------------------------------------
		// register 129: number of words to read, currently-  (+ 4) (ask Monica)
		//-----------------------------------------------------------------------------
		// int nw = fDtc->ReadROCRegister(roc,129,100);
		int nw = readRegister(REG_BLOCK_WRITE_CHECK);  // readback Number of Words passed
		                                               // to the config block
		// TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",129,nw);
		__FE_COUT__ << std::hex << "Reg: 0x" << REG_BLOCK_WRITE_CHECK << " Value: 0x"
		            << nw << __E__;

		nw = nw - 4;
		std::vector<uint16_t> vout;
		// fDtc->ReadROCBlock(vout,roc,REG_READ,nw,false,100);
		readBlock(vout, REG_READ, nw, increment_address);

		// if (PrintLevel > 0)
		// {
		// 	Stream << "--------------- link :" << i << std::endl;
		// 	if (PrintLevel & 0x1) {
		// 		PrintBuffer(vout.data(),nw,&Stream);
		// 	}

		// 	// if (PrintLevel & 0x2) {
		// 	// 	trkdaq::ControlRoc_Read_Output_t0* o =
		// (trkdaq::ControlRoc_Read_Output_t0*) vout.data();
		// 	// 	Stream << Form("adc_mode      : %i\n",o->adc_mode);
		// 	// 	Stream << Form("tdc_mode      : %i\n",o->tdc_mode);
		// 	// 	Stream << Form("num_lookback  : %i\n",o->num_lookback);
		// 	// 	Stream << Form("num_triggers  : %5i
		// %5i\n",o->num_triggers[0],o->num_triggers[1]);
		// 	// 	Stream << Form("ch_mask       : 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x
		// 0x%04x\n",
		// 	// 					o->ch_mask[0],o->ch_mask[1],o->ch_mask[2],
		// 	// 					o->ch_mask[3],o->ch_mask[4],o->ch_mask[5]);
		// 	// 	Stream << Form("num_samples   : %i\n"     ,o->num_samples);
		// 	// 	Stream << Form("enable_pulser : %i\n"    ,o->enable_pulser);
		// 	// 	Stream << Form("marker_clock  : %i\n"    ,o->marker_clock);
		// 	// 	Stream << Form("mode          : %i\n"    ,o->mode);
		// 	// 	Stream << Form("clock         : %i\n"    ,o->clock);
		// 	// 	Stream << Form("digi_read_0xb : 0x%04x\n",o->digi_read_0xb);
		// 	// 	Stream << Form("digi_read_0xe : 0x%04x\n",o->digi_read_0xe);
		// 	// 	Stream << Form("digi_read_0xd : 0x%04x\n",o->digi_read_0xd);
		// 	// 	Stream << Form("digi_read_0xc : 0x%04x\n",o->digi_read_0xc);
		// 	// }
		// }
	}
	//-----------------------------------------------------------------------------
	//  is it really needed to reser the ROC in the end ? - no
	//-----------------------------------------------------------------------------
	// ResetLinks();
	// return 0;

	__FE_COUT__ << "Done" << __E__;
}  // end ControlRoc_Read()

//==================================================================================================
// Copied from Calorimter on
void ROCTrackerInterface::SetupForDigiDataTaking(__ARGS__)
{
	__FE_COUT_INFO__ << "SetupForDigiDataTaking()" << __E__;

	// Steps
	//  	- Setup reg 0x8
	//	- Block write of config
	//	- Read register 18 and check for 0x00F
	//	- ROC Reset by writing to register 14

	//	- ROC Reset by writing to register 14
	writeRegister(
	    REG_ROC_RESET,
	    1);  // to clear any previous errors/counters and allow the config to proceed

	// 	- Setup reg 0x8	for digi data taking
	writeRegister(REG_CONFIG, 0x230F);
	writeRegister(REG_DATA_VERSION, 1);

	//	- Block write of config
	dtc_control_roc_read();  //,//int      LinkMask    ,
	                         //,//int      AdcMode     ,
	                         //,//int      TdcMode     ,
	                         //,//int      NumLookback ,
	                         //,//int      EnablePulser,
	                         //,//int      MarkerClock ,
	                         //,//int      NumSamples  ,
	                         //,//uint32_t MaskC       ,
	                         //,//uint32_t MaskD       ,
	                         //,//uint32_t MaskE       ,
	                         ////int      PcieAddr )

	//	- Read register 18 and check for 0xF00
	uint16_t checkData = readRegister(REG_DIGI_SETUP_CHECK);
	__FE_COUTV__(checkData);
	if(checkData != 0xF00)
	{
		__FE_SS__ << "Illegal status check after setup for Digi data taking!" << __E__;
		__FE_SS_THROW__;
	}

	__SET_ARG_OUT__("Result", "Done");
	__FE_COUT__ << "Done" << __E__;
}  // end SetupForDigiDataTaking()

//==================================================================================================
// Copied from Calorimter on
void ROCTrackerInterface::FindAlignment(__ARGS__)
{
	__FE_COUT_INFO__ << "FindAlignment()" << __E__;

	// First initialize the alignment

	// write parameters into roc to initiate routine
	std::vector<DTCLib::roc_data_t> writeable = {
	    4,                          // eye-monitor width
	    0,                          // initial adc phase
	    1,                          // flag to check adc patterns
	    static_cast<uint16_t>(-1),  // for channel remapping; unused
	    static_cast<uint16_t>(-1),  // for channel remapping; unused
	    0xFFFF,                     // bitmask for channels  0 - 15
	    0xFFFF,                     // bitmask for channels 16 - 31
	    0xFFFF,                     // bitmask for channels 32 - 47
	    0xFFFF,                     // bitmask for channels 48 - 63
	    0xFFFF,                     // bitmask for channels 64 - 79
	    0xFFFF,                     // bitmask for channels 80 - 95
	};

	// register 264: find alignment routine
	const bool increment_address = false;  // read via fifo
	writeBlock(writeable, REG_READ, increment_address);
	// fDtc->WriteROCBlock(Link, 264, writeable, false, increment_address, 100);

	// std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
	uint16_t u;
	while((u = readRegister(REG_UP_DONE)) != 0x8000)
	{
		usleep(10);
	};

	// // then, wait till reg 128 returns non-zero
	// uint16_t u;
	// while((u = fDtc->ReadROCRegister(Link, 128, 100)) != 0x8000)
	//   {
	//     // idle
	//   }

	int nw = readRegister(
	    REG_BLOCK_WRITE_CHECK);  // readback Number of Words passed to the config block
	__FE_COUT__ << std::hex << "Reg: 0x" << REG_BLOCK_WRITE_CHECK << " Value: 0x" << nw
	            << __E__;
	nw -= 4;

	std::vector<uint16_t> vout;
	readBlock(vout, REG_READ, nw, increment_address);
	// vector<DTCLib::roc_data_t> returned = this->ReadROCBlockEnsured(Link, 264);
	// auto rv = Alignment(returned);
	// return rv;

	// Next enable the pulser and perform the alignment

	// dtc_control_roc_digi_rw(0xFF,1,0,1,-1);
	// dtc_control_roc_read(i,0,0,0,1,0,1);     // enable pulser
	// dtc_control_roc_digi_rw(0x85,1,0,0,i);   // 240 kHz on
	// sleep(1);
	// dtc_control_roc_read(i,0,0,0,0,3,1);     // disable pulser
	// dtc_control_roc_digi_rw(0xFF,1,0,0,-1);	// // return

	writeRegister(0x0F, 1);
	dtc_control_roc_read(-1, 0, 0, 0, 1, 0, 1);
	writeRegister(0x85, 0);
	sleep(1);
	dtc_control_roc_read(-1, 0, 0, 0, 0, 3, 1);
	writeRegister(0x0F, 0);
	__SET_ARG_OUT__("Result", "Done");
	__FE_COUT__ << "Done" << __E__;
}  // end FindAlignment()

//==================================================================================================
void ROCTrackerInterface::GetUIParameters(__ARGS__)
{
	__SET_ARG_OUT__("fEnabled", fEnabled);
	__SET_ARG_OUT__("fPcieAddr", fPcieAddr);
	__SET_ARG_OUT__("fLinkMask", fLinkMask);

	__SET_ARG_OUT__("fRocReadoutMode", fRocReadoutMode);
	__SET_ARG_OUT__("fRocLaneMask", fRocLaneMask);
	__SET_ARG_OUT__("fRocNHitsPerLane", fRocNHitsPerLane);
	__SET_ARG_OUT__("fSampleEdgeMode", fSampleEdgeMode);
	__SET_ARG_OUT__("fEmulateCfo", fEmulateCfo);
	__SET_ARG_OUT__("fJAMode", fJAMode);

	__SET_ARG_OUT__("fOnSpill", fOnSpill);
	__SET_ARG_OUT__("fEventMode", fEventMode);

	__SET_ARG_OUT__("fDtcID", fDtcID);
	__SET_ARG_OUT__("fPartitionID", fPartitionID);
	__SET_ARG_OUT__("fMacAddrByte", fMacAddrByte);

	__SET_ARG_OUT__("fSleepTimeROCWrite", fSleepTimeROCWrite);
	__SET_ARG_OUT__("fSleepTimeROCReset", fSleepTimeROCReset);

	__FE_COUT__ << "Done" << __E__;
}  // end GetUIParameters()

//==================================================================================================
void ROCTrackerInterface::SetUIParameters(__ARGS__)
{
	fEnabled = __GET_ARG_IN__("fEnabled (Default := unchanged)", int, fEnabled);
	fEnabled = __GET_ARG_IN__("fPcieAddr (Default := unchanged)", int, fPcieAddr);
	fEnabled = __GET_ARG_IN__("fLinkMask (Default := unchanged)", int, fLinkMask);
	fEnabled =
	    __GET_ARG_IN__("fRocReadoutMode (Default := unchanged)", int, fRocReadoutMode);
	fEnabled = __GET_ARG_IN__("fRocLaneMask (Default := unchanged)", int, fRocLaneMask);
	fEnabled =
	    __GET_ARG_IN__("fRocNHitsPerLane (Default := unchanged)", int, fRocNHitsPerLane);
	fEnabled =
	    __GET_ARG_IN__("fSampleEdgeMode (Default := unchanged)", int, fSampleEdgeMode);
	fEnabled = __GET_ARG_IN__("fEmulateCfo (Default := unchanged)", int, fEmulateCfo);
	fEnabled = __GET_ARG_IN__("fJAMode (Default := unchanged)", int, fJAMode);
	fEnabled = __GET_ARG_IN__("fOnSpill (Default := unchanged)", int, fOnSpill);
	fEnabled = __GET_ARG_IN__("fEventMode (Default := unchanged)", int, fEventMode);
	fEnabled = __GET_ARG_IN__("fDtcID (Default := unchanged)", int, fDtcID);
	fEnabled = __GET_ARG_IN__("fPartitionID (Default := unchanged)", int, fPartitionID);
	fEnabled = __GET_ARG_IN__("fMacAddrByte (Default := unchanged)", int, fMacAddrByte);
	fEnabled = __GET_ARG_IN__(
	    "fSleepTimeROCWrite (Default := unchanged)", int, fSleepTimeROCWrite);
	fEnabled = __GET_ARG_IN__(
	    "fSleepTimeROCReset (Default := unchanged)", int, fSleepTimeROCReset);

	__SET_ARG_OUT__("fEnabled", fEnabled);
	__SET_ARG_OUT__("fPcieAddr", fPcieAddr);
	__SET_ARG_OUT__("fLinkMask", fLinkMask);

	__SET_ARG_OUT__("fRocReadoutMode", fRocReadoutMode);
	__SET_ARG_OUT__("fRocLaneMask", fRocLaneMask);
	__SET_ARG_OUT__("fRocNHitsPerLane", fRocNHitsPerLane);
	__SET_ARG_OUT__("fSampleEdgeMode", fSampleEdgeMode);
	__SET_ARG_OUT__("fEmulateCfo", fEmulateCfo);
	__SET_ARG_OUT__("fJAMode", fJAMode);

	__SET_ARG_OUT__("fOnSpill", fOnSpill);
	__SET_ARG_OUT__("fEventMode", fEventMode);

	__SET_ARG_OUT__("fDtcID", fDtcID);
	__SET_ARG_OUT__("fPartitionID", fPartitionID);
	__SET_ARG_OUT__("fMacAddrByte", fMacAddrByte);

	__SET_ARG_OUT__("fSleepTimeROCWrite", fSleepTimeROCWrite);
	__SET_ARG_OUT__("fSleepTimeROCReset", fSleepTimeROCReset);

	if(not fInitialized)
	{
		for(int i = 0; i < 96; i++)
		{
			int ich     = adc_index[i];
			int fpga    = i / 48;
			fgFpga[ich] = fpga;
		}

		for(int i = 0; i < trkdaq::TrkSpiDataNWords; i++)
		{
			fgSpiVarName[i] = kSpiVarName[i];
		}
		for(int i = 0; i < trkdaq::TrkKeyDataNWords; i++)
		{
			fgKeyVarName[i] = kKeyVarName[i];
		}
		for(int i = 0; i < trkdaq::TrkIlpDataNWords; i++)
		{
			fgIlpVarName[i] = kIlpVarName[i];
		}

		fInitialized = true;
	}

	__FE_COUT__ << "Done" << __E__;
}  // end SetUIParameters()

DEFINE_OTS_INTERFACE(ROCTrackerInterface)
