//
#include <format>
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>
#include <chrono>

#include "TSystem.h"
//-----------------------------------------------------------------------------
int find_thresholds_panel(int Link, int Channel = -1, float VThreshold = 15, float VTolerance = 1, int PcieAddr = -1) {

  std::mutex mtx; // For thread-safe output

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  {
    std::lock_guard<std::mutex> lock(mtx);
    dtc_i->FindAlignments(1,Link);
  }

  int ich1(Channel), ich2(Channel+1);
  if (Channel == -1) {
    ich1 = 0;
    ich2 = 96;
  }

  int nerrors;
  for (int ich=ich1; ich<ich2; ich++) {
    {
      std::lock_guard<std::mutex> lock(mtx);
      std::cout << "-- channel:" << ich << std::endl;
    }

    for (int k=0; k<2; ++k) {
      nerrors = 0;
      bool ok(false);
      while ((not ok) and (nerrors < 10)) {
        {
          std::lock_guard<std::mutex> lock(mtx);
          ok = dtc_i->FindThreshold(Link,ich,k,VThreshold,VTolerance,val[ich][k]);
        }
        if (not ok) {
          std::lock_guard<std::mutex> lock(mtx);
          printf(" -- ERROR ich=%i k=%i nerrors:%i\n",ich,k,nerrors);
          nerrors += 1;
        }
        else {
          std::lock_guard<std::mutex> lock(mtx);
          printf("-- ich:%2i k:%i thr:%i\n",ich,k,val[ich][k]);
        }
      }
    }
  }

  {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream of;
    
    // Open the file for writing
    std::string fn = std::format("{}_dtc_{}_link_{}.json",gSystem->Getenv("HOSTNAME"),dtc_i->PcieAddr(),Link);
    of.open(fn);
 
    // after which one only needs to print the thresholds
    of << "[\n";
    for (int ich=ich1; ich<ich2; ich++) {
      for (int k=0; k<2; ++k) {
        of << std::format("{{\"channel\":{}",ich)
           << std::format(", \"type\":\"{}\"",type[k])
           << std::format(", \"threshold\":{}",val[ich][k])
           << std::format(", \"gain\":370}}");
        if ((k == 0) or (ich < ich2)) of << ",";
        of << std::endl;
      }
    }
    of << "]" << std::endl;
  }
  return 0;
}

//-----------------------------------------------------------------------------
int test_find_thresholds_mt(int Link, int Channel = -1, float VThreshold = 15, float VTolerance = 1, int PcieAddr = -1) {
  std::vector<std::thread> threads;

  int lnk1(Link), lnk2(Link+1);
  if (Link == -1) {
    lnk1 = 0;
    lnk2 = 6;
  }

  for (int l=lnk1; l<lnk2; ++l) {
    threads.emplace_back(find_thresholds_panel, Link, Channel, VThreshold, VTolerance, PcieAddr);
  }

  // Wait for all threads to complete
  for (auto& t : threads) {
    t.join();
  }
    
  std::cout << "All threads completed!" << std::endl;
  return 0;
}


//-----------------------------------------------------------------------------
int find_thresholds(int Link1, int Link2, int Channel = -1, float VThreshold = 15, float VTolerance = 1, int PcieAddr = -1) {
  std::vector<std::thread> threads;

  for (int lnk=Link1; lnk<Link2; lnk++) {
    find_thresholds_panel(lnk, Channel, VThreshold, VTolerance, PcieAddr);
  }

  std::cout << __func__ << " completed!" << std::endl;
  return 0;
}
