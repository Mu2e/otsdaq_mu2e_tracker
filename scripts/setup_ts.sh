#!/usr/bin/bash
#------------------------------------------------------------------------------
#------------------------------------------------------------------------------
# call signature: setup_ts.sh pcie mask
# example :
#            ~mu2etrk/test_stand/scripts/setup_ts.sh 0 0x1000
#
# initializes DTC=0 with TS on link=3
# mask: in hex, 0x1001 configures links 0 and 3
#------------------------------------------------------------------------------
 dtc=$1
mask=$2

cd /home/mu2etrk/test_stand/pasha_405
source setup_daq.sh
spack env activate v001

root.exe -l -b <<EOF
int setup_ts(int PcieAddr, int Mask) {
  printf("aaaaaaaa: setup_ts($dtc,$mask)\n");
  gSystem->Setenv("DTCLIB_DTC",std::format("{}",PcieAddr).data());
  printf("-- initializing DTC%i\n",PcieAddr);
  auto dtc_i = trkdaq::DtcInterface::Instance(PcieAddr,Mask);
  printf("-- configuring the ROC mask 0x%x\n",Mask);
  dtc_control_roc_read();
  dtc_i->PrintStatus();
  return 0;
}

setup_ts($dtc,$mask);

EOF
#------------------------------------------------------------------------------
