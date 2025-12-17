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
                                        // registers
int REG_DIGI            = 385;
                                        // commands
int READ_DIGI_INFO      = 0;
int READ_DIGI_ID        = 1;
int ERASE_DIGI          = 2;          // unused
int PROGRAM_DIGI        = 3;

namespace {

//-----------------------------------------------------------------------------
// CalHV : 0 = CAL
//         1 = HV
// DebugMode: bit0: print one-liner
//            bit1: validate
//-----------------------------------------------------------------------------
int read_digi_id(trkdaq::DtcInterface* Dtc_i, int Link, uint16_t CalHV, int DebugMode = 0) {


  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  std::vector<uint16_t> input;
                                        // 5 words
  input.push_back(CalHV);               //
                                        //  input.push_back(1);                // starting address MSB
  input.push_back(READ_DIGI_ID);        // starting address MSB

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,REG_DIGI,input,false,increment_address,100);

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
  Dtc_i->fDtc->ReadROCBlock(res,roc,REG_DIGI,nw,false,100);

  if (DebugMode != 0) {
    std::cout << "nw read:" << nw << std::endl;
    if (DebugMode & 0x2) {
      Dtc_i->PrintBuffer(res.data(),nw);
    }
  }

  return nw;
}

//-----------------------------------------------------------------------------
int read_digi_info(trkdaq::DtcInterface* Dtc_i, int Link, uint16_t CalHV, int DelayUs = 0, int DebugMode = 0) {


  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  std::vector<uint16_t> input;
                                                  // 5 words
  input.push_back(CalHV);  //
  input.push_back(READ_DIGI_INFO);                // starting address MSB

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,REG_DIGI,input,false,increment_address,100);

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
  Dtc_i->fDtc->ReadROCBlock(res,roc,REG_DIGI,nw,false,100);

  if (DebugMode != 0) {
    std::cout << "nw read:" << nw << std::endl;
    if (DebugMode & 0x2) {
      Dtc_i->PrintBuffer(res.data(),nw);
    }
  }

  return nw;
}

//-----------------------------------------------------------------------------
// Cal:0  HV:1
// Fn: "CalV6.dat" or "HVV6.dat" , or smth similar
//-----------------------------------------------------------------------------
int program_digis(trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Fn, int DebugMode = 0) {
  int rc(0);

  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);
//-----------------------------------------------------------------------------
// read the input file in memory  -- 10 MBytes is nothing
//-----------------------------------------------------------------------------
  const std::string spi_directory("/home/mu2etrk/test_stand/spi_files/");

  uint16_t cal_hv;

  std::string ufn = Fn.substr(0,3);
  boost::algorithm::to_upper(ufn);
  if (ufn == "CAL") cal_hv = 0;
  else              cal_hv = 1;

  std::string pq_fn = spi_directory+Fn;
  std::ifstream file(pq_fn, std::ios::binary);

  if (not file.is_open()) {
    TLOG(TLVL_ERROR) << std::format("failed to open file:{} . BAIL OUT",Fn);
    return -1;
  }

  file.seekg(0, std::ios::end);
  int fsize = file.tellg();
  file.seekg(0, std::ios::beg);

  TLOG(TLVL_DEBUG) << std::format("fn:{} fsize:{} cal_hv:{}",Fn,fsize,cal_hv);
//-----------------------------------------------------------------------------
// read the input file
//-----------------------------------------------------------------------------
  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);
  file.close();
//-----------------------------------------------------------------------------
// initiate the transaction
//-----------------------------------------------------------------------------
  bool increment_address(false);

  std::vector<uint16_t> input;
                                        // 2 words
  input.push_back(cal_hv);              // CAL/HV
  input.push_back(PROGRAM_DIGI);        // starting address MSB
//-----------------------------------------------------------------------------
// starting point
//-----------------------------------------------------------------------------
  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,REG_DIGI,input,false,increment_address,100);

  TLOG(TLVL_DEBUG) << std::format("input: [0]:{} [1]:{}",input[0],input[1]);
//-----------------------------------------------------------------------------
// on return :: 3 words + next_offset , next_nbytes ..(each 2 uint16_t's)
//-----------------------------------------------------------------------------
  bool done(false);

  while (not done) {
//-----------------------------------------------------------------------------
// check that the previous operation has completed
// don't remember why sleep's are here
//-----------------------------------------------------------------------------
    uint16_t u;
    while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
    };
//-----------------------------------------------------------------------------
// expect reg 129 to return nw=8
//-----------------------------------------------------------------------------
    int nw (-1), ntimes(0);
    while ((nw = Dtc_i->fDtc->ReadROCRegister(roc,129,1000)) != 8) {
      std::this_thread::sleep_for(std::chrono::microseconds(2));
      ntimes++;
      if (ntimes > 0) { //  100) {
        TLOG(TLVL_ERROR) std::format("after ntimes:{} nw:{}. BAIL OUT",ntimes,nw);
        rc = -1;
        break;
      }
    }
    if (rc < 0) break;

    TLOG(TLVL_DEBUG) << std::format("reg 129 reports nw:{}",nw);
//-----------------------------------------------------------------------------
// reading back the offset and the number of bytes
// at this point, nw = 8 (check for that!)
//-----------------------------------------------------------------------------
    std::vector<uint16_t> res;
    nw -= 4;
    Dtc_i->fDtc->ReadROCBlock(res,roc,REG_DIGI,nw,false,100);
    int next_offset = int(res[0]) + (int)(res[1]<<16);
    int next_nbytes = int(res[2]) + (int)(res[3]<<16);

    TLOG(TLVL_DEBUG) << std::format("next_offset:0x{:08x} next_nbytes:{}",next_offset, next_nbytes);
//--------------------------------------------------
// normal exit in case of success
//-----------------------------------------------------------------------------
    if (next_offset + next_nbytes == 0)                     break;
//-----------------------------------------------------------------------------
// not everything has been written, form input and store it in a vector 'input'
//-----------------------------------------------------------------------------
    input.clear();

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

    Dtc_i->fDtc->WriteROCBlock(roc,REG_DIGI,input,false,increment_address,100);

    if (DebugMode & 0x8) {
      Dtc_i->PrintBuffer(input.data(),input.size())'
    }
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
