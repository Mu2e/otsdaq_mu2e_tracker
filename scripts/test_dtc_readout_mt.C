//
#include <format>
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>
#include "otsdaq-mu2e-tracker/Ui/test_program_roc.hh"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "TSystem.h"


using namespace trkdaq;
//-----------------------------------------------------------------------------
// int worker(int Link, int Channel = -1, float VThreshold = 15, float VTolerance = 1, int PcieAddr = -1) {
int print_dtc_status(int Link, int PcieAddr) {

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

  std::mutex mtx; // For thread-safe output

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  {
    std::lock_guard<std::mutex> lock(mtx);
    for (int i=0; i<10; i++) {
      std::cout << std::format("------------------------------------------------------ Link:{} cycle:{}\n",Link,i);
      dtc_i->PrintStatus();
      sleep(1);
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
int read_roc_register(int Link, int Reg, int PcieAddr) {

  std::mutex mtx; // For thread-safe output

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  int tmo_ms(2);
  for (int i=0; i<10; i++) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      uint16_t dat = dtc_i->fDtc->ReadROCRegister(DTCLib::DTC_Link_ID(Link),Reg,tmo_ms);
      std::cout << std::format("---- Link:{} cycle:{} reg:{} val:0x{:04x}\n",Link,i,Reg,dat);
      sleep(1);
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
int find_alignment(int Link, int PcieAddr) {

  std::mutex mtx; // For thread-safe output

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  for (int i=0; i<10; i++) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      uint16_t dat = dtc_i->FindAlignments(1,Link);
    }
  }
  return 0;
}

//-----------------------------------------------------------------------------
// 2025-11-15: keep NWords <= 127
//-----------------------------------------------------------------------------
int spi_read_record_thread(int Link, int NWords, int PcieAddr) {

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  test_program_roc t;

  // std::lock_guard<std::mutex> lock(mtx);
  for (int i=0; i<10; i++) {
    std::cout << std::format("[{}:{}] ------ Link:{} NWords:{} attempt:{}\n",__func__,__LINE__,Link,NWords,i);
    t.spi_read_record(dtc_i,Link,0x0,NWords,nullptr,0xf);
  }

  return 0;
}

//-----------------------------------------------------------------------------
int spi_read_segment_thread(int Link, uint32_t SpiOffset, int PcieAddr) {

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  test_program_roc t;

  std::vector<char> res;
  int debug_mode = 1;
  t.spi_read_segment(dtc_i,Link,SpiOffset,0x10000,res,debug_mode);

  return 0;
}

//-----------------------------------------------------------------------------
int dtc_validate_version_thread(int Link, const std::string& Version, const std::string& FileType, int PcieAddr) {

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  test_program_roc t;

  int debug_mode = 1;
  t.dtc_validate_version(dtc_i,Link,Version,FileType,debug_mode);

  return 0;
}

//-----------------------------------------------------------------------------
int test_dtc_readout(int Test, int Link1, int Link2, const char* Version, const char* Type, int PcieAddr) {
  std::vector<std::thread> threads;

  for (int lnk=Link1; lnk<Link2+1; ++lnk) {
    if (Test == 1) {
      threads.emplace_back(print_dtc_status, lnk, PcieAddr);
    }
    else if (Test == 2) {
      int reg = 0;
      threads.emplace_back(read_roc_register, lnk, reg, PcieAddr);
    }
    else if (Test == 3) {
      threads.emplace_back(find_alignment, lnk, PcieAddr);
    }
    else if (Test == 4) {
      int nw = 127;
      threads.emplace_back(spi_read_record_thread, lnk, nw, PcieAddr);
    }
    else if (Test == 5) {
      // threads.emplace_back(spi_read_segment_thread, lnk, 0x1010000, PcieAddr);
      threads.emplace_back(spi_read_segment_thread, lnk, 0x2010000, PcieAddr);
    }
    else if (Test == 6) {
      threads.emplace_back(dtc_validate_version_thread, lnk, Version,Type, PcieAddr);
    }
  }
                                        // Wait for all threads to complete
  for (auto& t : threads) {
    t.join();
  }

  std::cout << "All threads completed!" << std::endl;
  return 0;
}
