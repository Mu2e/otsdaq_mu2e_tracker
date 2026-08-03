///////////////////////////////////////////////////////////////////////////////
// separate print routines
///////////////////////////////////////////////////////////////////////////////
#include "iostream"
#include "vector"
#include "DtcInterface.hh"
#include "TString.h"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterface"

using namespace DTCLib;
using namespace std;

#include "otsdaq-mu2e-tracker/Ui/ControlRocTypes.hh"
#include "otsdaq-mu2e-tracker/Ui/TrackerRegisters.hh"

namespace trkdaq {

//-----------------------------------------------------------------------------
// print value of the DIGI register Reg, for multiple ROCs and both CAL and HV sides
//-----------------------------------------------------------------------------
  void DtcInterface::PrintDigiRegister(uint32_t Reg, std::string& Desc, int Format, int LinkMask, std::ostream& Stream) {
    int rc(0);
    
    TLOG(TLVL_DEBUG+1) << std::format("-- START: Reg:{} Format:{} LinkMask:0x{:08x}",Reg,Format,LinkMask);
    
    int print_level(0);
    
    trkdaq::ControlRoc_DigiRW_Input_t  par;
    trkdaq::ControlRoc_DigiRW_Output_t pcal;
    trkdaq::ControlRoc_DigiRW_Output_t phv;

    par.rw           = 0;                  // read
    par.address      = Reg;
    par.data[0]      = 0;
    par.data[1]      = 0;

    int format       = Format % 100;
    int soft_console = Format / 100;

    std::string text;
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      // need this if accidentally called directly
      if (not LinkLocked(i)) {
        TLOG(TLVL_ERROR) << std::format("link:{} enabled but not locked",i);
        continue;
      }
//-----------------------------------------------------------------------------
// read the register
//-----------------------------------------------------------------------------
      DTC_Link_ID link = DTC_Link_ID(i);

      par.hvcal        = fpga::digi::cal;
      if (soft_console) par.hvcal = fpga::digi::roc;
      
      rc               = ControlRoc_DigiRW(&par,&pcal,link,print_level,Stream);
      if (rc < 0) {
        TLOG(TLVL_ERROR) << std::format("failed to read CAL side");
      }
      text            += std::format("  0x{:04x}",pcal.data[0]);

      par.hvcal        = fpga::digi::hv;
      if (soft_console) par.hvcal = fpga::digi::roc;
      
      rc               = ControlRoc_DigiRW(&par,&phv,link,print_level,Stream);
      text            += std::format(" 0x{:04x}",phv.data[0]);
    }

    std::string sreg   = std::format("reg 0x{:04x}",Reg);

    if (format == 1) text += Form("  %s",Desc.data());
    Stream << Form("%-12s %s\n",sreg.data(),text.data());

    TLOG(TLVL_DEBUG+1) << std::format("-- END");
  }

