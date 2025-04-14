// Ed Callaghan
// Encapsulate channel-number and parity of preamp nomenclatur into singly-loopable objects
// July 2024

#include "otsdaq-mu2e-tracker/Ui/PreampChannel.hh"

PreampChannel::PreampChannel(unsigned int channel, PreampChannel::Parity side):
    channel(channel), side(side){
  if (this->side == PreampChannel::Parity::total){
    std::string msg = "Cannot instantiate a preamp on the \"total\" side";
    throw cet::exception("PreampChannel::PreampChannel") << msg << std::endl;
  }
}

unsigned int PreampChannel::Channel(){
  auto rv = this->channel;
  return rv;
}

PreampChannel::Parity PreampChannel::Side(){
  auto rv = this->side;
  return side;
}

bool PreampChannel::IsHV(){
  bool rv;
  if (this->Side() == Parity::hv){
    rv = true;
  }
  else{
    rv = false;
  }
  return rv;
}
