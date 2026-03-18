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
		std::shared_ptr<SharedDtcInterface>& Get(DTCLib::DTC*);

		bool PostInitialize(const trkdaq::DtcInterface::DtcConfiguration_t&);

		// below are thin forwards automagically completed by variadic templates

		// ControlROC paradigm
		template<typename... Args>
		int ReadSettings(Args... args);
		template<typename... Args>
		int PulserOn(Args... args);
		template<typename... Args>
		int PulserOff(Args... args);
		template<typename... Args>
		int Rates(Args... args);
		template<typename... Args>
		int Read(Args... args);
		template<typename... Args>
		int ReadDeviceID(Args... args);
		template<typename... Args>
		int ReadGitCommit(Args... args);
		template<typename... Args>
		int ReadIlp(Args... args);
		template<typename... Args>
		int GetKey (Args... args);
		template<typename... Args>
		int ReadSpi(Args... args);
		template<typename... Args>
		int MeasureThresholds(Args... args);
		template<typename... Args>
		int ReadThresholds(Args... args);
		template<typename... Args>
		int SetCalDac(Args... args);
		template<typename... Args>
		int SetThresholds(Args... args);

		// alignment
		template<typename... Args>
		int FindAlignment (Args... args);
		template<typename... Args>
		int FindAlignments(Args... args);

		// threshold-finding
		template<typename... Args>
		bool FindThreshold(Args... args);
		template<typename... Args>
		float ProgramAndQueryThreshold(Args... args);

		// ots api?
		template<typename... Args>
		std::vector<std::string> GetRocRegistersNames(Args... args);
		template<typename... Args>
		std::vector<uint32_t> GetRocRegisters(Args... args);
		template<typename... Args>
		std::vector<float> GetConvertedRocRegisters(Args... args);

		// non-conforming return-by-values
		template<typename... Args>
		std::string GetRocID(Args... args);
		template<typename... Args>
		std::string GetRocDesignInfo(Args... args);
		template<typename... Args>
		std::string GetRocFwGitCommit(Args... args);

	protected:
		// data members
		std::unique_ptr<trkdaq::DtcInterface> _interface;
		std::mutex _mutex;
		bool _initialized;

	private:
		// store of preconstructed instances
		static std::map< void*, std::shared_ptr<SharedDtcInterface> > instances;
};
} // namespace trkdaq

#endif
