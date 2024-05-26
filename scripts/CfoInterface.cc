//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_cfo_interface_cc__
#define __trkdaq_cfo_interface_cc__

#define __CLING__ 1

#include "iostream"
#include "CfoInterface.hh"

using namespace CFOLib;
using namespace DTCLib;

namespace trkdaq {

  CfoInterface* CfoInterface::fgInstance = nullptr;

//-----------------------------------------------------------------------------
  CfoInterface::CfoInterface(int PcieAddr, DTC_SimMode SimMode) {
    std::string expected_version("");              // dont check
    bool        skip_init       (false);
    std::string sim_file        ("mu2esim.bin");
    std::string uid             ("");

    fPcieAddr = PcieAddr;
    fCfo      = new CFO(SimMode,PcieAddr,expected_version,skip_init,uid);
  }

//-----------------------------------------------------------------------------
  CfoInterface::~CfoInterface()  {
    if (fgInstance) {
      delete fgInstance->fCfo;
      fgInstance = nullptr;
    }
  }

//-----------------------------------------------------------------------------
  CfoInterface* CfoInterface::Instance(int PcieAddr) {
    int pcie_addr = PcieAddr;
    if (pcie_addr < 0) {
//-----------------------------------------------------------------------------
// PCIE address is not specified, check environment
//-----------------------------------------------------------------------------
      if (gSystem->Getenv("CFOLIB_CFO") != nullptr) pcie_addr = atoi(gSystem->Getenv("CFOLIB_CFO"));
      else {
        printf (" ERROR: PcieAddr < 0 and $CFOLIB_CFO is not defined. BAIL out\n");
        return nullptr;
      }
    }

    if (fgInstance == nullptr) fgInstance = new CfoInterface(pcie_addr,DTC_SimMode_NoCFO);
      
    if (fgInstance->PcieAddr() != pcie_addr) {
      printf (" ERROR: CfoInterface::Instance has been already initialized with PcieAddress = %i. BAIL out\n", 
              fgInstance->PcieAddr());
      return nullptr;
    }
    else return fgInstance;
  }
  
};

#endif
