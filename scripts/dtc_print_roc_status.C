#define __CLING__ 1

#ifndef __dtc_print_roc_status_C__
#define __dtc_print_roc_status_C__

#include "dtc_init.C"

//-----------------------------------------------------------------------------
void dtc_print_roc_status(int Link, int PcieAddress = -1) {

  printf(">>>>> ROC %i registers:\n",Link);

  DTC* dtc = dtc_init(PcieAddress,Dtc::DefaultSimMode,1<<4*Link);

  DTC_Link_ID link = DTC_Link_ID(Link);
  uint32_t dat;

  uint reg = 0;
  dat = dtc->ReadROCRegister(link,reg,200);
  printf("register: %3i value: 0x%04x\n",reg, dat);

  reg = 8;
  dat = dtc->ReadROCRegister(link,reg,200);
  printf("register: %3i value: 0x%04x\n",reg, dat);

  reg = 18;
  dat = dtc->ReadROCRegister(link,reg,200);
  printf("register: %3i value: 0x%04x\n",reg, dat);

  uint iw, iw1, iw2;
  iw1 = dtc->ReadROCRegister(link,23,200);
  iw2 = dtc->ReadROCRegister(link,24,200);
  iw  = (iw2 << 16) | iw1;
  printf("SIZE_FIFO_FULL [28]+STORE_POS[25:24]+STORE_CNT[19:0]: 0x%08x\n",iw);

  iw1 = dtc->ReadROCRegister(link,25,200);
  iw2 = dtc->ReadROCRegister(link,26,200);
  iw  = (iw2 << 16) | iw1;
  printf("SIZE_FIFO_EMPTY[28]+FETCH_POS[25:24]+FETCH_CNT[19:0]: 0x%08x\n",iw);

  iw1 = dtc->ReadROCRegister(link,64,200);
  iw2 = dtc->ReadROCRegister(link,65,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. EVM      seen           : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,27,200);
  iw2 = dtc->ReadROCRegister(link,28,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. HB       seen           : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,29,200);
  iw2 = dtc->ReadROCRegister(link,30,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. null HB   seen          : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,31,200);
  iw2 = dtc->ReadROCRegister(link,32,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. HB   on hold            : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,33,200);
  iw2 = dtc->ReadROCRegister(link,34,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. PREFETCH seen           : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,35,200);
  iw2 = dtc->ReadROCRegister(link,36,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. data reqs seen          : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,37,200);
  iw2 = dtc->ReadROCRegister(link,38,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. data reqs read from DDR : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,39,200);
  iw2 = dtc->ReadROCRegister(link,40,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. data reqs sent to DTC   : %10i\n",iw);

  iw1 = dtc->ReadROCRegister(link,41,200);
  iw2 = dtc->ReadROCRegister(link,42,200);
  iw  = (iw2 << 16) | iw1;
  printf("no. data reqs with null data: %10i\n",iw);

  printf("\n");
  
  iw1 = dtc->ReadROCRegister(link,43,200);
  iw2 = dtc->ReadROCRegister(link,44,200);
  iw  = (iw2 << 16) | iw1;
  printf("last SPILL tag              : 0x%08x\n",iw);

  iw1 = dtc->ReadROCRegister(link,45,200);
  iw2 = dtc->ReadROCRegister(link,46,200);
  iw  = (iw2 << 16) | iw1;
  printf("last HB tag                 : 0x%08x\n",iw);

  iw1 = dtc->ReadROCRegister(link,48,200);
  iw2 = dtc->ReadROCRegister(link,49,200);
  iw  = (iw2 << 16) | iw1;
  printf("last PREFETCH tag           : 0x%08x\n",iw);

  iw1 = dtc->ReadROCRegister(link,51,200);
  iw2 = dtc->ReadROCRegister(link,52,200);
  iw  = (iw2 << 16) | iw1;
  printf("last FETCHED tag            : 0x%08x\n",iw);

  iw1 = dtc->ReadROCRegister(link,54,200);
  iw2 = dtc->ReadROCRegister(link,55,200);
  iw  = (iw2 << 16) | iw1;
  printf("last DATA REQ tag           : 0x%08x\n",iw);

  iw1 = dtc->ReadROCRegister(link,57,200);
  iw2 = dtc->ReadROCRegister(link,58,200);
  iw  = (iw2 << 16) | iw1;
  printf("OFFSET tag                  : 0x%08x\n",iw);

}
#endif
