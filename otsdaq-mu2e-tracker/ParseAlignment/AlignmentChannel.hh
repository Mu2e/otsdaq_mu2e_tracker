// Ed Callaghan
// Lowest-level book-keeping of results of adc alignment
// July 2024

#ifndef AlignmentChannel_h
#define AlignmentChannel_h

#include <map>

class AlignmentChannel{
  public:
    AlignmentChannel() = default;
    AlignmentChannel(unsigned int, bool, bool, bool, bool, unsigned int, bool);

    unsigned int ADC() const;
    unsigned int Channel() const;
    bool Active() const;
    bool Complete() const;
    bool Error() const;
    unsigned int BitSlipStep() const;
    bool BitSlipDone() const;
    bool PatternMatch() const;

  protected:
    static const std::map<unsigned int, unsigned int> adc_to_straw_map;

  private:
    unsigned int adc;
    unsigned int channel;
    bool active;
    bool complete;
    bool error;
    bool bitslip_done;
    unsigned int bitslip_step;
    bool pattern_match;
};

#endif
