// Ed Callaghan
// Wrap the precious heart in a mutexed singleton to keep it safe
// March 2026

#include "otsdaq-mu2e-tracker/Ui/SharedDtcInterface.hh"

SharedDtcInterface::SharedDtcInterface(DTCLib::DTC* dtc):
		DtcInterface::DtcInterface(dtc),
		_initialized(false){
	/**/
}

bool SharedDtcInterface::PostInitialize(const trkdaq::DtcInterface::DtcConfiguration_t& config){
	bool rv = false;
	std::lock_guard lock(_mutex);
	if (!_initialized){
		static_cast<trkdaq::DtcInterface*>(this)->PostInitialize(config);
		_initialized = true;
		rv = true;
	}
	return rv;
}