//-----------------------------------------------------------------------------
// most of the time Link = -1 meaning 'all enabled links'
// otherwise it is the link to print
//-----------------------------------------------------------------------------
  int DtcInterface::PrintDigis(uint32_t Format, int Link, std::ostream& Stream) {
    int rc(0);
    int link_mask(0);
    
    TLOG(TLVL_DBG) << Form("Format=%i Link:%i \n",Format,Link);

    int lnk1(Link), lnk2(Link+1);
    if (Link == -1) {
      lnk1 = 0;
      lnk2 = 6;
    }

    std::string text(" Register      ");
    for (int i=lnk1; i<lnk2; i++) {
      int enabled = LinkEnabled(i);
      if (enabled == 0)                                     continue;
      if (not LinkLocked(i)) {
        TLOG(TLVL_ERROR) << std::format("link:{} enabled but not locked",i);
        continue;
      }
      link_mask |= (1 << 4*i);
      text += Form("     ROC%i      ",i);
    }

    TLOG(TLVL_INFO) << std::format("link_mask:0x{:04x}",link_mask);
    
    if (link_mask == 0) {
      std::string msg = std::format("dtc:{} link:{} : no locked links.",PcieAddr(),Link);
      Stream << " ERROR: " << msg << "\n";
      TLOG(TLVL_ERROR) << msg;
      return rc;
    }
                     
    if (Format != 0) text += "Description";
    Stream << Form("%s\n",text.data());
    Stream << "-----------------------------------------------------------------------------";
    Stream << "-----------------------------------\n";

    std::string desc;
    uint32_t    reg;
    
    reg =  registers::digi::EWM_FIFOS0; desc = "EWM_FIFOS0";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);
    
    reg =  registers::digi::EWM_FIFOS1; desc = "EWM_FIFOS1";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);

    reg =  registers::digi::EWM_FIFOS2; desc = "EWM_FIFOS2";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);

    reg =  registers::digi::SERDES_ALIGNMENT; desc = "DIGI SERDES ALIGNMENT";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);

    reg =  registers::digi::EWM_COUNT1; desc = "EWM_COUNT1";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);
    
    reg =  registers::digi::EWM_COUNT2; desc = "EWM_COUNT2";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);
    
    reg =  registers::digi::EWM_COUNT3; desc = "EWM_COUNT3";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);

    reg =  registers::digi::EW_MISSED_COUNT; desc = "EW_MISSED_COUNT";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);

    reg =  registers::digi::EW_LAST_PERIOD; desc = "EW_LAST_PERIOD";
    PrintDigiRegister(reg,desc,Format,link_mask,Stream);
//-----------------------------------------------------------------------------
// the following is a hack, as B6-B9 are the soft_console registers
//-----------------------------------------------------------------------------
    reg =  registers::rocsc::DIGI_SERDES_ALIGNED; desc = "ROC from DIGI serdes aligned";
    PrintDigiRegister(reg,desc,Format+100,link_mask,Stream);

    reg =  registers::rocsc::DIGI_SERDES_ALIGNMENT; desc = "ROC from DIGI serdes alignment";
    PrintDigiRegister(reg,desc,Format+100,link_mask,Stream);

    reg =  registers::rocsc::CAL_SERDES_ERRORS; desc = "ROC from digi CAL serdes errors";
    PrintDigiRegister(reg,desc,Format+100,link_mask,Stream);

    reg =  registers::rocsc::HV_SERDES_ERRORS; desc = "ROC from digi HV serdes errors";
    PrintDigiRegister(reg,desc,Format+100,link_mask,Stream);

    return 0;
    
  }
//-----------------------------------------------------------------------------
// print value of the register Reg, for multiple ROCs
//-----------------------------------------------------------------------------
  void DtcInterface::PrintRocRegister(uint32_t Reg, std::string& Desc, int Format, int LinkMask,std::ostream& Stream) {
    TLOG(TLVL_DEBUG+1) << std::format("-- START: Reg:{} Format:{} LinkMask:0x{:08x}",Reg,Format,LinkMask);
    
    std::string text;
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      // need this if accidentally called directly
      if (not LinkLocked(i)) {
        TLOG(TLVL_ERROR) << std::format("link:{} enabled but not locked",i);
        continue;
      }
      
      DTC_Link_ID link = DTC_Link_ID(i);
      uint32_t dat;
      
      dat = fDtc->ReadROCRegister(link,Reg,100);
      text += Form("     0x%04x",dat);
    }
    std::string sreg = Form("reg(%2i)",Reg);

    if (Format == 1) text += Form(" %s",Desc.data());
    Stream << Form("%-18s %s\n",sreg.data(),text.data());

    TLOG(TLVL_DEBUG+1) << std::format("-- END");
  }

  //-----------------------------------------------------------------------------
  void DtcInterface::PrintRocRegister2(uint Reg, std::string& Desc, int Format, int LinkMask, std::ostream& Stream) {

    TLOG(TLVL_DEBUG+1) << std::format("-- START: Reg:{} Format:{} LinkMask:0x{:08x}",Reg,Format,LinkMask);

    std::string text;
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      // need this if accidentally called directly
      if (not LinkLocked(i)) {
        TLOG(TLVL_ERROR) << std::format("link:{} enabled but not locked",i);
        continue;
      }
      
      DTC_Link_ID link = DTC_Link_ID(i);
      uint32_t iw1, iw2, iw;
      
      iw1 = fDtc->ReadROCRegister(link,Reg  ,100);
      iw2 = fDtc->ReadROCRegister(link,Reg+1,100);
      iw  = (iw2 << 16) | iw1;
      text += Form(" 0x%08x",iw);
    }

    if (Format == 1) text += Form(" %s",Desc.data());

    std::string sreg = Form("reg(%2i)<<16|reg(%2i)",Reg+1,Reg);

    Stream << Form("%-18s%s\n",sreg.data(),text.data());

    TLOG(TLVL_DEBUG+1) << std::format("-- END");
  }
  
