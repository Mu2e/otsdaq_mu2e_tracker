#ifndef _ots_ROCTrackerInterface_h_
#define _ots_ROCTrackerInterface_h_

#include <fstream>
#include <iostream>
#include <string>

#include "otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterface.h"
#include "otsdaq/DataManager/DataProducer.h"
#include "otsdaq/FECore/FEProducerVInterface.h"

#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"
#include "otsdaq-mu2e-tracker/Ui/ControlRocTypes.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/Alignment.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/PrintLegacyTable.hh"
#include "artdaq-core-mu2e/Overlays/Decoders/TrackerDataDecoder.hh"


namespace ots
{
class ROCTrackerInterface : public ROCPolarFireCoreInterface
{
	// clang-format off
public:
	ROCTrackerInterface(const std::string &rocUID,
						const ConfigurationTree &theXDAQContextConfigTree,
						const std::string &interfaceConfigurationPath);

	~ROCTrackerInterface(void);

	// state machine
	//----------------
	void 									configure				(void) override;
	void 									start					(std::string runNumber) override;
	void 									stop					(void) override;
	bool 									running					(void) override;

	// write and read to registers
	virtual void 							writeEmulatorRegister	(uint16_t address, uint16_t data_to_write) override;
	virtual uint16_t						readEmulatorRegister	(uint16_t address) override;
	virtual void							readEmulatorBlock	(std::vector<uint16_t>& data, uint16_t address, uint16_t wordCount, bool incrementAddress) override;


	//------------ for tracker-specific Ui functions
	int          fEnabled = 1;   // if comes from ODB, could be 0
	int          fPcieAddr = 0;  //
	int          fLinkMask = 0;  // int is OK, bit 31 is never used for arithmetics
	                         // for now assume that all ROCs are doing the same
	                         // fRocReadoutMode: (fixed_length << 4) | readout_mode
	int fRocReadoutMode = 0;     // 0: 'counter patterns' 1:digis 2:checkerboard patterns
	int fRocLaneMask = 0xf;        // 0xf : all of them
	int fRocNHitsPerLane = 2;    // NHits per lane for Mode=2
	int fSampleEdgeMode = 1;     // 0:force raising 1:force falling 2:auto
	int fEmulateCfo = 1;         // 1: this DTC operated in the emulated CFO mode
	int fJAMode = 0x01;             // clock_source << 4 | reset

	int fOnSpill = 0;    // 1:on-spill, 0:off-spill
	int fEventMode = 1;  // whatever it is, hopefully, together they make 5 bytes

	int fDtcID = 0;  // unique DTC ID used by the DAQ (0x9154)
	int fPartitionID = 0;
	int fMacAddrByte = 0;

	int fSleepTimeROCWrite = 0;  // the two are different
	int fSleepTimeROCReset = 0;  //



	struct RocDataHeaderPacket_t
	{  // 8 16-byte words in total
	// 16-bit word 0
		uint16_t byteCount : 16;
		// 16-bit word 1
		uint16_t unused : 4;
		uint16_t packetType : 4;
		uint16_t linkID : 3;
		uint16_t DtcErrors : 4;
		uint16_t valid : 1;
		// 16-bit word 2
		uint16_t packetCount : 11;
		uint16_t unused2 : 2;
		uint16_t subsystemID : 3;
		// 16-bit words 3-5
		uint16_t eventTag[3];
		// 16-bit word 6
		uint8_t status : 8;
		uint8_t version : 8;
		// 16-bit word 7
		uint8_t dtcID : 8;
		uint8_t onSpill : 1;
		uint8_t subrun : 2;
		uint8_t eventMode : 5;

		ulong ewtag()
		{
			ulong x1  = eventTag[0];
			ulong x2  = eventTag[1];
			ulong x3  = eventTag[2];
			ulong ewt = x1 | (x2 << 16) | (x3 << 32);
			return ewt;
		}

		// decoding status

		int empty() { return (status & 0x01) == 0; }
		int invalid_dr() { return (status & 0x02); }
		int corrupt() { return (status & 0x04); }
		int timeout() { return (status & 0x08); }
		int overflow() { return (status & 0x10); }

		int error_code() { return (status & 0x1e); }
	};

	struct RocData_t
	{  // 8 16-byte words in total
		RocDataHeaderPacket_t header;
		uint16_t              data[1];
	};

	// UI_DEFINE_ROCTRACKERINTERFACE_FUNCTIONS

	#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface_Ui.hxx"
	#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface_Ui_declareFEMacros.hxx"

	//------------ end for tracker-specific Ui functions



