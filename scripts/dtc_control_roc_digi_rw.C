//-----------------------------------------------------------------------------
// test of the 'READ' command implementation over the fiber
// if LinkMask != -1, operate on the specified links only
//-----------------------------------------------------------------------------
int dtc_control_roc_digi_rw(int      Link,
                            int      Rw,
                            int      HvCal,
                            int      Address,
                            int      NumTrig) {

  DtcInterface* dtc_i = DtcInterface::Instance(-1);

  ControlRoc_DigiRW_Input_t  par;
  ControlRoc_DigiRW_Output_t pout;

  par.rw         = Rw;
  par.hvcal      = HvCal;        // -a
  par.address    = Address;        // -t
  par.data[0]    = NumTrig & 0xffff;
  par.data[1]    = (NumTrig >> 16) & 0xffff;

  // printf("dtc_i->fLinkMask: 0x%04x\n",dtc_i->fLinkMask);
  int  print_level(3);

  dtc_i->ControlRoc_DigiRW(&par,&pout,Link,print_level);
  return 0;
}
