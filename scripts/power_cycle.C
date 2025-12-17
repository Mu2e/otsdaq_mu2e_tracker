//

int power_cycle() {

  auto dtc_i = trkdaq::DtcInterface::Instance(-1);
  dtc_i->FindAlignments(1);
  dtc_i->FindAlignments(1);  // run two times to zeroes
  daq_scripts::EWLength=1000;
  dtc_control_roc_read();                // all channels, internal pulser
  dtc_control_roc_digi_rw(0x85,1,0,1);    // ~60 kHz, last parameter=0: high rate 1:low rate (calibration)
  dtc_buffer_test_emulated_cfo(100, 0x10001);
  dtc_control_roc_read(-1,4,0,0,1,0x2,0,0,1);   // int pulser, checkerboard, one channel, 1 ADC packet/hit
  daq_scripts::EWLength=4000;                     // for 50 kHz input, 25 us is OK
  dtc_control_roc_read(-1,0,0,0,0,0x2,0,0,2);   // ext pulser, two ADC packets/hit
  dtc_buffer_test_emulated_cfo(100, 0x10001);
}

int init_dtc() {

  auto dtc_i = trkdaq::DtcInterface::Instance(-1);
  dtc_i->FindAlignments(1);
  dtc_i->FindAlignments(1);                     // run several times to get all zeroes

  dtc_control_roc_digi_rw(0x85,1,0,1);          // last parameter=0: high rate 1:low rate (calibration)

  daq_scripts::EWLength=1000;
  dtc_control_roc_read();                       // all channels, internal pulser
  dtc_buffer_test_emulated_cfo(100, 0x10001);

  daq_scripts::EWLength=400;                    // for 50 kHz input, 25 us is OK
  dtc_control_roc_read(-1,4,0,0,1,0x2,0,0,1);   // int pulser, checkerboard, one channel, 1 ADC packet/hit
  dtc_buffer_test_emulated_cfo(100, 0x10001);

  dtc_control_roc_read(-1,0,0,0,0,0x2,0,0,2);   // num_lookback=0, 2 ADC packets/hit
  dtc_buffer_test_emulated_cfo(100, 0x10001);
}
