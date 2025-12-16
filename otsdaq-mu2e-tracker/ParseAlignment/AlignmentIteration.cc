// Ed Callaghan
// Aggregate of book-keeping across an iteration
// July 2024

#include <iostream>
#include "otsdaq-mu2e-tracker/ParseAlignment/AlignmentIteration.hh"

AlignmentIteration::ChannelMask::ChannelMask(unsigned int lo,
                                             unsigned int md,
                                             unsigned int hi):
                                               lo(lo), md(md), hi(hi){
  /**/
}

bool AlignmentIteration::ChannelMask::TestBit(unsigned int i){
  bool rv = false;
  if (i < 32){
    rv = this->lo & (1 << i);
  }
  else if (i < 64){
    i -= 32;
    rv = this->md & (1 << i);
  }
  else if (i < 96){
    i -= 64;
    rv = this->hi & (1 << i);
  }
  else{ 
    // should never reach here
  }

  return rv;
}

// described further in header
const unsigned int AlignmentIteration::payload_size = 127;

const unsigned int AlignmentIteration::channel_count = 96;

unsigned int AlignmentIteration::Index() const {
  auto rv = this->index;
  return rv;
}

int AlignmentIteration::ADCPhase() const {
  auto rv = this->adc_phase;
  return rv;
}

std::vector<AlignmentChannel> AlignmentIteration::Channels() const {
  auto& rv = this->channels;
  return rv;
}

AlignmentIteration::AlignmentIteration(unsigned int index, words_t words){
  this->index = index;
  this->adc_phase = static_cast<int>(words[0]);

  // reserve space for channel-level data
  this->channels.resize(AlignmentIteration::channel_count);

  // parse the payloads out of the structured representation
  // this->packed.structured
  // subtract two to compensate for routine-level header in rusu's decoding
  unsigned int mask_lo;
  unsigned int mask_md;
  unsigned int mask_hi;

  // first few masks are constructed out of first block of ushorts
  mask_lo = this->construct_concatenated_word(words, 0,   1,   2);
  mask_md = this->construct_concatenated_word(words, 2,   1,   2);
  mask_hi = this->construct_concatenated_word(words, 4,   1,   2);
  ChannelMask active_mask(mask_lo, mask_md, mask_hi);

  mask_lo = this->construct_concatenated_word(words, 0,   7,   9);
  mask_md = this->construct_concatenated_word(words, 4,   7,   9);
  mask_hi = this->construct_concatenated_word(words, 8,   7,   9);
  ChannelMask complete_mask(mask_lo, mask_md, mask_hi);

  mask_lo = this->construct_concatenated_word(words, 0,   8,  10);
  mask_md = this->construct_concatenated_word(words, 4,   8,  10);
  mask_hi = this->construct_concatenated_word(words, 8,   8,  10);
  ChannelMask error_mask(mask_lo, mask_md, mask_hi);

  mask_lo = this->construct_concatenated_word(words, 0,  19,  20);
  mask_md = this->construct_concatenated_word(words, 2,  19,  20);
  mask_hi = this->construct_concatenated_word(words, 4,  19,  20);
  ChannelMask bitslip_done_mask(mask_lo, mask_md, mask_hi);

  // final mask is constructed from second block of ushorts
  mask_lo = this->construct_concatenated_word(words, 0, 121, 122);
  mask_md = this->construct_concatenated_word(words, 2, 121, 122);
  mask_hi = this->construct_concatenated_word(words, 4, 121, 122);
  ChannelMask pattern_match_mask(mask_lo, mask_md, mask_hi);


  size_t steps_offset = 1 + 24; // after first packed blocks
  for (size_t i = 0 ; i < this->channels.size() ; i++){
    bool active = active_mask.TestBit(i);
    bool complete = complete_mask.TestBit(i);
    bool error = error_mask.TestBit(i);
    bool bitslip_done = bitslip_done_mask.TestBit(i);
    unsigned int bitslip_step = static_cast<unsigned int>(words[steps_offset+i]);
    bool pattern_match = pattern_match_mask.TestBit(i);
    this->channels[i] = AlignmentChannel(i,
                                         active, complete, error,
                                         bitslip_done, bitslip_step,
                                         pattern_match);
  }
}

unsigned int AlignmentIteration::construct_concatenated_word(const words_t& v,
                                                             size_t offset,
                                                             size_t idx_lo,
                                                             size_t idx_hi){
  unsigned int rv = 0;
  rv |= static_cast<unsigned int>(v[offset + idx_lo]) & 0xFFFF; // lower 16 bits
  rv |= static_cast<unsigned int>(v[offset + idx_hi]) << 16;    // upper 16 bits
  return rv;
}
