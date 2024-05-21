#ifndef __dtc_globals_hh__
#define __dtc_globals_hh__

#include "iostream"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

using namespace CFOLib;
using namespace DTCLib;

DTC* gDTC[2] = {nullptr, nullptr};
CFO* gCFO[2] = {nullptr, nullptr};

namespace Dtc {
  DTC_SimMode DefaultSimMode = DTC_SimMode_NoCFO;
};


#endif
