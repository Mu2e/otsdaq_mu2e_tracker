#ifndef __ControlRoc_Read_Par_t_hh__
#define __ControlRoc_Read_Par_t_hh__

#include "stdint.h"
#include <string>
//-----------------------------------------------------------------------------
// example:
// ---------
// read -a 8 -t 8 -s 1 -l8 - T 10 -m 3 -p 1 -C FFFFFFFF -D FFFFFFFF -E FFFFFFFF
//-----------------------------------------------------------------------------
namespace trkdaq {
  struct ControlRoc_Read_Par_t {
    uint16_t    adc_mode;                 // -a 8   (defailt:  0)
    uint16_t    tdc_mode;                 // -t 8   (default:  0)
    uint16_t    lookback;                 // -l 8   (default:  8)
    uint16_t    samples;                  // -s 1   (default: 16)
    int         triggers;                 // -T 10  (default:  0)
    uint16_t    chan_mask[6];             // FFFF FFFF FFFF FFFF FFFF FFFF 
    int         pulser;                   // -p 1     (default: 0)
    int         delay;                    // -d ???   (default: 1)
    int         mode;                     // -m 3 ??? )default: 0)
//-----------------------------------------------------------------------------
// output parameters
//-----------------------------------------------------------------------------
    uint16_t    hvcal_mask1;
    uint16_t    hvcal_mask2;
    uint16_t    hvcal_mask3;
    uint16_t    hvcal_enable_pulser;
    uint16_t    output_mode;
    uint16_t    return_code;
//-----------------------------------------------------------------------------
// the ones below are not passed to python readData
//-----------------------------------------------------------------------------
    int         clock;                    // = 99 and not used by Monica (?)
    int         channel;                  // -c
    std::string message;                  // -M 
  };
};

#endif
