// Ed Callaghan
// Encapsulate threshold triplets
// July 2024

#ifndef PreampThreshold_hh
#define PreampThreshold_hh

#include <string>
#include "cetlib_except/exception.h"
#include "dtcInterfaceLib/DTC.h"
#include "otsdaq-mu2e-tracker/Ui/PreampChannel.hh"

class PreampThreshold{
  public:
    PreampThreshold(double c_threshold, double h_threshold, double t_threshold);

    double GetThreshold(const PreampChannel::Parity& parity) const;
    double GetCalThreshold() const;
    double GetHVThreshold() const;
    double GetTotalThreshold() const;

    static double ComputeAnalogValue(const DTCLib::roc_data_t adc);
  protected:
    double c_threshold; // cal side
    double h_threshold; // hv  side
    double t_threshold; // total
  private:
    /**/
};

#endif