//-----------------------------------------------------------------------------
// most of the time Link = -1 meaning 'all enabled links'
// otherwise it is the link to print
//-----------------------------------------------------------------------------
  int DtcInterface::PrintRocStatus(uint32_t Format, int Link, std::ostream& Stream) {
    int rc(0);
    
    TLOG(TLVL_DBG) << Form("Format=%i Link:%i \n",Format,Link);

    std::string desc;

    int lnk1(Link), lnk2(Link+1);
    if (Link == -1) {
      lnk1 = 0;
      lnk2 = 6;
    }

    uint reg;

    int link_mask(0);

    std::string text("        Register     ");
    for (int i=lnk1; i<lnk2; i++) {
      int enabled = LinkEnabled(i);
      if (enabled == 0)                                     continue;
      if (not LinkLocked(i)) {
        TLOG(TLVL_ERROR) << std::format("link:{} enabled but not locked",i);
        continue;
      }
      link_mask |= (1 << 4*i);
      text += Form("    ROC%i   ",i);
    }

    if (link_mask == 0) {
      std::string msg = std::format("dtc:{} link:{} : no locked links.",PcieAddr(),Link);
      Stream << " ERROR: " << msg << "\n";
      TLOG(TLVL_ERROR) << msg;
      return rc;
    }
                     
    if (Format != 0) text += " Description";
    Stream << Form("%s\n",text.data());
    Stream << "------------------------------------------------------------------------\n";

    reg = registers::rocdcs::DBG; desc = "ALWAYS 0x1234";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::ROC_STATUS; desc = "ROC FIFO status";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::ROC_ENABLE; desc = "ROC pattern mode ??"; 
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::EVENT_TIMEOUT_L; desc = "ROC readout timeout delay "; 
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::DCS_DDR_ADDRESS_L; desc = "N simulated hits per lane"; //FIXME???
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::DCS_DDR_ADDRESS_H; desc = " ??";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::LOSS_LOCK; desc = "Fiber loss/lock counter";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::TWI_CONTROL; desc = "Bad Markers counter"; //FIXME???
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::LOOPBACK_COARSE_DELAY; desc = "Loopback coarse delay";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::DREQ_FIFO_WRCNT; desc = "SIZE_FIFO_FULL [28]+STORE_POS[25:24]+STORE_CNT[19:0]";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DREQ_FIFO_RDCNT; desc = "SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::EWM_CNT_L; desc = "Num EWM seen";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
    
    reg = registers::rocdcs::DCS_EVMCNT_L; desc = "Num windows seen";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_HB_CNT_L; desc = "Num HB seen";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_NULLHB_CNT_L; desc = "Num null HB seen";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_HBCNT_ONHOLD_L; desc = "Num HB on hold";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_PREFCNT_L; desc = "Num PREFETCH seen";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    Stream << Form("\n");

    reg = registers::rocdcs::DATAREQ_CNT_L; desc = "Num DATA REQ seen";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_DREQCNT_L; desc = "Num DATA REQ written to DDR";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::IS_SKIPPED_DREQ_CNT; desc = "Num skipped DATA REQ";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_DREQREAD_L; desc = "Num DATA REQ read from DDR";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::DCS_DREQSENT_L; desc = "Num DATA REQ sent to DTC";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::DCS_DREQNULL_L; desc = "Num DATA REQ with null data";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    Stream << Form("\n");
      
    reg = registers::rocdcs::DCS_SPILLCNT_L; desc = "Last spill tag";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::DCS_HBTAG_0; desc = "Last HB tag";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::DCS_PREFTAG_0; desc = "Last PREFETCH tag";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::DCS_FETCHTAG_0; desc = "Last fetched tag";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_DREQTAG_0; desc = "Last DATA REQ tag";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DCS_OFFSETTAG_0; desc = "OFFSET tag";
    PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    Stream << std::endl;
      
    reg = registers::rocdcs::HB_TAG_ERR_CNT; desc = "Num HB tag inconsistencies";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::HB_DREQ_ERR_CNT; desc = "Num DATA REQ tag inconsistencies";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::HB_LOST_CNT; desc = "Num HB tag lost";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);
      
    reg = registers::rocdcs::EWM_LOST_CNT; desc = "Num DATA REQ tag lost";
    PrintRocRegister(reg,desc,Format,link_mask,Stream);

    reg = registers::rocdcs::DTC_PKT_COUNT; desc = "total N packets (DCS+data)";    // r_144
    PrintRocRegister(reg,desc,Format,link_mask,Stream); // 
    reg = registers::rocdcs::DCS_PKT_COUNT; desc = "N(DCS) packets sent to DTC";    // r_145
    PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = registers::rocdcs::DREQ_PKT_COUNT; desc = "Num of non-DCS packets";        // r_146
    PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = registers::rocdcs::DREQ_HDR_PKT_COUNT; desc = "Num of data header packets";    // r_147
    PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = registers::rocdcs::DREQ_DATA_PKT_COUNT; desc = "Num of data payload packets";   // r_148
    PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = registers::rocdcs::DREQ_EMPTY_PKT_COUNT; desc = "Num of empty data packets";     // r_149
    PrintRocRegister(reg,desc,Format,link_mask,Stream); //

    Stream << "------------------------------------------------------------------------\n";
    TLOG(TLVL_DEBUG+1) << std::format("-- END");
    return rc;
  }


