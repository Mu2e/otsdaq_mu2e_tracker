#ifndef __dtc_init_C__
#define __dtc_init_C__

#define __CLING__ 1

#include "DtcInterface.cc"
//-----------------------------------------------------------------------------
// PcieAddress - DTC card index on PCIE bus
// by default, leave Link=0
//-----------------------------------------------------------------------------
DTC* dtc_init(int PcieAddress = -1, uint LinkMask = 0x11) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(PcieAddress);
  if (dtc_i) return dtc_i->Dtc();
  else         return nullptr;
}

#endif