	// For injection pulse readout
	//----------------
	//
	enum {
		REG_CONFIG      			= 8,
		REG_ROC_RESET      			= 14,
		REG_DIGI_SETUP_CHECK		= 18,
		REG_DATA_VERSION   			= 29,
		REG_UP_DONE       			= 128,
		REG_BLOCK_WRITE_CHECK      	= 129,
		REG_READSPI       			= 258,
		REG_DIGIRW        			= 263,
		REG_FINDALIGNMENT 			= 264,
		REG_READ          			= 265,
		REG_SETGAIN       			= 266,
		REG_SET_THR       			= 267,
		REG_PULSERON      			= 268,
		REG_PULSEROFF     			= 269,
		REG_MEAS_THR      			= 270,
		REG_READRATES     			= 271,
		REG_READGITCOMMIT 			= 272,
		REG_READILP       			= 273,
		REG_GETKEY        			= 274,
	};

	struct ControlRoc_Read_Input_t0 {
		uint16_t    adc_mode;               // -a 8   (defailt:  0)  [0]                   // *v2*
		uint16_t    tdc_mode;               // -t 8   (default:  0)  [1]                   // *v2*
		uint16_t    num_lookback;           // -l 8   (default:  8)  [2]                   // *v2*
		uint16_t    num_samples;            // -s 1   (default: 16)  [3] if>63, set to 63  // *v2*

		uint16_t    num_triggers[2];        // -T 10  (default:  0)  [4-5]                 // *v2*
		uint16_t    ch_mask[6];             // FFFF FFFF FFFF FFFF FFFF FFFF [6:11]        // *v2*

		uint16_t    enable_pulser;          // -p 1     (default: 0) [12]                  // *v2*

		uint16_t    marker_clock;           // -m 3 ??? )default: 0) [13]                  // *v2*
		uint16_t    mode;                   // [14] need to set mode=0                     // *v2*
		uint16_t    clock;                  // [15] need to set clock=99                   // *v2*
	};

	void 			dtc_control_roc_read(	int      LinkMask     = -1,
											int      AdcMode      = 0,
											int      TdcMode      = 0,
											int      NumLookback  = 0,
											int      EnablePulser = 0,
											int      MarkerClock  = 3,
											int      NumSamples   = 1,
											uint32_t MaskC        = 0xFFFFFFFF,
											uint32_t MaskD        = 0xFFFFFFFF,
											uint32_t MaskE        = 0xFFFFFFFF,
											int      PcieAddr     = -1);


    void         	ControlRoc_Read   (		ControlRoc_Read_Input_t0* Par        = nullptr,
											int                       LinkMask   = -1   ,
											int                       PrintLevel = 0    ,
											std::ostream&             Stream     = std::cout);

	// end For injection pulse readout
	//----------------


	bool emulatorWorkLoop(void) override;

	enum TrackerRegisters
	{

		ROC_ADDRESS_DDRRESET                 = 14,
		ROC_ADDRESS_ANALOGRESET              = 13,
		ROC_ADDRESS_IS_PATTERN               = 8,

		ROC_ADDRESS_ERRCNT                   = 17,


		ROC_ADDRESS_WORKMODE                 = 122,

		//ROC_ADDRESS_EW_LENGHT                = 123,
		//ROC_ADDRESS_EW_BLIND                 = 124,

		ROC_ADDRESS_ON_EW_SIZE                = 123,
		ROC_ADDRESS_ON_EW_BLIND               = 124,

		ROC_ADDRESS_OFF_EW_SIZE                = 512,
		ROC_ADDRESS_OFF_EW_BLIND               = 513,

		ROC_ADDRESS_EW_DELAY                 = 125,

		ROC_ADDRESS_MASK_A                   = 120,
		ROC_ADDRESS_MASK_B                   = 121,
		ROC_ADDRESS_BASE_THRESHOLD           = 100,

		ROC_ADDRESS_IS_COUNTER               = 79,
		ROC_ADDRESS_COUNTER_IS_FALLING       = 80,
		ROC_ADDRESS_COUNTER_SIZE             = 81,

		ROC_ADDRESS_IS_LASER                 = 78,
		ROC_ADDRESS_LASER_DELAY              = 77,

		ROC_ADDRESS_LOOPBACK_GROSS_DELAY     = 4

	};

	//	temperature--
	class Thermometer {
		private:
			double mnoiseTemp;

		public:
			void noiseTemp(double intemp) {
				mnoiseTemp = (double)intemp +
				   0.5 * (intemp * ((double)rand() / (RAND_MAX)) - 0.5);
				return;
			}
			double GetBoardTempC() { return mnoiseTemp; }
	};

	Thermometer temp1_;
	double inputTemp_;

	private:
		unsigned int TrackerParameter_1_;
		bool TrackerParameter_2_;

		unsigned int number_of_good_events_;
		unsigned int number_of_bad_events_;
		unsigned int number_of_empty_events_;
		std::ofstream datafile_;
		unsigned int event_number_;

  public:
	// void 			ReadTrackerFIFO			(__ARGS__);

	void 			ReadROCErrorCounter		(__ARGS__);
	virtual void 	GetStatus				(__ARGS__) override;
	void 			SetupForDigiDataTaking	(__ARGS__);
	void 			FindAlignment			(__ARGS__);
	void 			GetUIParameters			(__ARGS__);
	void 			SetUIParameters			(__ARGS__);

	// clang-format on
};

}  // namespace ots

#endif
