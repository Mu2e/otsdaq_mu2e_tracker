//

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

using namespace CFOLib;
using namespace DTCLib;

// Dtc: PCIE index
//-----------------------------------------------------------------------------
int configure_cfo_ja(int Cfo  = -1) {
  int link=0;

  CFO cfo(DTCLib::DTC_SimMode_Disabled,Cfo,"",true);


  cfo.ConfigureJitterAttenuator();

  return 0;
}

//-----------------------------------------------------------------------------
int configure_dtc_ja(int Dtc  = -1) {
  int link=0;
  DTC dtc(DTC_SimMode_NoCFO,Dtc,0x1<<4*link,"");

  dtc.ConfigureJitterAttenuator();

  return 0;
}
