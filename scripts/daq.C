//
#define __CLING__ 1

#ifndef DAQ_C
#define DAQ_C

#include "iostream"
#include "nlohmann/json.hpp"

#include "TH1.h"
#include "TStopwatch.h"

#include "dtcInterfaceLib/DTC.h"
#include "cfoInterfaceLib/CFO.h"
#include "cfoInterfaceLib/CFO_Compiler.hh"

using namespace CFOLib;
using namespace DTCLib;

#include "print_buffer.C"

#include "otsdaq-mu2e-tracker/Ui/CfoInterface.hh"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"

using namespace trkdaq;

// DTC control register : 0x9100 
enum { 
  kCFOEmulationEnableBit = 30,
  kAutogenDRPBit         = 23,
};

namespace daq_scripts {
  int EWLength    = 68;                 // in units of 25 ns, = 1700 ns
  int EdgeMode    = 0x0;                // two bits
  int CFOLinkMask = 0x1;
};

//-----------------------------------------------------------------------------
void cfo_measure_delay(int PcieAddress, CFO_Link_ID xLink) {

  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress); 
  if (cfo_i == nullptr) return;

  // comment out , for now 
  // CFO* cfo = cfo_i->Cfo();

	// cfo->ResetDelayRegister();	                 // reset 0x9380
	// cfo->DisableLinks();	                       // reset 0x9114
	//                                              // configure the DTC (to configure the ROC in a loop)
	// cfo->EnableLink(xLink, DTC_LinkEnableMode(true, true)); // enable Tx and Rx
	// cfo->EnableDelayMeasureMode(xLink);
	// cfo->EnableDelayMeasureNow(xLink);

	// uint32_t delay = cfo->ReadCableDelayValue(xLink);	// read delay

	// cout << "Delay measured: " << delay << " (ns) on link: " <<  xLink << std::endl;

	// // reset registers
	// cfo->ResetDelayRegister();
	// cfo->DisableLinks();

  // 	printf(" delay = %ui\n",delay);

}

//-----------------------------------------------------------------------------
// ROOT CLI
//-----------------------------------------------------------------------------
int cfo_halt(int PcieAddress = -1) {
  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress);
  int rc = cfo_i->Halt();
  return rc;
}

void cfo_soft_reset(int PcieAddress = -1) {
  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress); 
  cfo_i->Cfo()->SoftReset();
}

//-----------------------------------------------------------------------------
void cfo_compile_run_plan(const char* InputFn, const char* OutputFn) {
  CFOLib::CFO_Compiler compiler;

  std::string fn1(InputFn );
  std::string fn2(OutputFn);

  std::string ret = compiler.processFile(fn1,fn2);
  std::cout << std::format("compilation finished with ret:{}",ret);
}

//-----------------------------------------------------------------------------
// ROOT CLI: create CFO handle
//-----------------------------------------------------------------------------
trkdaq::CfoInterface* cfo_init(const char* ConfigName, int DeviceID=0) {
  mu2edaq::DtcInputData_t dat;
  
  mu2edaq::DtcInterface::InitConfiguration(ConfigName,DeviceID,&dat);
  
  trkdaq::CfoInterface* cfo_i = trkdaq::CfoInterface::Instance(dat.fPcieAddr);
   if (cfo_i) {
     cfo_i->fPcieAddr    = dat.fPcieAddr;
     cfo_i->fLinkMask    = dat.fLinkMask;   // link mask here is the time chain mask
     cfo_i->fEventMode   = dat.fEventMode;
  
     cfo_i->SetJAMode(dat.fJAMode);
   }
  return cfo_i;
}

//-----------------------------------------------------------------------------
// assume one timing chain
//-----------------------------------------------------------------------------
int cfo_init_readout_ext(const char* RunPlan, uint DtcMask) {
  int rc(0);
  CfoInterface* cfo_i = CfoInterface::Instance();  // assume already initialized
  rc = cfo_i->InitReadout(RunPlan,DtcMask);
  return rc;
}

//-----------------------------------------------------------------------------
void cfo_launch_run_plan(int PcieAddress = -1) {
  CfoInterface::Instance(PcieAddress)->LaunchRunPlan(); 
}

