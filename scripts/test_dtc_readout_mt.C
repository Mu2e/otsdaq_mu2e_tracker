//
#include <format>
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>

#include "TSystem.h"
//-----------------------------------------------------------------------------
// int worker(int Link, int Channel = -1, float VThreshold = 15, float VTolerance = 1, int PcieAddr = -1) {
int print_dtc_status(int Link, int PcieAddr) {

  std::mutex mtx; // For thread-safe output

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

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
      uint16_t dat = dtc_i->fDtc->ReadROCRegister(DTC_Link_ID(Link),Reg,tmo_ms);
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
int test_dtc_readout(int Test, int Link1, int Link2, int PcieAddr) {
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
  }
                                        // Wait for all threads to complete
  for (auto& t : threads) {
    t.join();
  }
    
  std::cout << "All threads completed!" << std::endl;
  return 0;
}
