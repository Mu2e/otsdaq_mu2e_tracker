//
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"
using namespace trkdaq;
using namespace DTCLib;

//-----------------------------------------------------------------------------
void monica_digi_clear(int Link) {
//-----------------------------------------------------------------------------
//  Monica's digi_clear
//  this will proceed in 3 steps each for HV and CAL DIGIs:
// 1) pass TWI address and data toTWI controller (fiber is enabled by default)
// 2) write TWI INIT high
// 3) write TWI INIT low
//-----------------------------------------------------------------------------
  auto link = DTC_Link_ID(Link);

  DTC* dtc = DtcInterface::Instance(-1)->Dtc();

  dtc->WriteROCRegister(link,28,0x10,false,1000); // 

  // Writing 0 & 1 to  address=16 for HV DIGIs ??? 

  dtc->WriteROCRegister(link,27,0x00,false,1000); // write 0 
  dtc->WriteROCRegister(link,26,0x01,false,1000); // toggle INIT 
  dtc->WriteROCRegister(link,26,0x00,false,1000); // 

  dtc->WriteROCRegister(link,27,0x01,false,1000); //  write 1 
  dtc->WriteROCRegister(link,26,0x01,false,1000); //  toggle INIT
  dtc->WriteROCRegister(link,26,0x00,false,1000); // 

  // echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
  dtc->WriteROCRegister(link,25,0x10,false,1000); // 

  dtc->WriteROCRegister(link,24,0x00,false,1000); // write 0
  dtc->WriteROCRegister(link,23,0x01,false,1000); // toggle INIT
  dtc->WriteROCRegister(link,23,0x00,false,1000); // 

  dtc->WriteROCRegister(link,24,0x01,false,1000); // write 1
  dtc->WriteROCRegister(link,23,0x01,false,1000); // toggle INIT
  dtc->WriteROCRegister(link,23,0x00,false,1000); // 
}
