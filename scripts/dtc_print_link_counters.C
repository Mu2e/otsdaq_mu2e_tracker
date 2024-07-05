#define __CLING__ 1

#ifndef __dtc_print_link_counters_C__
#define __dtc_print_link_counters_C__

#include "dtc_print_register.C"
//-----------------------------------------------------------------------------
void dtc_print_link_counters(int Link, int PcieAddress = -1) {

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddress);

  int r01 = 0x9320+4*Link;
  cout << Form("-----------------------------------------------------------------\n");
  dtc_i->PrintRegister(r01  ,Form("Retransmit Request Count Link %i",Link));

  int r01 = 0x9340+4*Link;
  cout << Form("-----------------------------------------------------------------\n");
  dtc_i->PrintRegister(r01  ,Form("Missed CFO Packet Count Link %i",Link));

  int r011 = 0x93B0+4*Link;
  cout << Form("-----------------------------------------------------------------\n");
  dtc_i->PrintRegister(r011 ,Form("RX CDR Unlock Count Link %i",Link));

  int r02 = 0x9630+4*Link;
  dtc_i->PrintRegister(r02   ,Form("TX DR packat count Link %i",Link));
  dtc_i->PrintRegister(r02+20,Form("TX HB packet count link %i",Link));
  dtc_i->PrintRegister(r02+40,Form("TX DH packet count link %i",Link));
  dtc_i->PrintRegister(r02+60,Form("TX DP        count link %i",Link));

  int r03 = 0xA400+4*Link;
  dtc_i->PrintRegister(r03   ,Form("TX Event Window Marker count link %i",Link));
  dtc_i->PrintRegister(r03+20,Form("RX Event Window Marker count link %i",Link));
 
}

#endif
