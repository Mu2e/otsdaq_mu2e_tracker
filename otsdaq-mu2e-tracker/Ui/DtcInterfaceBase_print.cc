//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __mu2edaq_dtc_interface_cc__
#define __mu2edaq_dtc_interface_cc__

#include "iostream"
#include "vector"

#include "DtcInterfaceBase.hh"
#include "TString.h"                 // includes ROOT's Form

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterfaceBase"

using namespace DTCLib;
using namespace std;

namespace mu2edaq {


namespace mu2edaq {

//-----------------------------------------------------------------------------
// 'nw' : number of 16-bit words to print.
// if Stream == nullptr , use TLOG, otherwise - *Stream
//-----------------------------------------------------------------------------
  void DtcInterface::PrintBuffer(const void* ptr, int nw, int Offset, std::ostream& Stream) {

    ushort*      p16 = (ushort*) ptr;

    int          n(0);
    std::string  line;

    // if (Stream == nullptr) { TLOG(TLVL_DEBUG+1) << Form("-------- nw = %i\n",nw); }
    // else                   { (*Stream)        << Form("-------- nw = %i\n",nw); }
   
    for (int i=0; i<nw; i++) {
      if (n == 0) line = Form("0x%08x:",i*2+Offset);
      ushort  word = p16[i];
      line += Form(" 0x%04x",word);
      
      n   += 1;
      if (n == 8) {
        if (Stream.rdbuf() == nullptr) TLOG(TLVL_INFO) << line << std::endl;
        else {
          Stream             << line << std::endl;
          TLOG(TLVL_DEBUG+1) << line << std::endl;
        }
        n = 0;
      }
    }
    
    if (n != 0) {
      if (Stream.rdbuf() == nullptr) TLOG(TLVL_INFO) << line << std::endl;
      else {
        Stream             << line << std::endl;
        TLOG(TLVL_DEBUG+1) << line << std::endl;
      }
    }
  }

//-----------------------------------------------------------------------------
  void DtcInterface::PrintFireflyTemp(std::ostream& Stream) {
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

    Stream << "rx_temp: " << rx_temp << " txrx_temp: " << txrx_temp << endl;
    
    TLOG(TLVL_DEBUG) << "END" << std::endl;
  }

//-----------------------------------------------------------------------------
  void DtcInterface::PrintRegister(uint16_t Register, const char* Title, std::ostream& Stream) {
    Stream << Form("(0x%04x) : 0x%08x : %s\n",Register,ReadRegister(Register),Title);
  }

//-----------------------------------------------------------------------------
// link 6: CFO
//-----------------------------------------------------------------------------
  void DtcInterface::PrintDtcLinkRegisters(uint FirstReg, const char* Desc, int NoCfo, std::ostream& Stream) {

    std::string text = Form("(0x%04x) : ",FirstReg);
    
    for (int i=0; i<7; i++) {
      int used = (fLinkMask >> 4*i) & 0x1;
      if ((i < 6) and (used == 0))                          continue;
      
      if ((i == 6) and NoCfo) {
        text      += Form("%11s","");
      }
      else {
        uint32_t reg = FirstReg+4*i;
        uint32_t iw  = ReadRegister(reg);
        text        += Form(" 0x%08x",iw);
      }
    }

    text += Form(" %s",Desc);
    Stream << Form("%-s\n",text.data());
  }
  
//-----------------------------------------------------------------------------
  int DtcInterface::PrintStatus(std::ostream& Stream) {
    int rc(0);
    TLOG(TLVL_DEBUG) << "-- START";
    
    Stream << Form("-----------------------------------------------------------------\n");
    Stream << Form(" PCIE address: %i link mask: 0x%04x SampleEdgeMode: %i RocReadoutMode: %i\n",
                   fPcieAddr,fLinkMask,fSampleEdgeMode,fRocReadoutMode);
    PrintRegister(0x9000,"DTC firmware link speed and design version ",Stream);
    PrintRegister(0x9004,"DTC FW version                             ",Stream);
    PrintRegister(0x9008,"Design status                              ",Stream);
    PrintRegister(0x900c,"Vivado version                             ",Stream);
    PrintRegister(0x9100,"DTC control register                       ",Stream);
    PrintRegister(0x9104,"DMA transfer length                        ",Stream);
    PrintRegister(0x9108,"SERDES loopback enable                     ",Stream);
    PrintRegister(0x9110,"ROC Emulation enable                       ",Stream);
    PrintRegister(0x9114,"Link Enable                                ",Stream);
    PrintRegister(0x9128,"SERDES PLL Locked                          ",Stream);
    PrintRegister(0x9140,"SERDES RX CDR lock (locked fibers)         ",Stream);
    PrintRegister(0x9144,"DMA Timeout Preset                         ",Stream);
    PrintRegister(0x9148,"ROC reply timeout                          ",Stream);
    PrintRegister(0x914c,"ROC reply timeout error                    ",Stream);
    PrintRegister(0x9154,"DTC ID/EVB partition ID/MAC address        ",Stream);
    PrintRegister(0x9158,"Event Builder Configuration                ",Stream);
    PrintRegister(0x91a8,"CFO Emulation Heartbeat Interval           ",Stream);
    PrintRegister(0x91ac,"CFO Emulation Number of HB Packets         ",Stream);
    PrintRegister(0x91bc,"CFO Emulation Number of Null HB Packets    ",Stream);
    PrintRegister(0x91f4,"CFO Emulation 40 MHz Clock Marker Interval ",Stream);
    PrintRegister(0x91f8,"CFO Marker Enables                         ",Stream);
    PrintRegister(0x91f8,"CFO Marker Enables                         ",Stream);

    PrintRegister(0x9218,"bytes received from CFO                    ",Stream);
    PrintRegister(0x9238,"received CFO packets                       ",Stream);
    PrintRegister(0x9258,"bytes sent to CFO                          ",Stream);
    PrintRegister(0x9278,"packets sent to CFO                        ",Stream);

    PrintRegister(0x9308,"Jitter Attenuator CSR                      ",Stream);

    std::string text1("          ");
    std::string text2(" offset  :");

    for (int i=0; i<7; i++) {
      int used = (fLinkMask >> 4*i) & 0x1;
      if ((i<6) and (used == 0))                            continue;
      if (i < 6) text1 += Form("   link %i  ",i);
      else       text1 += Form("     CFO    ");  // CFO
      int offset = 4*i;
      text2 += Form("   (0x%02x)  ",offset);
    }
    Stream << std::endl;
    Stream << Form("%-s\n",text1.data());
    Stream << Form("%-s\n",text2.data());
    
    PrintDtcLinkRegisters(0x9320,"Retransmit request count    ",1,Stream);
    PrintDtcLinkRegisters(0x9340,"Missed CFO packet count     ",1,Stream);
   
    PrintDtcLinkRegisters(0x9380,"Link status and errors      ",0,Stream);
    PrintDtcLinkRegisters(0x93B0,"RX CDR unlock count         ",1,Stream);
    PrintDtcLinkRegisters(0x9630,"TX Data Request Packet Count",0,Stream);
    PrintDtcLinkRegisters(0x9650,"TX Heartbeat    Packet Count",0,Stream);
    PrintDtcLinkRegisters(0x9670,"RX Data Header  Packet Count",0,Stream);
    PrintDtcLinkRegisters(0x9690,"RX Data         Packet Count",0,Stream);
    PrintDtcLinkRegisters(0xa400,"TX Event Window Marker Count",0,Stream);
    PrintDtcLinkRegisters(0xa420,"RX Data Header Timeout Count",0,Stream);
                          
    TLOG(TLVL_DEBUG) << std::format("-- END: rc:{}",rc);
    return rc;
  }
};

//-----------------------------------------------------------------------------
// most of the time Link = -1 meaning 'all enabled links'
// otherwise it is the link to print
//-----------------------------------------------------------------------------
  int DtcInterface::PrintRocStatus(uint32_t Format, int Link, std::ostream& Stream) {
    TLOG(TLVL_DBG+1) << Form("Format=%i Link:%i \n",Format,Link);

    std::string desc;

    int lnk1(Link), lnk2(Link+1);
    if (Link == -1) {
      lnk1 = 0;
      lnk2 = 6;
    }

    //    uint reg;

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
    Stream << "-------------to be completed-------------------------------------------\n";

    return 0;
  }
};
#endif
