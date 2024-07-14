///////////////////////////////////////////////////////////////////////////////
// mu2edaq09 : 
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"
int init_run_configuration(DtcGui* X) {
  int rc(0);

  printf("[init_run_configuration] : test : host:%s\n",gSystem->Getenv("HOSTNAME"));

  DtcGui::DtcData_t* dat = (DtcGui::DtcData_t*) X->fDtcData;

  X->fNDtcs           = 1; // 2;              // installed on a machine

  dat[0].fName        = "CFO";
  dat[0].fPcieAddr    = 0;
  dat[0].fLinkMask    = 0x2;            // 2 DTCs on link0
  gSystem->Setenv("CFOLIB_CFO","0");

  // dat[1].fName        = "DTC";
  // dat[1].fPcieAddr    = 1;
  // dat[1].fLinkMask    = 0x10;           // ROC0 and ROC1
  // gSystem->Setenv("DTCLIB_DTC","1");

  return rc;
}
