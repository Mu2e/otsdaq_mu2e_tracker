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

#include "otsdaq-mu2e-tracker/ui/DtcInterface.hh"

// #include "dtc_init.C"


using namespace DTCLib;
using namespace trkdaq;

void dtc_print_firefly_temp(int PcieAddr) {
  int tmo_ms(50);

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  DTC* dtc = dtc_i->Dtc();
//-----------------------------------------------------------------------------
// read RX firefly temp
//------------------------------------------------------------------------------
  dtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000100);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  dtc->GetDevice()->write_register(0x9288,tmo_ms,0x50160000);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  dtc->GetDevice()->write_register(0x928c,tmo_ms,0x00000002);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  dtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000000);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));

  uint data, rx_temp, txrx_temp;

  dtc->GetDevice()->read_register(0x9288,tmo_ms,&data);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  rx_temp = data & 0xff;
//-----------------------------------------------------------------------------
// read TX/RX firefly temp
//------------------------------------------------------------------------------
  dtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000400);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  dtc->GetDevice()->write_register(0x92a8,tmo_ms,0x50160000);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  dtc->GetDevice()->write_register(0x92ac,tmo_ms,0x00000002);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  dtc->GetDevice()->write_register(0x93a0,tmo_ms,0x00000000);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));

  dtc->GetDevice()->read_register(0x92a8,tmo_ms,&data);
  std::this_thread::sleep_for(std::chrono::milliseconds(tmo_ms));
  txrx_temp = data & 0xff;

  printf("rx_temp: %3i  txrx_temp: %3i\n",rx_temp, txrx_temp);

}
