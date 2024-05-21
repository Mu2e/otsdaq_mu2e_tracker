#ifndef __dtc_init_C__
#define __dtc_init_C__

#define __CLING__ 1


#include "dtc_globals.hh"

//-----------------------------------------------------------------------------
// PcieAddress - DTC card index on PCIE bus
// by default, leave Link=0
//-----------------------------------------------------------------------------
DTC* dtc_init(int PcieAddress = -1, DTC_SimMode Mode = Dtc::DefaultSimMode, uint LinkMask = 0x1) {
  int addr = PcieAddress;
  if (addr < 0) {
    if (gSystem->Getenv("DTCLIB_DTC") != nullptr) addr = atoi(gSystem->Getenv("DTCLIB_DTC"));
    else {
      printf(">>> ERROR: PcieAddress < 0 and $DTCLIB_DTC is not defined. BAIL OUT\n");
      return nullptr;
    }
  }

  if (gDTC[addr] == nullptr) gDTC[addr] = new DTC(Mode,addr,LinkMask,"",false,"","");

  return gDTC[addr];
}

#endif
