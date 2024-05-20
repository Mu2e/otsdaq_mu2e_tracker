#define __CLING__ 1

#ifndef __dtc_print_status_C__
#define __dtc_print_status_C__

#include "dtc_init.C"
#include "dtc_print_register.C"

//-----------------------------------------------------------------------------
void dtc_print_status(int PcieAddress = -1) {
  
  DTC* dtc = dtc_init(PcieAddress);

  printf("-----------------------------------------------------------------\n");
  dtc_print_register(0x9000,"DTC firmware link speed and design version ",PcieAddress);
  dtc_print_register(0x9004,"DTC version                                ",PcieAddress);
  dtc_print_register(0x9008,"Design status                              ",PcieAddress);
  dtc_print_register(0x900c,"Vivado version                             ",PcieAddress);
  dtc_print_register(0x9100,"DTC control register                       ",PcieAddress);
  dtc_print_register(0x9104,"DMA transfer length                        ",PcieAddress);
  dtc_print_register(0x9108,"SERDES loopback enable                     ",PcieAddress);
  dtc_print_register(0x9110,"ROC Emulation enable                       ",PcieAddress);
  dtc_print_register(0x9114,"Link Enable                                ",PcieAddress);
  dtc_print_register(0x9128,"SERDES PLL Locked                          ",PcieAddress);
  dtc_print_register(0x9140,"SERDES RX CDR lock (locked fibers)         ",PcieAddress);
  dtc_print_register(0x9144,"DMA Timeout Preset                         ",PcieAddress);
  dtc_print_register(0x9148,"ROC reply timeout                          ",PcieAddress);
  dtc_print_register(0x914c,"ROC reply timeout error                    ",PcieAddress);
  dtc_print_register(0x9158,"Event Builder Configuration                ",PcieAddress);
  dtc_print_register(0x91a8,"CFO Emulation Heartbeat Interval           ",PcieAddress);
  dtc_print_register(0x91ac,"CFO Emulation Number of HB Packets         ",PcieAddress);
  dtc_print_register(0x91bc,"CFO Emulation Number of Null HB Packets    ",PcieAddress);
  dtc_print_register(0x91f4,"CFO Emulation 40 MHz Clock Marker Interval ",PcieAddress);
  dtc_print_register(0x91f8,"CFO Marker Enables                         ",PcieAddress);

  dtc_print_register(0x9200,"Receive  Byte   Count Link 0               ",PcieAddress);
  dtc_print_register(0x9220,"Receive  Packet Count Link 0               ",PcieAddress);
  dtc_print_register(0x9240,"Transmit Byte   Count Link 0               ",PcieAddress);
  dtc_print_register(0x9260,"Transmit Packet Count Link 0               ",PcieAddress);

  dtc_print_register(0x9218,"Receive  Byte   Count CFO                  ",PcieAddress);
  dtc_print_register(0x9238,"Receive  Packet Count CFO                  ",PcieAddress);
  dtc_print_register(0x9258,"Transmit Byte   Count CFO                  ",PcieAddress);
  dtc_print_register(0x9278,"Transmit Packet Count CFO                  ",PcieAddress);
}

#endif