//-----------------------------------------------------------------------------
  void DtcInterface::PrintRatesSingleRoc(std::vector<uint16_t>* Rates, std::vector<int>* ChMask, std::ostream& Stream) {
    
//-----------------------------------------------------------------------------
// formatted printout
// should be 96*3*2+2*2 = 580 16-bit words
// 3 words per channel (straw)
// total counts the time in 200 MHz clock units, convert printed rate to kHz
//-----------------------------------------------------------------------------
    int nw = Rates->size();
    if (nw != 580) {
      TLOG(TLVL_ERROR) << "nw:" << nw << " != 580. BAIL OUT";
      return;
    }
//-----------------------------------------------------------------------------
// finally, the last two words - total counts
//-----------------------------------------------------------------------------
    float total[2];          // [0]:CAL  [1]:HV , as in lanes, an inversion takes place
    float clock_tick(5.e-9); // 5 ns <-> 200 MHz clock
      
    int loc = 576;   // = 96*6
    
    total[1]  = float((*Rates)[loc  ])+(int((*Rates)[loc+1]) << 16); // hv - check the order with Vadim
    total[0]  = float((*Rates)[loc+2])+(int((*Rates)[loc+3]) << 16); // cal

    Stream << " channel  mask Total(HV) Total(CAL) Total(HV.and.CAL)  Rate(HV)   Rate(CAL)   Rate(HV.and.CAL)\n";
    Stream << "-----------------------------------------------------------------------------------------\n";

    for (int ich=0; ich<96; ich++) {
      loc               = 6*ich;
      int   counts_hv   = int((*Rates)[loc  ])+(int((*Rates)[loc+1]) << 16);
      int   counts_cal  = int((*Rates)[loc+2])+(int((*Rates)[loc+3]) << 16);
      int   counts_coin = int((*Rates)[loc+4])+(int((*Rates)[loc+5]) << 16);
      int   ifpga        = fgFpga[ich];
      float rate_hv     = counts_hv /total[ifpga]/clock_tick/1000.;
      float rate_cal    = counts_cal/total[ifpga]/clock_tick/1000.;
      float rate_coin   = counts_coin/(total[0]+total[1])*2/clock_tick/1000.;
      
      int ch_mask = 1;
      if ((ChMask != nullptr) and (ChMask->size() == 96) and (ChMask->at(ich) == 0)) {
        ch_mask = 0;
      }
    
      Stream << std::format("- {:5d} {:3d} {:10d} {:10d} {:10d}         {:10.3f} {:10.3f} {:10.3f}\n",
                            ich,ch_mask,counts_hv,counts_cal,counts_coin,
                            rate_hv,rate_cal,rate_coin);
    }
      
    Stream << std::format(" total_hv: {:10.0f} total_cal: {:10.0f}\n",total[1],total[0]);
  }

