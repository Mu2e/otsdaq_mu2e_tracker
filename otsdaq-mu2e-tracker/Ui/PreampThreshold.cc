// Ed Callaghan
// Encapsulate threshold triplets
// July 2024

#include "otsdaq-mu2e-tracker/Ui/PreampThreshold.hh"

PreampThreshold::PreampThreshold(double c_threshold,
                                 double h_threshold,
                                 double t_threshold):
                                   c_threshold(c_threshold),
                                   h_threshold(h_threshold),
                                   t_threshold(t_threshold){
  /**/
}

double PreampThreshold::GetThreshold(const PreampChannel::Parity& parity) const{
  double rv = 0;
  if (parity == PreampChannel::Parity::cal){
    rv = this->GetCalThreshold();
  }
  else if (parity == PreampChannel::Parity::hv){
    rv = this->GetCalThreshold();
  }
  else if (parity == PreampChannel::Parity::total){
    rv = this->GetTotalThreshold();
  }
  else{
    std::string msg = "Impossible preamp parity: " + parity;
    throw cet::exception("PreampChannel::PreampChannel") << msg << std::endl;
  }
  return rv;
}

double PreampThreshold::GetCalThreshold() const{
  auto rv = this->c_threshold;
  return rv;
}

double PreampThreshold::GetHVThreshold() const{
  auto rv = this->h_threshold;
  return rv;
}

double PreampThreshold::GetTotalThreshold() const{
  auto rv = this->t_threshold;
  return rv;
}

double PreampThreshold::ComputeAnalogValue(const DTCLib::roc_data_t adc){
  // in mV
  auto rv = -100.0 + adc * (200.0 / 1024);
  return rv;
}
