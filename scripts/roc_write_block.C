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
void roc_write_block(int Link, int ROCSleepTime=5000) {

  auto link = DTC_Link_ID(Link);

  uint32_t roc_mask = 0x1 << 4*Link;

  DTC dtc(DTC_SimMode_NoCFO,-1,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(1));

  uint16_t u; 
  int      rc;

  std::vector<uint16_t> input_data;
  input_data.push_back(100);
  input_data.push_back(101);
  input_data.push_back(102);
  input_data.push_back(103);
  input_data.push_back(110);
  input_data.push_back(111);
  
  bool x = dtc.WriteROCBlock(link,259,input_data,0, 0, 100);
  // std::this_thread::sleep_for(std::chrono::microseconds(ROCSleepTime));

  while((u=dtc.ReadROCRegister(link,128,100)) == 0){}

  printf("r_128:0x%04x\n",u);

  u = dtc.ReadROCRegister(link,129,100); printf("r_129:0x%04x\n",u);

  int const nw = dtc.ReadROCRegister(link,255,100); 

  printf(" -------------- nw = %5i\n",nw);
  std::vector<uint16_t> output_data;
  dtc.ReadROCBlock(output_data,link,259,nw,false,100);

  print_buffer(output_data.data(),nw);
}


//-----------------------------------------------------------------------------
// an example of cloning a function
//-----------------------------------------------------------------------------
void roc_write_block_1() {

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
