
//-----------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//-----------------------------------------------------------------------------


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form

#include <filesystem>


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"


//==============================================================================
///	Ui_print_PrintRocRegister()
/// print value of the register Reg, for multiple ROCs
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_print_PrintRocRegister(uint Reg, std::string& Desc, int Format, int LinkMask,std::ostream& Stream) 
{

    std::string text;
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      
      DTCLib::DTC_Link_ID link = DTCLib::DTC_Link_ID(i);
      uint32_t dat;
      
      dat = getDTC()->ReadROCRegister(link,Reg,100);
      text += Form("     0x%04x",dat);
    }
    std::string sreg = Form("reg(%2i)",Reg);

    if (Format == 1) text += Form(" %s",Desc.data());
    Stream << Form("%-18s %s\n",sreg.data(),text.data());
  } // end Ui_print_PrintRocRegister()

//==============================================================================
///	Ui_print_PrintRocRegister2()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_print_PrintRocRegister2(uint Reg, std::string& Desc, int Format, int LinkMask, std::ostream& Stream) 
{

    std::string text;
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      
      DTCLib::DTC_Link_ID link = DTCLib::DTC_Link_ID(i);
      uint32_t iw1, iw2, iw;
      
      iw1 = getDTC()->ReadROCRegister(link,Reg  ,100);
      iw2 = getDTC()->ReadROCRegister(link,Reg+1,100);
      iw  = (iw2 << 16) | iw1;
      text += Form(" 0x%08x",iw);
    }

    if (Format == 1) text += Form(" %s",Desc.data());

    std::string sreg = Form("reg(%2i)<<16|reg(%2i)",Reg+1,Reg);

    Stream << Form("%-18s%s\n",sreg.data(),text.data());
  } // end Ui_print_PrintRocRegister2()

//==============================================================================
///	Ui_print_PrintRocStatus()
/// most of the time Link = -1 meaning 'all enabled links'
/// otherwise it is the link to print
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_print_PrintRocStatus(uint32_t Format, int Link, std::ostream& Stream) 
{
    TLOG(TLVL_DBG+1) << Form("Format=%i Link:%i \n",Format,Link);

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
      link_mask |= (1 << 4*i);
      text += Form("    ROC%i   ",i);
    }
                     
    if (Format != 0) text += " Description";
    Stream << Form("%s\n",text.data());
    Stream << "------------------------------------------------------------------------\n";

    reg =  0; desc = "ALWAYS 0x1234";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = 18; desc = " ??";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg =  8; desc = "ROC pattern mode ??"; 
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = 60; desc = "ROC readout timeout delay "; 
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = 15; desc = "N simulated hits per lane";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = 16; desc = " ??";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg =  7; desc = "Fiber loss/lock counter";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);

    reg =  6; desc = "Bad Markers counter";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg =  4; desc = "Loopback coarse delay";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
    
    reg = 23; desc = "SIZE_FIFO_FULL [28]+STORE_POS[25:24]+STORE_CNT[19:0]";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 25; desc = "SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 11; desc = "Num EWM seen";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
    
    reg = 64; desc = "Num windows seen";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 27; desc = "Num HB seen";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 29; desc = "Num null HB seen";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 31; desc = "Num HB on hold";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 33; desc = "Num PREFETCH seen";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    std::cout << Form("\n");

    reg =  9; desc = "Num DATA REQ seen";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 35; desc = "Num DATA REQ written to DDR";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 13; desc = "Num skipped DATA REQ";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);

    reg = 37; desc = "Num DATA REQ read from DDR";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = 39; desc = "Num DATA REQ sent to DTC";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = 41; desc = "Num DATA REQ with null data";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    std::cout << Form("\n");
      
    reg = 43; desc = "Last spill tag";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = 45; desc = "Last HB tag";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = 48; desc = "Last PREFETCH tag";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    reg = 51; desc = "Last fetched tag";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 54; desc = "Last DATA REQ tag";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);

    reg = 57; desc = "OFFSET tag";
    Ui_print_PrintRocRegister2(reg,desc,Format,link_mask,Stream);
      
    std::cout << std::endl;
      
    reg = 72; desc = "Num HB tag inconsistencies";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
      
    reg = 73; desc = "Num DATA REQ tag inconsistencies";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
      
    reg = 74; desc = "Num HB tag lost";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);
      
    reg = 75; desc = "Num DATA REQ tag lost";
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream);

    reg = 0x90; desc = "total N packets (DCS+data)";    // r_144
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream); // 
    reg = 0x91; desc = "N(DCS) packets sent to DTC";    // r_145
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = 0x92; desc = "Num of non-DCS packets";        // r_146
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = 0x93; desc = "Num of data header packets";    // r_147
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = 0x94; desc = "Num of data payload packets";   // r_148
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream); //
    reg = 0x95; desc = "Num of empty data packets";     // r_149
    Ui_print_PrintRocRegister(reg,desc,Format,link_mask,Stream); //

    Stream << "------------------------------------------------------------------------\n";
  } // end Ui_print_PrintRocStatus()

