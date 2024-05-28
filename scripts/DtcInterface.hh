//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_interface_hh__
#define __trkdaq_dtc_interface_hh__

#define __CLING__ 1

#include "iostream"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"

namespace trkdaq {

  class DtcInterface { 
  public:
    static DtcInterface* fgInstance[2];

    DTCLib::DTC*         fDtc;
    int                  fPcieAddr;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  private:
    DtcInterface(int PcieAddr = -1, uint LinkMask = 0x1);
  public:
    virtual ~DtcInterface();

    static DtcInterface* Instance(int PcieAddr, uint LinkMask = 0x1);

    int PcieAddr() { return fPcieAddr; }

    DTCLib::DTC* Dtc() { return fDtc; }

    void PrintRocStatus(int Link);

    void RocPatternConfig(int LinkMask);

    void LaunchRunPlan   (int NEvents );

  };

};

#endif
