#ifndef __trk_utils_c__
#define __trk_utils_c__

#define __CLING__ 1

#include "dtcInterfaceLib/DTC.h"
#include "dtcInterfaceLib/DTCSoftwareCFO.h"

#include "print_buffer.C"

#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

int gSleepTimeDTC      =  1000;  // [us]
int gSleepTimeROC      =  2000;  // [us]
int gSleepTimeROCReset =  4000;  // [us]

using namespace DTCLib;
using namespace trkdaq;
//-----------------------------------------------------------------------------
void monica_digi_clear(int LinkMask = 0, int PcieAddr = -1) {
//-----------------------------------------------------------------------------
//  Monica's digi_clear from Feb 2024 (~/test_stand/monica_002/digi_clear.sh)
//  this will proceed in 3 steps each for HV and CAL DIGIs:
// 1) pass TWI address and data toTWI controller (fiber is enabled by default)
// 2) write TWI INIT high
// 3) write TWI INIT low
//-----------------------------------------------------------------------------
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

  dtc_i->MonicaDigiClear(LinkMask);

//   DTC*          dtc   = dtc_i->Dtc();
  
//   if (LinkMask != 0) dtc_i->SetLinkMask(LinkMask);

//   int mask = dtc_i->GetLinkMask();
//   for (int i=0; i<6; i++) {
//     int used = (mask >> 4*i) & 0x1;
//     if (not used)                                           continue;
// //-----------------------------------------------------------------------------
// // link is active
// //-----------------------------------------------------------------------------
//     auto link = DTCLib::DTC_Link_ID(i);

//     // rocUtil write_register -l $LINK -a 28 -w 16 > /dev/null
//     dtc->WriteROCRegister(link,28,0x10,false,1000); // 

//     // Writing 0 & 1 to  address=16 for HV DIGIs ??? 
//     // rocUtil write_register -l $LINK -a 27 -w  0 > /dev/null # write 0 
//     // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null ## toggle INIT 
//     // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
//     dtc->WriteROCRegister(link,27,0x00,false,1000); // 
//     dtc->WriteROCRegister(link,26,0x01,false,1000); // toggle INIT 
//     dtc->WriteROCRegister(link,26,0x00,false,1000); // 
    

//     // rocUtil write_register -l $LINK -a 27 -w  1 > /dev/null # write 1  
//     // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null # toggle INIT
//     // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
//     dtc->WriteROCRegister(link,27,0x01,false,1000); // 
//     dtc->WriteROCRegister(link,26,0x01,false,1000); // 
//     dtc->WriteROCRegister(link,26,0x00,false,1000); // 
    
//     // echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
//     // rocUtil write_register -l $LINK -a 25 -w 16 > /dev/null
//     dtc->WriteROCRegister(link,25,0x10,false,1000); // 
    
//     // rocUtil write_register -l $LINK -a 24 -w  0 > /dev/null # write 0
//     // rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
//     // rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
//     dtc->WriteROCRegister(link,24,0x00,false,1000); // 
//     dtc->WriteROCRegister(link,23,0x01,false,1000); // 
//     dtc->WriteROCRegister(link,23,0x00,false,1000); // 

//     // rocUtil write_register -l $LINK -a 24 -w  1 > /dev/null # write 1
//     // rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
//     // rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
//     dtc->WriteROCRegister(link,24,0x01,false,1000); // 
//     dtc->WriteROCRegister(link,23,0x01,false,1000); // 
//     dtc->WriteROCRegister(link,23,0x00,false,1000); // 
//   }
}

// //-----------------------------------------------------------------------------
// void monica_var_link_config_old(int Link = 0, int PcieAddr) {

//   DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
//   DTC*          dtc   = dtc_i->Dtc();
//   mu2edev*      dev   = dtc->GetDevice();

//   auto link = DTCLib::DTC_Link_ID(Link);

//   dev->write_register(0x91a8,100,0);
//   std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDTC));

