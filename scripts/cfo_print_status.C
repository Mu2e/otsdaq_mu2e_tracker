#ifndef __cfo_print_status_C__
#define __cfo_print_status_C__

#define __CLING__ 1

#include "otsdaq-mu2e-tracker/Ui/CfoInterface.hh"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"
//-----------------------------------------------------------------------------
void cfo_print_status(int PcieAddress = -1) {

  try {
    trkdaq::CfoInterface* cfo_i = trkdaq::CfoInterface::Instance(PcieAddress);
    cfo_i->PrintStatus();
  }
  catch(...) {
    printf("ERROR\n");
  }
}

//-----------------------------------------------------------------------------
void dtc_print_status(int PcieAddress = -1) {

  try {
    trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(PcieAddress);
    dtc_i->PrintStatus();
  }
  catch(...) {
    printf("ERROR\n");
  }
}

//-----------------------------------------------------------------------------
void print_roc_status(int PcieAddress = -1, int Link = 0) {

  try {
    trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(PcieAddress);
    dtc_i->PrintRocStatus(Link);
  }
  catch(...) {
    printf("ERROR in %s\n",__func__);
  }
}


#endif
