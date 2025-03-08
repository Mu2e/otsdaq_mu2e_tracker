//

int power_cycle() {

  auto dtc_i = trkdaq::DtcInterface::Instance(-1);
  dtc_i->FindAlignments(1);
  dtc_i->FindAlignments(1);  // run two times to zeroes
  daq_scripts::EWLength=1000;
  dtc_control_roc_read(2);    // all channels, external pulser
  dtc_buffer_test_emulated_cfo(100, 0x10001);
  daq_scripts::EWLength=4000;
  dtc_control_roc_read(2,-1,0,0,0,0x2,0,0,2);
  dtc_buffer_test_emulated_cfo(100, 0x10001);
}