//==============================================================================
///	Ui_print_PrintBuffer()
/// 'nw' : number of 16-bit words to print.
/// if Stream == nullptr , use TLOG, otherwise - *Stream
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_print_PrintBuffer(const void* ptr, int nw, int Offset, std::ostream* Stream) 
{

    ushort*      p16 = (ushort*) ptr;

    int          n(0);
    std::string  line;

    // if (Stream == nullptr) { TLOG(TLVL_DEBUG) << Form("-------- nw = %i\n",nw); }
    // else                   { (*Stream)        << Form("-------- nw = %i\n",nw); }
   
    for (int i=0; i<nw; i++) {
      if (n == 0) line = Form("0x%08x:",i*2+Offset);
      ushort  word = p16[i];
      line += Form(" 0x%04x",word);
      
      n   += 1;
      if (n == 8) {
        if (Stream == nullptr) TLOG(TLVL_INFO) << line << std::endl;
        else {
          (*Stream)          << line << std::endl;
          TLOG(TLVL_DEBUG+1) << line << std::endl;
        }
        n = 0;
      }
    }
    
    if (n != 0) {
      if (Stream == nullptr) TLOG(TLVL_INFO) << line << std::endl;
      else {
        (*Stream)          << line << std::endl;
        TLOG(TLVL_DEBUG+1) << line << std::endl;
      }
    }
  } // end Ui_print_PrintBuffer()

//==============================================================================
///	Ui_print_PrintRatesSingleRoc()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_print_PrintRatesSingleRoc(std::vector<uint16_t>* Rates, std::vector<int>* ChMask, std::ostream& Stream) 
{
    
//-----------------------------------------------------------------------------
// formatted printout
// should be 96*3*2+2*2 = 580 16-bit words
// 3 words per channel (straw)
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
      int   fpga        = fgFpga[ich];
      float rate_hv     = counts_hv /total[fpga]/clock_tick/1000.;
      float rate_cal    = counts_cal/total[fpga]/clock_tick/1000.;
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
  } // end Ui_print_PrintRatesSingleRoc()

//==============================================================================
///	Ui_print_PrintRatesAllRocs()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_print_PrintRatesAllRocs(std::vector<uint16_t>* Rates, std::vector<int>* ChMask, std::ostream& Stream) 
{
//-----------------------------------------------------------------------------
// do the printing
// bit 2: formattted printout, parallel
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
          Stream << "             " << c;
        }
        else {
          int   counts_coin = int((*dat)[loc+4])+(int((*dat)[loc+5]) << 16);
          float rate_coin   = counts_coin/(total[lnk][0]+total[lnk][1])*2/clock_tick/1000.;
          Stream << std::format("     {:8.3f} {:c}",rate_coin,c);
        }
      }
      
      Stream << std::endl;
    }
  } // end Ui_print_PrintRatesAllRocs()

//==============================================================================
///	Ui_print_PrintSpiAll()
/// assume always data from 6 ROCs, some may be disabled
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc on Tue Dec 16 13:34:58 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_print.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_print_PrintSpiAll(trkdaq::TrkSpiData_t* Spi, std::ostream& Stream) 
{

    Stream << " SPI Parameter ";
    for (int lnk=0; lnk<6; lnk++) {
      if (not LinkEnabled(lnk)) continue;
      Stream << std::format("    link {:}",lnk);
    }
    Stream << std::endl
           << "--------------------------------------------------------------------------"
           << std::endl;
    
    for (int i=0; i<trkdaq::TrkSpiDataNWords; ++i) {
      Stream << std::format("{:15s}",fgSpiVarName[i]);
      for (int lnk=0; lnk<6; lnk++) {
        if (not LinkEnabled(lnk)) continue;
        Stream << std::format("{:10.3f}",Spi[lnk].Data(i));
      }
      Stream << std::endl;
    }
  } // end Ui_print_PrintSpiAll()

