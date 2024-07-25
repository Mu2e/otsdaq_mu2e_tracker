// Ed Callaghan
// Aggregate of book-keeping across an set of iterations
// July 2024

#ifndef Alignment_h
#define Alignment_h

#include <string>
#include <vector>
#include "cetlib_except/exception.h"
#include "otsdaq-mu2e-tracker/ParseAlignment/AlignmentIteration.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/Types.hh"

// EyeMonitorWidth:  1 per routine:               1 x unsigned short
// IfPatternCheck:   1 per routine:               1 x unsigned short
// Iteration blocks: N per routine:         N x 127 x unsigned short
// FaultedADC:       1 per routine:               1 x unsigned short

class Alignment{
  public:
    Alignment(words_t);

    unsigned int EyeMonitorWidth() const;
    bool IfPatternCheck() const;
    unsigned int FaultedADC() const;
    std::vector<AlignmentIteration> Iterations() const;
  protected:
    unsigned int eye_monitor_width;
    bool if_pattern_check;
    unsigned int faulted_adc;
    std::vector<AlignmentIteration> iterations;
  private:
    /**/
};

#endif
