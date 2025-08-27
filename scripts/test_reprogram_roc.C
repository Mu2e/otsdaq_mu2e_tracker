//-----------------------------------------------------------------------------
// 1. write_directory
// 2. spi_clear .. Make sure the right address is used 
//-----------------------------------------------------------------------------
#include "iostream"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"
#include "dtcInterfaceLib/DTC.h"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"
#include "TString.h"
// #include "TRACE/tracemf.h"


int RREG                  = 384;
int REG_STATUS            = 132;
//                             comamnds
int SPI_CLEAR             = 3;
int PROGRAM_AIP_W_INDEX   = 4;
int PROGRAM_AIP_W_ADDRESS = 5;
int AIP_AUTO_UPDATE       = 6;
int SPI_FLASH_READ        = 7;
int SPI_LOAD_IMAGE        = 8;
int SPI_DIRECTORY_WRITE   = 9;

namespace {
  
  struct data_t {
    int         index;
    int         offset;
    const char* fn;
    int         fsize;
  };

  int const kNImages = 5;
  
  data_t spi_data[kNImages] = {
    { 0,   0x10000, "/home/mu2etrk/test_stand/spi_files/GoldenV10.spi"          , 9524032},
    { 1, 0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV12.spi"             , 9480352},
    { 2, 0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV12-3_stage3init.bin",   82272},
    { 3, 0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV14.spi"             , 9482832},
    { 4, 0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV14_stage3init.bin"  ,   86384}
  };

};

//-----------------------------------------------------------------------------
// can't read more than 127 words (254 bytes) at a time
// DebugMode: bit0: print one-liner
//            bit1: validate
//-----------------------------------------------------------------------------
int spi_flash_read(int Link, uint32_t ReadAddress, int NWords, vector<uint16_t>* Res, int DebugMode = 0) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(SPI_FLASH_READ);                // SPI flash read
  input.push_back((ReadAddress      ) & 0xFFFF);  // starting address LSB
  input.push_back((ReadAddress >> 16) & 0xFFFF);  // starting address MSB
  input.push_back(NWords*2);                      // number of bytes to read (max 254 bytes = 127 words)
  input.push_back(0);                             // 5 words total

  auto roc  = DTCLib::DTC_Link_ID(Link);
  dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  if (DebugMode & 0x1) {
    std::cout << " input written " << std::endl;
  }
  
  uint16_t u; 
  while ((u = dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  int nw (-1);
  nw = dtc_i->fDtc->ReadROCRegister(roc,129,1000);  // should return NWords+4
//-----------------------------------------------------------------------------
// validation: reading back and comparing
//-----------------------------------------------------------------------------
  dtc_i->RocBlockRead(Link,RREG,*Res);
  int nw_read = Res->size();

  if (DebugMode != 0) {
    std::cout << "nw read:" << nw_read << std::endl;
    if (DebugMode & 0x2) {
      dtc_i->PrintBuffer(Res->data(),nw_read);
    }
  }
  
  return nw_read;
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
  input.push_back(kNImages & 0xFFFF);  // nimages LSB
  input.push_back(0                );  // nimages MSB
  input.push_back(0                );  // 2nd command : 2 16-bit words - uunused
  input.push_back(0                );  // unused

  for (int i=0; i<kNImages; ++i) {
    int offset = spi_data[i].offset;
    input.push_back( offset        & 0xFFFF);                             // 5 words total
    input.push_back((offset >> 16) & 0xFFFF);                             // 5 words total
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

//-----------------------------------------------------------------------------
// clears SPI memory in 64k blocks
//-----------------------------------------------------------------------------
// void spi_clear(int Link, int Address, int NBytes) {
void spi_clear(int Link, int Index) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  int nbytes = spi_data[Index].fsize;
  int offset = spi_data[Index].offset;

  input.push_back(SPI_CLEAR);                 // SPI clear
  input.push_back( offset         & 0xFFFF);  // 
  input.push_back((offset  >> 16) & 0xFFFF);  // 
  input.push_back( nbytes         & 0xFFFF);  // 
  input.push_back((nbytes  >> 16) & 0xFFFF);  // 

  auto roc  = DTCLib::DTC_Link_ID(Link);
  dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  std::cout << " input written " << std::endl;
  
  uint16_t u; 
  while ((u = dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
  // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
}


//-----------------------------------------------------------------------------
// load an SPI image, SPI address is an address in the SPI memory ,
// which has to include an initial offset
// returns 0 if OK and an error code otherwise
//-----------------------------------------------------------------------------
int spi_load_image(int Link, int FirstAddr, int NWords, uint16_t* Data) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(SPI_LOAD_IMAGE);                 // SPI clear
  input.push_back( FirstAddr        & 0xFFFF);  // 
  input.push_back((FirstAddr >> 16) & 0xFFFF);  // 
  input.push_back( NWords           & 0xFFFF);  // 
  input.push_back((NWords    >> 16) & 0xFFFF);  // 

  for (int i=0; i<NWords; ++i) {
    input.push_back(Data[i  ]);
  }

  auto roc  = DTCLib::DTC_Link_ID(Link);
  dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  // std::cout << " data written, nb:" << NWords*2 << std::endl;
  
  uint16_t u; 
  while ((u = dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  int rc = dtc_i->fDtc->ReadROCRegister(roc,132,1000);
  return rc;
}

//-----------------------------------------------------------------------------
// program IAP
//-----------------------------------------------------------------------------
void spi_program_iap_w_index(int Link, int Index) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_AIP_W_INDEX);                 // SPI clear
  input.push_back( Index        & 0xFFFF);  // 
  input.push_back((Index >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

                                        // wait till the command is executed
  uint16_t u;
  while ((u = dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  uint16_t status;
  status = dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  std::cout << " status:" << status << std::endl;
}

//-----------------------------------------------------------------------------
// program IAP
//-----------------------------------------------------------------------------
void spi_program_iap_w_address(int Link, int ImageStartAddr) {
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_AIP_W_ADDRESS);                 // SPI clear
  input.push_back( ImageStartAddr        & 0xFFFF);  // 
  input.push_back((ImageStartAddr >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

                                        // wait till the command is executed
  uint16_t u;
  while ((u = dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  uint16_t status;
  status = dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  std::cout << " status:" << status << std::endl;
}

//-----------------------------------------------------------------------------
// TestMode = 1: don't write to memory, just clear the memory and count
//  /home/mu2etrk/test_stand/spi_files:
//  drwxr-x---  2 mu2etrk mu2e    4096 Aug 26 16:20 .
//  drwxr-xr-x 60 mu2etrk mu2e    4096 Aug 20 16:47 ..
//  -rw-r-----  1 mu2etrk mu2e 9524032 Aug 20 16:47 GoldenV10.spi
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROC.spi
//  -rw-r-----  1 mu2etrk mu2e   82272 Aug 20 16:48 ROCV12-3_stage3init.bin
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROCV12.spi
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROCV13.spi
//-----------------------------------------------------------------------------
int test_spi_load_image(int Link, int Index, int TestMode = 1, int NWrites = -1, int Validate = 0) {
//-----------------------------------------------------------------------------
// open input file and determine its size
//-----------------------------------------------------------------------------
  std::cout << "fn:" << spi_data[Index].fn << std::endl;
                         
  std::ifstream file(spi_data[Index].fn, std::ios::binary);

  file.seekg(0, std::ios::end);
  int fsize = file.tellg();
  file.seekg(0, std::ios::beg);
//-----------------------------------------------------------------------------
// clear the spi memory
//-----------------------------------------------------------------------------
  std::cout << "-- before spi_clear : index:" << Index
            << " fn:" << spi_data[Index].fn
            << " fsize:" << std::dec << fsize << std::endl;
 
  //  spi_clear(Link, spi_data[Index].offset, fsize);
  spi_clear(Link, Index);

  std::cout << "-- after clear" << std::endl;
//-----------------------------------------------------------------------------
// actually read the file and upload its content to teh SPI memory
//-----------------------------------------------------------------------------
  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);

  int first_addr = spi_data[Index].offset;
  int nw         = 512;
  int done       = 0;
  int loc        = 0;
  int nwrites    = 0;
  int nerrors    = 0;
//-----------------------------------------------------------------------------
// can write only 1024 bytes (512 shorts) at a time
//-----------------------------------------------------------------------------
  while (not done) {
    uint16_t* x = (uint16_t*) &fileData[loc];
    if (loc + 2*nw > fsize) {
      nw   = (fsize-loc)/2;
    }
    
    if (nw <= 0) {
      done = 1;
    }
    else {
//-----------------------------------------------------------------------------
// write next record
//-----------------------------------------------------------------------------
      std::cout << "-- nw:" << std::setw(4) << nw
              << " first_addr:0x" << std::hex << first_addr
              << " loc:" << std::dec << loc
              << std::endl;

      if (TestMode == 0) {
        int rc = spi_load_image(Link, first_addr, nw, x);
        if (rc != 0) {
          std::cout << "-- ERROR: nwrites:" << nwrites << " rc:0x" << std::hex << rc << std::endl;
          nerrors++;
        }
      }
    
      loc        += 2*nw;
      first_addr += 2*nw;
      
      nwrites ++;
      if ((NWrites > 0) and (nwrites > NWrites)) break;
    }
  }

  std::cout << "END nwrites:" << nwrites << " fsize:" << fsize << " nerrors:" << nerrors << std::endl;
//-----------------------------------------------------------------------------
// validate writing
//-----------------------------------------------------------------------------
  int ierror  = 0;
  if (Validate) {
    first_addr  = spi_data[Index].offset;   // offset in SPI memory
    nw          = 127;
    done        = 0;
    loc         = 0;                    // offset in the 'file'
    int nreads  = 0;
//-----------------------------------------------------------------------------
// can read only 254 bytes (127 shorts) at a time
//-----------------------------------------------------------------------------
    while (not done) {
      if (loc + 2*nw > fsize) {
        nw   = (fsize-loc)/2;
      }
      
      if (nw <= 0) {
        done = 1;
      }
      else {
//-----------------------------------------------------------------------------
// read next record
//-----------------------------------------------------------------------------
        std::cout << "-- nw:" << std::setw(4) << nw
                  << " first_addr:0x" << std::hex << first_addr
                  << " loc:" << std::dec << loc
                  << std::endl;
        
        std::vector<uint16_t> res;
        spi_flash_read(Link, first_addr, nw, &res);
//-----------------------------------------------------------------------------
// compare to the original
//-----------------------------------------------------------------------------
        for (int iw=0; iw<nw; iw++) {
          if (res[iw] != fileData[loc+iw]) {
            std::cout << "ERROR at loc:" << loc << " iw:" << iw << std::endl;
            ierror = 1;
          }
        }

        if (ierror != 0) break;
        loc        += 2*nw;
        first_addr += nw;
      
        nreads ++;
      }
    }
  }
  
  return ierror;
}
