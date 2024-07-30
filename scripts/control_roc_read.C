///////////////////////////////////////////////////////////////////////////////
// 'read' command over the fiber
///////////////////////////////////////////////////////////////////////////////
#define __CLING__ 1

#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

using namespace DTCLib;
using namespace trkdaq;

//-----------------------------------------------------------------------------
// example:
// ---------
// read -a 8 -t 8 -s 1 -l8 - T 10 -m 3 -p 1 -C FFFFFFFF -D FFFFFFFF -E FFFFFFFF
//-----------------------------------------------------------------------------
struct Control_ROC_Read_Par_t {
  uint16_t    adc_mode;                 // -a 8   (defailt:  0)
  uint16_t    tdc_mode;                 // -t 8   (default:  0)
  uint16_t    lookback;                 // -l 8   (default:  8)
  uint16_t    samples;                  // -s 1   (default: 16)
  int         triggers;                 // -T 10  (default:  0)
  uint16_t    chan_mask[6];             // FFFF FFFF FFFF FFFF FFFF FFFF 
  int         pulser;                   // -p 1     (default: 0)
  int         delay;                    // -d ???   (default: 1)
  int         mode;                     // -m 3 ??? )default: 0)
//-----------------------------------------------------------------------------
// the ones below are not passed to python readData
//-----------------------------------------------------------------------------
  int         clock;                    // = 99 and not used by Monica (?)
  int         channel;                  // -c
  std::string message;                  // -M 
};

//-----------------------------------------------------------------------------
// so can't do that for every event ...
//-----------------------------------------------------------------------------
void control_roc_read(int PcieAddr, int LinkMask, Control_ROC_Read_Par_t* Par, int RocSleepTime = 2000) {
//-----------------------------------------------------------------------------
// convert into enum, assume DTC is already initialized and the ROC in question is active
//-----------------------------------------------------------------------------
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  DTC*          dtc   = dtc_i->Dtc();
//-----------------------------------------------------------------------------
// write parameters into reg 266 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
  uint16_t chan_mask[6]    = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};
  int      triggers        = 10;
  int      lookback        = 8;
  int      samples         = 16;
  int      pulser          = 0;
  int      delay           = 1;
  int      mode            = 0;

  vector<uint16_t> vec;
  
  vec.push_back(Par->adc_mode);
  vec.push_back(Par->tdc_mode);
  vec.push_back(Par->lookback);

  uint16_t w1 = ((triggers      ) & 0xffff);
  uint16_t w2 = ((triggers >> 16) & 0xffff);

  vec.push_back(w1);
  vec.push_back(w2);

  for (int i=0; i<6; i++) vec.push_back(Par->chan_mask[i]); 

  vec.push_back(Par->samples);
  vec.push_back(Par->pulser );
  vec.push_back(Par->delay  );
  vec.push_back(Par->mode   );

  bool increment_address(false);

  int link_mask = LinkMask;
  if (link_mask == -1) link_mask = dtc_i->fLinkMask;
  
  for (int i=0; i<6; i++) {
    int used = (link_mask >> 4*i) & 0x1;
    if (not used)                                           continue;
    auto roc  = DTC_Link_ID(i);
    dtc->WriteROCBlock   (roc,265,vec,false,increment_address,100);
    std::this_thread::sleep_for(std::chrono::microseconds(RocSleepTime));

    // 0x86 = 0x82 + 4
    uint16_t u; 
    while ((u = dtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
    printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
    int nw = dtc->ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

    nw = nw-4;
    vector<uint16_t> v2;
    dtc->ReadROCBlock(v2,roc,265,nw,false,100);

    dtc_i->PrintBuffer(v2.data(),nw);
  }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  dtc_i->ResetRoc();
  std::this_thread::sleep_for(std::chrono::microseconds(RocSleepTime));
}

//-----------------------------------------------------------------------------
int control_roc_read_test_001(int PcieAddr, int Link = -1) {
  Control_ROC_Read_Par_t par;
  par.adc_mode = 8;
  par.tdc_mode = 0;
  par.lookback = 8;
  par.samples  = 1;
  par.triggers = 10;
  
  for (int i=0; i<6; i++) par.chan_mask[i] = 0xffff;

  par.pulser   = 1;
  par.delay    = 1;
  par.mode     = 3;
  
  control_roc_read(PcieAddr,Link, &par);
  return 0;
}
