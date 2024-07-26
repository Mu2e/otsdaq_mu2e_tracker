///////////////////////////////////////////////////////////////////////////////
// mu2edaq09 : 
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"
int init_run_configuration(DtcGui* X) {
  int rc(0);

  printf("[init_run_configuration] : test : host:%s\n",gSystem->Getenv("HOSTNAME"));

  DtcGui::DtcData_t* dtc = (DtcGui::DtcData_t*) X->fDtcData;

  X->fNDtcs           = 1; // 2;              // installed on a machine

  // dtc[0].fName        = "CFO";
  // dtc[0].fPcieAddr    = 0;
  // dtc[0].fLinkMask    = 0x2;            // 2 DTCs on link0
  // gSystem->Setenv("CFOLIB_CFO","0");

  dtc[0].fName        = "DTC";
  dtc[0].fPcieAddr    = 1;
  dtc[0].fLinkMask    = 0x00000111;          // ROC6: link1 stopped responding
  dtc[0].fReadoutMode = 1;               // 0:patterns 1:digis
  gSystem->Setenv("DTCLIB_DTC","1");

  return rc;
}
