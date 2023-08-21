//
#define __CLING__ 1

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// print 16 bytes per line
// size - number of bytes to print, even
//-----------------------------------------------------------------------------
void print_buffer(const void* ptr, int nw) {

  // int     nw  = nbytes/2;
  ushort* p16 = (ushort*) ptr;
  int     n   = 0;

  for (int i=0; i<nw; i++) {
    if (n == 0) printf(" 0x%08x: ",i*2);

    ushort  word = p16[i];
    printf("0x%04x ",word);

    n   += 1;
    if (n == 8) {
      printf("\n");
      n = 0;
    }
  }

  if (n != 0) printf("\n");
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void write_roc() {

  DTC dtc(DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint16_t u; 
  int      rc;

  std::vector<uint16_t> data;
  data.push_back(100);
  data.push_back(101);
  data.push_back(102);
  
  bool x = dtc.WriteROCBlock(DTC_Link_0,259,data,0, 0, 100);

  u = dtc.ReadROCRegister(DTC_Link_0,128,100); printf("0x%04x\n",u);
  u = dtc.ReadROCRegister(DTC_Link_0,129,100); printf("0x%04x\n",u);


  vector<uint16_t> output_data;
  int const nw = 3;
  dtc.ReadROCBlock(output_data,DTC_Link_0,259,nw,false,100);

  print_buffer(output_data.data(),nw);
}


//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void write_roc_1() {

  DTC dtc(DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint16_t u; 
  int      rc;

  std::vector<uint16_t> data;
  data.push_back(100);
  data.push_back(101);
  data.push_back(102);
  
  bool x = dtc.WriteROCBlock(DTC_Link_0,259,data,0, 0, 100);

  u = dtc.ReadROCRegister(DTC_Link_0,128,100); printf("0x%04x\n",u);
  u = dtc.ReadROCRegister(DTC_Link_0,129,100); printf("0x%04x\n",u);


  vector<uint16_t> output_data;
  int const nw = 3;
  dtc.ReadROCBlock(output_data,DTC_Link_0,259,nw,false,100);

  print_buffer(output_data.data(),nw);
}