//-----------------------------------------------------------------------------
// default: 1700 ns
//-----------------------------------------------------------------------------
void cfo_set_event_window(int EWLength = 68) {
  daq_scripts::EWLength = EWLength;
}
//-----------------------------------------------------------------------------
// first 8 bytes contain nbytes, but written into the CFO are 0x10000 bytes
// (the sizeof(mu2e_databuff_t) 0
//-----------------------------------------------------------------------------
void cfo_set_run_plan(const char* Fn = "commands.bin", int PcieAddress = -1) {

  CfoInterface* cfo_i = CfoInterface::Instance(PcieAddress);
  cfo_i->SetRunPlan(Fn);
}

//-----------------------------------------------------------------------------
// doesn't seem to be needed - launch does this anyway
//-----------------------------------------------------------------------------
void cfo_reset_run_plan(int PcieAddress = -1) {
  CFO* cfo = CfoInterface::Instance(PcieAddress)->Cfo(); 

  cfo->DisableBeamOnMode (CFO_Link_ID::CFO_Link_ALL);
  cfo->DisableBeamOffMode(CFO_Link_ID::CFO_Link_ALL);
  cfo->SoftReset();
}

//-----------------------------------------------------------------------------
// for convenience (CLI)
//-----------------------------------------------------------------------------
int cfo_configure_ja(int Clock, int Reset, int PcieAddress = -1) {
  return CfoInterface::Instance(PcieAddress)->ConfigureJA(Clock,Reset);
}

//-----------------------------------------------------------------------------
int dtc_configure_ja(int Clock, int Reset, int PcieAddress = -1) {
  return DtcInterface::Instance(PcieAddress)->ConfigureJA(Clock,Reset);
}


//-----------------------------------------------------------------------------
// test of the 'READ' command implementation over the fiber
// if LinkMask != -1, operate on the specified links only
//-----------------------------------------------------------------------------
int dtc_control_roc_find_alignment(int Link = -1, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  int n_bitslips(0);
  int rc = dtc_i->FindAlignments(Link,n_bitslips,2);
  return rc;
}

//-----------------------------------------------------------------------------
// test of the 'READ' command implementation over the fiber
// if LinkMask != -1, operate on the specified links only
//-----------------------------------------------------------------------------
int dtc_control_roc_measure_thresholds(int Link, int PrintLevel = 2, int PcieAddr = -1) {
  uint32_t mask[3] = {0xffffffff,0xffffffff,0xffffffff};
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ControlRoc_MeasureThresholds(Link,mask[0],mask[1],mask[2],PrintLevel,std::cout);
  return 0;
}

//-----------------------------------------------------------------------------
// test of the 'READ' command implementation over the fiber
// if Link != -1, Link is a single link number (0-5)
//-----------------------------------------------------------------------------
int dtc_control_roc_digi_rw(int      Address          ,
                            int      Rw               , // 0:read, 1:write
                            int      HvCal            , // 0: both, 1:hv 2:cal 3: ???
                            int      Data             ,
                            int      Link         = -1,
                            int      PcieAddr     = -1) {
  int rc(0);
  
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

  ControlRoc_DigiRW_Input_t  ip;
  ControlRoc_DigiRW_Output_t op;
  
  ip.rw        = Rw;                    // -a
  ip.hvcal     = HvCal;                 // -t 
  ip.address   = Address;               // -t 
  ip.data[0]   = (Data >>  0) & 0xFFFF;
  ip.data[1]   = (Data >> 16) & 0xFFFF;
  
  //  printf("dtc_i->fLinkMask: 0x%04x\n",dtc_i->fLinkMask);

  int print_level(2);
  // std::ostream null(nullptr);
  rc = dtc_i->ControlRoc_DigiRW(&ip,&op,Link,print_level); // ,null);
  
  // std::cout << std::format("link:{} rw:{} hvcal:{} address:0x{:04x}",Link,op.rw,op.hvcal,op.address)
  //           << std::format(" data[0]:0x{:04x} data[1]:0x{:04x} adc_num:0x{:04x} adc_mask:0x{:04x}\n",
  //                          op.data[0],op.data[1],op.adc_num,op.adc_mask);
  return rc;
}

