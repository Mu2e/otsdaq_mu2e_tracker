//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_hh__
#define __trkdaq_dtc_hh__

#define __CLING__ 1

#include "iostream"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

using namespace CFOLib;
using namespace DTCLib;

#include "DtcInterface.C"
#include "CfoInterface.C"



DTC* gDTC[2] = {nullptr, nullptr};
CFO* gCFO    = {nullptr, nullptr};

namespace trkdaq {

  DTC_SimMode DefaultSimMode = DTC_SimMode_NoCFO;

  class DtcInterface { 
    DTC* fDtc;

//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
    void PrintRocStatus(int Link);

    void Set(DTC* Dtc) { fDtc = Dtc; }

    static DtcInterface* Instance(int PcieAddr);
  };

};

#endif
