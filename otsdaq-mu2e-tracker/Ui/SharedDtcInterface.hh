// Ed Callaghan
// Wrap the precious heart in a mutexed singleton to keep it safe
// March 2026

#ifndef __trkdaq_shared_dtc_interface_hh__
#define __trkdaq_shared_dtc_interface_hh__

// stl
#include <map>
#include <mutex>

// otsdaq-mu2e-tracker
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

namespace trkdaq{
	class SharedDtcInterface{
		public:
			// real constructor --- should hide, tbd
			SharedDtcInterface(DTCLib::DTC*);
			// lazy constructor
			static std::shared_ptr<SharedDtcInterface>& Get(DTCLib::DTC*);

			bool PostInitialize(const trkdaq::DtcInterface::DtcConfiguration_t&);

			// below are thin forwards automagically completed by variadic templates

			// basics
			template<typename... Args>
			uint16_t ReadROCRegister(Args&&... args);
			template<typename... Args>
			int ResetLink(Args&&... args);

			// ControlROC paradigm
			template<typename... Args>
			int ReadSettings(Args&&... args);
			template<typename... Args>
			int PulserOn(Args&&... args);
			template<typename... Args>
			int PulserOff(Args&&... args);
			template<typename... Args>
			int Rates(Args&&... args);
			template<typename... Args>
			int Read(Args&&... args);
			template<typename... Args>
			int ReadDeviceID(Args&&... args);
			template<typename... Args>
			int ReadGitCommit(Args&&... args);
			template<typename... Args>
			int ReadIlp(Args&&... args);
			template<typename... Args>
			int GetKey (Args&&... args);
			template<typename... Args>
			int ReadSpi(Args&&... args);
			template<typename... Args>
			int MeasureThresholds(Args&&... args);
			template<typename... Args>
			int ReadThresholds(Args&&... args);
			template<typename... Args>
			int SetCalDac(Args&&... args);
			template<typename... Args>
			int SetThreshold(Args&&... args);

			// alignment
			template<typename... Args>
			int FindAlignment (Args&&... args);
			template<typename... Args>
			int FindAlignments(Args&&... args);

			// threshold-finding
			template<typename... Args>
			bool FindThreshold(Args&&... args);
			template<typename... Args>
			float ProgramAndQueryThreshold(Args&&... args);

			// ots api?
			template<typename... Args>
			std::vector<std::string> GetRocRegistersNames(Args&&... args);
			template<typename... Args>
			std::vector<uint32_t> GetRocRegisters(Args&&... args);
			template<typename... Args>
			std::vector<float> GetConvertedRocRegisters(Args&&... args);

			// non-conforming return-by-values
			template<typename... Args>
			std::string GetRocID(Args&&... args);
			template<typename... Args>
			std::string GetRocDesignInfo(Args&&... args);
			template<typename... Args>
			std::string GetRocFwGitCommit(Args&&... args);

		protected:
			// data members
			std::unique_ptr<trkdaq::DtcInterface> _interface;
			std::mutex _mutex;
			bool _initialized;

		private:
			// store of preconstructed instances
			static std::map< void*, std::shared_ptr<SharedDtcInterface> > instances;
			static std::mutex _get_mutex;
	};

	template<typename... Args>
	uint16_t SharedDtcInterface::ReadROCRegister(Args&&... args){
		std::lock_guard lock(_mutex);
		uint16_t rv = _interface->ReadROCRegister(std::forward<Args>(args)...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::ResetLink(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ResetLink(std::forward<Args>(args)...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::ReadSettings(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_ReadSettings(std::forward<Args>(args)...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::PulserOn(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_PulserOn(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::PulserOff(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_PulserOff(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::Rates(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_Rates(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::Read(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_Read(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::ReadDeviceID(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_ReadDeviceID(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::ReadGitCommit(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_ReadGitCommit(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::ReadIlp(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_ReadIlp(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::GetKey (Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_GetKey (args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::ReadSpi(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_ReadSpi(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::MeasureThresholds(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_MeasureThresholds(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::ReadThresholds(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_ReadThresholds(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::SetCalDac(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_SetCalDac(args...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::SetThreshold(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->ControlRoc_SetThreshold(std::forward<Args>(args)...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::FindAlignment (Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->FindAlignment(std::forward<Args>(args)...);
		return rv;
	}

	template<typename... Args>
	int SharedDtcInterface::FindAlignments(Args&&... args){
		std::lock_guard lock(_mutex);
		int rv = _interface->FindAlignments(std::forward<Args>(args)...);
		return rv;
	}

	template<typename... Args>
	bool SharedDtcInterface::FindThreshold(Args&&... args){
		std::lock_guard lock(_mutex);
		bool rv = _interface->FindThreshold(args...);
		return rv;
	}

	template<typename... Args>
	float SharedDtcInterface::ProgramAndQueryThreshold(Args&&... args){
		std::lock_guard lock(_mutex);
		float rv = _interface->ProgramAndQueryThreshold(args...);
		return rv;
	}

	template<typename... Args>
	std::vector<std::string> SharedDtcInterface::GetRocRegistersNames(Args&&... args){
		std::lock_guard lock(_mutex);
		std::vector<std::string> rv = _interface->GetRocRegistersNames(args...);
		return rv;
	}

	template<typename... Args>
	std::vector<uint32_t> SharedDtcInterface::GetRocRegisters(Args&&... args){
		std::lock_guard lock(_mutex);
		std::vector<uint32_t> rv = _interface->GetRocRegisters(args...);
		return rv;
	}

	template<typename... Args>
	std::vector<float> SharedDtcInterface::GetConvertedRocRegisters(Args&&... args){
		std::lock_guard lock(_mutex);
		std::vector<float> rv = _interface->GetConvertedRocRegisters(args...);
		return rv;
	}

	template<typename... Args>
	std::string SharedDtcInterface::GetRocID(Args&&... args){
		std::lock_guard lock(_mutex);
		std::string rv = _interface->GetRocID(args...);
		return rv;
	}

	template<typename... Args>
	std::string SharedDtcInterface::GetRocDesignInfo(Args&&... args){
		std::lock_guard lock(_mutex);
		std::string rv = _interface->GetRocDesignInfo(args...);
		return rv;
	}

	template<typename... Args>
	std::string SharedDtcInterface::GetRocFwGitCommit(Args&&... args){
		std::lock_guard lock(_mutex);
		std::string rv = _interface->GetRocFwGitCommit(args...);
		return rv;
	}
} // namespace trkdaq

#endif
