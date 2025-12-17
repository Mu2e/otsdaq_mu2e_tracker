//

int dtc_control_roc_read_ddr(int Link, int Block, int PcieAddr = -1) {
  int rc(0);
  // ROC reg 15 - last memory block read
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

  DTC_Link_ID link_id = DTC_Link_ID(Link);

  // write block number to reg 33
  dtc_i->fDtc->WriteROCRegister(link_id,33,Block,false,1000);
  dtc_i->fDtc->WriteROCRegister(link_id,32, 0x01,false,1000);
  dtc_i->fDtc->WriteROCRegister(link_id,32, 0x00,false,1000);

  int reg_20 = dtc_i->fDtc->ReadROCRegister (link_id,20,1000);              // ox8080
  int nw     = dtc_i->fDtc->ReadROCRegister (link_id,21,1000);         // number of 16-bit words in a 1 kByte block (512)
  // at this point, if everything was OK, can read the data

  std::cout << std::format("reg_21(nwords):{:d}  reg_20:0x{:4x}\n",nw,ret);

  if (nw == 512) {
    std::vector<uint16_t> v;
    dtc_i->fDtc->ReadROCBlock(v,link_id,512,512,false,1000);
    dtc_i->PrintBuffer(v2.data(),512);
  }
  else {
    std::cout << "ERROR: smth went wrong, try again\n";
    rc = -1;
  }
  return rc;
}
