///////////////////////////////////////////////////////////////////////////////
// mu2edaq09/pcie0 : 
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"
int init_run_configuration(DtcGui* X) {
  int rc(0);

  printf("[init_run_configuration] : test : host:%s\n",gSystem->Getenv("HOSTNAME"));

  DtcGui::DtcData_t* dtc = (DtcGui::DtcData_t*) X->fDtcData;

  X->fNDtcs           = 1; // 2;              // installed on a machine

  dtc[0].fName        = "DTC" ; // "CFO";
  dtc[0].fPcieAddr    = 0;
  dtc[0].fLinkMask    = 0x111111;           // start from 2 DTCs on link0
  // dtc[0].fLinkMask    = 0x0010;           // MN180
  // dtc[0].fLinkMask    = 0x1000;           // MN162
  // dtc[0].fLinkMask    = 0x1001;           // MN234+MN162
  dtc[0].fJAMode      = 0x01;           // no ext clock 
  //gSystem->Setenv("CFOLIB_CFO","0");
  gSystem->Setenv("DTCLIB_DTC","0");

  // dtc[0].fName        = "DTC";
  // dtc[0].fPcieAddr    = 1;
  // dtc[0].fLinkMask    = 0x111;           // ROC0 and ROC1
  // gSystem->Setenv("DTCLIB_DTC","1");

  return rc;
}
