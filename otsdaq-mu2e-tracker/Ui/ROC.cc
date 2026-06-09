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

    int ROC::ResetDigis(){
        auto rv = _dtc->ResetDigis(_link);
        return rv;
    }

    int ROC::RebootMCU(){
        auto rv = _dtc->RebootMcu(_link);
        return rv;
    }

    int ROC::InitReadoutMode(std::ostream& stream){
        auto rv = _dtc->InitRocReadoutMode(stream);
        return rv;
    }

    void ROC::SetLaneMask(int mask){
        _dtc->SetRocLaneMask(mask);
    }

    void ROC::SetNHitsPerLane(int nhits){
        _dtc->SetRocNHitsPerLane(nhits);
    }

    int ROC::SetEventWindowDelay(uint16_t delay_5ns, std::ostream& stream){
        auto rv = _dtc->SetRocDelay(_link, delay_5ns, stream);
        return rv;
    }

    int ROC::SetDigitizationWindow(uint16_t t_start,
                                   uint16_t t_stop,
                                   int print_level,
                                   std::ostream& stream){
        auto rv = _dtc->SetRocDigitizationWindow(_link, t_start, t_stop, print_level, stream);
        return rv;
    }

    int ROC::DigiRead(int addr,
                      int hv_cal,
                      uint32_t& res,
                      int print_level,
                      std::ostream& stream){
        auto rv = _dtc->DigiRead(addr, hv_cal, res, _link, print_level, stream);
        return rv;
    }

    int ROC::DigiWrite(int addr,
                       int hv_cal,
                       uint16_t dat,
                       int print_level,
                       std::ostream& stream){
        auto rv = _dtc->DigiWrite(addr, hv_cal, dat, _link, print_level, stream);
        return rv;
    }

    int ROC::ReadPanelID(int print_level){
        auto rv = _dtc->ReadPanelID(_link, print_level);
        return rv;
    }

    roc_serial_t ROC::ReadSerialNumber(){
        auto rv = _dtc->ReadSerialNumber(DTCLib::DTC_Link_ID(_link));
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

	bool ROC::FindThreshold(int                 channel,
	                        int                 preamp,
	                        float               threshold_mv,
	                        float               tolerance_mv,
	                        DTCLib::roc_data_t& out){
		auto rv = _dtc->FindThreshold(_link, channel, preamp, threshold_mv, tolerance_mv, out);
		return rv;
	}

  int ROC::EnableChargeInjection(int first_channel_mask,
                                 int duty_cycle,
                                 int delay,
                                 int print_level,
                                 std::ostream& stream){
    auto rv = _dtc->PulserOn(_link, first_channel_mask, duty_cycle, delay, print_level, stream);
    return rv;
  }

  int ROC::DisableChargeInjection(int print_level, std::ostream& stream){
    auto rv = _dtc->PulserOff(_link, print_level, stream);
    return rv;
  }

    const Alignment& ROC::LatestAlignment(){
        return _alignment;
    }
} // namespace trkdaq
