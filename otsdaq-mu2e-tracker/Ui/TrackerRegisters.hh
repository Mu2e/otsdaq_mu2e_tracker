// Ed Callaghan
// Factor out registers on the ROC and Digi FPGAs
// June 2026

#ifndef __trkdaq_trackerregisters_hh__
#define __trkdaq_trackerregisters_hh__

// stl
#include <cstdint>

namespace trkdaq{
  using fpga_t = int;
  namespace fpga{
    namespace digi{
      inline constexpr fpga_t both = 0;
      inline constexpr fpga_t cal = 1;
      inline constexpr fpga_t hv = 2;
      inline constexpr fpga_t roc = 3;
    }; // namespace digi
  }; // namespace fpgas
  namespace registers{
    using address_t = uint8_t;
    namespace rocdcs{
      inline constexpr registers::address_t DBG = 0;
      inline constexpr registers::address_t BITSLIP = 1;
      inline constexpr registers::address_t NWRDCS = 2;
      inline constexpr registers::address_t NRDDCS = 3;
      inline constexpr registers::address_t LOOPBACK_COARSE_DELAY = 4;
      inline constexpr registers::address_t DCS_TO_SERIAL = 5;
      inline constexpr registers::address_t TWI_CONTROL = 6;
      inline constexpr registers::address_t LOSS_LOCK = 7;
      inline constexpr registers::address_t ROC_ENABLE = 8;
      inline constexpr registers::address_t DATAREQ_CNT_L = 9;
      inline constexpr registers::address_t DATAREQ_CNT_H = 10;
      inline constexpr registers::address_t EWM_CNT_L = 11;
      inline constexpr registers::address_t EWM_CNT_H = 12;
      inline constexpr registers::address_t IS_SKIPPED_DREQ_CNT = 13;
      inline constexpr registers::address_t EW_DONE_CNT = 14;
      inline constexpr registers::address_t DCS_DDR_ADDRESS_L = 15;
      inline constexpr registers::address_t DCS_DDR_ADDRESS_H = 16;
      inline constexpr registers::address_t DCS_ERROR_COUNTER = 17;
      inline constexpr registers::address_t ROC_STATUS = 18;
      inline constexpr registers::address_t DDR_FIFO_WR_STATUS = 20;
      inline constexpr registers::address_t DDR_FIFO_RD_STATUS = 21;
      inline constexpr registers::address_t DREQ_FIFO_WRCNT = 23;
      inline constexpr registers::address_t DREQ_FIFO_WR_STATUS = 24;
      inline constexpr registers::address_t DREQ_FIFO_RDCNT = 25;
      inline constexpr registers::address_t DREQ_FIFO_RD_STATUS = 26;
      inline constexpr registers::address_t DCS_HB_CNT_L = 27;
      inline constexpr registers::address_t DCS_HB_CNT_H = 28;
      inline constexpr registers::address_t DCS_NULLHB_CNT_L = 29;
      inline constexpr registers::address_t DCS_NULLHB_CNT_H = 30;
      inline constexpr registers::address_t DCS_HBCNT_ONHOLD_L = 31;
      inline constexpr registers::address_t DCS_HBCNT_ONHOLD_H = 32;
      inline constexpr registers::address_t DCS_PREFCNT_L = 33;
      inline constexpr registers::address_t DCS_PREFCNT_H = 34;
      inline constexpr registers::address_t DCS_DREQCNT_L = 35;
      inline constexpr registers::address_t DCS_DREQCNT_H = 36;
      inline constexpr registers::address_t DCS_DREQREAD_L = 37;
      inline constexpr registers::address_t DCS_DREQREAD_H = 38;
      inline constexpr registers::address_t DCS_DREQSENT_L = 39;
      inline constexpr registers::address_t DCS_DREQSENT_H = 40;
      inline constexpr registers::address_t DCS_DREQNULL_L = 41;
      inline constexpr registers::address_t DCS_DREQNULL_H = 42;
      inline constexpr registers::address_t DCS_SPILLCNT_L = 43;
      inline constexpr registers::address_t DCS_SPILLCNT_H = 44;
      inline constexpr registers::address_t DCS_HBTAG_0 = 45;
      inline constexpr registers::address_t DCS_HBTAG_1 = 46;
      inline constexpr registers::address_t DCS_HBTAG_2 = 47;
      inline constexpr registers::address_t DCS_PREFTAG_0 = 48;
      inline constexpr registers::address_t DCS_PREFTAG_1 = 49;
      inline constexpr registers::address_t DCS_PREFTAG_2 = 50;
      inline constexpr registers::address_t DCS_FETCHTAG_0 = 51;
      inline constexpr registers::address_t DCS_FETCHTAG_1 = 52;
      inline constexpr registers::address_t DCS_FETCHTAG_2 = 53;
      inline constexpr registers::address_t DCS_DREQTAG_0 = 54;
      inline constexpr registers::address_t DCS_DREQTAG_1 = 55;
      inline constexpr registers::address_t DCS_DREQTAG_2 = 56;
      inline constexpr registers::address_t DCS_OFFSETTAG_0 = 57;
      inline constexpr registers::address_t DCS_OFFSETTAG_1 = 58;
      inline constexpr registers::address_t DCS_OFFSETTAG_2 = 59;
      inline constexpr registers::address_t EVENT_TIMEOUT_L = 60;
      inline constexpr registers::address_t EVENT_TIMEOUT_H = 61;
      inline constexpr registers::address_t DCS_EVMCNT_L = 64;
      inline constexpr registers::address_t DCS_EVMCNT_H = 65;
      inline constexpr registers::address_t DCS_FULLTAG_0 = 66;
      inline constexpr registers::address_t DCS_FULLTAG_1 = 67;
      inline constexpr registers::address_t DCS_FULLTAG_2 = 68;
      inline constexpr registers::address_t TWI_BUSY = 69;
      inline constexpr registers::address_t TWI_CAL_DATAOUT = 70;
      inline constexpr registers::address_t TWI_HV_DATAOUT = 71;
      inline constexpr registers::address_t HB_TAG_ERR_CNT = 72;
      inline constexpr registers::address_t HB_DREQ_ERR_CNT = 73;
      inline constexpr registers::address_t HB_LOST_CNT = 74;
      inline constexpr registers::address_t EWM_LOST_CNT = 75;
      inline constexpr registers::address_t BAD_MARKER_CNT = 76;
      inline constexpr registers::address_t DCS_FORMAT_VER = 29;
      inline constexpr registers::address_t DCS_DTC_ID = 30;
      inline constexpr registers::address_t DCS_SUBSYSTEM_ID = 31;
      inline constexpr registers::address_t DCS_MEM_READ = 32;
      inline constexpr registers::address_t DCM_MEM_OFFSET_L = 33;
      inline constexpr registers::address_t DCM_MEM_OFFSET_H = 34;
      inline constexpr registers::address_t RESET_DIGI_FIFOS = 13;
      inline constexpr registers::address_t RESET_DDR = 14;
      inline constexpr registers::address_t EXT_IRQ = 15;
      inline constexpr registers::address_t DIGIRESET = 103;
      inline constexpr registers::address_t CAL_TWI_INIT = 23;
      inline constexpr registers::address_t CAL_TWI_DATA_IN = 24;
      inline constexpr registers::address_t CAL_TWI_ADDR = 25;
      inline constexpr registers::address_t HV_TWI_INIT = 26;
      inline constexpr registers::address_t HV_TWI_DATA_IN = 27;
      inline constexpr registers::address_t HV_TWI_ADDR = 28;
      inline constexpr registers::address_t LED = 9;
      inline constexpr registers::address_t DCS_CMD_STATUS = 128;
      inline constexpr registers::address_t DCS_TX_BUFFER_FIFO_STATUS = 129;
      inline constexpr registers::address_t DCS_RX_BUFFER_FIFO_STATUS = 130;
      inline constexpr registers::address_t DCS_PROG_RETURN = 132;
      inline constexpr registers::address_t DTC_PKT_COUNT = 144;
      inline constexpr registers::address_t DCS_PKT_COUNT = 145;
      inline constexpr registers::address_t DREQ_PKT_COUNT = 146;
      inline constexpr registers::address_t DREQ_HDR_PKT_COUNT = 147;
      inline constexpr registers::address_t DREQ_DATA_PKT_COUNT = 148;
      inline constexpr registers::address_t DREQ_EMPTY_PKT_COUNT = 149;
      inline constexpr registers::address_t DCS_DIAG_DATA = 255;
    } //namespace rocdcs
    namespace rocsc{
      inline constexpr registers::address_t DDRRESETN = 0x10;
      inline constexpr registers::address_t INVERTCALSPICLCK = 0x11;
      inline constexpr registers::address_t TIMERENABLE = 0x12;
      inline constexpr registers::address_t TIMERRESET = 0x13;
      inline constexpr registers::address_t TIMERCOUNTER = 0x14;
      inline constexpr registers::address_t DTCALIGNRESETN = 0x15;
      inline constexpr registers::address_t TVSRESETN = 0x16;
      inline constexpr registers::address_t LEDOFF = 0x17;
      inline constexpr registers::address_t DCSTEST = 0x18;
      inline constexpr registers::address_t IRQCLR = 0x19;
      inline constexpr registers::address_t DDRCTRLREADY = 0x3E;
      inline constexpr registers::address_t SERDESRE = 0x40;
      inline constexpr registers::address_t SERDESDATA = 0x41;
      inline constexpr registers::address_t SERDESFULL = 0x42;
      inline constexpr registers::address_t SERDESEMPTY = 0x43;
      inline constexpr registers::address_t SERDESRESET = 0x44;
      inline constexpr registers::address_t SERDESRDCNT = 0x45;
      inline constexpr registers::address_t SERDES_RE = 0x48;
      inline constexpr registers::address_t CALSPIINIT = 0x60;
      inline constexpr registers::address_t CALSPIBUSY = 0x61;
      inline constexpr registers::address_t CALSPIADDRESS = 0x62;
      inline constexpr registers::address_t CALSPIDATA = 0x63;
      inline constexpr registers::address_t HVSPIINIT = 0x70;
      inline constexpr registers::address_t HVSPIBUSY = 0x71;
      inline constexpr registers::address_t HVSPIADDRESS = 0x72;
      inline constexpr registers::address_t HVSPIDATA = 0x73;
      inline constexpr registers::address_t EWM = 0x80;
      inline constexpr registers::address_t EWMENABLE = 0x81;
      inline constexpr registers::address_t EWMDELAY = 0x82;
      inline constexpr registers::address_t ROCTVS_VAL = 0x90;
      inline constexpr registers::address_t ROCTVS_ADDR = 0x91;
      inline constexpr registers::address_t FIFO_RESET = 0xA3;
      inline constexpr registers::address_t ENABLE_FIBER_CLOCK = 0xB0;
      inline constexpr registers::address_t ENABLE_FIBER_MARKER = 0xB1;
      inline constexpr registers::address_t DTC_ENABLE_RESET = 0xB4;
      inline constexpr registers::address_t DIGI_SERDES_RESETS = 0xB5;
      inline constexpr registers::address_t DIGI_SERDES_ALIGNED = 0xB6;
      inline constexpr registers::address_t DIGI_SERDES_ALIGNMENT = 0xB7;
      inline constexpr registers::address_t CAL_SERDES_ERRORS = 0xB8;
      inline constexpr registers::address_t HV_SERDES_ERRORS = 0xB9;
      inline constexpr registers::address_t RS485_ADDR = 0xC3;
      inline constexpr registers::address_t RS485_DELAY = 0xC4;
      inline constexpr registers::address_t PRBS_EN = 0xD0;
      inline constexpr registers::address_t PRBS_ERROROUT = 0xD2;
      inline constexpr registers::address_t PRBS_ERRORCLR = 0xD3;
      inline constexpr registers::address_t PRBS_ERRORCNT = 0xD9;
      inline constexpr registers::address_t PRBS_ON = 0xDA;
      inline constexpr registers::address_t PRBS_LOCK = 0xDB;
      inline constexpr registers::address_t ERROR_ADDRESS = 0xE0;
      inline constexpr registers::address_t ERROR_COUNTER = 0xE1;
      inline constexpr registers::address_t DTCSERDES_RESET = 0xED;
      inline constexpr registers::address_t ROCDIGI_WORDALIGN = 0xEE;
      inline constexpr registers::address_t SERIALFORCEFULL = 0xEF;
      inline constexpr registers::address_t USE_UART = 0xF1;
      inline constexpr registers::address_t DIGIRW_SEL = 0xF2;
      inline constexpr registers::address_t LEAK_MUX = 0xF3;
      inline constexpr registers::address_t LEAK_SDIR = 0xF4;
      inline constexpr registers::address_t LEAK_SCLK = 0xF5;
      inline constexpr registers::address_t LEAK_SDA = 0xF6;
    } //namespace rocsc
    namespace rocsccmd{
    } //namespace rocsccmd
    namespace digi{
      inline constexpr registers::address_t HOWMANY = 0x03;
      inline constexpr registers::address_t LOOKBACK = 0x04;
      inline constexpr registers::address_t SDIO = 0x06;
      inline constexpr registers::address_t SPIREAD = 0x07;
      inline constexpr registers::address_t CS = 0x08;
      inline constexpr registers::address_t SCLK = 0x09;
      inline constexpr registers::address_t TRIGGERMODE = 0x0A;
      inline constexpr registers::address_t CHANNELMASK = 0x0B;
      inline constexpr registers::address_t ENABLEPULSER = 0x0C;
      inline constexpr registers::address_t CHANNELMASK2 = 0x0D;
      inline constexpr registers::address_t CHANNELMASK1 = 0x0E;
      inline constexpr registers::address_t CALIBRATE = 0x0F;
      inline constexpr registers::address_t FIFORESET = 0x10;
      inline constexpr registers::address_t READCHANNEL = 0x15;
      inline constexpr registers::address_t GT0 = 0x16;
      inline constexpr registers::address_t GT1 = 0x17;
      inline constexpr registers::address_t GT2 = 0x18;
      inline constexpr registers::address_t GT3 = 0x19;
      inline constexpr registers::address_t HV = 0x1A;
      inline constexpr registers::address_t CAL = 0x1B;
      inline constexpr registers::address_t COINC = 0x1C;
      inline constexpr registers::address_t LATCH = 0x1D;
      inline constexpr registers::address_t READHISTO = 0x20;
      inline constexpr registers::address_t HISTODATA = 0x22;
      inline constexpr registers::address_t SELECTHISTO = 0x23;
      inline constexpr registers::address_t TVS_VAL = 0x40;
      inline constexpr registers::address_t TVS_ADDR = 0x41;
      inline constexpr registers::address_t SELECTSMA = 0x50;
      inline constexpr registers::address_t SMAREADREQ = 0x51;
      inline constexpr registers::address_t SMADATA = 0x52;
      inline constexpr registers::address_t RX_CH_MASK1 = 0x60;
      inline constexpr registers::address_t RX_CH_MASK2 = 0x61;
      inline constexpr registers::address_t RX_CH_MASK3 = 0x62;
      inline constexpr registers::address_t BITALIGN_EWM_WIDTH = 0x63;
      inline constexpr registers::address_t BITALIGN_RSTRT = 0x64;
      inline constexpr registers::address_t BITSLIP_CONTROLLER_STRT = 0x65;
      inline constexpr registers::address_t BITALIGN_CMP1 = 0x66;
      inline constexpr registers::address_t BITALIGN_CMP2 = 0x67;
      inline constexpr registers::address_t BITALIGN_CMP3 = 0x68;
      inline constexpr registers::address_t BITALIGN_ERR1 = 0x69;
      inline constexpr registers::address_t BITALIGN_ERR2 = 0x6A;
      inline constexpr registers::address_t BITALIGN_ERR3 = 0x6B;
      inline constexpr registers::address_t BITSLIP_DONE1 = 0x6C;
      inline constexpr registers::address_t BITSLIP_DONE2 = 0x6D;
      inline constexpr registers::address_t BITSLIP_DONE3 = 0x6E;
      inline constexpr registers::address_t BITALIGN_RSETN = 0x6F;
      inline constexpr registers::address_t BSC_OPERATION_TYPE = 0x70;
      inline constexpr registers::address_t BSC_PATTERN_MATCH1 = 0x71;
      inline constexpr registers::address_t BSC_PATTERN_MATCH2 = 0x72;
      inline constexpr registers::address_t BSC_PATTERN_MATCH3 = 0x73;
      inline constexpr registers::address_t EWMEARLY = 0x81;
      inline constexpr registers::address_t EWMLATE = 0x82;
      inline constexpr registers::address_t SLOWPULSER = 0x85;
      inline constexpr registers::address_t DIGINUM = 0x90;
      inline constexpr registers::address_t EWM_FIFOS0 = 0xA4;
      inline constexpr registers::address_t EWM_FIFOS1 = 0xA5;
      inline constexpr registers::address_t EWM_FIFOS2 = 0xA6;
      inline constexpr registers::address_t SERDES_ALIGNMENT = 0xC0;
      inline constexpr registers::address_t SERDES_RESETS = 0xC1;
      inline constexpr registers::address_t EWM_COUNT1 = 0xD0;
      inline constexpr registers::address_t EWM_COUNT2 = 0xD1;
      inline constexpr registers::address_t EWM_COUNT3 = 0xD2;
      inline constexpr registers::address_t EW_EXPECTED_PERIOD = 0xE0;
      inline constexpr registers::address_t EW_MISSED_COUNT = 0xE1;
      inline constexpr registers::address_t EW_LAST_PERIOD = 0xE2;
      inline constexpr registers::address_t ADC_DIAG_SELECT = 0xF0;
      inline constexpr registers::address_t SYNC_ERR_CLEAR = 0xF3;
      inline constexpr registers::address_t EXT_PULSER_ENABLE = 0xF4;
      inline constexpr registers::address_t DELAYADC = 0xF5;
    } // namespace digi
  }; // namespace registers
} // namespace trkdaq

#endif
