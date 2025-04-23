#ifndef __ControlRocTypes_hh__
#define __ControlRocTypes_hh__

#include "stdint.h"
#include <string>
//-----------------------------------------------------------------------------
// example:
// ---------
// read -a 8 -t 8 -s 1 -l8 - T 10 -m 3 -p 1 -C FFFFFFFF -D FFFFFFFF -E FFFFFFFF
//-----------------------------------------------------------------------------
#define READ_ROC_VERSION 1

namespace trkdaq {

  enum {
    PULSER_OFF        = 269,
    REG_READRATES     = 271,
    REG_READGITCOMMIT = 272,
  };

  struct ControlRoc_Read_Input_t0 {
    uint16_t    adc_mode;               // -a 8   (defailt:  0)  [0]                   // *v2* 
    uint16_t    tdc_mode;               // -t 8   (default:  0)  [1]                   // *v2* 
    uint16_t    num_lookback;           // -l 8   (default:  8)  [2]                   // *v2* 
    uint16_t    num_samples;            // -s 1   (default: 16)  [3] if>63, set to 63  // *v2* 

    uint16_t    num_triggers[2];        // -T 10  (default:  0)  [4-5]                 // *v2* 
    uint16_t    ch_mask[6];             // FFFF FFFF FFFF FFFF FFFF FFFF [6:11]        // *v2* 

    uint16_t    enable_pulser;          // -p 1     (default: 0) [12]                  // *v2* 

    uint16_t    marker_clock;           // -m 3 ??? )default: 0) [13]                  // *v2* 
    uint16_t    mode;                   // [14] need to set mode=0                     // *v2* 
    uint16_t    clock;                  // [15] need to set clock=99                   // *v2*
  };

  struct ControlRoc_Read_Output_t0 {
    uint16_t    adc_mode;               // [0]                  // *v2* 
    uint16_t    tdc_mode;               // [1]                  // *v2* 
    uint16_t    num_lookback;           // [2]                  // *v2* 
    uint16_t    num_samples;            // [3]                  // *v2* 
    uint16_t    num_triggers[2];        // [4-5]                // *v2* 
    uint16_t    ch_mask[6];             // [6--11]              // *v2* 
    uint16_t    enable_pulser;          // [13]                 // *v2* 
    uint16_t    marker_clock;           // [12]                 // *v2* 
    uint16_t    mode;                   // [14] returned 0      // *v2* 
    uint16_t    clock;                  // [15] returned 99,    // *v2* 
    uint16_t    digi_read_0xb;          // [16]                 // *v2* 
    uint16_t    digi_read_0xe;          // [17]                 // *v2* 
    uint16_t    digi_read_0xd;          // [18]                 // *v2* 
    uint16_t    digi_read_0xc;          // [19]                 // *v2* 
  };

  struct ControlRoc_DigiRW_Input_t {
    uint16_t    rw;                  // = dtcbuffer[0];                                  // -a
    uint16_t    hvcal;               // = dtcbuffer[1];                                  // -t
    uint16_t    address;             // = dtcbuffer[2];                                  // -l
    uint16_t    data[2];             // num_triggers = (dtcbuffer[4] << 16) + dtcbuffer[3];         // -T
  };
 
  struct ControlRoc_DigiRW_Output_t {
    uint16_t    rw;                  // = dtcbuffer[0];                                  // -a
    uint16_t    hvcal;               // = dtcbuffer[1];                                  // -t
    uint16_t    address;             // = dtcbuffer[2];                                  // -l
    uint16_t    data[2];             // 
    uint16_t    adc_num;             //
    uint16_t    adc_mask;
  };
 
  struct ControlRoc_DeviceID_t {
    std::string    BackLevelVer;
    std::string    DesignInfo;
    std::string    DesignVer;
    std::string    DeviceSerial;
  };
 
  struct ControlRoc_Rates_t {
    uint16_t    num_lookback;       // default : 100 = dtcbuffer[0];                                  // -a
    uint16_t    num_samples ;       // default :  10 = dtcbuffer[1];                                  // -t
    uint16_t    chan_mask[6];       // default : all channels (0xffff)

