//

//-----------------------------------------------------------------------------
int test_read_ilp(int Link, int PrintLevel=0, int PcieAddr = -1) {
  std::vector<uint16_t> dat;
  auto dtc_i = DtcInterface::Instance(PcieAddr);

  dtc_i->ControlRoc_ReadIlp(dat,Link,PrintLevel);

  int   ilp_id   = dat[0];
  float temp     = float(dat[1])/100.;
  float pressure = float(int(dat[3]) << 16 | int(dat[2]))/524288.;

  printf(" ilp_id  :    %5i\n temp    : %8.3f\n pressure: %8.3f\n",ilp_id,temp,pressure);
  return 0;
}
