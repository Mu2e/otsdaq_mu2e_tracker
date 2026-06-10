// Ed Callaghan
// Factor out registers on the ROC and Digi FPGAs
// June 2026

#include "otsdaq-mu2e-tracker/Ui/TrackerRegisters.hh"

namespace trkdaq{
  namespace fpga{
    namespace digi{
      const fpga_t cal = 1;
      const fpga_t hv  = 2;
    }; // namespace digi
  }; // namespace fpgas

  namespace registers{
    namespace roc{
    } //namespace roc
    namespace digi{
      const address_t channel_mask_lo = 0x0B;
      const address_t channel_mask_md = 0x0E;
      const address_t channel_mask_hi = 0x0D;
    } // namespace digi
  }; // namespace registers
} // namespace trkdaq
