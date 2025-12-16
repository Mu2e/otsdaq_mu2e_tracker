// Ed Callaghan
// Aggregate of book-keeping across an set of iterations
// July 2024

#include "otsdaq-mu2e-tracker/ParseAlignment/Alignment.hh"

Alignment::Alignment(words_t buffer){
  // account for two header and one footer word
  unsigned int adjusted = buffer.size() - 3;
  if (adjusted % AlignmentIteration::payload_size != 0){
    std::string msg = "Malformed alignment returned payload: ";
    msg +=  "iteration payload size (";
    msg +=  std::to_string(AlignmentIteration::payload_size);
    msg += ") ";
    msg +=  "does not divide adjusted size (";
    msg +=  std::to_string(buffer.size());
    msg +=  " - 3 = ";
    msg += std::to_string(adjusted) + ")";
    throw cet::exception("ALIGNMENT::Alignment") << msg;
  }
  unsigned int n_iterations = adjusted / AlignmentIteration::payload_size;
  this->iterations.resize(n_iterations);

  // first, the simple part
  this->eye_monitor_width = static_cast<unsigned int>(buffer[0]);
  this->if_pattern_check = static_cast<bool>(buffer[1]);

  // next, forward iteration-level blocks
  for (size_t i = 0 ; i < this->iterations.size() ; i++){
    size_t offset = 2 + i*AlignmentIteration::payload_size;
    words_t tmp(AlignmentIteration::payload_size);
    for (size_t j = 0 ; j < tmp.size() ; j++){
      tmp[j] = buffer[offset+j];
    }
    this->iterations[i] = AlignmentIteration(i, tmp);
  }

  // one last simple part
  this->faulted_adc = static_cast<unsigned int>(buffer.size() - 1);
}

unsigned int Alignment::EyeMonitorWidth() const{
  auto rv = this->eye_monitor_width;
  return rv;
}

bool Alignment::IfPatternCheck() const{
  auto rv = this->if_pattern_check;
  return rv;
}

unsigned int Alignment::FaultedADC() const{
  auto rv = this->faulted_adc;
  return rv;
}

std::vector<AlignmentIteration> Alignment::Iterations() const{
  auto& rv = this->iterations;
  return rv;
}
