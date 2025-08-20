//
#include "iostream"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"
#include "dtcInterfaceLib/DTC.h"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"


int RREG           = 384;
//                             comamnds
int SPI_INIT              = 3;
int PROGRAM_AIP_W_INDEX   = 4;
int PROGRAM_AIP_W_ADDRESS = 5;
int AIP_AUTO_UPDATE       = 6;
int SPI_FLASH_READ        = 7;
int SPI_LOAD_IMAGE        = 8;
int SPI_DIRECTORY_WRITE   = 9;
//-----------------------------------------------------------------------------
int spi_flash_read(int Link, uint32_t ReadAddress) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(SPI_FLASH_READ);                // SPI flash read
  input.push_back((ReadAddress      ) & 0xFFFF);  // starting address LSB
  input.push_back((ReadAddress >> 16) & 0xFFFF);  // starting address MSB
  input.push_back(254);                           // number of words to read (max 254 bytes = 127 words)
  input.push_back(0);                             // 5 words total

  auto roc  = DTCLib::DTC_Link_ID(Link);
  dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  std::cout << " input written " << std::endl;
  
  uint16_t u; 
  while ((u = dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
  // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
  

  int nw (-1);
  nw = dtc_i->fDtc->ReadROCRegister(roc,129,1000);
  
  // std::vector<uint16_t> res;

  // dtc_i->RocBlockRead(Link,RREG,res);
  // int nw = res.size();

  std::cout << "nw read:" << nw << std::endl;
  // dtc_i->PrintBuffer(res.data(),nw);

  return nw;
}


//-----------------------------------------------------------------------------
// mu2etrk@mu2edaq22:~/test_stand/daquser_001>cat ../monica_008/flash_nmap 
// 0x10000 
// 0x1010000
// 0x5000000
// works - can read back
//-----------------------------------------------------------------------------
void spi_write_directory(int Link) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  int const nimages(3);
  int flash_map[nimages] = {0x10000, 0x1010000, 0x5000000};

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(SPI_DIRECTORY_WRITE);                // SPI flash read
  input.push_back(nimages & 0xFFFF);  // nimages LSB
  input.push_back(0               );  // nimages MSB
  input.push_back(0               );  // 2nd command : 2 16-bit words - uunused
  input.push_back(0               );  // unused

  for (int i=0; i<nimages; ++i) {
    input.push_back(flash_map[i]         & 0xFFFF);                             // 5 words total
    input.push_back((flash_map[i] >> 16) & 0xFFFF);                             // 5 words total
  }

  auto roc  = DTCLib::DTC_Link_ID(Link);
  dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  std::cout << " input written " << std::endl;
  
  uint16_t u; 
  while ((u = dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
  // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
  

  // int nw (-1);
  // nw = dtc_i->fDtc->ReadROCRegister(roc,129,1000);
  
  // std::vector<uint16_t> res;

  // dtc_i->RocBlockRead(Link,RREG,res);
  // int nw = res.size();

  // std::cout << "nw read:" << nw << std::endl;
  // dtc_i->PrintBuffer(res.data(),nw);

  //  return nw;
}