//-----------------------------------------------------------------------------
  void DtcInterface::PrintRatesAllRocs(std::vector<uint16_t>* Rates, std::vector<int>* ChMask, std::ostream& Stream) {
//-----------------------------------------------------------------------------
// do the printing
// bit 2: formattted printout, parallel
// total counts the time in 200 MHz clock units, convert printed rate to kHz
//-----------------------------------------------------------------------------
    float clock_tick(5.e-9); // 5 ns <-> 200 MHz clock
    
    Stream << "ch|   link 0     |   link 1     |   link 2     |   link 3     |   link 4     |   link 5     |\n";
    Stream << "  | counts rate  | counts rate  | counts rate  | counts rate  | counts rate  | counts rate  |\n";
    Stream << "--------------------------------------------------------------------------------------------\n";

    float total[6][2];          // [0]:CAL  [1]:HV , as in lanes, an inversion takes place

    int loc = 576;
    for (int lnk=0; lnk<6; lnk++) {
      std::vector<uint16_t>* dat = &Rates[lnk];
      int nw = dat->size();
      // Stream << std::dec << "lnk:" << lnk << " nw:" << nw <<  " ChMask[lnk].size():"<< ChMask[lnk].size() << std::endl;
      if ((LinkEnabled(lnk) == 0) or (nw != 580)) {
        total[lnk][0] = -1;
        total[lnk][1] = -1;
      }
      else {
        total[lnk][1]  = float((*dat)[loc  ])+(int((*dat)[loc+1]) << 16); // hv - check the order with Vadim
        total[lnk][0]  = float((*dat)[loc+2])+(int((*dat)[loc+3]) << 16); // cal
      }
    }

    for (int ich=0; ich<96; ich++) {
      int loc               = 6*ich;
      Stream << std::format("{:2d}|",ich);
      
      for (int lnk=0; lnk<6; lnk++) {
        std::vector<uint16_t>* dat = &Rates[lnk];
        int nw = dat->size();

        char c = '|';
        // int ch_mask = 1;
        if ((ChMask[lnk].size() == 96) and (ChMask[lnk].at(ich) == 0)) {
          // ch_mask = 0;
           c = '*';
        }
        if ((LinkEnabled(lnk) == 0) or (nw != 580)) {
          Stream << "              " << c;
        }
        else {
          int   counts_coin = int((*dat)[loc+4])+(int((*dat)[loc+5]) << 16);
          float rate_coin   = counts_coin/(total[lnk][0]+total[lnk][1])*2/clock_tick/1000.;
          Stream << std::format("     {:8.3f} {:c}",rate_coin,c);
        }
      }
      
      Stream << std::endl;
    }
  }
  
