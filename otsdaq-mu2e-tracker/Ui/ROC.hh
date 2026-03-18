// Ed Callaghan
// Internal view of individual tracker ROC
// March 2026

#ifndef __trkdaq_roc_hh__
#define __trkdaq_roc_hh__

// otsdaq-mu2e-tracker
#include "otsdaq-mu2e-tracker/Ui/SharedDtcInterface.hh"

namespace trkdaq{
	class ROC{
		public:
			using link_t = int;
			ROC(link_t, DTCLib::DTC*);

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

    protected:
			link_t _link;
			std::shared_ptr<SharedDtcInterface> _dtc;

		private:
			/**/
	};
} // namespace trkdaq

#endif
