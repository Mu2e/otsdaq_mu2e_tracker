///////////////////////////////////////////////////////////////////////////////
// mu2edaq07 
//
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

int init_run_configuration(DtcGui* X) {
  int rc(0);

  printf("[init_run_configuration] : test : %s\n",gSystem->Getenv("HOSTNAME"));

  DtcGui::DtcData_t* dat = (DtcGui::DtcData_t*) X->fDtcData;

  X->fNDtcs           = 1;

  dat[0].fName        = "DTC";
  dat[0].fPcieAddr    = 0;
  dat[0].fLinkMask    = 0x1;           // ROC1
  gSystem->Setenv("DTCLIB_DTC","0");
  
  return rc;
}
