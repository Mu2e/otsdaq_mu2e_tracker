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
  dat[0].fPcieAddr    = 1;
  dat[0].fLinkMask    = 0x10;           // ROC1
  dat[1].fNLinkedDtcs = 0;              // doesn't matter for a DTC
  gSystem->Setenv("DTCLIB_DTC","1");
  
  return rc;
}