//   dtc->WriteROCRegister(link,14,     1,false,1000);              // reset ROC
//   std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROCReset));

//   dtc->WriteROCRegister(link, 8,0x030f,false,1000);             // configure ROC to read all 4 lanes
//   std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

//   // added register for selecting kind of data to report in DTC status bits
//   // Use with pattern data. Set to zero, ie STATUS=0x55, when taking DIGI data 
//   // rocUtil -a 30  -w 0  -l $LINK write_register > /dev/null

//   dtc->WriteROCRegister(link,30,0x0000,false,1000);        // Set to zero, ie STATUS=0x55
//   std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

//   // echo "Setting packet format version to 1"
//   // rocUtil -a 29  -w 1  -l $LINK write_register > /dev/null

//   dtc->WriteROCRegister(link,29,0x0001,false,1000);        // configure ROC to read all 4 lanes
//   std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));
// }

//-----------------------------------------------------------------------------
// Link = -1: configure all 6 ROCs
//-----------------------------------------------------------------------------
void monica_var_link_config(int LinkMask = 0, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  DTC*          dtc   = dtc_i->Dtc();
  mu2edev*      dev   = dtc->GetDevice();

  dtc_i->ResetRoc(LinkMask);
  
  for (int i=0; i<6; i++) {
    int used = (LinkMask >> 4*i) & 0x1;
    if (not used)                                           continue;

    auto link = DTCLib::DTC_Link_ID(i);
    // dont' need to disable WMs
    
    // dev->write_register(0x91a8,100,0);
    // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDTC));

    // dtc->WriteROCRegister(link,14,     1,false,1000);              // reset ROC
    // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROCReset));

    dtc->WriteROCRegister(link, 8,0x030f,false,1000);             // configure ROC to read all 4 lanes

    // added register for selecting kind of data to report in DTC status bits
    // Use with pattern data. Set to zero, ie STATUS=0x55, when taking DIGI data 
    // rocUtil -a 30  -w 0  -l $LINK write_register > /dev/null
    
    // dtc->WriteROCRegister(link,30,0x0000,false,1000);        // configure ROC to read all 4 lanes
    // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));
//-----------------------------------------------------------------------------
// echo "Setting packet format version to 1"
//-----------------------------------------------------------------------------
    dtc->WriteROCRegister(link,29,0x0001,false,1000);
    std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));
  }
}

// //-----------------------------------------------------------------------------
// mu2e_databuff_t* readDTCBuffer(mu2edev* device, bool& readSuccess, bool& timeout, size_t& sts) {
//   mu2e_databuff_t* buffer;
//   auto tmo_ms = 1500;
//   readSuccess = false;

//   sts = device->read_data(DTC_DMA_Engine_DAQ, reinterpret_cast<void**>(&buffer), tmo_ms);
//   std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDTC));
  
//   if (sts > 0) {
//     readSuccess   = true;
//     void* readPtr = &buffer[0];
//     uint16_t bufSize = static_cast<uint16_t>(*static_cast<uint64_t*>(readPtr));
//     readPtr = static_cast<uint8_t*>(readPtr) + 8;
    
//     timeout = false;
//     if (sts > sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader) + 8) {
//       // Check for 'dead' or 'cafe' in first packet
//       readPtr = static_cast<uint8_t*>(readPtr) + sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader);
//       std::vector<size_t> wordsToCheck{ 1, 2, 3, 7, 8 };
//       for (auto& word : wordsToCheck) 	{
// 				uint16_t* wordPtr = static_cast<uint16_t*>(readPtr) + (word - 1);
// 				if ((*wordPtr == 0xcafe) or (*wordPtr == 0xdead)) {
// 					printf(" Buffer Timeout detected! word=%5lu data: 0x%04x\n",word, *wordPtr);
// 					DTCLib::Utilities::PrintBuffer(readPtr, 16, 0, /*TLVL_TRACE*/4 + 3);
// 					timeout = true;
// 					break;
// 				}
//       }
//     }
//   }
//   return buffer;
// }

#endif
