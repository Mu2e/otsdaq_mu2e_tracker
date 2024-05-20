#ifndef __cfo_print_register_C__
#define __cfo_print_register_C__

#define __CLING__ 1

#include "iostream"
// #include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/cfoInterfaceLib/CFO.h"

#include "cfo_read_register.C"

//-----------------------------------------------------------------------------
void cfo_print_register(uint16_t Register, const char* Title = "", int PcieAddress = -1) {
  std::cout << Form("%s (0x%04x) : 0x%08x\n",Title,Register,cfo_read_register(Register,PcieAddress));
}


#endif
