#ifndef __trk_utils_c__
#define __trk_utils_c__

#define __CLING__ 1

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

#include "print_buffer.C"

int gSleepTimeDTC      =  1000;  // [us]
int gSleepTimeROC      =  2000;  // [us]
int gSleepTimeROCReset =  4000;  // [us]

using namespace DTCLib;
//-----------------------------------------------------------------------------
void monica_digi_clear(DTCLib::DTC* dtc, int Link = 0) {
//-----------------------------------------------------------------------------
//  Monica's digi_clear
//  this will proceed in 3 steps each for HV and CAL DIGIs:
// 1) pass TWI address and data toTWI controller (fiber is enabled by default)
// 2) write TWI INIT high
// 3) write TWI INIT low
//-----------------------------------------------------------------------------
// rocUtil write_register -l $LINK -a 28 -w 16 > /dev/null
  auto link = DTCLib::DTC_Link_ID(Link);

  dtc->WriteROCRegister(link,28,0x10,false,1000); // 

  // Writing 0 & 1 to  address=16 for HV DIGIs ??? 
  // rocUtil write_register -l $LINK -a 27 -w  0 > /dev/null # write 0 
  // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null ## toggle INIT 
  // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
  dtc->WriteROCRegister(link,27,0x00,false,1000); // 
  dtc->WriteROCRegister(link,26,0x01,false,1000); // toggle INIT 
  dtc->WriteROCRegister(link,26,0x00,false,1000); // 


  // rocUtil write_register -l $LINK -a 27 -w  1 > /dev/null # write 1  
  // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null # toggle INIT
  // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
  dtc->WriteROCRegister(link,27,0x01,false,1000); // 
  dtc->WriteROCRegister(link,26,0x01,false,1000); // 
  dtc->WriteROCRegister(link,26,0x00,false,1000); // 

  // echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
  // rocUtil write_register -l $LINK -a 25 -w 16 > /dev/null
  dtc->WriteROCRegister(link,25,0x10,false,1000); // 

// rocUtil write_register -l $LINK -a 24 -w  0 > /dev/null # write 0
// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
  dtc->WriteROCRegister(link,24,0x00,false,1000); // 
  dtc->WriteROCRegister(link,23,0x01,false,1000); // 
  dtc->WriteROCRegister(link,23,0x00,false,1000); // 

// rocUtil write_register -l $LINK -a 24 -w  1 > /dev/null # write 1
// rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
// rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
  dtc->WriteROCRegister(link,24,0x01,false,1000); // 
  dtc->WriteROCRegister(link,23,0x01,false,1000); // 
  dtc->WriteROCRegister(link,23,0x00,false,1000); // 
}

//-----------------------------------------------------------------------------
// reset: write 1 into ROC r14 
// Link < 0: reset all six ROCs
//-----------------------------------------------------------------------------
void reset_roc(int DtcID, int Link) {
  if (Link >= 0) { 
    uint mask = 0x1<<4*Link;
    DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,DtcID,mask,"");
    auto link = DTCLib::DTC_Link_ID(Link);
    dtc.WriteROCRegister(link,14,0x01,false,1000); 
  }
  else {
//-----------------------------------------------------------------------------
// reset all links
//-----------------------------------------------------------------------------
    DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,DtcID,0x111111,"");
    for (int i=0; i<6; i++) {
      auto link = DTC_Link_ID(i);
      dtc.WriteROCRegister(link,14,0x01,false,1000); 
    }
  }
}

//-----------------------------------------------------------------------------
// this implements the DTC Hard Reset
//-----------------------------------------------------------------------------
void monica_dtc_hardreset(DTCLib::DTC* Dtc) {

  mu2edev* dev = Dtc->GetDevice();
  dev->write_register(0x9100,100,0x00000001);
}

//-----------------------------------------------------------------------------
// DTC_softreset should work as reset with the latest versions of the DTC firmware
// but not with 23090211
//-----------------------------------------------------------------------------
void monica_dtc_softreset(DTCLib::DTC* Dtc) {

  mu2edev* dev = Dtc->GetDevice();

  dev->write_register(0x9100,100,0x80000000);
}

