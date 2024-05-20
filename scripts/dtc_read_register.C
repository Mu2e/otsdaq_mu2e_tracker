#define __CLING__ 1

#ifndef __dtc_read_register_C__
#define __dtc_read_register_C__

#include "dtc_init.C"

//-----------------------------------------------------------------------------
uint32_t dtc_read_register(uint16_t Register, int PcieAddress = -1) {

  DTC* dtc = dtc_init(PcieAddress);
  
  mu2edev* dev = dtc->GetDevice();

  uint32_t data;

  int timeout(150);
  dev->read_register(Register,timeout,&data);

  return data;
}

#endif