//-----------------------------------------------------------------------------
// test of the 'READ' command implementation over the fiber
// Link is either a link number, of -1, in which case operate on all enabled DTC links
//-----------------------------------------------------------------------------
int dtc_control_roc_read(int      Link         = -1,
                         int      AdcMode      = 0,
                         int      TdcMode      = 0,
                         int      NumLookback  = 0,
                         int      EnablePulser = 0, 
                         int      MarkerClock  = 3, 
                         int      NumSamples   = 1,
                         uint32_t MaskC        = 0xFFFFFFFF,
                         uint32_t MaskD        = 0xFFFFFFFF,
                         uint32_t MaskE        = 0xFFFFFFFF,
                         int      PcieAddr     = -1) {
  
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

  ControlRoc_Read_Input_t0 par;
  
  par.adc_mode        = AdcMode;        // -a
  par.tdc_mode        = TdcMode;        // -t 
  par.num_lookback    = NumLookback;    //  

  par.num_samples     = NumSamples;     // -s
  par.num_triggers[0] = 10;             // -T 10
  par.num_triggers[1] = 0;              // -T (high bytes)
  
  par.ch_mask[0]      = (MaskC >>  0) & 0xffff;
  par.ch_mask[1]      = (MaskC >> 16) & 0xffff;
  par.ch_mask[2]      = (MaskD >>  0) & 0xffff;
  par.ch_mask[3]      = (MaskD >> 16) & 0xffff;
  par.ch_mask[4]      = (MaskE >>  0) & 0xffff;
  par.ch_mask[5]      = (MaskE >> 16) & 0xffff;

  par.enable_pulser   = EnablePulser;   // -p 1
  par.marker_clock    = MarkerClock;    // -m 3
  par.mode            = 0;              // 
  par.clock           = 99;             //

  printf("dtc_i->fLinkMask: 0x%04x\n",dtc_i->fLinkMask);
  int  print_level(3);
  
  dtc_i->ControlRoc_Read(&par,Link,print_level);
  return 0;
}

//-----------------------------------------------------------------------------
// just print the ROC information 
//-----------------------------------------------------------------------------
int dtc_control_roc_rates(int Link, trkdaq::ControlRoc_Rates_t* Par = nullptr, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

  std::vector<uint16_t> rates;
  dtc_i->ControlRoc_Rates(Link,&rates,4,Par,&std::cout);

  std::vector<int>      chmask;
  for (int i=0; i<96; ++i) chmask.emplace_back(1);
  dtc_i->PrintRatesSingleRoc(&rates,&chmask,std::cout);

  return 0;
}

//-----------------------------------------------------------------------------
// just print the ROC information 
//-----------------------------------------------------------------------------
int dtc_control_roc_dump_settings(int Link, int Channel, int PrintLevel = 0xf, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

  int rc = dtc_i->ControlRoc_DumpSettings(Link,Channel,PrintLevel);

  return rc;
}

//-----------------------------------------------------------------------------
// read a given 1 kByte-long block of the ROC DDR memory
// last block - in reg 15
//-----------------------------------------------------------------------------
int dtc_control_roc_read_ddr(int Link, int Block, int PcieAddr = -1) {
  int rc(0);
  // ROC reg 15 - last memory block read
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);

  DTC_Link_ID link_id = DTC_Link_ID(Link);
  
  // write block number to reg 33
  dtc_i->fDtc->WriteROCRegister(link_id,33,Block,false,1000);
  // cycle reg 32
  dtc_i->fDtc->WriteROCRegister(link_id,32, 0x01,false,1000);
  dtc_i->fDtc->WriteROCRegister(link_id,32, 0x00,false,1000);
  // success: reg 20:0x8080  reg21:nwords to read (512)
  int reg20 = dtc_i->fDtc->ReadROCRegister (link_id,20,1000);         // ox8080
  int nw    = dtc_i->fDtc->ReadROCRegister (link_id,21,1000);         // number of 16-bit words in a 1 kByte block (512)
