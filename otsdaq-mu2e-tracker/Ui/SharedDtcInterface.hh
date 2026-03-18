// Ed Callaghan
// Wrap the precious heart in a mutexed singleton to keep it safe
// March 2026

// stl
#include <mutex>

// otsdaq-mu2e-tracker
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

class SharedDtcInterface{
	public:
		SharedDtcInterface(DTCLib::DTC* dtc);

		bool PostInitialize(const trkdaq::DtcInterface::DtcConfiguration_t&);

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

		template<typename... Args>
		int FindAlignment (Args... args);
		template<typename... Args>
		int FindAlignments(Args... args);

		template<typename... Args>
		bool FindThreshold(Args... args);

		template<typename... Args>
		float ProgramAndQueryThreshold(Args... args);

		template<typename... Args>
		std::vector<std::string> GetRocRegistersNames(Args... args);
		template<typename... Args>
		std::vector<uint32_t> GetRocRegisters(Args... args);
		template<typename... Args>
		std::vector<float> GetConvertedRocRegisters(Args... args);

		template<typename... Args>
		std::string GetRocID(Args... args);
		template<typename... Args>
		std::string GetRocDesignInfo(Args... args);
		template<typename... Args>
		std::string GetRocFwGitCommit(Args... args);

	protected:
		std::unique_ptr<trkdaq::DtcInterface> _interface;
		std::mutex _mutex;
		bool _initialized;

  private:
		/**/
};
