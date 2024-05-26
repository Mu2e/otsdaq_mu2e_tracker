//

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

using namespace CFOLib;
using namespace DTCLib;

// Dtc: PCIE index
//-----------------------------------------------------------------------------
int cfo_configure_ja(int PcieAddress  = -1) {
  int link=0;

  CFO cfo(DTCLib::DTC_SimMode_NoCFO,PcieAddress,"",true);


  cfo.ConfigureJitterAttenuator();

  return 0;
}

//-----------------------------------------------------------------------------
int dtc_configure_ja(int PcieAddress  = -1) {
  int link=0;

  DTC dtc(DTC_SimMode_NoCFO,PcieAddress,0x1<<4*link,"");

  dtc.ConfigureJitterAttenuator();

  return 0;
}
