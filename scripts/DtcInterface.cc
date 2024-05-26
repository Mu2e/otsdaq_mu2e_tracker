//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_interface_cc__
#define __trkdaq_dtc_interface_cc__

#define __CLING__ 1

#include "iostream"
#include "DtcInterface.hh"

using namespace DTCLib;

namespace trkdaq {

  DtcInterface* DtcInterface::fgInstance[2] = {nullptr, nullptr};

//-----------------------------------------------------------------------------
  DtcInterface::DtcInterface(int PcieAddr, uint LinkMask) {
    std::string expected_version("");              // dont check
    bool        skip_init       (false);
    std::string sim_file        ("mu2esim.bin");
    std::string uid             ("");
      
    fPcieAddr = PcieAddr;
    fDtc      = new DTC(DTC_SimMode_NoCFO,PcieAddr,LinkMask,expected_version,
                        skip_init,sim_file,uid);
  }

//-----------------------------------------------------------------------------
  DtcInterface::~DtcInterface() { }

//-----------------------------------------------------------------------------
  DtcInterface* DtcInterface::Instance(int PcieAddr, uint LinkMask) {
    int pcie_addr = PcieAddr;
    if (pcie_addr < 0) {
//-----------------------------------------------------------------------------
// PCIE address is not specified, check environment
//-----------------------------------------------------------------------------
      if (gSystem->Getenv("DTCLIB_DTC") != nullptr) pcie_addr = atoi(gSystem->Getenv("DTCLIB_DTC"));
      else {
        printf (" ERROR: PcieAddr < 0 and $DTCLIB_DTC is not defined. BAIL out\n");
        return nullptr;
      }
    }

    if (fgInstance[pcie_addr] == nullptr) fgInstance[pcie_addr] = new DtcInterface(pcie_addr,LinkMask);
    
    if (fgInstance[pcie_addr]->PcieAddr() != pcie_addr) {
      printf (" ERROR: DtcInterface::Instance has been already initialized with PcieAddress = %i. BAIL out\n", 
              fgInstance[pcie_addr]->PcieAddr());
      return nullptr;
    }
    else return fgInstance[pcie_addr];
  }

//-----------------------------------------------------------------------------
  void DtcInterface::PrintRocStatus(int Link) {
    cout << Form("-------------------- ROC %i registers:\n",Link);

    DTC* dtc = fDtc;

    DTC_Link_ID link = DTC_Link_ID(Link);
    uint32_t dat;

    uint reg = 0;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : ALWAYS\n",reg, dat);

    reg = 8;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x :\n",reg, dat);

    reg = 18;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x :\n",reg, dat);

    reg = 16;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x :\n",reg, dat);

    reg =  7;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Fiber loss/lock counter\n",reg, dat);

    reg =  6;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Bad Markers counter\n",reg, dat);

    reg =  4;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Loopback coarse delay\n",reg, dat);

    uint iw, iw1, iw2;

    reg = 23;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : SIZE_FIFO_FULL [28]+STORE_POS[25:24]+STORE_CNT[19:0]\n",
                 reg+1,reg,iw);

    reg = 25;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]\n",
                 reg+1,reg,iw);

    reg = 11;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num EWM seen\n", reg+1,reg,iw);

    reg = 64;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num windows seen\n", reg+1,reg,iw);

    reg = 27;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num HB seen\n", reg+1,reg,iw);

    reg = 29;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num null HB seen\n", reg+1,reg,iw);

    reg = 31;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num HB on hold\n", reg+1,reg,iw);

    reg = 33;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num PREFETCH seen\n\n", reg+1,reg,iw);

    reg = 35;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num DATA REQ seen\n", reg+1,reg,iw);

    reg = 13;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Num skipped DATA REQ\n",reg,dat);

    reg = 37;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num DATA REQ read from DDR\n", reg+1,reg,iw);

    reg = 39;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num DATA REQ sent to DTC\n", reg+1,reg,iw);


    reg = 41;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Num DATA REQ with null data\n", reg+1,reg,iw);

    cout << Form("\n");
  
    reg = 43;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Last spill tag\n", reg+1,reg,iw);

    uint16_t iw3;

    reg = 45;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw3 = dtc->ReadROCRegister(link,reg+2,100);
  
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Last HB tag\n", reg+1,reg,iw);

    reg = 48;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw3 = dtc->ReadROCRegister(link,reg+2,100);
  
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Last PREFETCH tag\n", reg+1,reg,iw);
  
    reg = 51;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw3 = dtc->ReadROCRegister(link,reg+2,100);
  
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Last fetched tag\n", reg+1,reg,iw);

    reg = 54;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw3 = dtc->ReadROCRegister(link,reg+2,100);
  
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : Last DATA REQ tag\n", reg+1,reg,iw);

    reg = 57;
    iw1 = dtc->ReadROCRegister(link,reg  ,100);
    iw2 = dtc->ReadROCRegister(link,reg+1,100);
    iw3 = dtc->ReadROCRegister(link,reg+2,100);
  
    iw  = (iw2 << 16) | iw1;
    cout << Form("reg(%i)<<16+reg(%i) : 0x%08x : OFFSET tag\n", reg+1,reg,iw);

    cout << std::endl;

    reg = 72;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Num HB tag inconsistencies\n",reg, dat);

    reg = 74;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Num HB tag lost\n",reg, dat);

    reg = 73;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Num DREQ tag inconsistencies\n",reg, dat);

    reg = 75;
    dat = dtc->ReadROCRegister(link,reg,100);
    cout << Form("reg(%2i)             :     0x%04x : Num DREQ tag lost\n",reg, dat);

    cout << "------------------------------------------------------------------------\n";
  }



};

#endif
