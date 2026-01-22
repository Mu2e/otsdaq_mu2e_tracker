///////////////////////////////////////////////////////////////////////////////
// mu2edaq09/pcie1 : 
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"
int init_run_configuration(mu2edaq::DtcInputData_t* dtc) {
  int rc(0);

  printf("[init_run_configuration] : test : host:%s\n",gSystem->Getenv("HOSTNAME"));

  //  mu2edaq::DtcInputData_t* dtc = (mu2edaq::DtcInputData_t*) X->fDtcData;

  //  X->fNDtcs           = 1; // 2;              // installed on a machine

  dtc[0].fName        = "DTC" ; // "CFO";
  dtc[0].fPcieAddr    = 1;
  // dtc[0].fLinkMask    = 0x11111;           // start from 2 DTCs on link0
  // dtc[0].fLinkMask    = 0x11111;           // can read 3 ROCs
  dtc[0].fLinkMask    = 0x111111 ;           // leave only 2, 
  dtc[0].fJAMode      = 0x01;           // no ext clock 
  //gSystem->Setenv("CFOLIB_CFO","0");

  dtc[0].fDtcID       = 1;              // for one machine, make it the same as the PcieAddr
  dtc[0].fPartitionID = 0;
  dtc[0].fEventMode   = 0;
  dtc[0].fMacAddrByte = 0;

  gSystem->Setenv("DTCLIB_DTC","1");

  // dtc[0].fName        = "DTC";
  // dtc[0].fPcieAddr    = 1;
  // dtc[0].fLinkMask    = 0x111;           // ROC0 and ROC1
  // gSystem->Setenv("DTCLIB_DTC","1");

  return rc;
}
