//-----------------------------------------------------------------------------
// 1. spi_write_directory
// 2. spi_clear - done by spi_load_image .. Make sure the right address is used
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
int RREG                  = 385;
                                        // commands
int READ_DEVICE_INFO      = 0;
int READ_DEVICE_ID        = 1;
int ERASE_DIGI            = 2;          // unused
int PROGRAM_DIGI          = 3;

namespace {
  
  struct data_t {
    int         index;
    int         offset;
    const char* fn;
    int         fsize;
  };

//  data_t spi_data[] = {
//    { 0,   0x10000, "/home/mu2etrk/test_stand/spi_files/GoldenV10.spi"          , 9524032 },
//    { 1, 0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV12.spi"             , 9480352 },
//    { 2, 0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV12-3_stage3init.bin",   82272 },
//    { 3, 0x2010000, "/home/mu2etrk/test_stand/spi_files/ROCV14.spi"             , 9482832 },
//    { 4, 0x5040000, "/home/mu2etrk/test_stand/spi_files/ROCV14_stage3init.bin"  ,   86384 },
//    {-1,        -1, ""                                                          ,      -1 }
//  };
//
  struct drac_fw_version_t {
    std::string name;
    int         index;       // in spi_offsets and bin_offsets
    data_t      spi_file;
    data_t      bin_file;
  };
//-----------------------------------------------------------------------------
// offsets of different images - no freedom here
//-----------------------------------------------------------------------------
  // int spi_offset[5] = { 0x10000, 0x1010000, 0x2010000, 0x3010000, 0x4010000 };
  // int bin_offset[5] = {      -1, 0x5000000, 0x5040000, 0x5080000, 0x50c0000 };
//-----------------------------------------------------------------------------
// GoldenV10 should always be there in the beginning
// the rest versions could be overriding each other
// name = nullptr: end of data, to avoid hardcoded constants
// assume that the image index is incremented monotonically
//-----------------------------------------------------------------------------
  drac_fw_version_t drac_fw[] = {
    { "GoldenV10",  0,
      { 0,    0x10000, "/home/mu2etrk/test_stand/spi_files/GoldenV10.spi"          , 9524032 },
      {-1,         -1, ""                                                          ,      -1 }
    },
    { "ROCV12",     1,
      { 1,  0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV12.spi"             , 9524032 },
      { 2,  0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV12-3_stage3init.bin",   82272 }
    },
    { "ROCV14",     2,
      { 3,  0x2010000, "/home/mu2etrk/test_stand/spi_files/ROCV14.spi"             , 9482832 },
      { 4,  0x5040000, "/home/mu2etrk/test_stand/spi_files/ROCV14_stage3init.bin"  ,   86384 }
    },
                                        // end of data marker 
    { "",          -1,
      {-1,         -1, ""                                                          ,      -1 },
      {-1,         -1, ""                                                          ,      -1 }
    }
  };
};

//-----------------------------------------------------------------------------
// CalHV : 0 = CAL
//         1 = HV
// DebugMode: bit0: print one-liner
//            bit1: validate
//-----------------------------------------------------------------------------
int read_device_id(trkdaq::DtcInterface* Dtc_i, int Link, uint16_t CalHV, int DebugMode = 0) {

  
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(CalHV);  //
  //  input.push_back(1);                // starting address MSB
  input.push_back(READ_DEVICE_ID);                // starting address MSB

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  if (DebugMode & 0x1) {
    std::cout << " input:" << "[0]:" << input[0] << " [1]:" << input[1] << std::endl;
  }
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {
    std::this_thread::sleep_for(std::chrono::microseconds(2));
  }; 

  int nw (-1);
  nw = Dtc_i->fDtc->ReadROCRegister(roc,129,1000);  // should return NWords+4
  if (DebugMode != 0) {
    std::cout << "reg 129 reports nw:" << nw << std::endl;
  }
//-----------------------------------------------------------------------------
// validation: reading back and comparing
//-----------------------------------------------------------------------------
  std::vector<uint16_t> res;
  nw -= 4;
  Dtc_i->fDtc->ReadROCBlock(res,roc,RREG,nw,false,100);

  if (DebugMode != 0) {
    std::cout << "nw read:" << nw << std::endl;
    if (DebugMode & 0x2) {
      Dtc_i->PrintBuffer(res.data(),nw);
    }
  }
  
  return nw;
}

//-----------------------------------------------------------------------------
int read_device_info(trkdaq::DtcInterface* Dtc_i, int Link, uint16_t CalHV, int DelayUs = 0, int DebugMode = 0) {

  
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(CalHV);  //
  input.push_back(READ_DEVICE_INFO);                // starting address MSB

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  if (DebugMode & 0x1) {
    std::cout << " input:" << "[0]:" << input[0] << " [1]:" << input[1] << std::endl;
  }

  int ntimes(0);
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {
    if (DelayUs > 0) {
      std::this_thread::sleep_for(std::chrono::microseconds(DelayUs));
      ntimes++;
      if (ntimes > 100) {
        std::cout << "ERROR: after ntimes:" << ntimes << " r128 not ready  BAIL OUT" << std::endl;
        return -1;
      }
    }
  }; 

  ntimes = 0;
  int nw (-1);
  nw = Dtc_i->fDtc->ReadROCRegister(roc,129,1000);  // should return NWords+4
  if (DebugMode != 0) {
    std::cout << "reg 129 reports nw:" << nw << std::endl;
  }
//-----------------------------------------------------------------------------
// validation: reading back and comparing
//-----------------------------------------------------------------------------
  std::vector<uint16_t> res;
  nw -= 4;
  Dtc_i->fDtc->ReadROCBlock(res,roc,RREG,nw,false,100);

  if (DebugMode != 0) {
    std::cout << "nw read:" << nw << std::endl;
    if (DebugMode & 0x2) {
      Dtc_i->PrintBuffer(res.data(),nw);
    }
  }
  
  return nw;
}

//-----------------------------------------------------------------------------
int program_digis(trkdaq::DtcInterface* Dtc_i, int Link, uint16_t CalHV, int DebugMode = 0) {
  int rc(0);
  
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

//-----------------------------------------------------------------------------
// read the input file in memory  -- 10 MBytes is nothing
//-----------------------------------------------------------------------------
  std::string fn = "CalV5.dat";
  if (CalHV == 1) fn = "HVV5.dat";
                    
  std::ifstream file(fn, std::ios::binary);

  file.seekg(0, std::ios::end);
  int fsize = file.tellg();
  file.seekg(0, std::ios::beg);

  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);
  file.close();
//-----------------------------------------------------------------------------
// initiate the transaction
//-----------------------------------------------------------------------------
  bool increment_address(false);

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(CalHV);  //
  input.push_back(PROGRAM_DIGI);                // starting address MSB

  // starting point

  auto roc  = DTCLib::DTC_Link_ID(0); // Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  if (DebugMode & 0x1) {
    std::cout << " input:" << "[0]:" << input[0] << " [1]:" << input[1] << std::endl;
  }
// //-----------------------------------------------------------------------------
// // validation: reading back and comparing
// //-----------------------------------------------------------------------------
//   std::vector<uint16_t> res;
//   nw -= 4;
//   Dtc_i->fDtc->ReadROCBlock(res,roc,RREG,nw,false,100);

//   if (DebugMode != 0) {
//     std::cout << "nw read:" << nw << std::endl;
//     if (DebugMode & 0x2) {
//       Dtc_i->PrintBuffer(res.data(),nw);
//     }
//   }
//-----------------------------------------------------------------------------
// on return :: 3 words + next_offset , next_nbytes ..(each 2 uint16_t's)
//-----------------------------------------------------------------------------
  bool done(false);
  