//-----------------------------------------------------------------------------
// assume always data from 6 ROCs, some may be disabled
//-----------------------------------------------------------------------------
  void DtcInterface::PrintSpiAll(trkdaq::TrkSpiData_t* Spi, std::ostream& Stream) {

    Stream << " SPI Parameter ";
    for (int lnk=0; lnk<6; lnk++) {
      if (not LinkEnabled(lnk)) continue;
      Stream << std::format("    link {:}",lnk);
    }
    Stream << std::endl
           << "--------------------------------------------------------------------------"
           << std::endl;
    
    for (int i=0; i<TrkSpiDataNWords; ++i) {
      Stream << std::format("{:15s}",fgSpiVarName[i]);
      for (int lnk=0; lnk<6; lnk++) {
        if (not LinkEnabled(lnk)) continue;
        Stream << std::format("{:10.3f}",Spi[lnk].Data(i));
      }
      Stream << std::endl;
    }
  }


//-----------------------------------------------------------------------------
  void DtcInterface::PrintSumThresholds(std::vector<float>* Thresholds, uint32_t MaskC, uint32_t MaskD, uint32_t MaskE, int PrintLevel, std::ostream& Stream) {
//-----------------------------------------------------------------------------
// do the printing
// bit 2: formattted printout, parallel
//-----------------------------------------------------------------------------
//    float clock_tick(5.e-9); // 5 ns <-> 200 MHz clock
    
    int mask[3];
    mask[0] = MaskC;
    mask[1] = MaskD;
    mask[2] = MaskE;
    
    Stream << "ch|   link 0     |   link 1     |   link 2     |   link 3     |   link 4     |   link 5     |\n";
    Stream << "  |              |              |              |              |              |              |\n";
    Stream << "--------------------------------------------------------------------------------------------\n";

    // float total[6];          // [0]:CAL  [1]:HV , as in lanes, an inversion takes place

      // if the size in zero, don't print the link
    for (int ich=0; ich<96; ich++) {
      int iw = ich/32;
      int ib = ich -iw*32;

      if (((mask[iw] >> ib) & 0x1) == 0)                    continue;
        
      Stream << std::format("{:2d}|",ich);
      
      for (int lnk=0; lnk<6; lnk++) {
        // int loc               = 6*ich;
      
        std::vector<float>* dat = &Thresholds[lnk];
        int nw = dat->size();

        char c = '|';
        if ((nw == 0) or (nw < 3*ich)) {
          Stream << "              " << c;
        }
        else {
          float sum_thr = dat->at(3*ich+2);
          Stream << std::format("     {:8.3f}  {:c}",sum_thr,c);
        }
      }
      
      Stream << std::endl;
    }
  }
  
//-----------------------------------------------------------------------------
// Link: link number
// expect that in most cases read all channels : all masks are set to 0xFFFFFFFF
//-----------------------------------------------------------------------------
  int DtcInterface::PrintThresholds(int                 Link,
                                    std::vector<float>& Thr ,
                                    uint32_t            MaskC,
                                    uint32_t            MaskD,
                                    uint32_t            MaskE,
                                    int                 PrintLevel,
                                    std::ostream&       Stream) {
//-----------------------------------------------------------------------------
//  print, if requested
//-----------------------------------------------------------------------------
    if (PrintLevel & 0x2) {
      uint32_t mask[3];
      mask[0] = MaskC;
      mask[1] = MaskD;
      mask[2] = MaskE;
//-----------------------------------------------------------------------------
// to keep the output compact, print thresholds only for the channels defined by the mask
//-----------------------------------------------------------------------------
      printf(" chID     thr(CAL)    thr(HV)    sum  \n");
      printf("--------------------------------------\n");
      for (int i=0; i<96; i++) {
        int iw = i/32;
        int ib = i -iw*32;

        if (((mask[iw] >> ib) & 0x1) == 1) {
          float hw  = Thr[3*i  ];
          float cal = Thr[3*i+1];
          float tot = Thr[3*i+2];
          Stream << std::format(" {:4d} {:10.3f} {:10.3f} {:10.3f}",i,hw,cal,tot) << std::endl;
        }
      }
    }
    return 0;
  }
  
};
