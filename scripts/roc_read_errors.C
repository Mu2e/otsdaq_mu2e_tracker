//
// 2024-06-20: to make this functional, Monica needs to update the firmware

#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

using namespace DTCLib;
using namespace trkdaq;

//------------------------------------------------------------------------------
void roc_read_errors(int Link, uint16_t Reg) {

  printf("aaaaaaaa\n");
  
  DTC* dtc = DtcInterface::Instance(-1)->Dtc();

  auto link = DTC_Link_ID(Link);
  dtc->WriteROCRegister(link,17,Reg,false,1000);

  dtc->WriteROCRegister(link, 8, 64,false,1000);

  uint32_t dat = dtc->ReadROCRegister(link,17,1000);

  printf(" Reg %2i value : 0x%04x\n",Reg,dat);

  dtc->WriteROCRegister(link, 8,  0,false,1000);
}
