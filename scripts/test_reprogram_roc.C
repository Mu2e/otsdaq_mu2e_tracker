//-----------------------------------------------------------------------------
// 1. spi_write_directory
// 2. spi_clear - done by test_spi_load_image .. Make sure the right address is used
// 3. spi_program_aip_w_index for the .spi file
// 4. wait for the upload to complete
// this is how the offsets should be:
//
// 0x0010000      : spi#0 ("Golden", no microcontroller code - why ?)
// 0x1010000      : spi#1
// 0x2010000      : spi#2
// 0x3010000      : spi#3
// 0x4010000      : spi#4
// 
// 0x5000000      : bin#1 (no bin#0)
// 0x5040000      : bin#2
// 0x5080000      : bin#2
// 0x50c0000      : bin#2
// 0x6000000      : bin#2
//-----------------------------------------------------------------------------
#include "iostream"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"
#include "dtcInterfaceLib/DTC.h"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"
#include "TString.h"
#include "TSystem.h"
// #include "TRACE/tracemf.h"
                                        // registers
int RREG                  = 384;
int REG_STATUS            = 132;
                                        // commands
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

  data_t spi_data[] = {
    { 0,   0x10000, "/home/mu2etrk/test_stand/spi_files/GoldenV10.spi"          , 9524032 },
    { 1, 0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV12.spi"             , 9480352 },
    { 2, 0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV12-3_stage3init.bin",   82272 },
    { 3, 0x2010000, "/home/mu2etrk/test_stand/spi_files/ROCV14.spi"             , 9482832 },
    { 4, 0x5040000, "/home/mu2etrk/test_stand/spi_files/ROCV14_stage3init.bin"  ,   86384 },
    {-1,        -1, ""                                                          ,      -1 }
  };

  struct drac_fw_version_t {
    std::string name;
    data_t      spi_file;
    data_t      bin_file;
  };

//-----------------------------------------------------------------------------
// GoldenV10 should always be there in the beginning
// the rest versions could be overriding each other
// name = nullptr: end of data, to avoid hardcoded constants
// assume that the image index is incremented monotonically
//-----------------------------------------------------------------------------
  drac_fw_version_t drac_fw[] = {
    { "GoldenV10",
      { 0,    0x10000, "/home/mu2etrk/test_stand/spi_files/GoldenV10.spi"          , 9524032 },
      {-1,         -1, ""                                                          ,      -1 }
    },
    { "ROCV12",
      { 1,  0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV12.spi"             , 9524032 },
      { 2,  0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV12-3_stage3init.bin",   82272 }
    },
    { "ROCV14",
      { 3,  0x2010000, "/home/mu2etrk/test_stand/spi_files/ROCV14.spi"             , 9482832 },
      { 4,  0x5040000, "/home/mu2etrk/test_stand/spi_files/ROCV14_stage3init.bin"  ,   86384 }
    },
    { "",
      {-1,         -1, ""                                                          ,      -1 },
      {-1,         -1, ""                                                          ,      -1 }
    }
  };
};

//-----------------------------------------------------------------------------
// can't read more than 127 words (254 bytes) at a time
// DebugMode: bit0: print one-liner
//            bit1: validate
//-----------------------------------------------------------------------------
int spi_flash_read(trkdaq::DtcInterface* Dtc_i, int Link, uint32_t ReadAddress, int NWords, vector<uint16_t>* Res, int DebugMode = 0) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(SPI_FLASH_READ);                // SPI flash read
  input.push_back((ReadAddress      ) & 0xFFFF);  // starting address LSB
  input.push_back((ReadAddress >> 16) & 0xFFFF);  // starting address MSB
  input.push_back(NWords*2);                      // number of bytes to read (max 254 bytes = 127 words)
  input.push_back(0);                             // 5 words total

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  if (DebugMode & 0x1) {
    std::cout << " input written " << std::endl;
  }
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  int nw (-1);
  nw = Dtc_i->fDtc->ReadROCRegister(roc,129,1000);  // should return NWords+4
//-----------------------------------------------------------------------------
// validation: reading back and comparing
//-----------------------------------------------------------------------------
  Dtc_i->RocBlockRead(Link,RREG,*Res);
  int nw_read = Res->size();

  if (DebugMode != 0) {
    std::cout << "nw read:" << nw_read << std::endl;
    if (DebugMode & 0x2) {
      Dtc_i->PrintBuffer(Res->data(),nw_read);
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
// what's written is defined by the config file
//-----------------------------------------------------------------------------
void spi_write_directory(trkdaq::DtcInterface* Dtc_i, int Link) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  int nimages = 0;
  for (int i=0; drac_fw[i].name != ""; ++i) {
    if (drac_fw[i].spi_file.index >= 0) ++nimages;
    if (drac_fw[i].bin_file.index >= 0) ++nimages;
  }

  std::cout << __func__ << ": nimages:" << nimages << std::endl;

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(SPI_DIRECTORY_WRITE);           // SPI flash read
  input.push_back(nimages & 0xFFFF);    // nimages LSB
  input.push_back(0               );                   // nimages MSB
  input.push_back(0               );    // 2nd command : 2 16-bit words - uunused
  input.push_back(0               );    // unused
  
                                        // assume that the image index is incremented monotonically 
  for (int i=0; drac_fw[i].name != ""; ++i) {
    drac_fw_version_t* fw = &drac_fw[i];
    if (fw->spi_file.index >= 0) {
      int offset = fw->spi_file.offset;
      input.push_back( offset        & 0xFFFF);                             //
      input.push_back((offset >> 16) & 0xFFFF);                             //
    }
    if (fw->bin_file.index >= 0) {
      int offset = fw->bin_file.offset;
      input.push_back( offset        & 0xFFFF);                             //
      input.push_back((offset >> 16) & 0xFFFF);                             //
    }
  }

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  std::cout << __func__ << ": SPI_DIRECTORY_WRITE input written " << std::endl;
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
  // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
  

  // int nw (-1);
  // nw = dtc_i->fDtc->ReadROCRegister(roc,129,1000);
  
  // std::vector<uint16_t> res;

  // dtc_i->RocBlockRead(Link,RREG,res);
  // int nw = res.size();

  // std::cout << "nw read:" << nw << std::endl;
  // dtc_i->PrintBuffer(res.data(),nw);

  //  return nw;
  std::cout << __func__ << ":END" << std::endl;
}

//-----------------------------------------------------------------------------
// clears SPI memory in 64k blocks
//-----------------------------------------------------------------------------
// void spi_clear(int Link, int Address, int NBytes) {
void spi_clear(trkdaq::DtcInterface* Dtc_i, int Link, int Index) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

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
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  std::cout << __func__ << ": SPI_CLEAR input written " << std::endl;
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
  // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
  std::cout << __func__ << ":END" << std::endl;
}


//-----------------------------------------------------------------------------
// load an SPI image, SPI address is an address in the SPI memory ,
// which has to include an initial offset
// returns 0 if OK and an error code otherwise
//-----------------------------------------------------------------------------
int spi_load_image(trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords, uint16_t* Data) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

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
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  // std::cout << " data written, nb:" << NWords*2 << std::endl;
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  int rc = Dtc_i->fDtc->ReadROCRegister(roc,132,1000);

  std::cout << __func__ << ":END rc:" << rc << std::endl;
  return rc;
}

//-----------------------------------------------------------------------------
// program IAP
//-----------------------------------------------------------------------------
void spi_program_iap_w_index(trkdaq::DtcInterface* Dtc_i, int Link, int Index) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_AIP_W_INDEX);                 // SPI clear
  input.push_back( Index        & 0xFFFF);  // 
  input.push_back((Index >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  sleep(40);
                                        // wait till the command is executed
  uint16_t u;
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  uint16_t status;
  status = Dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  std::cout << __func__ << ":END status:" << status << std::endl;
}


//-----------------------------------------------------------------------------
// program IAP by address - not really needed
//-----------------------------------------------------------------------------
void spi_program_iap_w_address(trkdaq::DtcInterface* Dtc_i, int Link, int ImageStartAddr) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_AIP_W_ADDRESS);                 // SPI clear
  input.push_back( ImageStartAddr        & 0xFFFF);  // 
  input.push_back((ImageStartAddr >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

                                        // wait till the command is executed
  uint16_t u;
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  uint16_t status;
  status = Dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  std::cout << __func__ << ":END status:" << status << std::endl;
}

//-----------------------------------------------------------------------------
//  /home/mu2etrk/test_stand/spi_files:
//  drwxr-x---  2 mu2etrk mu2e    4096 Aug 26 16:20 .
//  drwxr-xr-x 60 mu2etrk mu2e    4096 Aug 20 16:47 ..
//  -rw-r-----  1 mu2etrk mu2e 9524032 Aug 20 16:47 GoldenV10.spi
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROC.spi
//  -rw-r-----  1 mu2etrk mu2e   82272 Aug 20 16:48 ROCV12-3_stage3init.bin
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROCV12.spi
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROCV13.spi
//
// Index       : index in the spi_data table of the image to load
// TestMode = 1: don't write to memory, just clear the memory and count
// NWrites     : number of records to write, -1: write all
//-----------------------------------------------------------------------------
int test_spi_load_image(trkdaq::DtcInterface* DtcInterface, int Link, int Index, int TestMode = 1, int NWrites = -1, int Validate = 0) {
//-----------------------------------------------------------------------------
// open input file and determine its size
//-----------------------------------------------------------------------------
  std::cout << __func__ << ": fn:" << spi_data[Index].fn << std::endl;
                         
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
  spi_clear(DtcInterface, Link, Index);

  std::cout << "-- after clear" << std::endl;
//-----------------------------------------------------------------------------
// read the input file and upload its content to the SPI memory
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
                                        // last record could be shorter
    if (loc + 2*nw > fsize) {
                                        // assume the file is written in 2-byte words
      nw   = (fsize-loc)/2;
      done = 1;
    }
    
    if (nw > 0) {
//-----------------------------------------------------------------------------
// write next record
//-----------------------------------------------------------------------------
      std::cout << __func__ << ": nwrites" << std::setw(6) << nwrites << " nw:" << std::setw(4) << nw
              << " first_addr:0x" << std::hex << first_addr
              << " loc:" << std::dec << loc
              << std::endl;

      if (TestMode == 0) {
        uint16_t* x = (uint16_t*) &fileData[loc];
        int rc      = spi_load_image(DtcInterface,Link,first_addr,nw,x);
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
// validate writing (not debugged yet)
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
        spi_flash_read(DtcInterface, Link, first_addr, nw, &res);
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
  
  std::cout << __func__ << ":END ierror:" << ierror << std::endl;
  return ierror;
}


//-----------------------------------------------------------------------------
// Index1: index of the .spi file
// Index2: index of the .bin file, -1: no bin file
// if Link = -1, reprogram all links 
//-----------------------------------------------------------------------------
// int reprogram_roc(int PcieAddr, int Link, int Index1, int Index2 = -1) {
int reprogram_roc(int PcieAddr, int Link, const char* Version) {
                                        // find firmware version
  drac_fw_version_t* fw(nullptr);
  
  for (int i=0; drac_fw[i].name != ""; ++i) {
    if (drac_fw[i].name == Version) {
      fw = &drac_fw[i];
      break;
    }
  }

  if (fw == nullptr) {
    std::cout << "ERROR: fw version:" << Version << " is not defined, BAIL OUT." << std::endl;
    return -1;
  }
//-----------------------------------------------------------------------------
// firmware to be uploaded found, proceed with the upload.
// 1. initialize the DTC
//-----------------------------------------------------------------------------
  int  link_mask = (1 << 4*Link);
  bool skip_init(false);

  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(PcieAddr,link_mask,skip_init);

  gSystem->Setenv("DTCLIB_DTC",Form("%i",PcieAddr));
//-----------------------------------------------------------------------------
// 2. spi directory has always to be mapped in full 
//-----------------------------------------------------------------------------
  spi_write_directory(dtc_i,Link);

  int test_mode = 0;                    // load for real
  
                                        // upload the .spi image

  test_spi_load_image(dtc_i,Link,fw->spi_file.index,test_mode);

                                        // upload the .bin image, if defined
  if (fw->bin_file.index >= 0) {
    test_spi_load_image(dtc_i,Link,fw->bin_file.index,test_mode);
  }
                                        // activate the image
  spi_program_iap_w_index(dtc_i,Link,fw->spi_file.index);
  
  return 0;
}
