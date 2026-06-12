// Ed Callaghan
// Internal view of individual tracker ROC
// March 2026

#include "otsdaq-mu2e-tracker/Ui/ROC.hh"

namespace trkdaq{
    ROC::ROC(link_t link, DTCLib::DTC* dtc):
            _link(link),
            _dtc(SharedDtcInterface::Get(dtc)),
            _nullstream(std::make_unique<trkdaq::NullStream>()),
            _null(_nullstream.get()){
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

    int ROC::SetChannelMask(uint32_t mask_lo,
                            uint32_t mask_md,
                            uint32_t mask_hi){
        int rv = 0;
        int rc;

        // 96 = 32x3 bits must be repartitioned into 2x48 = 2x3x16 bit words
        uint16_t digi_mask_lo;
        uint16_t digi_mask_md;
        uint16_t digi_mask_hi;

        // lower bits go to cal-side
        digi_mask_lo = (mask_lo & 0x0000FFFF) >>  0;
        digi_mask_md = (mask_lo & 0xFFFF0000) >> 16;
        digi_mask_hi = (mask_md & 0x0000FFFF) >>  0;
        rc = this->SetDigiChannelMask(trkdaq::fpga::digi::cal, digi_mask_lo,
                                                               digi_mask_md,
                                                               digi_mask_hi);
        rv += rc;

        // upper bits go to hv-side
        digi_mask_lo = (mask_md & 0xFFFF0000) >> 16;
        digi_mask_md = (mask_hi & 0x0000FFFF) >>  0;
        digi_mask_hi = (mask_hi & 0xFFFF0000) >> 16;
        rc = this->SetDigiChannelMask(trkdaq::fpga::digi::hv,  digi_mask_lo,
                                                               digi_mask_md,
                                                               digi_mask_hi);
        rv += rc;

        return rv;
    }

    int ROC::SetDigiChannelMask(trkdaq::fpga_t fpga, uint16_t mask_lo,
                                                     uint16_t mask_md,
                                                     uint16_t mask_hi){
        int rv = 0;
        int print_level = 0;
        rv += this->DigiWrite(registers::digi::channel_mask_lo,
                              fpga, mask_lo, print_level, _null);
        rv += this->DigiWrite(registers::digi::channel_mask_md,
                              fpga, mask_md, print_level, _null);
        rv += this->DigiWrite(registers::digi::channel_mask_hi,
                              fpga, mask_hi, print_level, _null);
        return rv;
    }

	int ROC::DigiRW(uint16_t rw,
	                uint16_t hv_cal,
	                uint16_t address,
	                uint32_t data,
	                std::ostream& stream){
		trkdaq::ControlRoc_DigiRW_Input_t  in;
		trkdaq::ControlRoc_DigiRW_Output_t out;
		in.rw      = rw;
		in.hvcal   = hv_cal;
		in.address = address;
		// 32-bit word splits into two 16-bit words, low word first
		in.data[0] = (data      ) & 0xffff;
		in.data[1] = (data >> 16) & 0xffff;

		// PrintLevel bit 1 => emit the parsed output to the stream
		int print_level = 0x2;
		auto rv = _dtc->DigiRW(&in, &out, _link, print_level, stream);
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

	std::vector<uint16_t> ROC::InitializeDigis(){
		std::vector<uint16_t> res;
		_dtc->RocBlockRead(_link, 0x116, res);
		return res;
	}

	int ROC::PrintStatus(uint32_t format, std::ostream& stream){
		auto rv = _dtc->PrintRocStatus(format, _link, stream);
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

    bool ROC::FindThreshold(int channel,
                            int preamp,
                            float threshold_mv,
                            float tolerance_mv,
                            DTCLib::roc_data_t& out){
        auto rv = _dtc->FindThreshold(_link, channel, preamp, threshold_mv, tolerance_mv, out);
        return rv;
    }

    int ROC::FindThresholds(int channel,
                            float threshold_mv,
                            float tolerance_mv,
                            DTCLib::roc_data_t& cal_out,
                            DTCLib::roc_data_t& hv_out){
        // preamp 0 = CAL, preamp 1 = HV
        int n_failed = 0;
        if (not this->FindThreshold(channel, 0, threshold_mv, tolerance_mv, cal_out)) n_failed++;
        if (not this->FindThreshold(channel, 1, threshold_mv, tolerance_mv, hv_out )) n_failed++;
        return n_failed;
    }

    int ROC::FindThresholds(float threshold_mv,
                            float tolerance_mv,
                            std::vector<DTCLib::roc_data_t>& out){
        out.assign(2 * 96, 0);
        int n_failed = 0;
        for (int channel = 0; channel < 96; channel++){
            DTCLib::roc_data_t cal_dac = 0;
            DTCLib::roc_data_t hv_dac  = 0;
            n_failed += this->FindThresholds(channel, threshold_mv, tolerance_mv, cal_dac, hv_dac);
            out[2*channel + 0] = cal_dac;  // CAL
            out[2*channel + 1] = hv_dac;   // HV
        }
        return n_failed;
    }

    int ROC::NotoriousRead(uint16_t adc_mode,
                         uint16_t tdc_mode,
                         uint16_t num_lookback,
                         uint16_t num_samples,
                         uint32_t num_triggers,
                         uint32_t mask_lo,
                         uint32_t mask_md,
                         uint32_t mask_hi,
                         uint16_t enable_pulser,
                         uint16_t marker_clock,
                         uint16_t mode,
                         uint16_t clock,
                         std::ostream& stream){
    trkdaq::ControlRoc_Read_Input_t0 par;
    par.adc_mode        = adc_mode;
    par.tdc_mode        = tdc_mode;
    par.num_lookback    = num_lookback;
    par.num_samples     = num_samples;
    // 32-bit count splits into two 16-bit words, low word first:
    // num_triggers[0] = count & 0xffff, num_triggers[1] = count >> 16
    par.num_triggers[0] = (num_triggers      ) & 0xffff;
    par.num_triggers[1] = (num_triggers >> 16) & 0xffff;
    // each 32-bit mask splits into two 16-bit words, low word first:
    // ch_mask[2*i] = mask & 0xffff, ch_mask[2*i+1] = mask >> 16
    par.ch_mask[0]      = (mask_lo      ) & 0xffff;
    par.ch_mask[1]      = (mask_lo >> 16) & 0xffff;
    par.ch_mask[2]      = (mask_md      ) & 0xffff;
    par.ch_mask[3]      = (mask_md >> 16) & 0xffff;
    par.ch_mask[4]      = (mask_hi      ) & 0xffff;
    par.ch_mask[5]      = (mask_hi >> 16) & 0xffff;
    par.enable_pulser   = enable_pulser;
    par.marker_clock    = marker_clock;
    par.mode            = mode;
    par.clock           = clock;

    // PrintLevel bit 1 => emit the parsed fields to the stream
    int print_level = 0x2;
    auto rv = _dtc->Read(&par, _link, print_level, stream);
    return rv;
  }

  int ROC::ConfigureDigis(uint16_t tdc_mode,
                          uint16_t num_lookback,
                          uint16_t num_samples,
                          uint32_t mask_lo,
                          uint32_t mask_md,
                          uint32_t mask_hi,
                          std::ostream& stream){
    // hardcoded defaults for the rarely-changed parameters
    uint16_t adc_mode      = 0;
    uint32_t num_triggers  = 0;
    uint16_t enable_pulser = 0;
    uint16_t marker_clock  = 3;
    uint16_t mode          = 0;
    uint16_t clock         = 99;

    auto rv = this->NotoriousRead(adc_mode,
                                  tdc_mode,
                                  num_lookback,
                                  num_samples,
                                  num_triggers,
                                  mask_lo,
                                  mask_md,
                                  mask_hi,
                                  enable_pulser,
                                  marker_clock,
                                  mode,
                                  clock,
                                  stream);
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
