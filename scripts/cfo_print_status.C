#ifndef __cfo_print_status_C__
#define __cfo_print_status_C__

#define __CLING__ 1

#include "iostream"
#include "cfo_init.C"
#include "cfo_print_register.C"

//-----------------------------------------------------------------------------
void cfo_print_status(int PcieAddress = -1) {
  
  CFO* cfo = cfo_init(PcieAddress);
  if (cfo == nullptr) return;

  printf("-----------------------------------------------------------------\n");
  cfo_print_register(0x9004,"CFO version                                ",PcieAddress);
  cfo_print_register(0x9030,"Kernel driver version                      ",PcieAddress);
  cfo_print_register(0x9100,"CFO control register                       ",PcieAddress);
  cfo_print_register(0x9104,"DMA Transfer Length                        ",PcieAddress);
  cfo_print_register(0x9108,"SERDES loopback enable                     ",PcieAddress);
  cfo_print_register(0x9114,"CFO link enable                            ",PcieAddress);
  cfo_print_register(0x9128,"CFO PLL locked                             ",PcieAddress);
  cfo_print_register(0x9140,"SERDES RX CDR lock                         ",PcieAddress);
  cfo_print_register(0x9144,"Beam On Timer Preset                       ",PcieAddress);
  cfo_print_register(0x9148,"Enable Beam On Mode                        ",PcieAddress);
  cfo_print_register(0x914c,"Enable Beam Off Mode                       ",PcieAddress);
  cfo_print_register(0x918c,"Number of DTCs                             ",PcieAddress);
                                                                                     
  cfo_print_register(0x9200,"Receive  Byte   Count Link 0               ",PcieAddress);
  cfo_print_register(0x9220,"Receive  Packet Count Link 0               ",PcieAddress);
  cfo_print_register(0x9240,"Transmit Byte   Count Link 0               ",PcieAddress);
  cfo_print_register(0x9260,"Transmit Packet Count Link 0               ",PcieAddress);
}


#endif
