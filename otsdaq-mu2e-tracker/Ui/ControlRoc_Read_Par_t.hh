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
  struct ControlRoc_Read_Input_t {
    // *V2* uint16_t    adc_mode;               // -a 8   (defailt:  0)  [0]
    // *V2* uint16_t    tdc_mode;               // -t 8   (default:  0)  [1]
    // *V2* uint16_t    num_lookback;           // -l 8   (default:  8)  [2]
    // *V2* uint16_t    num_samples;            // -s 1   (default: 16)  [3] if>63, set to 63
    // *V2* uint16_t    num_triggers[2];        // -T 10  (default:  0)  [4-5]
    // *V2* uint16_t    ch_mask[6];             // FFFF FFFF FFFF FFFF FFFF FFFF [6:11]
    // *V2* uint16_t    enable_pulser;          // -p 1     (default: 0) [12]
    // *V2* uint16_t    marker_clock;           // -m 3 ??? )default: 0) [13]
    // *V2* uint16_t    mode;                   // [14] need to set mode=0
    // *V2* uint16_t    clock;                  // [15] need to set clock=99

    uint16_t    adc_mode;               // = dtcbuffer[0];                                  // -a
    uint16_t    tdc_mode;               // = dtcbuffer[1];                                  // -t
    uint16_t    num_lookback;           // = dtcbuffer[2];                              // -l
    uint16_t    num_triggers[2];        // num_triggers = (dtcbuffer[4] << 16) + dtcbuffer[3];         // -T
    uint16_t    ch_mask[6];             // channel_mask[0] = (dtcbuffer[6] << 16) + dtcbuffer[5];  // -C
                                        // channel_mask[1] = (dtcbuffer[8] << 16) + dtcbuffer[7];    // -D
                                        // channel_mask[2] = (dtcbuffer[10] << 16) + dtcbuffer[9];    // -E
    uint16_t    num_samples;            // = dtcbuffer[11];                     // -s
    uint16_t    enable_pulser;          // = (uint8_t) dtcbuffer[12];         // -p
    uint16_t    max_total_delay;        // = dtcbuffer[13];                 // -d (def 1)
    uint16_t    marker_clock;           // = (uint8_t) dtcbuffer[14];          // -m
    
    uint16_t    mode;                   // need to set mode=0
    uint16_t    clock;                  // need to set clock=99
    
  };
//-----------------------------------------------------------------------------
// output parameters : same as input , plus four words
//-----------------------------------------------------------------------------
  struct ControlRoc_Read_Output_t {
    // uint16_t    adc_mode;               // [0]
    // uint16_t    tdc_mode;               // [1]
    // uint16_t    num_lookback;           // [2]
    // uint16_t    num_samples;            // [3]
    // uint16_t    num_triggers[2];        // [4-5]
    // uint16_t    ch_mask[6];             // [6--11]
    // uint16_t    enable_pulser;          // [13]
    // uint16_t    marker_clock;           // [12]
    // uint16_t    mode;                   // [14] returned 0
    // uint16_t    clock;                  // [15] returned 99, 
    // uint16_t    digi_read_0xb;          // [16]
    // uint16_t    digi_read_0xe;          // [17]
    // uint16_t    digi_read_0xd;          // [18]
    // uint16_t    digi_read_0xc;          // [19]

    uint16_t    enable_pulser;          // [0] *(registers_1_addr + CRDCS_WRITE_TX) = enable_pulser;
    uint16_t    num_samples;            // [1] *(registers_1_addr + CRDCS_WRITE_TX) = num_samples;
    uint16_t    num_lookback;           // [2] *(registers_1_addr + CRDCS_WRITE_TX) = num_lookback;
    uint16_t    ch_mask[6];             // [4-9]
    uint16_t    adc_mode;               // [10] *(registers_1_addr + CRDCS_WRITE_TX) = adc_mode;
    uint16_t    tdc_mode;               // [11] *(registers_1_addr + CRDCS_WRITE_TX) = tdc_mode;
    uint16_t    num_triggers[2];        // [12-13]
    uint16_t    digi_read_0xb;          // [14]
    uint16_t    digi_read_0xe;          // [15]
    uint16_t    digi_read_0xd;          // [16]
    uint16_t    digi_read_0xc;          // [17]
    uint16_t    mode;                   // [18]
    uint16_t    clock;                  // need to set clock=99
    uint16_t    marker_clock;           //
  };
};

#endif