//-----------------------------------------------------------------------------
// at this point, if everything was OK (nw=512), can read the data
//-----------------------------------------------------------------------------
  std::cout << std::format("reg_21(nwords):{:d}  reg_20:0x{:4x}\n",nw,reg20);

  if (nw == 512) {
    std::vector<uint16_t> v;
    dtc_i->fDtc->ReadROCBlock(v,link_id,0x200,nw,false,1000);
    if (nw != 512) {
      std::cout << std::format("ERROR:002 read {} instead of 512 words, try again\n",nw);
      rc = -1;
    }
    else {
      dtc_i->PrintBuffer(v.data(),nw);
    }
  }
  else {
    std::cout << "ERROR:001 smth went wrong, try again\n";
    rc = -1;
  }
  return rc;
}

//-----------------------------------------------------------------------------
int dtc_control_roc_get_key(int Link, int PcieAddr = -1) {
  std::vector<uint16_t> dat;
  auto dtc_i = DtcInterface::Instance(PcieAddr);

  int print_level = 0;
  dtc_i->ControlRoc_GetKey(dat,Link,print_level);
  float temp     = float(dat[0])/4096.*3300./10;
  float v2p5     = float(dat[1])/4096*3.355;
  float v5p1     = float(dat[2])/4096.*3.355*2;
  float dcdctemp = float(dat[3])/4096*3300/10;

  printf(" temp: %10.3f\n v2p5: %10.3f\n v5p1: %10.3f\n dcdctemp: %7.3f\n",
         temp,v2p5,v5p1,dcdctemp);
  return 0;
}


//-----------------------------------------------------------------------------
// just print the ROC information
//-----------------------------------------------------------------------------
int dtc_control_roc_read_device_id(int Link, int PcieAddr = -1) {
  trkdaq::ControlRoc_DeviceID_t dt;
  
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ControlRoc_ReadDeviceID(Link,dt,1);
  return 0;
}

//-----------------------------------------------------------------------------
int dtc_control_roc_read_ilp(int Link, int PrintLevel=0, int PcieAddr = -1) {
  std::vector<uint16_t> dat;
  auto dtc_i = DtcInterface::Instance(PcieAddr);

  dtc_i->ControlRoc_ReadIlp(dat,Link,PrintLevel);

  int   ilp_id   = dat[0];
  float temp     = float(dat[1])/100.;
  float pressure = float(int(dat[3]) << 16 | int(dat[2]))/524288.;
  
  printf(" ilp_id  :    %5i\n temp    : %8.3f\n pressure: %8.3f\n",ilp_id,temp,pressure);
  return 0;
}

//-----------------------------------------------------------------------------
int dtc_control_roc_set_thresholds(int Link, const char* Fn = "settings_vadim.json", int PcieAddr = -1, int PrintLevel = 0) {
  auto dtc_i = trkdaq::DtcInterface::Instance(-1);

  std::ifstream ifs(Fn);
  nlohmann::json jf = nlohmann::json::parse(ifs);

  for (auto& elm : jf.items()) {
    nlohmann::json o = elm.value();
    int ich  = o["channel"];
    int gain = o["gain"];
    int thr  = o["threshold"];
    std::string type = o["type"];

    int cal_hv(-1);
    
    if      (type == "cal") cal_hv = 0;
    else if (type == "hv" ) cal_hv = 1;

    if (PrintLevel != 0) {
      std::cout << ich << " " << gain << " " << std::setw(3) << thr << " " << type << std::endl;
    }

    dtc_i->ControlRoc_SetThreshold(Link,ich,cal_hv,thr );
    dtc_i->ControlRoc_SetGain     (Link,ich,cal_hv,gain);
  }
  
  return 0;
}

//-----------------------------------------------------------------------------
// initialize the DTC interface, don't forget to call InitReadout()
//-----------------------------------------------------------------------------
trkdaq::DtcInterface* dtc_init(const char* ConfigName, int DeviceID=0) {
  mu2edaq::DtcInputData_t dat;
  
  mu2edaq::DtcInterface::InitConfiguration(ConfigName,DeviceID,&dat);
  
  trkdaq::DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(dat.fPcieAddr);
   if (dtc_i) {
     dtc_i->fPcieAddr    = dat.fPcieAddr;
     dtc_i->fDtcID       = dat.fDtcID;
     dtc_i->fLinkMask    = dat.fLinkMask;
     dtc_i->fPartitionID = dat.fPartitionID;
     dtc_i->fOnSpill     = dat.fOnSpill;
     dtc_i->fEventMode   = dat.fEventMode;
     dtc_i->fMacAddrByte = dat.fMacAddrByte;
  
     dtc_i->SetRocReadoutMode (dat.fRocReadoutMode);
     dtc_i->SetRocLaneMask    (dat.fRocLaneMask);
     dtc_i->SetRocNHitsPerLane(dat.fRocNHitsPerLane);
  
     dtc_i->SetJAMode(dat.fJAMode);
     dtc_i->SetEmulateCfo(dat.fEmulateCfo);
   }
  return dtc_i;
}

