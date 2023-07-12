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
// read_spi: read slow control data
//-----------------------------------------------------------------------------
void read_spi() {

  DTC dtc(DTC_SimMode_NoCFO,-1,0x1,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(500));

  uint16_t u; 
  int      rc;

  dtc.WriteROCRegister(DTC_Link_0,258,0x0000,false,100);
  u = dtc.ReadROCRegister(DTC_Link_0,roc_address_t(128),100); printf("0x%04x\n",u);
  u = dtc.ReadROCRegister(DTC_Link_0,roc_address_t(129),100); printf("0x%04x\n",u);

  vector<uint16_t> dat;
  int nw = 36;
  dtc.ReadROCBlock(dat,DTC_Link_0,258,nw,false,100);

  print_buffer(&dat[0],nw);
}
