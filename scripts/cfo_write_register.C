#ifndef __cfo_write_register_C__
#define __cfo_write_register_C__

#define __CLING__ 1

#include "iostream"
// #include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

#include "cfo_init.C"

//-----------------------------------------------------------------------------
void cfo_write_register(uint16_t Register, uint32_t Data, int PcieAddress = -1) {

  CFO*     cfo = cfo_init(PcieAddress);
  if (cfo == nullptr) return;

  int timeout(150);

  mu2edev* dev = cfo->GetDevice();
  dev->write_register(Register,timeout,Data);
}


#endif
