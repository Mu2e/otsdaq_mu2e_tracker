////
void read_roc_0(int N) {
  int timeout_ms(100);
  
  DTC* dtc = dtc_init();
  for (int i=0; i<N; i++) {
    uint16_t val = dtc->ReadROCRegister(DTC_Link_ID(0),0,timeout_ms);
    printf("----------------------------- i, val: %i 0x%04x\n",i,val);
  }

}
