//



//-----------------------------------------------------------------------------
int test_get_key(int Link, int PcieAddr = -1) {
  std::vector<uint16_t> dat;
  auto dtc_i = DtcInterface::Instance(PcieAddr);

  int print_level = 0;
  dtc_i->ControlRoc_GetKey(dat,Link,print_level);
  float temp     = float(dat[0])/4096.*3300./10;
  float v2p5     = float(dat[1])/4096*3.355;
  float v5p1     = float(dat[2])/4096.*3.355*2;
  float dcdctemp = float(dat[3])/4096*3300/10;

  printf(" temp: %10.3f\n v2p5: %10.3f\n v5p1: %10.3f\n dcdctemp: %7.3f\n",
         temp,v2p5,v5p1,dcdctemp);
  return 0;
}
