//
#define __CLING__ 1

// #include "trk_utils.C"

#include "dtcInterfaceLib/DTC.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// PrintLevel=0: no printout
//            1: hex dump
//            2: formatted printout
//
// return panel_id or -1 if error
//-----------------------------------------------------------------------------
int read_panel_id(trkdaq::DtcInterface* Dtc_i, int Link, int PrintLevel=0) {
//-----------------------------------------------------------------------------
// convert Link into Ryan's enum, DTC has already been initialized, don't redo
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);

  DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);
  DTCLib::DTC*  dtc   = dtc_i->Dtc();
//-----------------------------------------------------------------------------
// write parameters into reg ***  (block write) , sleep for some time,
// then wait till reg 128 returns 0x8000
// chan mask always includes the first channel
//-----------------------------------------------------------------------------
  dtc->WriteROCRegister   (roc,279,0x0000,false,100);
  std::this_thread::sleep_for(std::chrono::microseconds(1000));

  uint16_t u;
  while ((u = dtc->ReadROCRegister(roc,128,5000)) != 0x8000) {};
  if ((PrintLevel & 0x1) != 0) printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
  int nw = dtc->ReadROCRegister(roc,129,100);
  if ((PrintLevel & 0x1) != 0) printf("reg:%03i val:0x%04x\n",129,nw);

  nw = nw-4;
  vector<uint16_t> v2;
  dtc->ReadROCBlock(v2,roc,279,nw,false,100);
//-----------------------------------------------------------------------------
// print output - in two formats
// expect only one word
//-----------------------------------------------------------------------------
  if ((PrintLevel & 0x1) != 0) {
    printf("------------------------------------------------nw = %i(0x%x)\n",nw,nw);
    print_buffer(v2.data(),nw);
  }

  if (nw != 1) {
    printf("ERROR: wrong number of words: %i, BAIL OUT\n",nw);
    return -1;
  }

  int panel_id = v2[0];
  if ((PrintLevel & 0x2) != 0) {
    printf("panel ID: MN%03d\n",panel_id);
  }

  return panel_id;
}
