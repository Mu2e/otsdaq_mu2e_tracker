// Ed Callaghan
// Aggregate of book-keeping across an iteration
// July 2024

#ifndef AlignmentIteration_h
#define AlignmentIteration_h

#include <cstddef>
#include <vector>
#include "otsdaq-mu2e-tracker/ParseAlignment/AlignmentChannel.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/Types.hh"

// ADCPhase: 1 per iteration:                1 x unsigned short
// ActiveCh, AlignCmp, AlignErr, BitSlipDone:
//     1 per channel:           packed into 24 x unsigned short = 96 x 4 bits
// BitSlipStep: 1 per channel:              96 x unsigned short
// PatternMatch: 1 per channel: packed into  6 x unsigned short = 96 x 1 bits
//                                  Total: 127 x unsigned short

class AlignmentIteration{
  class ChannelMask{
    public:
      ChannelMask(unsigned int, unsigned int, unsigned int);
      bool TestBit(unsigned int);
    protected:
      unsigned int lo;
      unsigned int md;
      unsigned int hi;
    private:
      /**/
  };

  public:
    static const unsigned int payload_size; // 127 words, as above
    static const unsigned int channel_count; // 96 channels

    AlignmentIteration() = default;
    AlignmentIteration(unsigned int, words_t);

    unsigned int Index() const;
    int ADCPhase() const;
    std::vector<AlignmentChannel> Channels() const;
  protected:
    unsigned int index;
    int adc_phase;
    std::vector<AlignmentChannel> channels;

    unsigned int construct_concatenated_word(const words_t&, size_t, size_t, size_t);
  private:
    /**/
};

#endif
