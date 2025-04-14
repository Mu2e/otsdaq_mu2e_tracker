#ifndef __TrkSpiData_hh__
#define __TrkSpiData_hh__

namespace trkdaq {
  
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
  };
};

#endif

