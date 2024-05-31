//

//    # TX Firefly
//    my_cntl -d$ii write 0x93a0 0x00000100 >/dev/null 2>&1
//    my_cntl -d$ii write 0x9288 0x50160000 >/dev/null 2>&1
//    my_cntl -d$ii write 0x928c 0x00000002 >/dev/null 2>&1
//    my_cntl -d$ii write 0x93a0 0x00000000 >/dev/null 2>&1
//    txtempval=`my_cntl -d$ii read 0x9288|grep 0x`
//    tempvalue=`echo "print(int($txtempval & 0xFF))"|python -`
//
//    if [[ $VERBOSE -ne 0 ]]; then
//      echo "TX Firefly temperature: $tempvalue"
//    fi
//    if [[ $tempvalue -gt $FIREFLY_TEMP_THRESHOLD ]] && [[ $tempvalue -le $FIREFLY_MAX_TEMP ]]; then
//      errstring="${errstring+$errstring$'\n'}TX Firefly Overtemp $HOSTNAME:/dev/mu2e$ii: $tempvalue!"
//    fi
//
//    send_epics $ii txtemp $tempvalue
//    
//    # TX/RX Firefly
//    my_cntl -d$ii write 0x93a0 0x00000400 >/dev/null 2>&1
//    my_cntl -d$ii write 0x92a8 0x50160000 >/dev/null 2>&1
//    my_cntl -d$ii write 0x92ac 0x00000002 >/dev/null 2>&1
//    my_cntl -d$ii write 0x93a0 0x00000000 >/dev/null 2>&1
//    txrxtempval=`my_cntl -d$ii read 0x92a8|grep 0x`
//    tempvalue=`echo "print(int($txrxtempval & 0xFF))"|python -`
//
//

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"

#include "dtc_init.C"

using namespace DTCLib;

void dtc_print_firefly_temp(int PcieAddr) {
  int tmo_ms(500);

  DTC dtc(DTC_SimMode_Disabled,PcieAddr);
//-----------------------------------------------------------------------------
// read RX firefly temp
//------------------------------------------------------------------------------
  dtc.GetDevice()->write_register(0x93a0,tmo_ms,0x00000100);
  dtc.GetDevice()->write_register(0x9288,tmo_ms,0x50160000);
  dtc.GetDevice()->write_register(0x928c,tmo_ms,0x00000002);
  dtc.GetDevice()->write_register(0x93a0,tmo_ms,0x00000000);

  uint data, rx_temp, txrx_temp;

  usleep(1000);

  dtc.GetDevice()->read_register(0x9288,tmo_ms,&data);
  rx_temp = data & 0xff;
//-----------------------------------------------------------------------------
// read TX/RX firefly temp
//------------------------------------------------------------------------------
  dtc.GetDevice()->write_register(0x93a0,tmo_ms,0x00000400);
  dtc.GetDevice()->write_register(0x92a8,tmo_ms,0x50160000);
  dtc.GetDevice()->write_register(0x92ac,tmo_ms,0x00000002);
  dtc.GetDevice()->write_register(0x93a0,tmo_ms,0x00000000);
  usleep(1000);

  dtc.GetDevice()->read_register(0x92a8,tmo_ms,&data);
  txrx_temp = data & 0xff;

  printf("rx_temp: %3i  txrx_temp: %3i\n",rx_temp, txrx_temp);

}
