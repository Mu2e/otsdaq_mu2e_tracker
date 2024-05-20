#ifndef __cfo_init_C__
#define __cfo_init_C__

#define __CLING__ 1

#include "iostream"
// #include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

using namespace CFOLib;
using namespace DTCLib;

CFO* gCfo[2] = {nullptr, nullptr};
//-----------------------------------------------------------------------------
// PcieAddress - CFO card index on PCIE bus
// CFO is initialized in DTC_SimMode_NoCFO mode
//-----------------------------------------------------------------------------
CFO* cfo_init(int PcieAddress = -1) {
  int addr = PcieAddress;
  if (addr < 0) {
    if (gSystem->Getenv("CFOLIB_CFO") != nullptr) addr = atoi(gSystem->Getenv("CFOLIB_CFO"));
    else {
      printf (" ERROR: PcieAddress < 0 and $CFOLIB_CFO is not defined. BAIL out\n");
      return nullptr;
    }
  }

  if (gCfo[addr] == nullptr) gCfo[addr] = new CFO(DTC_SimMode_NoCFO,addr,"",true,"UID");

  return gCfo[addr];
}


#endif