//-----------------------------------------------------------------------------
// test of the 'READ' command implementation over the fiber
// if LinkMask != -1, operate on the specified links only
//-----------------------------------------------------------------------------
// EW length         : in units of 25 ns (clock)
// EWMOde            : 1 for buffer test
// EnableClockMarkers: set to 0
// EnableAutogenDRP  : set to 1
//-----------------------------------------------------------------------------
int dtc_init_emulated_cfo_readout_mode(int PcieAddr = -1) { 
  //                                 int EWMode, int EnableClockMarkers, int EnableAutogenDRP) {

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  int rc = dtc_i->InitEmulatedCFOReadoutMode();
  return rc;
}

//-----------------------------------------------------------------------------
int dtc_init_readout(int EmulateCfo, int RocReadoutMode, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  int rc = dtc_i->InitReadout(EmulateCfo,RocReadoutMode);
  return rc;
}

//-----------------------------------------------------------------------------
int dtc_configure_roc_readout_mode(int ReadoutMode, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);    // assume already initialized
  dtc_i->SetRocReadoutMode(ReadoutMode);                     // 0:patterns 1:digis
  dtc_i->InitRocReadoutMode();
  return 0;
}

//-----------------------------------------------------------------------------
// to be executed on each node with a DTC, after the CFO run plan was launched
// don't validate
// a read should always end with releasing  buffers ???
//-----------------------------------------------------------------------------
int dtc_read_subevents(uint64_t FirstTS = 0, int PrintLevel = 1, int Validate = 0, int PcieAddr = -1, const char* OutputFn = nullptr) {
  std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>> list_of_subevents;

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ReadSubevents(list_of_subevents,FirstTS,PrintLevel,Validate,OutputFn);
  
  return list_of_subevents.size();
}

//-----------------------------------------------------------------------------
// LinkMask : hex digit per link, i.e. 0x111 for links 0,1,2
//-----------------------------------------------------------------------------
int dtc_reprogram_roc(int Link, const char* Version, int Doit = 1, int PrintLevel = 0, int PcieAddr = -1) {
  DtcInterface*       dtc_i       = DtcInterface::Instance(PcieAddr);
  const RocFwData_t*  roc_fw_data = DtcInterface::RocFwData();
  dtc_i->SpiProgramRoc(Link,roc_fw_data,Version,Doit,PrintLevel);
  return 0;
}

//-----------------------------------------------------------------------------
// LinkMask : hex digit per link, i.e. 0x111 for links 0,1,2
//-----------------------------------------------------------------------------
int dtc_reset_links(int LinkMask, int PcieAddr = -1) {
  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ResetLinks(LinkMask);
  return 0;
}

// struct RocData_t {
//   ushort  nb;
//   ushort  header;
//   ushort  n_data_packets;  // n data packets, 16 bytes each
//   ushort  ewt[3];
//   ushort  status;
//   ushort  xxx2;
//   ushort  data; // array, use it just for memory mapping
// };
  
//-----------------------------------------------------------------------------
// returns number of found errors in the payload data
//-----------------------------------------------------------------------------
// int validate_dtc_block(ulong EwTag, ushort* Data, ulong* Offset, int PrintData) {

//   int nhits[64] = {
//     1,   2,  3,  0,  0,  0,  7,  8,
//     9,  10, 11, 12, 13, 14, 15, 16,
//     0,  20, 21, 22, 12, 13, 11, 12,
//     0,   0,  8,  4, 12, 11, 12, 13,
//     16,  6,  3,  1, 12,  0, 16, 17,
//     18, 19, 12,  1, 12, 12, 11, 11,
//      0,  0,  0,  0, 13, 14, 10, 13,
//     11, 14, 14, 15,  8,  9, 10, 32
//   };

