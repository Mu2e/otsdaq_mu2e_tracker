///////////////////////////////////////////////////////////////////////////////
// separate print routines
///////////////////////////////////////////////////////////////////////////////
#include "iostream"
#include "vector"
#include "DtcInterface.hh"
#include "TString.h"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterface_print"

using namespace DTCLib;
using namespace std;

namespace trkdaq {

//-----------------------------------------------------------------------------
// 'nw' : number of 16-bit words to print
//-----------------------------------------------------------------------------
  void DtcInterface::PrintBuffer(const void* ptr, int nw) {

    ushort* p16 = (ushort*) ptr;
    int     n   = 0;

    std::cout << Form("-------- nw = %i\n",nw);
    for (int i=0; i<nw; i++) {
      if (n == 0) cout << Form(" 0x%08x: ",i*2);
      
      ushort  word = p16[i];
      cout << Form("0x%04x ",word);
      
      n   += 1;
      if (n == 8) {
        cout << std::endl;
        n = 0;
      }
    }
    
    if (n != 0) cout << std::endl;
  }

//-----------------------------------------------------------------------------
  void DtcInterface::PrintFireflyTemp() {
    int tmo_ms(50);
    TLOG(TLVL_DEBUG) << "START" << std::endl;
//-----------------------------------------------------------------------------
// read RX firefly temp
//------------------------------------------------------------------------------
    fDtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000100);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
    fDtc->GetDevice()->write_register(0x9288,tmo_ms,0x50160000);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
    fDtc->GetDevice()->write_register(0x928c,tmo_ms,0x00000002);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
    fDtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000000);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));

    uint data, rx_temp, txrx_temp;

    fDtc->GetDevice()->read_register(0x9288,tmo_ms,&data);
    rx_temp = data & 0xff;
//-----------------------------------------------------------------------------
// read TX/RX firefly temp
//------------------------------------------------------------------------------
    fDtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000400);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
    fDtc->GetDevice()->write_register(0x92a8,tmo_ms,0x50160000);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
    fDtc->GetDevice()->write_register(0x92ac,tmo_ms,0x00000002);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
    fDtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000000);
    std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));

    fDtc->GetDevice()->read_register(0x92a8,tmo_ms,&data);
    txrx_temp = data & 0xff;

    cout << "rx_temp: " << rx_temp << " txrx_temp: " << txrx_temp << endl;
    
    TLOG(TLVL_DEBUG) << "END" << std::endl;
  }

