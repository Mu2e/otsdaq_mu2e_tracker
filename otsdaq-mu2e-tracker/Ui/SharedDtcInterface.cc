// Ed Callaghan
// Wrap the precious heart in a mutexed singleton to keep it safe
// March 2026

#include "otsdaq-mu2e-tracker/Ui/SharedDtcInterface.hh"

SharedDtcInterface::SharedDtcInterface(DTCLib::DTC* dtc):
		interface(std::make_unique<trkdaq::DtcInterface>(dtc)),
		_initialized(false){
	/**/
}

bool SharedDtcInterface::PostInitialize(const trkdaq::DtcInterface::DtcConfiguration_t& config){
	bool rv = false;
	std::lock_guard lock(_mutex);
	if (!_initialized){
		_interface->PostInitialize(config);
		_initialized = true;
		rv = true;
	}
	return rv;
}

template<typename... Args>
int SharedDtcInterface::ReadSettings(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_ReadSettings(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::PulserOn(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_PulserOn(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::PulserOff(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_PulserOff(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::Rates(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_Rates(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::Read(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_Read(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::ReadDeviceID(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_ReadDeviceID(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::ReadGitCommit(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_ReadGitCommit(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::ReadIlp(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_ReadIlp(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::GetKey (Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_GetKey (args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::ReadSpi(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_ReadSpi(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::MeasureThresholds(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_MeasureThresholds(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::ReadThresholds(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_ReadThresholds(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::SetCalDac(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_SetCalDac(args...);
	return rv;
}

template<typename... Args>
int SharedDtcInterface::SetThresholds(Args... args){
	std::lock_guard lock(_mutex);
	int rv = _interface->ControlRoc_SetThresholds(args...);
	return rv;
}

template<typename... Args>
bool SharedDtcInterface::FindThreshold(Args... args){
	std::lock_guard lock(_mutex);
	bool rv = _interface->FindThreshold(args...);
	return rv;
}

template<typename... Args>
float SharedDtcInterface::ProgramAndQueryThreshold(Args... args){
	std::lock_guard lock(_mutex);
	float rv = _interface->ProgramAndQueryThreshold(args...);
	return rv;
}

template<typename... Args>
std::vector<std::string> SharedDtcInterface::GetRocRegistersNames(Args... args){
	std::lock_guard lock(_mutex);
	std::vector<std::string> rv = _interface->GetRocRegistersNames(args...);
	return rv;
}

template<typename... Args>
std::vector<uint32_t> SharedDtcInterface::GetRocRegisters(Args... args){
	std::lock_guard lock(_mutex);
	std::vector<uint32_t> rv = _interface->GetRocRegisters(args...);
	return rv;
}

template<typename... Args>
std::vector<float> SharedDtcInterface::GetConvertedRocRegisters(Args... args){
	std::lock_guard lock(_mutex);
	std::vector<float> rv = _interface->GetConvertedRocRegisters(args...);
	return rv;
}

template<typename... Args>
std::string SharedDtcInterface::GetRocID(Args... args){
	std::lock_guard lock(_mutex);
	std::string rv = _interface->GetRocID(args...);
	return rv;
}

template<typename... Args>
std::string SharedDtcInterface::GetRocDesignInfo(Args... args){
	std::lock_guard lock(_mutex);
	std::string rv = _interface->GetRocDesignInfo(args...);
	return rv;
}

template<typename... Args>
std::string SharedDtcInterface::GetRocFwGitCommit(Args... args){
	std::lock_guard lock(_mutex);
	std::string rv = _interface->GetRocFwGitCommit(args...);
	return rv;
}