// //-----------------------------------------------------------------------------
// // calculate offsets just once
// //-----------------------------------------------------------------------------
//   static int   initialized(0);
  
//   if (initialized == 0) {
//     //    calculate_offsets(nhits,offset);
//     initialized = 1;
//   }
// //-----------------------------------------------------------------------------
// // check consistency of the lengths
// // 1. total number of 2-byte words
// //
// //-----------------------------------------------------------------------------
//   int ewt    = EwTag % 64 ;
//   int nb_dtc = *Data;

//   RocData_t* roc = (RocData_t*) (Data+0x18);

//   int nb_rocs = 0;
//   for (int i=0; i<6; i++) {
//     int nb   = roc->nb;
//     nb_rocs += nb;
//     roc      = (RocData_t*) ( ((char*) roc) + roc->nb);
//   }

//   int nerr     = 0;

//   if (nb_dtc != nb_rocs+0x30) {
//     if (PrintData > 0) printf("ERROR: nb_dtc, nb_rocs : 0x%04x 0x%04x\n",nb_dtc,nb_rocs);
//     nerr += 1;
//   }
// //-----------------------------------------------------------------------------
// // event length checks out, check ROC payload
// // check the ROC payload, assume a hit = 2 packets
// //-----------------------------------------------------------------------------
//   roc = (RocData_t*) (Data+0x18);
//   for (int i=0; i<6; i++) {
//     int nb = roc->nb;
// //-----------------------------------------------------------------------------
// // validate ROC header
// //-----------------------------------------------------------------------------
//     // ... TODO
//     ulong ewtag_roc = ulong(roc->ewt[0]) | (ulong(roc->ewt[1]) << 16) | (ulong(roc->ewt[2]) << 32);

//     if (ewtag_roc != EwTag) {
//       if (PrintData > 0) printf("ERROR: roc EwTag ewt_roc : %i 0x%08lx 0x%08lx\n",i,EwTag,ewtag_roc);
//       nerr += 1;
//     }
    
//     if (roc->nb > 0x10) { 
// //-----------------------------------------------------------------------------
// // non-zero payload
// //-----------------------------------------------------------------------------
//       uint*   pattern  = (uint*) &roc->data;
//       if (PrintData > 10) printf("data[0]  = nb = 0x%04x\n",pattern[0]);
 
//       int npackets     = roc->n_data_packets;
//       int npackets_exp = nhits[ewt]*2;       // assume two packets per hit (this number is stored somewhere)

//       if (npackets != npackets_exp) {
//         if (PrintData > 0) printf("ERROR: EwTag roc npackets npackets_exp: 0x%08lx %i %5i %5i\n",
//                                   EwTag,i,npackets,npackets_exp);
//         nerr += 1;
//       }
      
//       if (PrintData > 10) {
//         printf("EwTag, ewt, npackets, npackets_exp,  offset: %10lu %3i %2i %2i %10lu\n",
//                EwTag,  ewt, npackets, npackets_exp, *Offset);
//       }

//       uint nw      = npackets*4;        // N 4-byte words
    
//       for (uint i=0; i<nw; i++) {
//         uint exp_pattern = (i+*Offset) & 0xffffffff;
    
//         if (pattern[i] != exp_pattern) {
//           nerr += 1;
//           if (PrintData > 0) {
//             printf("ERROR: EwTag, ewt i  payload[i]  offset exp_word: %10lu %3i %3i 0x%08x 0x%08lx 0x%08x\n",
//                    EwTag, ewt, i, pattern[i],*Offset,exp_pattern);
//           }
//         }
//       }
//     }
//     roc = (RocData_t*) (((char*) roc) + roc->nb);
//   }
  
//   *Offset += 2*4*nhits[ewt];

//   if (PrintData > 10) printf("EwTag = %10lx, nb_dtc = %i nerr = %i\n",EwTag,nb_dtc,nerr);

//   return nerr;
// }

//-----------------------------------------------------------------------------
// do this per ROC
//-----------------------------------------------------------------------------
void dtc_read_spi(int Link, int PrintLevel = 2, int PcieAddr = -1) {
  vector<uint16_t>   spi_data;

  DtcInterface* dtc_i = DtcInterface::Instance(PcieAddr);
  dtc_i->ControlRoc_ReadSpi(spi_data,Link,PrintLevel);
}

