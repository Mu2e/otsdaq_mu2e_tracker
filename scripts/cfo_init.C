#ifndef __cfo_init_C__
#define __cfo_init_C__

#define __CLING__ 1

using namespace CFOLib;

#include "CfoInterface.cc"   // TODO: don't forget to switch to .hh
//-----------------------------------------------------------------------------
// PcieAddress - CFO card index on PCIE bus
// CFO is initialized in DTC_SimMode_NoCFO mode
//-----------------------------------------------------------------------------
CFO* cfo_init(int PcieAddress = -1) {
  trkdaq::CfoInterface* cfo_i = trkdaq::CfoInterface::Instance(PcieAddress);
  if (cfo_i) return cfo_i->Cfo();
  else         return nullptr;
}

#endif
