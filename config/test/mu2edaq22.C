///////////////////////////////////////////////////////////////////////////////
// mu2edaq22 : not checked, see mu2edaq09.C for reference
// ---------
// init_run_configuration : the name and the call signature are is fixed 
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/gui/DtcGui.hh"

int init_run_configuration(DtcGui::DtcData_t* DtcData) {
  int rc(0);

  printf("[init_run_configuration] : test : DtcGui : %s\n",gSystem->Getenv("HOSTNAME"));

  DtcData[0].fName     = "DTC";
  DtcData[0].fPcieAddr = 0;
  DtcData[0].fLinkMask = 0x1;  // 

  DtcData[1].fName     = "DTC";
  DtcData[1].fPcieAddr = 1;
  DtcData[1].fLinkMask = 0x1;  // 

  return rc;
}