//-----------------------------------------------------------------------------
void monica_var_link_config_old(DTCLib::DTC* dtc, int Link = 0) {
  mu2edev* dev = dtc->GetDevice();

  auto link = DTCLib::DTC_Link_ID(Link);

  dev->write_register(0x91a8,100,0);
  std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDTC));

  dtc->WriteROCRegister(link,14,     1,false,1000);              // reset ROC
  std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROCReset));

  dtc->WriteROCRegister(link, 8,0x030f,false,1000);             // configure ROC to read all 4 lanes
  std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

  // added register for selecting kind of data to report in DTC status bits
  // Use with pattern data. Set to zero, ie STATUS=0x55, when taking DIGI data 
  // rocUtil -a 30  -w 0  -l $LINK write_register > /dev/null

  dtc->WriteROCRegister(link,30,0x0000,false,1000);        // Set to zero, ie STATUS=0x55
  std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

  // echo "Setting packet format version to 1"
  // rocUtil -a 29  -w 1  -l $LINK write_register > /dev/null

  dtc->WriteROCRegister(link,29,0x0001,false,1000);        // configure ROC to read all 4 lanes
  std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));
}

//-----------------------------------------------------------------------------
// Link = -1: configure all 6 ROCs
//-----------------------------------------------------------------------------
void monica_var_link_config(DTCLib::DTC* dtc, int Link = 0) {
  mu2edev* dev = dtc->GetDevice();

  if ((Link >= 0) and (Link < 6)) { 
    auto link = DTCLib::DTC_Link_ID(Link);

  // dev->write_register(0x91a8,100,0);
  // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDTC));

  // dtc->WriteROCRegister(link,14,     1,false,1000);              // reset ROC
  // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROCReset));

  // dtc->WriteROCRegister(link, 8,0x030f,false,1000);             // configure ROC to read all 4 lanes
  // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));

  // added register for selecting kind of data to report in DTC status bits
  // Use with pattern data. Set to zero, ie STATUS=0x55, when taking DIGI data 
  // rocUtil -a 30  -w 0  -l $LINK write_register > /dev/null

  // dtc->WriteROCRegister(link,30,0x0000,false,1000);        // configure ROC to read all 4 lanes
  // std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));
//-----------------------------------------------------------------------------
// echo "Setting packet format version to 1"
//-----------------------------------------------------------------------------
  // rocUtil -a 29  -w 1  -l $LINK write_register > /dev/null

    dtc->WriteROCRegister(link,29,0x0001,false,1000);
    std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));
  }
  else if (Link == -1) {
    for (int i=0; i<6; i++) {
      auto link = DTCLib::DTC_Link_ID(i);
      dtc->WriteROCRegister(link,29,0x0001,false,1000);
      std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeROC));
    }
  }
  else {
    printf("trk_utils;:monica_var_link_config ERROR: wrong link : %i\n",Link);
  }
}

//-----------------------------------------------------------------------------
void print_dtc_registers(DTCLib::DTC* Dtc) {
  uint32_t res; 
  int      rc;

  mu2edev* dev = Dtc->GetDevice();

  rc = dev->read_register(0x9100,100,&res); printf("0x9100: DTC status       : 0x%08x\n",res); // expect: 0x40808404
  rc = dev->read_register(0x9138,100,&res); printf("0x9138: SERDES Reset Done: 0x%08x\n",res); // expect: 0xbfbfbfbf
  rc = dev->read_register(0x91a8,100,&res); printf("0x9158: time window      : 0x%08x\n",res); // expect: 
  rc = dev->read_register(0x91c8,100,&res); printf("0x91c8: debug packet type: 0x%08x\n",res); // expect: 0x00000000
}


//-----------------------------------------------------------------------------
void print_dtc_counters(int DtcID, int Link) {
  uint32_t reg, res; 
  int      rc;

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,DtcID,0x1<<4*Link,"");

  mu2edev* dev = dtc.GetDevice();
//-----------------------------------------------------------------------------
// packet counters
//-----------------------------------------------------------------------------
  reg = 0x9630 + (Link << 2);
  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(TX DTC data requests                   ): 0x%08x\n",reg,res);
  reg = 0x9650 + (Link << 2);
  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(TX hearbeat packets                    ): 0x%08x\n",reg,res);
  reg = 0x9670 + (Link << 2);
  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX data header packets                 ): 0x%08x\n",reg,res);
  reg = 0x9690 + (Link << 2);
  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX data packets                        ): 0x%08x\n",reg,res);
//-----------------------------------------------------------------------------
// ERROR counters
//-----------------------------------------------------------------------------
//  reg = 0x9500 + (Link << 2);
//  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX SERDES Character Not In Table Errors): 0x%08x\n",reg,res);
//  reg = 0x9520 + (Link << 2);
//  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX SERDES Disparity Errors             ): 0x%08x\n",reg,res);
  reg = 0x9540 + (Link << 2);
  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX SERDES PRBS Errors                  ): 0x%08x\n",reg,res);
  reg = 0x9560 + (Link << 2);
  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX SERDES CRC Errors                   ): 0x%08x\n",reg,res);
  reg = 0x9590;
  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(EVB SERDES RX Packet Errors            ): 0x%08x\n",reg,res);
}

