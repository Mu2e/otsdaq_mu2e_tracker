///////////////////////////////////////////////////////////////////////////////
// mu2edaq09 : 
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/gui/DtcGui.hh"
int init_run_configuration(DtcGui* X) {
  int rc(0);

  printf("[init_run_configuration] : test : host:%s\n",gSystem->Getenv("HOSTNAME"));

  DtcGui::DtcData_t* dat = (DtcGui::DtcData_t*) X->fDtcData;

  X->fNDtcs           = 2;  // on a machine

  dat[0].fName        = "CFO";
  dat[0].fPcieAddr    = 0;
  dat[0].fLinkMask    = 0x1;  // timing chain link
  dat[0].fNLinkedDtcs = 2;

  dat[1].fName        = "DTC";
  dat[1].fPcieAddr    = 1;
  dat[1].fLinkMask    = 0x1;  // ROC0
  dat[1].fNLinkedDtcs = 0;

  return rc;
}