    ControlRoc_Rates_t() : num_lookback(100), num_samples(10) {
      for (int i=0; i<6; i++) chan_mask[i] = 0xffff;
    }
  };
 
//-----------------------------------------------------------------------------
// Tracker ROC raw SPI data (mapping of the array of shorts
//-----------------------------------------------------------------------------
  struct TrkSpiRawData_t {
    uint16_t  I3_3;               // 0
    uint16_t  I2_5;               // 1
    uint16_t  I1_8HV;             // 2
    uint16_t  IHV5_0;             // 3
    uint16_t  VDMBHV5_0;          // 4
    uint16_t  V1_8HV;             // 5
    uint16_t  V3_3HV;             // 6
    uint16_t  V2_5;               // 7
    uint16_t  A0;                 // 8
    uint16_t  A1;                 // 9
    uint16_t  A2;                 // 10
    uint16_t  A3;                 // 11
    uint16_t  I1_8CAL;            // 12
    uint16_t  I1_2;               // 13
    uint16_t  ICAL5_0;            // 14
    uint16_t  ADCSPARE;           // 15
    uint16_t  V3_3;               // 16
    uint16_t  VCAL5_0;            // 17
    uint16_t  V1_8CAL;            // 18
    uint16_t  V1_0;               // 19
    uint16_t  ROCPCBTEMP;         // 20  
    uint16_t  HVPCBTEMP;          // 21
    uint16_t  CALPCBTEMP;         // 22
    uint16_t  RTD;                // 23
    uint16_t  ROC_RAIL_1V;        // 24
    uint16_t  ROC_RAIL_1_8V;      // 25
    uint16_t  ROC_RAIL_2_5V;      // 26
    uint16_t  ROC_TEMP;           // 27
    uint16_t  CAL_RAIL_1V;        // 28
    uint16_t  CAL_RAIL_1_8V;      // 29
    uint16_t  CAL_RAIL_2_5V;      // 30
    uint16_t  CAL_TEMP;           // 31
    uint16_t  HV_RAIL_1V;         // 32
    uint16_t  HV_RAIL_1_8V;       // 33
    uint16_t  HV_RAIL_2_5V;       // 34
    uint16_t  HV_TEMP;            // 35
  };

  int const TrkSpiDataNWords = sizeof(TrkSpiRawData_t)/sizeof(uint16_t); 
//-----------------------------------------------------------------------------
// converted data
//-----------------------------------------------------------------------------
  struct TrkSpiData_t {
    float     I3_3;               // 0
    float     I2_5;               // 1
    float     I1_8HV;             // 2
    float     IHV5_0;             // 3
    float     VDMBHV5_0;          // 4
    float     V1_8HV;             // 5
    float     V3_3HV;             // 6
    float     V2_5;               // 7
    float     A0;                 // 8
    float     A1;                 // 9
    float     A2;                 // 10
    float     A3;                 // 11
    float     I1_8CAL;            // 12
    float     I1_2;               // 13
    float     ICAL5_0;            // 14
    float     ADCSPARE;           // 15
    float     V3_3;               // 16
    float     VCAL5_0;            // 17
    float     V1_8CAL;            // 18
    float     V1_0;               // 19
    float     ROCPCBTEMP;         // 20  
    float     HVPCBTEMP;          // 21
    float     CALPCBTEMP;         // 22
    float     RTD;                // 23
    float     ROC_RAIL_1V;        // 24
    float     ROC_RAIL_1_8V;      // 25
    float     ROC_RAIL_2_5V;      // 26
    float     ROC_TEMP;           // 27
    float     CAL_RAIL_1V;        // 28
    float     CAL_RAIL_1_8V;      // 29
    float     CAL_RAIL_2_5V;      // 30
    float     CAL_TEMP;           // 31
    float     HV_RAIL_1V;         // 32
    float     HV_RAIL_1_8V;       // 33
    float     HV_RAIL_2_5V;       // 34
    float     HV_TEMP;            // 35

    float Data(int I) { return *(&I3_3 + I); }
  };
};

#endif
