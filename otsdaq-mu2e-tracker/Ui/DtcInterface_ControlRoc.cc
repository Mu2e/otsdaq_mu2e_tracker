//
#include <vector>
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterface_ControlRoc"

#include "TString.h"     // includes ROOT's Form

using namespace DTCLib;

namespace  trkdaq {
//-----------------------------------------------------------------------------
// a boilerplate for a generic control_ROC.py CLI command - do we need it at  all ?
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc(const char* Command, void* Par) {
    return 0;
  }


//-----------------------------------------------------------------------------
// digi_rw over the fiber: reg 263
// if Link = -1, use fLinkMask, otherwise operate assuming a single link
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_DigiRW(ControlRoc_DigiRW_Input_t*  Input     ,
                                      ControlRoc_DigiRW_Output_t* Output    ,
                                      int                         Link      ,
                                      int                         PrintLevel,
                                      std::ostream&               Stream    ) {
//-----------------------------------------------------------------------------
//    const int  reg (263);  // for digi_rw
    
    std::vector<uint16_t> vec;
  
    vec.push_back(Input->rw     );
    vec.push_back(Input->hvcal  );
    vec.push_back(Input->address);
    vec.push_back(Input->data[0]);
    vec.push_back(Input->data[1]);
    
    if (PrintLevel > 0) {
      Stream << Form("Input->rw           : %i\n"    ,Input->rw);
      Stream << Form("Input->hwcal        : 0x%04x\n",Input->hvcal);
      Stream << Form("Input->address      : 0x%04x\n",Input->address);
      Stream << Form("Input->data[0]      : 0x%04x\n",Input->data[0]);
      Stream << Form("Input->data[1]      : 0x%94x\n",Input->data[1]);
    }
    
    bool increment_address(false);
//-----------------------------------------------------------------------------
// if Link != -1, use it, but don't redefine fLinkMask - that would be wa-a-ay too smart !
//-----------------------------------------------------------------------------
    int link_mask = fLinkMask;
    if (Link != -1) link_mask = (1 << 4*Link);
//-----------------------------------------------------------------------------
// loop over the links and execute
//-----------------------------------------------------------------------------
    for (int i=0; i<6; i++) {
      int used = (link_mask >> 4*i) & 0x1;
      if (not used)                                           continue;
      auto roc  = DTC_Link_ID(i);
      fDtc->WriteROCBlock   (roc,REG_DIGIRW,vec,false,increment_address,100);
      std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
      
      uint16_t u; 
      while ((u = fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
      Stream << Form("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
      int nw = fDtc->ReadROCRegister(roc,129,100);
      Stream << Form("reg:%03i val:0x%04x\n",129,nw);

      nw = nw-4;
      std::vector<uint16_t> v2;
      fDtc->ReadROCBlock(v2,roc,REG_DIGIRW,nw,false,100);

      if (PrintLevel > 0) {
        Stream << " ---------------- link:" << i << std::endl;
        if (PrintLevel & 0x1) PrintBuffer(v2.data(),nw,&Stream);
        if (PrintLevel & 0x2) {
          
          trkdaq::ControlRoc_DigiRW_Output_t* o = (trkdaq::ControlRoc_DigiRW_Output_t*) v2.data();
          
          Stream << Form("rw           : %i\n"        ,o->rw);
          Stream << Form("hvcal        : 0x%04x\n"    ,o->hvcal);
          Stream << Form("address      : 0x%04x\n"    ,o->address);
          Stream << Form("data[32 bit] : 0x%04x%04x\n",o->data[1],o->data[0]);
          Stream << Form("adc_num      : 0x%04x\n"    ,o->adc_num);
          Stream << Form("adc_mask     : 0x%04x\n"    ,o->adc_mask);
        }
      }
    }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    // ResetLinks();
    return 0;
  }

  
//-----------------------------------------------------------------------------
// if Link = -1, use fLinkMask, otherwise operate assuming a single link
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_Read(ControlRoc_Read_Input_t0* Par       ,
                                    int                       Link      ,
                                    int                       PrintLevel,
                                    std::ostream&             Stream    ) {
//-----------------------------------------------------------------------------
// write parameters into reg 266 (via block write), sleep for some time, 
// then wait till reg 128 returns 0x8000
/*
            adc_mode = dtcbuffer[0];                                  // -a
            tdc_mode = dtcbuffer[1];                                  // -t
            num_lookback = dtcbuffer[2];                              // -l
            num_triggers = (dtcbuffer[4] << 16) + dtcbuffer[3];         // -T
            channel_mask[0] = (dtcbuffer[6] << 16) + dtcbuffer[5];    // -C
            channel_mask[1] = (dtcbuffer[8] << 16) + dtcbuffer[7];    // -D
            channel_mask[2] = (dtcbuffer[10] << 16) + dtcbuffer[9];    // -E
            num_samples = dtcbuffer[11];                     // -s
            enable_pulser = (uint8_t) dtcbuffer[12];         // -p
            max_total_delay = dtcbuffer[13];                 // -d (def 1)
            marker_clock = (uint8_t) dtcbuffer[14];          // -m
*/    
//-----------------------------------------------------------------------------
//    const int  reg (265);  // for control_ROC.py(read)
    std::vector<uint16_t> vec;
    
    TLOG(TLVL_DEBUG) << "Link: 0x" << std::hex << Link << std::dec << " PrintLevel:" << PrintLevel;

    if (Par == nullptr) {
                                        // reasonable defaults, to run w/o passing anything
      uint16_t adc_mode(0), tdc_mode(0), num_lookback(0), num_samples(1);
      uint16_t num_triggers[2] = {10,0};
      uint16_t ch_mask     [6] = {0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};
      uint16_t enable_pulser(0), marker_clock(3), mode(0), clock(99);
        
      vec.push_back(adc_mode);
      vec.push_back(tdc_mode);
      vec.push_back(num_lookback);
      vec.push_back(num_samples);
      vec.push_back(num_triggers[0]);
      vec.push_back(num_triggers[1]);
      for (int i=0; i<6; i++) vec.push_back(ch_mask[i]);
      vec.push_back(enable_pulser);
      vec.push_back(marker_clock);
      vec.push_back(mode);
      vec.push_back(clock);
    }
    else {
      vec.push_back(Par->adc_mode);
      vec.push_back(Par->tdc_mode);
      vec.push_back(Par->num_lookback);
    
      if (Par->num_samples > 63) {
        TLOG(TLVL_WARNING) << "num_samples:" << Par->num_samples << " gt 63, truncate to 63" ;
        Par->num_samples = 63;
      }

      vec.push_back(Par->num_samples);
      
      uint16_t w1 = Par->num_triggers[0];
      uint16_t w2 = Par->num_triggers[1];
      
      vec.push_back(w1);
      vec.push_back(w2);
      
      for (int i=0; i<6; i++) vec.push_back(Par->ch_mask[i]); 
      
      vec.push_back(Par->enable_pulser);
      vec.push_back(Par->marker_clock );
      vec.push_back(Par->mode  );
      vec.push_back(Par->clock );
    }
      
    bool increment_address(false);
//-----------------------------------------------------------------------------
// if LinkMask != -1, use it
// in addition, if UpdateMask=true, update the DTC link mask (fLinkMask)
//-----------------------------------------------------------------------------
    int link_mask = fLinkMask;
    if (Link != -1) {
      link_mask = (1 << 4*Link);
    }
    
    for (int i=0; i<6; i++) {
      int used = (link_mask >> 4*i) & 0x1;
      if (not used)                                         continue;
      auto roc  = DTC_Link_ID(i);
      fDtc->WriteROCBlock   (roc,REG_READ,vec,false,increment_address,100);
      std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
      
                                        // 0x86 = 0x82 + 4
      uint16_t u; 
      while ((u = fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
      TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
      int nw = fDtc->ReadROCRegister(roc,129,100);
      TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",129,nw);
      
      nw = nw-4;
      std::vector<uint16_t> vout;
      fDtc->ReadROCBlock(vout,roc,REG_READ,nw,false,100);

      if (PrintLevel > 0) {
        Stream << "--------------- link :" << i << std::endl;
        if (PrintLevel & 0x1) {
          PrintBuffer(vout.data(),nw,&Stream);
        }
      
        if (PrintLevel & 0x2) {
          trkdaq::ControlRoc_Read_Output_t0* o = (trkdaq::ControlRoc_Read_Output_t0*) vout.data();
          Stream << Form("adc_mode      : %i\n",o->adc_mode);
          Stream << Form("tdc_mode      : %i\n",o->tdc_mode);
          Stream << Form("num_lookback  : %i\n",o->num_lookback);
          Stream << Form("num_triggers  : %5i %5i\n",o->num_triggers[0],o->num_triggers[1]);
          Stream << Form("ch_mask       : 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x\n",
                         o->ch_mask[0],o->ch_mask[1],o->ch_mask[2],
                         o->ch_mask[3],o->ch_mask[4],o->ch_mask[5]);
          Stream << Form("num_samples   : %i\n"     ,o->num_samples);
          Stream << Form("enable_pulser : %i\n"    ,o->enable_pulser);
          Stream << Form("marker_clock  : %i\n"    ,o->marker_clock);
          Stream << Form("mode          : %i\n"    ,o->mode);
          Stream << Form("clock         : %i\n"    ,o->clock);
          Stream << Form("digi_read_0xb : 0x%04x\n",o->digi_read_0xb);
          Stream << Form("digi_read_0xe : 0x%04x\n",o->digi_read_0xe);
          Stream << Form("digi_read_0xd : 0x%04x\n",o->digi_read_0xd);
          Stream << Form("digi_read_0xc : 0x%04x\n",o->digi_read_0xc);
        }
      }
    }
//-----------------------------------------------------------------------------
//  is it really needed to reser the ROC in the end ? - no
//-----------------------------------------------------------------------------
    // ResetLinks();
    return 0;
  }
  
//-----------------------------------------------------------------------------  
  int DtcInterface::ControlRoc_SetGain(int Link, int ChannelID, int PreampType, int Gain) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
    auto roc  = DTC_Link_ID(Link);
//-----------------------------------------------------------------------------
// write parameters into reg 266 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
    std::vector<uint16_t> vec;
    vec.push_back(uint16_t(ChannelID ));
    vec.push_back(uint16_t(Gain      ));
    vec.push_back(uint16_t(PreampType));

    bool increment_address(false);
    fDtc->WriteROCBlock   (roc,REG_SETGAIN,vec,false,increment_address,100);
    std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

                                        // 0x86 = 0x82 + 4
    uint16_t u; 
    while ((u = fDtc->ReadROCRegister(roc,128,100)) != 0x8000) {}; 
    TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
    int nw = fDtc->ReadROCRegister(roc,129,100);
    TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",129,nw);

    nw = nw-4;
    std::vector<uint16_t> v2;
    fDtc->ReadROCBlock(v2,roc,REG_SETGAIN,nw,false,100);

    PrintBuffer(v2.data(),nw);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    // ResetLink(Link);
    return 0;
  }


//-----------------------------------------------------------------------------
// from https://github.com/bonventre/trackerScripts/blob/master/control_ROC.py
// chan_mask = int(get_key_value(keys,"C"),16) 
// oddoreven = int(get_key_value(keys,"P"),16)
// channel   = int(get_key_value(keys,"c",-1)) - not used any more !!
// delay     = int(get_key_value(keys,"d",1000))
// dutycycle = int(get_key_value(keys,"y",10))
// FirstChanelMask : a bit mask, defines the first pulsd channel, the rest pulsed: first+8*i
//                   0x10 : first pulsed channel is channel 4, max value : 0x80
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_PulserOn(int Link, int FirstChannelMask, int DutyCycle, int PulserDelay,
                                        int PrintLevel, std::ostream& Stream) {
    int rc (0), reg(268);
    TLOG(TLVL_DEBUG) << "Link:" << Link << " FirstChannelMask:0x" << std::hex << FirstChannelMask
                     << std::dec << " DutyCycle:" << DutyCycle << " PulserDelay:" << PulserDelay;

    Stream << "Link:" << Link << " FirstChannelMask:0x" << std::hex << FirstChannelMask
           << std::dec << " DutyCycle:" << DutyCycle << " PulserDelay:" << PulserDelay;
//-----------------------------------------------------------------------------
// write parameters into reg 268 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
    std::vector<uint16_t> vec;
    vec.push_back(uint16_t(FirstChannelMask));
    vec.push_back(uint16_t(DutyCycle));
    vec.push_back(uint16_t((PulserDelay >>  0) & 0xffff));
    vec.push_back(uint16_t((PulserDelay >> 16) & 0xffff));
    
    bool increment_address(false);
//-----------------------------------------------------------------------------
// Link = -1 means all links
//-----------------------------------------------------------------------------
    int l1(Link), l2(Link+1);
    if (Link == -1) {
      l1 = 0;
      l2 = 6;
    }
    
    for (int i=l1; i<l2; i++) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
      auto roc  = DTC_Link_ID(i);
      fDtc->WriteROCBlock   (roc,REG_PULSERON,vec,false,increment_address,100);
      std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

                                        // 0x86 = 0x82 + 4
      uint16_t u; 
      while ((u = fDtc->ReadROCRegister(roc,128,100)) != 0x8000) {}; 
      TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
      int nw = fDtc->ReadROCRegister(roc,129,100);
      TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",129,nw);

      nw = nw-4;
      if (nw == 4) {
        std::vector<uint16_t> v2;
        fDtc->ReadROCBlock(v2,roc,REG_PULSERON,nw,false,100);

        if (PrintLevel & 0x1) {
          PrintBuffer(v2.data(),nw,&Stream);
        }
      }
      else {
        TLOG(TLVL_ERROR) << Form("wrong number of words : nw = %i, expected = 4",nw);
        rc = -1;
        break;
      }
    }

    return rc;
  }


//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_PulserOff(int Link, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    
    int l1(Link), l2(Link+1);
    if (Link == -1) {
      l1 = 0;
      l2 = 6;
    }
    
    for (int i=l1; i<l2; i++) {
//-----------------------------------------------------------------------------
// PULSER_OFF: reg 269
//-----------------------------------------------------------------------------
      std::vector<uint16_t> res;
      RocBlockRead(i,REG_PULSEROFF,res);

      int nw = res.size();
      TLOG(TLVL_DEBUG) << "link:" << i << " nw:" << nw; 

      if (PrintLevel & 0x1) {
        PrintBuffer(res.data(),nw,&Stream);
      }
    }
    return rc;
  }
  

//-----------------------------------------------------------------------------  
  int DtcInterface::ControlRoc_SetThreshold(int Link, int ChannelID, int PreampType, int Threshold) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
    auto roc  = DTC_Link_ID(Link);
//-----------------------------------------------------------------------------
// write parameters into reg 267 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
    std::vector<uint16_t> vec;
    vec.push_back(uint16_t(ChannelID));
    vec.push_back(uint16_t(Threshold));
    vec.push_back(uint16_t(PreampType));

    bool increment_address(false);
    fDtc->WriteROCBlock   (roc,REG_SET_THR,vec,false,increment_address,100);
    std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

                                        // 0x86 = 0x82 + 4
    uint16_t u; 
    while ((u = fDtc->ReadROCRegister(roc,128,100)) != 0x8000) {}; 
    TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
    int nw = fDtc->ReadROCRegister(roc,129,100);
    TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",129,nw);

    nw = nw-4;
    std::vector<uint16_t> v2;
    fDtc->ReadROCBlock(v2,roc,REG_SET_THR,nw,false,100);

    PrintBuffer(v2.data(),nw);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    // ResetLink(Link);
    return 0;
  }


//-----------------------------------------------------------------------------
// Link: link number
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_MeasureThresholds(int Link, int PrintLevel, std::ostream& Stream,
                                                 uint32_t MaskC, uint32_t MaskD, uint32_t MaskE) {
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
    auto roc  = DTC_Link_ID(Link);

    // int roc_mask = 1 << (4*Link);
//-----------------------------------------------------------------------------
// write parameters into reg 264 (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
//-----------------------------------------------------------------------------
    if (PrintLevel) Stream << std::format("--------- link:{:d} thresholds ----------------",Link) << std::endl;

    std::vector<uint16_t> vec;

    vec.push_back((MaskC      ) & 0xffff);
    vec.push_back((MaskC >> 16) & 0xffff);
    vec.push_back((MaskD      ) & 0xffff);
    vec.push_back((MaskD >> 16) & 0xffff);
    vec.push_back((MaskE      ) & 0xffff);
    vec.push_back((MaskE >> 16) & 0xffff);

    bool increment_address(false);
    fDtc->WriteROCBlock   (roc,REG_MEAS_THR,vec,false,increment_address,100);
    std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

    // 0x86 = 0x82 + 4
    uint16_t u; 
    while ((u = fDtc->ReadROCRegister(roc,128,100)) != 0x8000) {}; 
    if (PrintLevel & 0x1) {
      Stream << std::format("reg:{:03d} val:0x{:04x}",128,u) << std::endl;
    }
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
    int nw = fDtc->ReadROCRegister(roc,129,100);
    if (PrintLevel & 0x1) Stream << std::format("reg:{:03d} val:0x{:04x}",129,nw) << std::endl;

    nw = nw-4;
    std::vector<uint16_t> v2;
    fDtc->ReadROCBlock(v2,roc,REG_MEAS_THR,nw,false,100);
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    // fDtc->WriteROCRegister(roc,14,0x01,false,1000); 

    if (PrintLevel & 0x1) PrintBuffer(v2.data(),nw,&Stream);
    // expect nw=288 = 96*3, if not - in trouble

    if (PrintLevel & 0x2) {
      int mask[3];
      mask[0] = MaskC;
      mask[1] = MaskD;
      mask[2] = MaskE;
//-----------------------------------------------------------------------------
// print thresholds only for the channels defined by the mask
//-----------------------------------------------------------------------------
      printf(" chID     thr(CAL)    thr(HV)    sum\n");
      printf("--------------------------------------\n");
      for (int i=0; i<96; i++) {
        int iw = i/32;
        int ib = i -iw*32;

        if (((mask[iw] >> ib) & 0x1) == 1) {
        
          float hw  = (-1000. + v2[    i]*2000./1024.)/10.;
          float cal = (-1000. + v2[96 +i]*2000./1024.)/10.;
          float tot = (-1000. + v2[192+i]*2000./1024.)/10.;

          Stream << std::format(" {:4d} {:10.3f} {:10.3f} {:10.3f}",i,hw,cal,tot) << std::endl;
        }
      }
    }
    return 0;
  }
  

//-----------------------------------------------------------------------------
  int DtcInterface::ConvertSpiData(const std::vector<uint16_t>& Data, TrkSpiData_t* Spi, int PrintLevel, std::ostream& Stream) {
    const char* keys[] = {
      "I3.3","I2.5","I1.8HV","IHV5.0","VDMBHV5.0","V1.8HV","V3.3HV" ,"V2.5"    , 
      "A0"  ,"A1"  ,"A2"    ,"A3"    ,"I1.8CAL"  ,"I1.2"  ,"ICAL5.0","ADCSPARE",
      "V3.3","VCAL5.0","V1.8CAL","V1.0","ROCPCBTEMP","HVPCBTEMP","CALPCBTEMP","RTD",
      "ROC_RAIL_1V(mV)","ROC_RAIL_1.8V(mV)","ROC_RAIL_2.5V(mV)","ROC_TEMP(CELSIUS)",
      "CAL_RAIL_1V(mV)","CAL_RAIL_1.8V(mV)","CAL_RAIL_2.5V(mV)","CAL_TEMP(CELSIUS)",
      "HV_RAIL_1V(mV)","HV_RAIL_1.8V(mV)","HV_RAIL_2.5V(mV)","HV_TEMP(CELSIUS)"
    };
//-----------------------------------------------------------------------------
// primary source : https://github.com/bonventre/trackerScripts/blob/master/constants.py#L99
//-----------------------------------------------------------------------------
    struct constants_t {
      float iconst  = 3.3 /(4096*0.006*20);
      float iconst5 = 3.25/(4096*0.500*20);
      float iconst1 = 3.25/(4096*0.005*20);
      float toffset = 0.509;
      float tslope  = 0.00645;
      float tconst  = 0.000806;
      float tlm45   = 0.080566;  
    } constants;

    int nw = Data.size();
    
    float* val = (float*) Spi;

    for (int i=0; i<nw; i++) {
      if (i==20 or i==21 or i==22) {
        val[i] = Data[i]*constants.tlm45;
      }
      else if (i==0 or i==1 or i==2 or i==12 or i==13) {
        val[i] = Data[i]*constants.iconst;
      }
      else if (i==3 or i==14) {
        val[i] = Data[i]*constants.iconst5 ;
      }
      else if (i==4 or i==5 or i==6 or i==7 or i==16 or i==17 or i==18 or i==19) {
        val[i] = Data[i]*3.3*2/4096 ; 
      }
      else if (i==15) {
        val[i] = Data[i]*3.3/4096;
      }
      else if (i==23) {
        val[i] = Data[i]*3.3/4096;
      }
      else if (i==8 or i==9 or i==10 or i==11) {
        val[i] = Data[i];
      }
      else if (i > 23) {
        if   ((i%4) < 3) val[i] = Data[i]/8.;
        else             val[i] = Data[i]/16.-273.15;
      }
      
      if (PrintLevel > 0) {
        Stream << Form("%-20s : %10.3f\n",keys[i],val[i]);
      }
    }

    return 0;
  }

//-----------------------------------------------------------------------------
// read SPI, return vector of short's, optionally print
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_ReadSpi(std::vector<uint16_t>& SpiRawData, int Link, int PrintLevel, std::ostream& Stream) {
//-----------------------------------------------------------------------------
// ReadSPI: reg 258
//-----------------------------------------------------------------------------
    int rc = RocBlockRead(Link,REG_READSPI,SpiRawData);

    int nw = SpiRawData.size();

    if (nw != TrkSpiDataNWords) {
      TLOG(TLVL_ERROR) << "expected N(words)=" << TrkSpiDataNWords << " , reported nw=" << nw;
      rc = -1;
    }
//-----------------------------------------------------------------------------
// PrintLevel bit 0: print SPI data in hex 
//-----------------------------------------------------------------------------
    if ((PrintLevel & 0x1) != 0) {
      PrintBuffer(SpiRawData.data(),nw,&Stream);
    }
//-----------------------------------------------------------------------------
// PrintLevel bit 1: parse SPI data and print them
//-----------------------------------------------------------------------------
    if ((rc == 0) and (PrintLevel & 0x2) != 0) {
      struct TrkSpiData_t spi;
      ConvertSpiData(SpiRawData,&spi,PrintLevel,Stream);  // &spi[0]
    }

    return rc;
  }
  
//-----------------------------------------------------------------------------
// read SPI, convert into floats
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_ReadSpi_1(TrkSpiData_t* Spi, int Link, int PrintLevel, std::ostream& Stream) {
    int rc(0);
//-----------------------------------------------------------------------------
// ReadSPI: reg 258
//-----------------------------------------------------------------------------
    std::vector<uint16_t> data;
    rc = RocBlockRead(Link,REG_READSPI,data,TrkSpiDataNWords);
//-----------------------------------------------------------------------------
// PrintLevel bit 0: print SPI data in hex
//-----------------------------------------------------------------------------
    if ((PrintLevel & 0x1) != 0) {
      int nw = data.size();
      PrintBuffer(data.data(),nw,&Stream);
    }
//-----------------------------------------------------------------------------
// do not perform conversion, if wrong number of words
// PrintLevel bit 1: parse SPI data and print them
//-----------------------------------------------------------------------------
    if (rc == 0) {
      ConvertSpiData(data,Spi,PrintLevel,Stream);  // &spi[0]
    }

    return rc;
  }
  

//-----------------------------------------------------------------------------
// pay with performance for compactness
// if Link = -1, a git commit for last enabled link is returned
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_ReadGitCommit(std::string& GitCommit, int Link, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    int nw_expected(40);
//-----------------------------------------------------------------------------
// ReadGitCommit: reg 
//-----------------------------------------------------------------------------
    std::vector<uint16_t> data;

    int link_mask = (Link == -1) ? fLinkMask : (1 << 4*Link) ;

    for (int i=0; i<6; i++) {
      int link_enabled = (link_mask >> 4*i) & 0x1;
      // Stream << "link:" << i << " link_enabled:" << link_enabled << std::endl;
      if (link_enabled) {
        int rc = RocBlockRead(i,REG_READGITCOMMIT,data,nw_expected);
        if (rc < 0) {
          GitCommit = "READ_ERROR";
        }
        else {
          std::stringstream ss;
          int nw = data.size();
          if (PrintLevel & 0x1) PrintBuffer(data.data(),nw,&Stream);
          
          for (int iw=0; iw<nw; iw++) ss << std::format("{:c}",data[iw]);
          GitCommit = ss.str();
        }

        if (PrintLevel & 0x2) Stream << std::format("GitCommit:{}\n",GitCommit);
      }
    }
    
    return rc;
  }

//-----------------------------------------------------------------------------
// if Link = -1, interested in printing
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_ReadIlp(std::vector<uint16_t>& Data, int Link, int PrintLevel, std::ostream& Stream) {
    int rc(0);
//-----------------------------------------------------------------------------

    int link_mask = (Link == -1) ? fLinkMask : (1 << 4*Link) ;

    if (Link == -1) {
      for (int i=0; 0<6; i++) {
        int link_enabled = (link_mask >> 4*i) & 0x1;
        if (link_enabled) {
          RocBlockRead(i,REG_READILP,Data);
          int nw = Data.size();
          if (PrintLevel & 0x1) PrintBuffer(Data.data(),nw,&Stream);
        }
      }
    }
    else {
      RocBlockRead(Link,REG_READILP,Data);
      int nw = Data.size();
      if (PrintLevel & 0x1) PrintBuffer(Data.data(),nw,&Stream);
    }
    
    return rc;
  }

//-----------------------------------------------------------------------------
// no data conversion
//-----------------------------------------------------------------------------
  int DtcInterface::ControlRoc_GetKey (std::vector<uint16_t>& Data, int Link, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    
    int link_mask = (Link == -1) ? fLinkMask : (1 << 4*Link) ;

    if (Link == -1) {
      for (int i=0; 0<6; i++) {
        int link_enabled = (link_mask >> 4*i) & 0x1;
        if (link_enabled) {
          RocBlockRead(i,REG_GETKEY,Data);
          int nw = Data.size();
          if (PrintLevel & 0x1) PrintBuffer(Data.data(),nw,&Stream);
        }
      }
    }
    else {
      RocBlockRead(Link,REG_GETKEY,Data);
      int nw = Data.size();
      if (PrintLevel & 0x1) PrintBuffer(Data.data(),nw,&Stream);
    }

    return rc;
  }

//-----------------------------------------------------------------------------
// at this point, assume just one Link. If needed, make it more general (a mask) later
//-----------------------------------------------------------------------------
  int  DtcInterface::ControlRoc_Rates(int Link, int PrintLevel, ControlRoc_Rates_t* Par,
                                      std::ostream& Stream) {
    int rc(0);

    ControlRoc_Rates_t par;
    if (Par != nullptr) {
      par = *Par;
    }

    TLOG(TLVL_DEBUG) << "par:{" << par.num_lookback << ","
                     << par.num_samples << ","
                     << std::hex
                     << "0x" << par.chan_mask[0] << ","
                     << "0x" << par.chan_mask[1] << ","
                     << "0x" << par.chan_mask[2] << ","
                     << "0x" << par.chan_mask[3] << ","
                     << "0x" << par.chan_mask[4] << ","
                     << "0x" << par.chan_mask[5] << "}";

    int i1(Link), i2(Link+1);
    if (Link == -1) {
      i1 = 0;
      i2 = 6;
    }

    for (int i=i1; i<i2; i++) {
      auto roc  = DTC_Link_ID(i);
//-----------------------------------------------------------------------------
// write parameters into reg ***  (block write) , sleep for some time, 
// then wait till reg 128 returns 0x8000
// ch_mask always includes the first channel
//-----------------------------------------------------------------------------
      std::vector<uint16_t> vec;
      
      vec.push_back(par.num_lookback);
      vec.push_back(par.num_samples );

      for (int i=0; i<6; i++) {
        vec.push_back(par.chan_mask[i]);
      }
  
      fDtc->WriteROCBlock   (roc,REG_READRATES,vec,false,false,1000);
      std::this_thread::sleep_for(std::chrono::microseconds(1000));

      // 0x86 = 0x82 + 4
      uint16_t u; 
      while ((u = fDtc->ReadROCRegister(roc,128,5000)) != 0x8000) {}; 
      printf("reg:%03i val:0x%04x\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
      int nw = fDtc->ReadROCRegister(roc,129,100); printf("reg:%03i val:0x%04x\n",129,nw);

      nw = nw-4;
      std::vector<uint16_t> v2;
      fDtc->ReadROCBlock(v2,roc,REG_READRATES,nw,false,100);
//-----------------------------------------------------------------------------
// print output - in two formats
//-----------------------------------------------------------------------------
      if (PrintLevel & 0x1) {
        PrintBuffer(v2.data(),nw,&Stream);
      }

      if (PrintLevel & 0x2) {
//-----------------------------------------------------------------------------
// formatted printout
// should be 96*3*2+2*2 = 580 16-bit words
// 3 words per channel (straw)
//-----------------------------------------------------------------------------
        if (nw != 580) {
          printf("ERROR: nw = %5i != 580. BAIL OUT\n",nw);
          return -1;
        }
//-----------------------------------------------------------------------------
// finally, the last two words - total counts
//-----------------------------------------------------------------------------
        float total[2];          // [0]:CAL  [1]:HV , as in lanes, an inversion takes place
        float clock_tick(5.e-9); // 5 ns <-> 200 MHz clock

        int loc = 576;
      
        total[1]  = float(v2[loc  ])+(int(v2[loc+1]) << 16); // hv - check the order with Vadim
        total[0]  = float(v2[loc+2])+(int(v2[loc+3]) << 16); // cal

        Stream << " channel  Total(HV) Total(CAL) Total(HV.and.CAL)  Rate(HV)   Rate(CAL)   Rate(HV.and.CAL)\n";
        Stream << "-----------------------------------------------------------------------------------------\n";

        for (int ich=0; ich<96; ich++) {
          loc               = 6*ich;
          int   counts_hv   = int(v2[loc  ])+(int(v2[loc+1]) << 16);
          int   counts_cal  = int(v2[loc+2])+(int(v2[loc+3]) << 16);
          int   counts_coin = int(v2[loc+4])+(int(v2[loc+5]) << 16);
          int   fpga        = fgFpga[ich];
          float rate_hv     = counts_hv /total[fpga]/clock_tick/1000.;
          float rate_cal    = counts_cal/total[fpga]/clock_tick/1000.;
          float rate_coin   = counts_coin/(total[0]+total[1])*2/clock_tick/1000.;
          Stream << std::format(" {:5d} {:10d} {:10d} {:10d}         {:10.3f} {:10.3f} {:10.3f} \n",
                                ich,counts_hv,counts_cal,counts_coin,
                                rate_hv,rate_cal,rate_coin);
        }

        Stream << std::format(" total_hv: {:10.0f} total_cal: {:10.0f}\n",total[1],total[0]);
      }
    }
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
    return rc;
  }

//-----------------------------------------------------------------------------
  int  DtcInterface::ControlRoc_ReadDeviceID(int                    Link      ,
                                             ControlRoc_DeviceID_t& DevId     ,
                                             int                    PrintLevel,
                                             std::ostream&          Stream    ) {

    std::vector<uint16_t> dat = ReadDeviceID(DTC_Link_ID(Link));

    std::stringstream ss;
    // first 16 bytes are the serial number
    for (int i = 15 ; i >= 0 ; i--) ss << std::format("{:02x}",dat[i]);

    DevId.DeviceSerial = "'"+ss.str()+"'";

    ss.str({});
    ss.clear();
    //    ss << "0x";
    // next 32 bytes are the designInfo
    for (int i = 47 ; i >= 16 ; i--) ss << std::format("{:02x}",dat[i]);
    DevId.DesignInfo = "'"+ss.str()+"'";

    ss.str({});
    ss.clear();
    // ss << "0x";
    // next 2 bytes are the design version
    for (int i = 49 ; i >= 48 ; i--) ss << std::format("{:02x}",dat[i]);
    DevId.DesignVer = "'"+ss.str()+"'";

    ss.str({});
    ss.clear();
    // ss << "0x";
    // next 2 bytes are the backlevel vesrsion, whatever it is
    for (int i = 51 ; i >= 50 ; i--) ss << std::format("{:02x}",dat[i]);
    DevId.BackLevelVer = "'"+ss.str()+"'";


    if (PrintLevel & 0x1) {
      Stream << std::format("BackLevelVer:{}\n",DevId.BackLevelVer); 
      Stream << std::format("DesignInfo  :{}\n",DevId.DesignInfo); 
      Stream << std::format("DesignVer   :{}\n",DevId.DesignVer); 
      Stream << std::format("DeviceSerial:{}\n",DevId.DeviceSerial); 
    }

    return 0;
  }
};