//-----------------------------------------------------------------------------
// this test is performed in a pattern readout mode
// Mode : 0xXXRRVVPP
//        PP : print level - up to 256
//        VV : validation level : 00 : don't validate 01: validate
//        RR : ROC readout mode : 00 : ROC patterns   01: digis 02: patterns fixed size
//        XX : reserved
//-----------------------------------------------------------------------------
int dtc_buffer_test_emulated_cfo(int         NEvents  = 3      ,
                                 int         Mode     = 0x01   ,
                                 uint64_t    FirstTS  = 0      ,
                                 const char* OutputFn = nullptr) {
  int pcie_addr(-1), rc(0);                                 // assume initialized
  
  DtcInterface* dtc_i = DtcInterface::Instance(pcie_addr);  // assume already initialized

  int print_level      = (Mode >>  0) & 0xff;
  int validation_level = (Mode >>  8) & 0xff;
  int roc_readout_mode = (Mode >> 16) & 0xff;
                                                     // 68x25ns = 1700 ns
                                                     // this call doesn't send EWMs
  int emulate_cfo = 1;
  rc = dtc_i->InitReadout(emulate_cfo,roc_readout_mode);
  if (rc < 0) return rc;
                                                    // in emulated mode, always read after
  
  dtc_i->LaunchRunPlanEmulatedCfo(daq_scripts::EWLength,NEvents+1,FirstTS); //

  dtc_read_subevents(FirstTS,print_level,validation_level,pcie_addr,OutputFn);

  return rc;
}

//-----------------------------------------------------------------------------
// Mode   : (roc_readout_mode<<16) | (validation_level<<8) | print_level
// DtcMask: a bit mask, each hex digit tells the number of DTCs on a given time chain
// so, by default, tests reading ROC variable length patterns 
//-----------------------------------------------------------------------------
int dtc_buffer_test_external_cfo(const char* RunPlan   = "commands.bin",
                                 int         Mode      = 0x1           ,
                                 uint        DtcMask   = 0x1           ,
                                 const char* OutputFn  = nullptr       ) {
  int rc(0);
  int pcie_addr = -1; // assume initialized

  int print_level      = (Mode >>  0) & 0xff;
  int validation_level = (Mode >>  8) & 0xff;
  int roc_readout_mode = (Mode >> 16) & 0xff;

  DtcInterface* dtc_i = DtcInterface::Instance(pcie_addr);
  int emulate_cfo(0);
  rc = dtc_i->InitReadout(emulate_cfo,roc_readout_mode);
  if (rc < 0) return rc;
//-----------------------------------------------------------------------------
// for now, assume only one time chain, but provide for future
//-----------------------------------------------------------------------------
  cfo_init_readout_ext(RunPlan,DtcMask);      // for now, assume one time chain
  cfo_launch_run_plan();
//-----------------------------------------------------------------------------
// read events
//-----------------------------------------------------------------------------
  uint64_t first_ts(0);
  dtc_read_subevents(first_ts,print_level,validation_level,pcie_addr,OutputFn);
  return rc;
}

//-----------------------------------------------------------------------------
// .L dtc_gui.C
//  x = dtc_gui("test",0x1,0)
// DeviceID: 0x01: DTC@PCIE=0,
//           0x10: DTC@PCIE=1
//           0x21: CFO@PCIE=1
//-----------------------------------------------------------------------------
DtcGui* dtc_gui(const char* Project = "test", int DeviceID = 0, int DebugLevel = 0) {
  // 950x1000: dimensions of the main frame
  DtcGui* x = new DtcGui(Project,DeviceID,gClient->GetRoot(),950,1000,DebugLevel);
  return x;
} 

//-----------------------------------------------------------------------------
void set_digi_serial_readout(unsigned dtc_pcie, unsigned roc_link) {
  auto dtc_i = DtcInterface::Instance(dtc_pcie);
  auto dtc = dtc_i->Dtc();
  dtc->WriteROCRegister(DTC_Link_ID(roc_link), 8, 0x30F, false, 100);
  dtc->WriteROCRegister(DTC_Link_ID(roc_link), 29, 1, false, 100);
}

void daq() {}

#endif