//-----------------------------------------------------------------------------
// print value of the register Reg, for multiple ROCs
//-----------------------------------------------------------------------------
  void DtcInterface::PrintRocRegister(uint Reg, std::string& Desc, int Format, int LinkMask) {

    std::string text;
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      
      DTC_Link_ID link = DTC_Link_ID(i);
      uint32_t dat;
      
      dat = fDtc->ReadROCRegister(link,Reg,100);
      text += Form("     0x%04x",dat);
    }
    std::string sreg = Form("reg(%2i)",Reg);

    if (Format == 1) text += Form(" %s",Desc.data());
    cout << Form("%-18s %s\n",sreg.data(),text.data());
  }

  //-----------------------------------------------------------------------------
  void DtcInterface::PrintRocRegister2(uint Reg, std::string& Desc, int Format, int LinkMask) {

    std::string text;
    for (int i=0; i<6; i++) {
      int used = (LinkMask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      
      DTC_Link_ID link = DTC_Link_ID(i);
      uint32_t iw1, iw2, iw;
      
      iw1 = fDtc->ReadROCRegister(link,Reg  ,100);
      iw2 = fDtc->ReadROCRegister(link,Reg+1,100);
      iw  = (iw2 << 16) | iw1;
      text += Form(" 0x%08x",iw);
    }

    if (Format == 1) text += Form(" %s",Desc.data());

    std::string sreg = Form("reg(%2i)<<16|reg(%2i)",Reg+1,Reg);

    cout << Form("%-18s%s\n",sreg.data(),text.data());
  }
  
  
//-----------------------------------------------------------------------------
// most of the time LinkMask = -1
//-----------------------------------------------------------------------------
  void DtcInterface::PrintRocStatus(int Format, int LinkMask) {
    TLOG(TLVL_DEBUG) << Form("Format=%i LinkMask 0x%08x \n",Format,LinkMask);

    std::string desc;

    int link_mask = LinkMask;
    if (LinkMask == -1) link_mask = fLinkMask;

    uint reg;

    std::string text("        Register     ");
    for (int i=0; i<6; i++) {
      int used = (link_mask >> 4*i) & 0x1;
      if (used == 0)                                        continue;
      text += Form("    ROC%i   ",i);
    }
                     
    if (Format != 0) text += " Description";
    cout << Form("%s\n",text.data());
    cout << "------------------------------------------------------------------------\n";

    reg =  0; desc = "ALWAYS 0x1234";
    PrintRocRegister(reg,desc,Format,link_mask);
    
    reg =  8; desc = "ROC pattern mode ??"; 
    PrintRocRegister(reg,desc,Format,link_mask);
    
    reg = 18; desc = " ??";
    PrintRocRegister(reg,desc,Format,link_mask);
    
    reg = 16; desc = " ??";
    PrintRocRegister(reg,desc,Format,link_mask);
    
    reg =  7; desc = "Fiber loss/lock counter";
    PrintRocRegister(reg,desc,Format,link_mask);

    reg =  6; desc = "Bad Markers counter";
    PrintRocRegister(reg,desc,Format,link_mask);
    
    reg =  4; desc = "Loopback coarse delay";
    PrintRocRegister(reg,desc,Format,link_mask);
    
    reg = 23; desc = "SIZE_FIFO_FULL [28]+STORE_POS[25:24]+STORE_CNT[19:0]";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 25; desc = "SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 11; desc = "Num EWM seen";
    PrintRocRegister2(reg,desc,Format,link_mask);
    
    reg = 64; desc = "Num windows seen";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 27; desc = "Num HB seen";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 29; desc = "Num null HB seen";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 31; desc = "Num HB on hold";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 33; desc = "Num PREFETCH seen";
    PrintRocRegister2(reg,desc,Format,link_mask);

    cout << Form("\n");

    reg =  9; desc = "Num DATA REQ seen";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 35; desc = "Num DATA REQ written to DDR";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 13; desc = "Num skipped DATA REQ";
    PrintRocRegister(reg,desc,Format,link_mask);

    reg = 37; desc = "Num DATA REQ read from DDR";
    PrintRocRegister2(reg,desc,Format,link_mask);
      
    reg = 39; desc = "Num DATA REQ sent to DTC";
    PrintRocRegister2(reg,desc,Format,link_mask);
      
    reg = 41; desc = "Num DATA REQ with null data";
    PrintRocRegister2(reg,desc,Format,link_mask);
      
    cout << Form("\n");
      
    reg = 43; desc = "Last spill tag";
    PrintRocRegister2(reg,desc,Format,link_mask);
      
    reg = 45; desc = "Last HB tag";
    PrintRocRegister2(reg,desc,Format,link_mask);
      
    reg = 48; desc = "Last PREFETCH tag";
    PrintRocRegister2(reg,desc,Format,link_mask);
      
    reg = 51; desc = "Last fetched tag";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 54; desc = "Last DATA REQ tag";
    PrintRocRegister2(reg,desc,Format,link_mask);

    reg = 57; desc = "OFFSET tag";
    PrintRocRegister2(reg,desc,Format,link_mask);
      
    cout << std::endl;
      
    reg = 72; desc = "Num HB tag inconsistencies";
    PrintRocRegister(reg,desc,Format,link_mask);
      
    reg = 74; desc = "Num HB tag lost";
    PrintRocRegister(reg,desc,Format,link_mask);
      
    reg = 73; desc = "Num DATA REQ tag inconsistencies";
    PrintRocRegister(reg,desc,Format,link_mask);
      
    reg = 75; desc = "Num DATA REQ tag lost";
    PrintRocRegister(reg,desc,Format,link_mask);

    cout << "------------------------------------------------------------------------\n";
  }

//-----------------------------------------------------------------------------
  void DtcInterface::PrintRegister(uint16_t Register, const char* Title) {
    std::cout << Form("(0x%04x) : 0x%08x : %s\n",Register,ReadRegister(Register),Title);
  }

//-----------------------------------------------------------------------------
  void DtcInterface::PrintStatus() {
    cout << Form("-----------------------------------------------------------------\n");
    cout << Form(" PCIE address: %i link mask: 0x%04x SampleEdgeMode: %i ReadoutMode: %i\n",
                 fPcieAddr,fLinkMask,fSampleEdgeMode,fReadoutMode);
    PrintRegister(0x9000,"DTC firmware link speed and design version ");
    PrintRegister(0x9004,"DTC version                                ");
    PrintRegister(0x9008,"Design status                              ");
    PrintRegister(0x900c,"Vivado version                             ");
    PrintRegister(0x9100,"DTC control register                       ");
    PrintRegister(0x9104,"DMA transfer length                        ");
    PrintRegister(0x9108,"SERDES loopback enable                     ");
    PrintRegister(0x9110,"ROC Emulation enable                       ");
    PrintRegister(0x9114,"Link Enable                                ");
    PrintRegister(0x9128,"SERDES PLL Locked                          ");
    PrintRegister(0x9140,"SERDES RX CDR lock (locked fibers)         ");
    PrintRegister(0x9144,"DMA Timeout Preset                         ");
    PrintRegister(0x9148,"ROC reply timeout                          ");
    PrintRegister(0x914c,"ROC reply timeout error                    ");
    PrintRegister(0x9158,"Event Builder Configuration                ");
    PrintRegister(0x91a8,"CFO Emulation Heartbeat Interval           ");
    PrintRegister(0x91ac,"CFO Emulation Number of HB Packets         ");
    PrintRegister(0x91bc,"CFO Emulation Number of Null HB Packets    ");
    PrintRegister(0x91f4,"CFO Emulation 40 MHz Clock Marker Interval ");
    PrintRegister(0x91f8,"CFO Marker Enables                         ");

    PrintRegister(0x9308,"Jitter Attenuator CSR                      ");

    PrintRegister(0x9630,"TX Data Request Packet Count Link 0        ");
    PrintRegister(0x9631,"TX Data Request Packet Count Link 1        ");

    PrintRegister(0x9650,"TX Heartbeat    Packet Count Link 0        ");
    PrintRegister(0x9651,"TX Heartbeat    Packet Count Link 1        ");

    PrintRegister(0x9670,"RX Data Header  Packet Count Link 0        ");
    PrintRegister(0x9671,"RX Data Header  Packet Count Link 1        ");

    PrintRegister(0x9690,"RX Data         Packet Count Link 0        ");
    PrintRegister(0x9691,"RX Data         Packet Count Link 1        ");

    PrintRegister(0xa400,"TX Event Window Marker Count Link 0        ");
    PrintRegister(0xa404,"TX Event Window Marker Count Link 0        ");

    PrintRegister(0xa420,"RX Data Header Timeout Count Link 0        ");
    PrintRegister(0xa424,"RX Data Header Timeout Count Link 0        ");

  }
};
