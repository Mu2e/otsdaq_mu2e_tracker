// Ed Callaghan
// Internal view of individual tracker ROC
// March 2026

#include "otsdaq-mu2e-tracker/Ui/ROC.hh"

namespace trkdaq{
	ROC::ROC(link_t link, DTCLib::DTC* dtc):
			_link(link),
			_dtc(SharedDtcInterface::Get(dtc)){
		/**/
	}

	uint32_t ROC::ReadRegister(address_t address){
		auto rv = _dtc->ReadROCRegister(_link, address);
		return rv;
	}

	int ROC::Reset(){
		auto rv = _dtc->ResetLink(_link);
		return rv;
	}

	int ROC::FindAlignment(){
		auto rv = _dtc->FindAlignment(DTCLib::DTC_Link_ID(_link), _alignment);
		return rv;
	}

	int ROC::ReadThresholds(std::vector<float>& out,
													uint32_t mask_lo,
													uint32_t mask_md,
													uint32_t mask_hi,
													int print_level,
													std::ostream& stream){
		auto rv = _dtc->ReadThresholds(_link, out, mask_lo, mask_md, mask_hi, print_level, stream);
		return rv;
	}

	int ROC::SetThreshold(int channel, int preamp, int dac, int print_level){
		auto rv = _dtc->SetThreshold(_link, channel, preamp, dac, print_level);
		return rv;
	}

	const Alignment& ROC::LatestAlignment(){
		return _alignment;
	}
} // namespace trkdaq
