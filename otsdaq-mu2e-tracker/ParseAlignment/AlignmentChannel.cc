// Ed Callaghan
// Lowest-level book-keeping of results of adc alignment
// July 2024

#include "otsdaq-mu2e-tracker/ParseAlignment/AlignmentChannel.hh"

// copied from default serial mapping
const std::map<unsigned int, unsigned int>
    AlignmentChannel::adc_to_straw_map = {
      { 0,91}, { 1,85}, { 2,79}, { 3,73}, { 4,67}, { 5,61}, { 6,55}, { 7,49},
      { 8,43}, { 9,37}, {10,31}, {11,25}, {12,19}, {13,13}, {14, 7}, {15, 1},
      {16,90}, {17,84}, {18,78}, {19,72}, {20,66}, {21,60}, {22,54}, {23,48},
      {24,42}, {25,36}, {26,30}, {27,24}, {28,18}, {29,12}, {30, 6}, {31, 0},
      {32,93}, {33,87}, {34,81}, {35,75}, {36,69}, {37,63}, {38,57}, {39,51},
      {40,45}, {41,39}, {42,33}, {43,27}, {44,21}, {45,15}, {46, 9}, {47, 3},
      {48,44}, {49,38}, {50,32}, {51,26}, {52,20}, {53,14}, {54, 8}, {55, 2},
      {56,92}, {57,86}, {58,80}, {59,74}, {60,68}, {61,62}, {62,56}, {63,50},
      {64,47}, {65,41}, {66,35}, {67,29}, {68,23}, {69,17}, {70,11}, {71, 5},
      {72,95}, {73,89}, {74,83}, {75,77}, {76,71}, {77,65}, {78,59}, {79,53},
      {80,46}, {81,40}, {82,34}, {83,28}, {84,22}, {85,16}, {86,10}, {87, 4},
      {88,94}, {89,88}, {90,82}, {91,76}, {92,70}, {93,64}, {94,58}, {95,52},
    };

AlignmentChannel::AlignmentChannel(unsigned int adc,
                                   bool active,
                                   bool complete,
                                   bool error,
                                   bool bitslip_done,
                                   unsigned int bitslip_step,
                                   bool pattern_match):
                                     adc(adc),
                                     active(active),
                                     complete(complete),
                                     error(error),
                                     bitslip_done(bitslip_done),
                                     bitslip_step(bitslip_step),
                                     pattern_match(pattern_match){
  this->channel = AlignmentChannel::adc_to_straw_map[this->adc];
}

unsigned int AlignmentChannel::ADC() const{
  auto rv = this->adc;
  return rv;
}

unsigned int AlignmentChannel::Channel() const{
  auto rv = this->channel;
  return rv;
}

bool AlignmentChannel::Active() const{
  auto rv = this->active;
  return rv;
}

bool AlignmentChannel::Complete() const{
  auto rv = this->complete;
  return rv;
}

bool AlignmentChannel::Error() const{
  auto rv = this->error;
  return rv;
}

unsigned int AlignmentChannel::BitSlipStep() const{
  auto rv = this->bitslip_step;
  return rv;
}

bool AlignmentChannel::BitSlipDone() const{
  auto rv = this->bitslip_done;
  return rv;
}

bool AlignmentChannel::PatternMatch() const{
  auto rv = this->pattern_match;
  return rv;
}
