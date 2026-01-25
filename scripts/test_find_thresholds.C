//
#include <format>
#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <chrono>

#include "TSystem.h"

#include "frontends/utils/OdbInterface.hh"
#include "frontends/utils/utils.hh"

//-----------------------------------------------------------------------------
// link is the link number,  -1 is not allowed
//-----------------------------------------------------------------------------
int get_panel_name_from_odb(int PcieAddress, int Link, std::string& PanelName) {
  int rc(0);

  try {
    cm_connect_experiment("mu2e-dl-01-data","tracker","test_get_mnid",nullptr);
    
    OdbInterface* odb_i = OdbInterface::Instance();
    HNDLE         h_arc = odb_i->GetActiveRunConfigHandle();
  
    std::string   subnet = odb_i->GetString(h_arc, "DAQ/PublicSubnet");
    
    std::string   host_label = get_short_host_name(subnet.data());
    
    std::string path = std::format("DAQ/Nodes/{}/DTC{}/Link{}/DetectorElement/Name",
                                   host_label,PcieAddress,Link);
    
    PanelName = odb_i->GetString(h_arc,path.data());
    std::cout << std::format("PanelName:{}\n",PanelName);
  }
  catch (...) {
    std::cout << "ERROR ... rc=-1\n";
    rc = -1;
  }
  cm_disconnect_experiment();
  return rc;
}

//-----------------------------------------------------------------------------
int find_thresholds_panel(int Link, float VThreshold = 15, int Channel = -1, float VTolerance = 1, int PcieAddr = -1) {

  std::mutex mtx; // For thread-safe output

  uint16_t val[96][2];
  const char* type[2] = {"cal","hv"};

  auto dtc_i = DtcInterface::Instance(PcieAddr);

  {
    // std::lock_guard<std::mutex> lock(mtx);
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
          // std::lock_guard<std::mutex> lock(mtx);
          ok = dtc_i->FindThreshold(Link,ich,k,VThreshold,VTolerance,val[ich][k]);
        }
        if (not ok) {
          //    std::lock_guard<std::mutex> lock(mtx);
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

//-----------------------------------------------------------------------------
// try to figure the file name - if can get the MNID from ODB, use that,
// if not - use host-dtc_id-link -based name
//-----------------------------------------------------------------------------
  int pcie_addr = dtc_i->PcieAddr();
  std::string fn, panel_name;

  // int lrc = get_panel_name_from_odb(pcie_addr,Link,panel_name);
  // if (lrc == 0) {
  //   fn = std::format("{}.json",panel_name);
  // }
  //  else {
  fn = std::format("{}_dtc_{}_link_{}.json",gSystem->Getenv("HOSTNAME"),dtc_i->PcieAddr(),Link);
  // }
  
  {
    std::lock_guard<std::mutex> lock(mtx);
    std::ofstream of;
    of.open(fn);
 
    // after which one only needs to print the thresholds
    of << "[\n";
    for (int ich=ich1; ich<ich2; ich++) {
      for (int k=0; k<2; ++k) {
        of << std::format("{{\"channel\":{}",ich)
           << std::format(", \"type\":\"{}\"",type[k])
           << std::format(", \"threshold\":{}",val[ich][k])
           << std::format(", \"gain\":370}}");
        if ((k == 0) or (ich < ich2-1)) of << ",";
        of << std::endl;
      }
    }
    of << "]" << std::endl;
  }
  return 0;
}

//-----------------------------------------------------------------------------
int test_find_thresholds_mt(int Link, float VThreshold = 15, int Channel = -1, float VTolerance = 1, int PcieAddr = -1) {
  std::vector<std::thread> threads;

  int lnk1(Link), lnk2(Link+1);
  if (Link == -1) {
    lnk1 = 0;
    lnk2 = 6;
  }

  for (int l=lnk1; l<lnk2; ++l) {
    threads.emplace_back(find_thresholds_panel, Link, VThreshold, Channel, VTolerance, PcieAddr);
  }

  // Wait for all threads to complete
  for (auto& t : threads) {
    t.join();
  }
    
  std::cout << "All threads completed!" << std::endl;
  return 0;
}


//-----------------------------------------------------------------------------
int find_thresholds(int Link1, int Link2, float VThreshold = 15, int Channel = -1, float VTolerance = 1, int PcieAddr = -1) {
  std::vector<std::thread> threads;

  
  for (int lnk=Link1; lnk<Link2; lnk++) {
    find_thresholds_panel(lnk, Channel, VThreshold, VTolerance, PcieAddr);
  }

  std::cout << __func__ << " completed!" << std::endl;
  return 0;
}
