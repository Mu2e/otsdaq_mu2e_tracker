#define __CLING__ 1

#ifndef __dtc_print_register_C__
#define __dtc_print_register_C__

#include "dtc_init.C"
#include "dtc_read_register.C"

//-----------------------------------------------------------------------------
void dtc_print_register(uint16_t Register, const char* Title = "", int PcieAddress = -1) {
  std::cout << Form("%s (0x%04x) : 0x%08x\n",Title,Register,dtc_read_register(Register,PcieAddress));
}

#endif
