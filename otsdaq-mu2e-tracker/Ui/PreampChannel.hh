// Ed Callaghan
// Encapsulate channel-number and Parity of preamp nomenclatur into singly-loopable objects
// July 2024

#ifndef PreampChannel_hh
#define PreampChannel_hh

#include <string>
#include "cetlib_except/exception.h"

class PreampChannel{
  public:
    enum Parity {cal=0, hv=1, total=2};
    PreampChannel(unsigned int channel, Parity side);

    unsigned int Channel();
    Parity Side();
    bool IsHV();
  protected:
    unsigned int channel;
    Parity side;
  private:
    /**/
};

#endif
