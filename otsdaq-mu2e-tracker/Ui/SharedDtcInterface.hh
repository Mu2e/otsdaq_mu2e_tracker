// Ed Callaghan
// Wrap the precious heart in a mutexed singleton to keep it safe
// March 2026

// stl
#include <mutex>

// otsdaq-mu2e-tracker
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

class SharedDtcInterface: protected trkdaq::DtcInterface{
	public:
		SharedDtcInterface(DTCLib::DTC* dtc);

		bool PostInitialize(const trkdaq::DtcInterface::DtcConfiguration_t&);

	protected:
		std::mutex _mutex;
		bool _initialized;

  private:
		/**/
};
