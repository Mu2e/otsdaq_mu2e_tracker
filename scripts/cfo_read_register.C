#ifndef __cfo_read_register_C__
#define __cfo_read_register_C__

#define __CLING__ 1

#include "iostream"
// #include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

#include "cfo_init.C"
//-----------------------------------------------------------------------------
uint32_t cfo_read_register(uint16_t Register, int PcieAddress = -1) {

  CFO* cfo = cfo_init(PcieAddress);
  if (cfo == nullptr) return 0xffffffff;
  
  mu2edev* dev = cfo->GetDevice();

  uint32_t data;

  int timeout(150);
  dev->read_register(Register,timeout,&data);

  return data;
}


#endif