  while (not done) {

    uint16_t u; 
    while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
    };

    int nw (-1), ntimes(0); // 
    while ((nw = Dtc_i->fDtc->ReadROCRegister(roc,129,1000)) != 8) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
      ntimes++;
      if (ntimes < 0) { //  100) {
        std::cout << "ERROR: after ntimes:" << ntimes << " nw:" << nw << "  BAIL OUT" << std::endl;
        rc = -1;
        break;
      }
    }
    if (rc < 0) break;
    
    if (DebugMode != 0) {
      std::cout << "reg 129 reports nw:" << nw << std::endl;
    }
//-----------------------------------------------------------------------------
// reading back the offset and the number of bytes
// at this point, nw = 8 (check for that!)
//-----------------------------------------------------------------------------
    std::vector<uint16_t> res;
    nw -= 4;
    Dtc_i->fDtc->ReadROCBlock(res,roc,RREG,nw,false,100);
    int next_offset = int(res[0]) + (int)(res[1]<<16);
    int next_nbytes = int(res[2]) + (int)(res[3]<<16);
    std::cout << "next_offset:" << next_offset << " next_nbytes:" << next_nbytes << std::endl; 
//--------------------------------------------------
// normal exit in case of success
//-----------------------------------------------------------------------------
    if (next_offset + next_nbytes == 0) {
      break;
    }
//-----------------------------------------------------------------------------
// form the input
//-----------------------------------------------------------------------------
    input.clear() ; // ???
    
    for (int i=0; i<next_nbytes; i+=2) {
      int      loc = next_offset+i;
      uint16_t w16 = (fileData[loc] & 0xff);
      // std::cout << "(1) w16:" << std::hex << w16 << std::endl;
      if (i < next_nbytes-1) {
        uint16_t bb = (fileData[loc+1] & 0xff);
        // std::cout << "bb:" << std::hex << bb << std::endl;
        w16 = w16 | (bb << 8);
      }
      // std::cout << "(2) w16:" << std::hex << w16 << std::endl;
      input.push_back(w16);
    }
    
    Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);
  }
  
  return rc;
}

//-----------------------------------------------------------------------------
void test_read_file(const char* Fn) {
//-----------------------------------------------------------------------------
// read the input file in memory  -- 10 MBytes is nothing
//-----------------------------------------------------------------------------
  std::ifstream file(Fn, std::ios::binary);

  file.seekg(0, std::ios::end);
  int fsize = file.tellg();

  std::cout << "fsize:" << fsize << std::endl;
  file.seekg(0, std::ios::beg);

  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);
  file.close();
}
