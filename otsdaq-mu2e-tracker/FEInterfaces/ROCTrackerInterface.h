#ifndef _ots_ROCTrackerInterface_h_
#define _ots_ROCTrackerInterface_h_

#include <fstream>
#include <iostream>
#include <string>

#include "otsdaq-mu2e/FEInterfaces/ROCPolarFireCoreInterface.h"
#include "otsdaq/DataManager/DataProducer.h"
#include "otsdaq/FECore/FEProducerVInterface.h"

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

	// clang-format on
};

}  // namespace ots

#endif
