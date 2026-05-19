// Ed Callaghan
// Internal view of individual tracker ROC
// March 2026

#ifndef __trkdaq_roc_hh__
#define __trkdaq_roc_hh__

// stl
#include <streambuf>

// otsdaq-mu2e-tracker
#include "otsdaq-mu2e-tracker/Ui/SharedDtcInterface.hh"

namespace trkdaq{
	class ROC{
		public:
			using link_t = int;
			using address_t = DTCLib::roc_address_t;
			ROC(link_t, DTCLib::DTC*);

			uint32_t ReadRegister(address_t address);
			int Reset();
			int FindAlignment();
			int ReadThresholds(std::vector<float>&        Thr       ,
												 uint32_t      MaskC      = 0xFFFFFFFF,
												 uint32_t      MaskD      = 0xFFFFFFFF,
												 uint32_t      MaskE      = 0xFFFFFFFF,
												 int           PrintLevel = 0x2       ,
												 std::ostream& Stream     = std::cout );
			int SetThreshold(int ChannelID,
											 int PreampType,
											 int Threshold,
											 int PrintLevel = 0);

      int EnableChargeInjection(int FirstChannelMask = 0x10,
                                int DutyCycle        = 10  ,
                                int PulserDelay      = 1000,
                                int PrintLevel       = 0x2,
                                std::ostream& Stream = std::cout);
      int DisableChargeInjection(int PrintLevel = 0,
                                 std::ostream& Stream = std::cout);

			const Alignment& LatestAlignment();

    protected:
			link_t _link;
			std::shared_ptr<SharedDtcInterface> _dtc;
			Alignment _alignment;

		private:
			/**/
	};

	class NullStream: public std::streambuf{
		public:
			int overflow(int c){
				return 0;
			}
	};
} // namespace trkdaq

#endif
