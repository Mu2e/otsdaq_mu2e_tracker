//
#define __CLING__ 1

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;


//-----------------------------------------------------------------------------
void parse_fi_output(char* Data, int NBytes) {
  vector<int> val;

  int niteration = int((NBytes-2)/(1+24*2+96+6*2));
  printf(">>> ninteration = %i\n",niteration);

  val.push_back(Data[0]);
  val.push_back(Data[1]);

  for (int i=0; i<niterations; i++) {
    // 1 char
    val.push_back(Data[2]);
    // 24 shorts
    for (int k=0; k<24; k++) {
      loc = k*2+3;
      short w = Data[loc] + Data[loc+1]<<8;
      val.push_back(w);
    }

    // 96 chars
    for (int k=0; k<96; k++) {
      loc = k*2+3+48;
      short w = Data[loc] + Data[loc+1]<<8;
      val.push_back(w);
    }
    
    // 96 chars
    for (int k=0; k<6; k++) {
      loc = k*2+3+48+96;
      short w = Data[loc] + Data[loc+1]<<8;
      val.push_back(w);
    }
    
  }
}


//-----------------------------------------------------------------------------
// find alignment:
// on mu2edaq09, a delay > 1.4 usec is needed after WriteROCRegister(258...)
// so can't do that for every event ...
//-----------------------------------------------------------------------------
void find_alignment(int PcieAddress, int Link, int ROCSleepTime = 2000) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(Link);

  int roc_mask = 1 << (4*Link);
  DTC dtc(DTC_SimMode_NoCFO,PcieAddress,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  //  monica_var_link_config(&dtc,roc);
//-----------------------------------------------------------------------------
// write 0x1 into reg 264, sleep for some time, 
// then wait till reg 128 returns non-zero
//-----------------------------------------------------------------------------
  int16_t  eye_monitor_width( 4);
  int16_t  init_adc_phase   ( 0);
  int16_t  ifcheck          ( 1);
  int16_t  chan_num         (-1);
  int16_t  adc_num          (-1);
  uint16_t chan_mask[6] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};


  vector<uint16_t> vec;
  vec.push_back(eye_monitor_width);
  vec.push_back(init_adc_phase);
  vec.push_back(ifcheck);
  vec.push_back(chan_num);
  vec.push_back(adc_num);
  vec.push_back(chan_mask[0]);
  vec.push_back(chan_mask[1]);
  vec.push_back(chan_mask[2]);
  vec.push_back(chan_mask[3]);
  vec.push_back(chan_mask[4]);
  vec.push_back(chan_mask[5]);

  bool increment_address(false);
  dtc.WriteROCBlock   (roc,264,vec,false,increment_address,100);
  std::this_thread::sleep_for(std::chrono::microseconds(ROCSleepTime));

  // 0x86 = 0x82 + 4
  uint16_t u; 
  while ((u = dtc.ReadROCRegister(roc,128,100)) != 0x8000) {}; 
  printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
  int nw = dtc.ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

  nw = nw-4;
  vector<uint16_t> v2;
  dtc.ReadROCBlock(v2,roc,264,nw,false,100);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  dtc.WriteROCRegister(roc,14,0x01,false,1000); 

  print_buffer(v2.data(),nw);

  int nbytes = nw*2;

//-----------------------------------------------------------------------------
// parse output  
//-----------------------------------------------------------------------------
  parse_fi_output(v2.data(),nw*2)
// sleep
//-----------------------------------------------------------------------------
  // std::this_thread::sleep_for(std::chrono::milliseconds(SPISleepTime));
}
