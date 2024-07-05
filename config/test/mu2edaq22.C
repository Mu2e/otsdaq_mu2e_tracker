///////////////////////////////////////////////////////////////////////////////
// mu2edaq22 : not checked, see mu2edaq09.C for reference
// ---------
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
// this is an example of why one can't just use DTCLIB_DTC - there are two of them
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

int init_run_configuration(DtcGui* X) {
  int rc(0);

  printf("[init_run_configuration] : test : DtcGui : %s\n",gSystem->Getenv("HOSTNAME"));

  DtcGui::DtcData_t* dat = (DtcGui::DtcData_t*) X->fDtcData;

  X->fNDtcs           = 1;              // installed on a machine

  dat[0].fName        = "DTC";
  dat[0].fPcieAddr    = 0;
  dat[0].fLinkMask    = 0x1;            // 
  dat[0].fNLinkedDtcs = 0;              // irrelevant for a DTC

  // dat[1].fName        = "DTC";
  // dat[1].fPcieAddr    = 1;
  // dat[1].fLinkMask    = 0x1;  // 
  // dat[1].fNLinkedDtcs = 2;

  gSystem->Setenv("DTCLIB_DTC","0");

  return rc;
}