//-----------------------------------------------------------------------------
// for a given Link
//-----------------------------------------------------------------------------
void reset_dtc_counters(int DtcID, int Link, int Print=0) {
  uint32_t reg, res; 
  int      rc;

  DTCLib::DTC dtc(DTCLib::DTC_SimMode_NoCFO,DtcID,0x1<<4*Link,"");

  mu2edev* dev = dtc.GetDevice();
//-----------------------------------------------------------------------------
// packet counters
//-----------------------------------------------------------------------------
  rc  = dev->write_register(0x9630 + (Link << 2),100,0x1);
  rc  = dev->write_register(0x9650 + (Link << 2),100,0x1);
  rc  = dev->write_register(0x9670 + (Link << 2),100,0x1);
  rc  = dev->write_register(0x9690 + (Link << 2),100,0x1);
//-----------------------------------------------------------------------------
// ERROR counters
//-----------------------------------------------------------------------------
//  reg = 0x9500 + (Link << 2);
//  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX SERDES Character Not In Table Errors): 0x%08x\n",reg,res);
//  reg = 0x9520 + (Link << 2);
//  rc  = dev->read_register(reg,100,&res); printf("0x%04x: N(RX SERDES Disparity Errors             ): 0x%08x\n",reg,res);

  rc  = dev->write_register(0x9540+(Link << 2),100,0x1);
  rc  = dev->write_register(0x9560+(Link << 2),100,0x1);
  rc  = dev->write_register(0x9590            ,100,0x1);

  if (Print != 0) print_dtc_counters(DtcID,Link);
}

//-----------------------------------------------------------------------------
void print_roc_registers(DTCLib::DTC* Dtc, DTCLib::DTC_Link_ID RocID, const char* Header) {
  uint16_t  roc_reg[100];
  printf("---------------------- %s print_roc_registers\n",Header);

  // roc_reg[11] = Dtc->ReadROCRegister(RocID,11,100);
  // printf("roc_reg[11] = 0x%04x\n",roc_reg[11]);
  // roc_reg[13] = Dtc->ReadROCRegister(RocID,13,100);
  // printf("roc_reg[13] = 0x%04x\n",roc_reg[13]);
  // roc_reg[14] = Dtc->ReadROCRegister(RocID,14,100);
  // printf("roc_reg[14] = 0x%04x\n",roc_reg[14]);

  printf("---------------------- END print_roc_registers\n");
}

//-----------------------------------------------------------------------------
mu2e_databuff_t* readDTCBuffer(mu2edev* device, bool& readSuccess, bool& timeout, size_t& sts) {
  mu2e_databuff_t* buffer;
  auto tmo_ms = 1500;
  readSuccess = false;

  sts = device->read_data(DTC_DMA_Engine_DAQ, reinterpret_cast<void**>(&buffer), tmo_ms);
  std::this_thread::sleep_for(std::chrono::microseconds(gSleepTimeDTC));
  
  if (sts > 0) {
    readSuccess   = true;
    void* readPtr = &buffer[0];
    uint16_t bufSize = static_cast<uint16_t>(*static_cast<uint64_t*>(readPtr));
    readPtr = static_cast<uint8_t*>(readPtr) + 8;
    
    timeout = false;
    if (sts > sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader) + 8) {
      // Check for 'dead' or 'cafe' in first packet
      readPtr = static_cast<uint8_t*>(readPtr) + sizeof(DTCLib::DTC_EventHeader) + sizeof(DTCLib::DTC_SubEventHeader);
      std::vector<size_t> wordsToCheck{ 1, 2, 3, 7, 8 };
      for (auto& word : wordsToCheck) 	{
				uint16_t* wordPtr = static_cast<uint16_t*>(readPtr) + (word - 1);
				if ((*wordPtr == 0xcafe) or (*wordPtr == 0xdead)) {
					printf(" Buffer Timeout detected! word=%5lu data: 0x%04x\n",word, *wordPtr);
					DTCLib::Utilities::PrintBuffer(readPtr, 16, 0, /*TLVL_TRACE*/4 + 3);
					timeout = true;
					break;
				}
      }
    }
  }
  return buffer;
}

#endif
