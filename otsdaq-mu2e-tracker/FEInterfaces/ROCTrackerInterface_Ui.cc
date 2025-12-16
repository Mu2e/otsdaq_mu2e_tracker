
//-----------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//-----------------------------------------------------------------------------


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form

#include <filesystem>


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"


//==============================================================================
///	Ui_ReadSerialNumber()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_ReadSerialNumber(const DTCLib::DTC_Link_ID& Link) 
{

    bool ok(false);
    for (int i=0; i<6; i++) {
      int enabled = (fLinkMask >> 4*i) & 1;
      if (enabled and (i == Link)) {
        ok = true;
      }
    }
    if (not ok) {
      TLOG(TLVL_ERROR) << "Link " << int(Link) << " is not enabled" << std::endl; 
      return "";
    }

    auto returned = this->Ui_ReadDeviceID(Link);

    std::stringstream ss;
    ss << "0x";

    // first 16 words are the serial number, print it in the right order
    for (int i = 15 ; i >= 0 ; i--){
      ss << std::hex << returned[i];

    }

    auto rv = ss.str();
    return rv;
  } // end Ui_ReadSerialNumber()

//==============================================================================
///	Ui_InitRocReadoutMode()
/// this is fully tracker-specific
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_InitRocReadoutMode() 
{
    int rc(0);
    
    TLOG(TLVL_DEBUG) << Form("-- START: fRocReadoutMode=%i\n",fRocReadoutMode);
//-----------------------------------------------------------------------------
// this should be the only place where we reset the ROC
// ROC readout mode (fixed_length << 4) | readout_mode
//-----------------------------------------------------------------------------
// 2025-01-19 PM    Ui_base_ResetLinks();       // forget it ! ... /*this seems to be necesary*/
    
    if (((fRocReadoutMode & 0xf) == 0) || ((fRocReadoutMode & 0xf) == 2)) {
      rc = Ui_MonicaVarPatternConfig();                  // readout ROC patterns
    }
    else if ((fRocReadoutMode & 0xf) == 1) {
      rc = Ui_MonicaVarLinkConfig();                      // readout ROC digis
      if (rc < 0) {
        TLOG(TLVL_ERROR) << "failed to configure the links, rc:" << rc;
        return rc;
      }
        

      // ostd::stringstream sout;
      // PrintRocStatus(1,-1,sout);
      // TLOG(TLVL_DEBUG) << "after Ui_MonicaVarLinkConfig:\n" << sout.str();
      
      rc = Ui_MonicaDigiClear();                          //
      if (rc < 0) {
        return rc;
      }
    }
    else {
      TLOG(TLVL_DEBUG) << "unknown mode:" << fRocReadoutMode << "> BAIL OUT";
    }
    TLOG(TLVL_DEBUG) << Form("-- END: fRocReadoutMode=%i\n",fRocReadoutMode);

    return rc;
  } // end Ui_InitRocReadoutMode()

//==============================================================================
///	Ui_RebootMcu()
/// reboot microcontroller unit: write 1 to Reg 15
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_RebootMcu(int Link) 
{
    int tmo_ms(100), rc(0);

    int lnk1(Link), lnk2(Link+1);
    if (Link == -1) {
      lnk1 = 0;
      lnk2 = 6;
    }
    for (int lnk=lnk1; lnk<lnk2; ++lnk) {
      try {
        getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(lnk),15,1,false,tmo_ms);       // 1 --> r14: reset ROC
        std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCReset));
      }
      catch(...) {
        TLOG(TLVL_ERROR) << "Failed to reboot the MCU:" << lnk;
        rc = -1;
      }
    }
    return rc;
  } // end Ui_RebootMcu()

//==============================================================================
///	Ui_ResetLink()
/// ROC reset : write 0x1 to R14 of each ROC specified as active by the mask
/// by default, don't redefine the link mask
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ResetLink(int Link) 
{
    int tmo_ms(100), rc(0);

    int lnk1(Link), lnk2(Link+1);
    if (Link == -1) {
      lnk1 = 0;
      lnk2 = 6;
    }
    for (int lnk=lnk1; lnk<lnk2; ++lnk) {
      try {
        getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(lnk),14,1,false,tmo_ms);       // 1 --> r14: reset ROC
        std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCReset));
      }
      catch(...) {
        TLOG(TLVL_ERROR) << "Failed to reset link:" << lnk;
        rc = -1;
      }
    }
    return rc;
  } // end Ui_ResetLink()

//==============================================================================
///	Ui_RocConfigurePatternMode()
/// preserve historic naming convention- Monica named her script 'var_pattern_config'
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_RocConfigurePatternMode() 
{
    Ui_MonicaVarPatternConfig();
  } // end Ui_RocConfigurePatternMode()

//==============================================================================
///	Ui_RocSetDataVersion()
/// Version --> R29k
/// as thre is no point inhaving different ROCs with different data versions, assume
/// that specifying the mask means that we want it to be redefined
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_RocSetDataVersion(int Version, int LinkMask) 
{
    if (LinkMask != 0) fLinkMask = LinkMask;
    
    int tmo_ms(100);
    for (int i=0; i<6; i++) {
      int enabled = (fLinkMask >> 4*i) & 0x1;
      if (enabled != 0) {
        getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i),29,Version,false,tmo_ms);
      }
    }
    std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
  } // end Ui_RocSetDataVersion()

//==============================================================================
///	Ui_ReadDeviceID()
/// assume that only one link is specified (not DTC_Link_ALL)
/// read serial number and device info
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<DTCLib::roc_data_t> ROCTrackerInterface::Ui_ReadDeviceID(DTCLib::DTC_Link_ID Link, int PrintLevel, std::ostream& Stream) 
{
    std::vector<DTCLib::roc_data_t> rv;

    int ilink = int(Link);
    if (not LinkEnabled(ilink)) {
      Stream << "ERROR: Link " << ilink << " is not enabled" << std::endl; 
      return rv;
    }
                                        // reset only ROC in question
                                        // 2024-11-14: Monica tells reset is not needed
    //    this->Ui_ResetRoc(Link,0);
    // write nothing to trigger query
    std::vector<DTCLib::roc_data_t> empty;
    getDTC()->WriteROCBlock(Link, 260, empty, false, false, 1000);
    std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

    // read back payload
    rv = this->Ui_ReadROCBlockEnsured(Link, 260);

    if (PrintLevel & 0x1) {
      Ui_print_PrintBuffer(rv.data(),rv.size(),0,&Stream);
    }

    return rv;
  } // end Ui_ReadDeviceID()

//==============================================================================
///	Ui_FindAlignment()
/// align ROC fpga/adc signals, and optionally print summary table
/// REG_FINDALIGNMENT=264
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
Alignment ROCTrackerInterface::Ui_FindAlignment(DTCLib::DTC_Link_ID Link) 
{
    // write parameters into roc to initiate routine
    std::vector<DTCLib::roc_data_t> writeable = {
      4,                             // eye-monitor width
      0,                             // initial adc phase
      1,                             // flag to check adc patterns
      static_cast<uint16_t>(-1),     // for channel remapping; unused
      static_cast<uint16_t>(-1),     // for channel remapping; unused
      0xFFFF,                        // bitmask for channels  0 - 15
      0xFFFF,                        // bitmask for channels 16 - 31
      0xFFFF,                        // bitmask for channels 32 - 47
      0xFFFF,                        // bitmask for channels 48 - 63
      0xFFFF,                        // bitmask for channels 64 - 79
      0xFFFF,                        // bitmask for channels 80 - 95
    };

    // register 264: find alignment routine
    bool increment_address = false; // read via fifo
    getDTC()->WriteROCBlock(Link, trkdaq::REG_FINDALIGNMENT, writeable, false, increment_address, 100);
    std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

    // then, wait till reg 128 returns non-zero
    uint16_t u;
    while ((u = getDTC()->ReadROCRegister(Link, 128, 100)) != 0x8000){
      // idle
    }

    std::vector<DTCLib::roc_data_t> returned = this->Ui_ReadROCBlockEnsured(Link,trkdaq::REG_FINDALIGNMENT);

    // return
    auto rv = Alignment(returned);
    return rv;
  } // end Ui_FindAlignment()

//==============================================================================
///	Ui_FindAlignments()
/// align ROC FPGA/ADC signals, and optionally print the summary
/// 
/// if 'Link' = -1, use the DTC link mask
/// there is no practical need to pass a random link mask,
/// so 'Link' is either all enabled DTC links, or a specific one
/// 
/// returns the number of channels with non-zero number of bit slip steps
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_FindAlignments(int PrintLevel, int Link, std::ostream& Stream) 
{
    int n_slipped(0);
    
    int link_mask = fLinkMask;
    if (Link != -1) link_mask = 0x1 << 4*Link;

    
    for (int i = 0 ; i < 6 ; i++){
      int enabled = (link_mask >> 4*i) & 0x1;
      if (enabled == 0)                                     continue;
//-----------------------------------------------------------------------------
// perform one iteration
//-----------------------------------------------------------------------------
      auto link      = DTCLib::DTC_Link_ID(i);
      auto alignment = Ui_FindAlignment(link);

      int n_non_null   =  0;
      int nsteps_tot   =  0;
      int max_steps_ch = -1;
      int worst_ch     = -1;
      for (const auto& iteration: alignment.Iterations()) {
        const auto& channels = iteration.Channels();
        for (size_t i = 0 ; i < channels.size() ; i++){
          auto channel = channels[i];
          int nsteps     = (int) channel.BitSlipStep();
          if (nsteps > 0) n_non_null++;
          nsteps_tot  += nsteps;
          if (nsteps > max_steps_ch) {
            max_steps_ch = nsteps;
            worst_ch     = i;
          }
        }
      }

      if (PrintLevel & 0x2) {
        print_legacy_table(alignment,Stream);
      }

      if (PrintLevel & 0x1) {
        Stream << " link:" << i << " n_non_null:" << std::setw(3) << n_non_null
               << " nsteps_tot:" << std::setw(3) << nsteps_tot
               << " worst_ch:" << std::setw(3) << worst_ch
               << " max_steps_ch:" << std::setw(3) << max_steps_ch << std::endl;
      }
      n_slipped += n_non_null;
    }
    
    return n_slipped;
  } // end Ui_FindAlignments()

//==============================================================================
///	Ui_ValidateDigiPatterns ()
/// validate data taken in the tracker ROC pattern generation mode, focus on payload
/// returns number of found errors in the payload data
/// assume ROC pattern generation
/// 'Offset' is the
/// PrintLevel =  0: print nothing
///            =  1: print all about errors
///            > 10: full printout
/// also returned NErrRoc[6]: number of errors per ROC
/// returns nerrors, where does the error code goes ?
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ValidateDigiPatterns (ushort* DtcData, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc) 
{

  int n_adc_packets(1);
  int nerr   = 0;
  
  RocData_t* roc = (RocData_t*) (DtcData+0x18);  // 0x30 bytes
  for (int i=0; i<6; i++) {
    // nb_roc[i]    = roc->header.byteCount;
    // nb_rocs_tot += nb_roc[i];

    int nhits        = roc->header.packetCount/(n_adc_packets+1);
    
    short* first_address = (short*) roc;
  
    for (int ihit=0; ihit<nhits; ihit++) {
      mu2e::TrackerDataDecoder::TrackerDataPacket* hit ;
      int offset          = ihit*(8+8*n_adc_packets);   // in 2-byte words
      //int offset_in_bytes = offset*2;
      hit     = (mu2e::TrackerDataDecoder::TrackerDataPacket*) (first_address+0x08+offset);
      if (hit->ErrorFlags != 0) {  // 4 bits
        nerr += 1;
      }
//-----------------------------------------------------------------------------
// check hit straaw ID - TODO: correct the chid check
//-----------------------------------------------------------------------------
      int ich = hit->StrawIndex;

      if (ich > 128) ich = ich-128;

      if (ich > 95) {
//-----------------------------------------------------------------------------
// non existing channel ID : flag an error, don't save the hit, but continue
//-----------------------------------------------------------------------------
        nerr += 1;
      }
      if (hit->NumADCPackets != n_adc_packets) {
        nerr += 1;
                                        // assume errors are localized within the ROC payload
        break;
      }
    }
    
    roc = (RocData_t*) ( ((char*) roc) + roc->header.byteCount);
  }
  return 0;
} // end Ui_ValidateDigiPatterns ()

//==============================================================================
///	Ui_ValidateFixedPatterns()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ValidateFixedPatterns(ushort* DtcData, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc) 
{
  //  int ewt    = EwTag % 64 ;
  int nb_dtc = *DtcData;
  
  int nerr   = 0;

  int nb_roc[6];
  int nb_rocs_tot = 0;
  int last_nb(-1);

  RocData_t* roc = (RocData_t*) (DtcData+0x18);  // 0x30 bytes
  for (int i=0; i<6; i++) {
    nb_roc[i]    = roc->header.byteCount;
    nb_rocs_tot += nb_roc[i];
//-----------------------------------------------------------------------------
// although some ROC may not respond,  all responding ones should report
// the same number of bytes
//-----------------------------------------------------------------------------
    if (roc->header.error_code() == 0) { 
      if ((last_nb > 0) and (nb_roc[i] != last_nb)) {
        nerr += 1;
        if (PrintLevel > 1) {
          printf("ERROR: EWtag, nb_dtc, i, nb_roc[i-1], nb[roc] : %10lu 0x%04x %i 0x%04x 0x%04x\n",
                 EwTag,nb_dtc,i,nb_roc[i-1], nb_roc[i]);
        }
      }
      last_nb = nb_roc[i];
    }
    roc = (RocData_t*) ( ((char*) roc) + roc->header.byteCount);
  }
  
                                        // DTC header is 0x30 bytes - 3 packets
  if (nb_dtc != nb_rocs_tot+0x30) {
    if (PrintLevel > 1) printf("ERROR: EWtag, nb_dtc, nb_rocs_tot : %10lu 0x%04x 0x%04x\n",EwTag,nb_dtc,nb_rocs_tot);
    nerr += 1;
  }
  
  return nerr;
} // end Ui_ValidateFixedPatterns()

//==============================================================================
///	Ui_ValidateVarPatterns  ()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ValidateVarPatterns  (ushort* DtcData, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc) 
{

  int nhits[64] = {
    1,   2,  3,  0,  0,  0,  7,  8,
    9,  10, 11, 12, 13, 14, 15, 16,
    0,  20, 21, 22, 12, 13, 11, 12,
    0,   0,  8,  4, 12, 11, 12, 13,
    16,  6,  3,  1, 12,  0, 16, 17,
    18, 19, 12,  1, 12, 12, 11, 11,
     0,  0,  0,  0, 13, 14, 10, 13,
    11, 14, 14, 15,  8,  9, 10, 32
  };

//-----------------------------------------------------------------------------
// check consistency of the lengths
// 1. total number of 2-byte words
//
//-----------------------------------------------------------------------------
  int ewt    = EwTag % 64 ;
  int nb_dtc = *DtcData;

  RocData_t* roc = (RocData_t*) (DtcData+0x18);

  int nb_rocs = 0;
  for (int i=0; i<6; i++) {
    int nb   = roc->header.byteCount;
    nb_rocs += nb;
    roc      = (RocData_t*) ( ((char*) roc) + nb);
  }

  int nerr   = 0;

  if (nb_dtc != nb_rocs+0x30) {
    if (PrintLevel > 1) printf("ERROR: EWtag, nb_dtc, nb_rocs : %10lu 0x%04x 0x%04x\n",EwTag,nb_dtc,nb_rocs);
    nerr += 1;
  }
//-----------------------------------------------------------------------------
// event length checks out, check ROC payload
// check the ROC payload, assume a hit = 2 packets
//-----------------------------------------------------------------------------
  roc = (RocData_t*) (DtcData+0x18);
  for (int iroc=0; iroc<6; iroc++) {
    NErrRoc[iroc] = 0;
    if (PrintLevel > 10) printf("  ---- roc # %i\n",iroc);
//-----------------------------------------------------------------------------
// offsets are the same for all non-emty ROC's in the DTC data block
//-----------------------------------------------------------------------------
    ulong offset = *Offset;
    //    int   nb     = roc->header.byteCount;
//-----------------------------------------------------------------------------
// validate ROC header
//-----------------------------------------------------------------------------
    // ... TODO
    ulong ewtag_roc = roc->header.ewtag();

    if (ewtag_roc != EwTag) {
      if (PrintLevel > 1) printf("ERROR: EwTag ewtag_roc roc : 0x%08lx 0x%08lx %i\n",EwTag,ewtag_roc,iroc);
      nerr          += 1;
      NErrRoc[iroc] += 1;
    }
    
    if (roc->header.byteCount > 0x10) { 
//-----------------------------------------------------------------------------
// non-zero payload
//-----------------------------------------------------------------------------
      uint32_t*   pattern  = (uint32_t*) &roc->data[0];
      if (PrintLevel > 10) printf("data[0]  = nb = 0x%04x\n",pattern[0]);
 
      int npackets     = roc->header.packetCount;
      int npackets_exp = nhits[ewt]*2;       // assume two packets per hit (this number is stored somewhere)

      if (npackets != npackets_exp) {
        if (PrintLevel > 1) printf("ERROR: EwTag roc npackets npackets_exp: 0x%08lx %i %5i %5i\n",
                                  EwTag,iroc,npackets,npackets_exp);
        nerr          += 1;
        NErrRoc[iroc] += 1;
      }
      
      if (PrintLevel > 10) {
        printf("EwTag, ewt, roc, npackets, npackets_exp,  offset: %10lu %3i %i %2i %2i %10lu\n",
               EwTag,  ewt, iroc, npackets, npackets_exp,  offset);
      }

      uint nw      = npackets*4;        // N 4-byte words
    
      for (uint iw=0; iw<nw; iw++) {
        uint exp_pattern = (iw+offset) & 0xffffffff;
    
        if (pattern[iw] != exp_pattern) {
          nerr          += 1;
          NErrRoc[iroc] += 1;
          if (PrintLevel > 1) {
            printf("ERROR: EwTag, ewt roc iw  offset payload[iw] exp_word: %10lu %3i %i %3i %10li 0x%08x 0x%08x\n",
                   EwTag, ewt, iroc, iw, offset,pattern[iw],exp_pattern);
          }
        }
      }
    }
    roc = (RocData_t*) (((char*) roc) + roc->header.byteCount);
  }
  
  *Offset += 2*4*nhits[ewt];

  if (PrintLevel > 10) printf("EwTag = %10lx, nb_dtc = %i nerr = %i nerr_roc: %5i %5i %5i %5i %5i %5i\n",
                              EwTag,nb_dtc,nerr,
                              NErrRoc[0],NErrRoc[1],NErrRoc[2],NErrRoc[3],NErrRoc[4],NErrRoc[5]);

  return nerr;
} // end Ui_ValidateVarPatterns  ()

//==============================================================================
///	Ui_MonicaDigiClear()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_MonicaDigiClear() 
{

    for (int i=0; i<6; i++) {
      int used = (fLinkMask >> 4*i) & 0x1;
      if (not used)                                           continue;
//-----------------------------------------------------------------------------
// link is active
//-----------------------------------------------------------------------------
      auto link = DTCLib::DTC_Link_ID(i);

      // rocUtil write_register -l $LINK -a 28 -w 16 > /dev/null
      getDTC()->WriteROCRegister(link,28,0x10,false,1000); // 

      // Writing 0 & 1 to  address=16 for HV DIGIs ??? 
      // rocUtil write_register -l $LINK -a 27 -w  0 > /dev/null # write 0 
      // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null ## toggle INIT 
      // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
      getDTC()->WriteROCRegister(link,27,0x00,false,1000); // 
      getDTC()->WriteROCRegister(link,26,0x01,false,1000); // toggle INIT 
      getDTC()->WriteROCRegister(link,26,0x00,false,1000); // 
    

      // rocUtil write_register -l $LINK -a 27 -w  1 > /dev/null # write 1  
      // rocUtil write_register -l $LINK -a 26 -w  1 > /dev/null # toggle INIT
      // rocUtil write_register -l $LINK -a 26 -w  0 > /dev/null
      getDTC()->WriteROCRegister(link,27,0x01,false,1000); // 
      getDTC()->WriteROCRegister(link,26,0x01,false,1000); // 
      getDTC()->WriteROCRegister(link,26,0x00,false,1000); // 
    
      // echo "Writing 0 & 1 to  address=16 for CAL DIGIs"
      // rocUtil write_register -l $LINK -a 25 -w 16 > /dev/null
      getDTC()->WriteROCRegister(link,25,0x10,false,1000); // 
    
      // rocUtil write_register -l $LINK -a 24 -w  0 > /dev/null # write 0
      // rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
      // rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
      getDTC()->WriteROCRegister(link,24,0x00,false,1000); // 
      getDTC()->WriteROCRegister(link,23,0x01,false,1000); // 
      getDTC()->WriteROCRegister(link,23,0x00,false,1000); // 

      // rocUtil write_register -l $LINK -a 24 -w  1 > /dev/null # write 1
      // rocUtil write_register -l $LINK -a 23 -w  1 > /dev/null # toggle INIT
      // rocUtil write_register -l $LINK -a 23 -w  0 > /dev/null
      getDTC()->WriteROCRegister(link,24,0x01,false,1000); // 
      getDTC()->WriteROCRegister(link,23,0x01,false,1000); // 
      getDTC()->WriteROCRegister(link,23,0x00,false,1000); // 
    }
    return 0;
  } // end Ui_MonicaDigiClear()

//==============================================================================
///	Ui_MonicaVarLinkConfig()
/// LaneMask bits:
///           0x1 : CAL lane 0
///           0x2 : HV  lane 0
///           0x4 : CAL lane 1
///           0x8 : HV  lane 1
/// origin: ~mu2etrk/test_stand/monica_002/var_link_config.sh from Mar 12 2024
///  -rwxr-xr-x  1 mu2etrk mu2e      1553 Mar 12 10:11 var_link_config.sh
/// configure_ROC 'read' command should be followed by ROC reset
/// to be added 
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_MonicaVarLinkConfig() 
{
    int rc(0);
    
    fRocReadoutMode = 1;                            // 1: read digis
                                        // bit 13 - disable reset of the counters by the HB next to the null HB
    // int lane_mask = 0x0300 | LaneMask;
    int lane_mask = 0x2300 | fRocLaneMask; // assumed to be the same for all ROCs
    
    for (int i=0; i<6; i++) {
      int enabled = (fLinkMask >> 4*i) & 0x1;
      if (enabled) {
        getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 8,lane_mask,false,1000);              // enable lanes
        std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
        TLOG(TLVL_INFO) << "wrote lane_mask:" << std::hex << lane_mask
                        << " to ROC:" << i <<" reg:8, read back:" << getDTC()->ReadROCRegister(DTCLib::DTC_Link_ID(i), 8,100);
      }
    }
    
    std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

    int data_version = 1;
    Ui_RocSetDataVersion(data_version);    // Version --> R29

    Ui_base_ResetLinks();                         // use fLinkMask
//-----------------------------------------------------------------------------
// according to Monica, this is the place for find_alignment and control_roc_read
// check if all lanes are ready to be read
//-----------------------------------------------------------------------------
    for (int i=0; i<6; i++) {
      int used = (fLinkMask >> 4*i) & 0x1;
      if (used != 0) {
        uint16_t u = getDTC()->ReadROCRegister(DTCLib::DTC_Link_ID(i),18,100);
        if ((u >> 0x8) != 0xF) {
          // try to recover - write 1, then - 0 to reg 13
          getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 13,0x1,false,1000);
          std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
          getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 13,0x0,false,1000);
          std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
          // and check again
          u = getDTC()->ReadROCRegister(DTCLib::DTC_Link_ID(i),18,100);
          if ((u >> 0x8) != 0xF) {
            // still in trouble
            TLOG(TLVL_ERROR) << Form("ROC on link %i is not ready to read the DIGIs  link mask is 0x%04x, call Monica and Richie\n",
                                     i,u);
            rc -= 1;
          }
        }
      }
    }
//-----------------------------------------------------------------------------
    return rc;
  } // end Ui_MonicaVarLinkConfig()

//==============================================================================
///	Ui_MonicaVarPatternConfig()
/// origin: test_stand/monica_002/var_pattern_config.sh from Feb 14 2024
///  -rwxr-xr-x  1 mu2etrk mu2e      1820 Feb 14 15:00 var_pattern_config.sh
/// adding 0x2000 prevents ROC from reinitializing the pattern, so two subsequent
/// buffer test runs would return different results
/// lane mask default: 0xf
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_MonicaVarPatternConfig(int LaneMask, int NHitsPerLane) 
{

    Ui_base_ResetLinks();                                      // use fLinkMask
    int version = 1;
    Ui_RocSetDataVersion(version);                      // Version --> R29

    int ro_mode            = (fRocReadoutMode >> 0) & 0xf;
    int var_pattern_length = (fRocReadoutMode >> 4) & 0xf;
    
    if ((ro_mode != 0) and (ro_mode != 2)) {
      TLOG(TLVL_ERROR) << "unknown mode:" << fRocReadoutMode << " BAIL OUT";
      return -1;
    }

    for (int i=0; i<6; i++) {
      int used = (fLinkMask >> 4*i) & 0x1;
      if (used != 0) {
        if (ro_mode == 0) {
//-----------------------------------------------------------------------------
// mask bit#04=1: variable length
// mask bit#12=0: 'ROC counter;
//-----------------------------------------------------------------------------
          getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 8,0x2010,false,1000); // configure ROC to send variable length patterns
          std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
        }
        else {
//-----------------------------------------------------------------------------
// can only be Mode == 2
// set number of simulated hits per lane - where that number is coming from?
// have only 10 bits for the number  of hits
// mask bit#04=1: variable length
// mask bit#13=1: don't reset the conters when receiving a null HB
// mask bit#12=1: 'ROC checkerboard'
// mask bit#11=1: fixed length patters
// NHits : 10 LS bits in reg@15
//-----------------------------------------------------------------------------
          int lane_mask = LaneMask;
          if (lane_mask < 0) lane_mask = fRocLaneMask;
          uint16_t mask = 0x3800 | lane_mask;
          if (var_pattern_length == 1) mask = mask | 0x00000010;
          else                         mask = mask & 0xffffffef;
          getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i), 8,mask,false,1000);   // configure ROC to send fixed length patterns
          std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

          int nhits = NHitsPerLane;
          if (nhits < 0) nhits = fRocNHitsPerLane;
          uint16_t w15 = (nhits & 0x3ff);
          getDTC()->WriteROCRegister(DTCLib::DTC_Link_ID(i),15,w15,false,1000);
          std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

          TLOG(TLVL_DEBUG) << "var_pattern_length:" << var_pattern_length
                           << " reg#08:0x" << std::hex << std::setw(4) << std::setfill('0') << mask
                           << " reg#15:0x" << std::hex << std::setw(4) << std::setfill('0') << w15;
        }
      }
    }

    return 0;
  } // end Ui_MonicaVarPatternConfig()

//==============================================================================
///	Ui_ReadSubevents()
/// ROC reset : write 0x1 to register 14
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
void ROCTrackerInterface::Ui_ReadSubevents(std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& VSub, 
                                   ulong       FirstEWT   ,
                                   int         PrintLevel,
                                   int         Validate  ,
                                   const char* Fn        ) 
{
    ulong    ewt      = FirstEWT;
    bool     match_ts = false;
    int      nerr_tot  (0);
    ulong    nbytes_tot(0);
    ulong    offset    (0);               // used in validation mode
    int      nerr_roc[6], nerr_roc_tot[6];

    FILE*    file(nullptr);
    if (Fn != nullptr) {
//-----------------------------------------------------------------------------
// check if Fn exists 
//-----------------------------------------------------------------------------
      if((file = fopen(Fn,"r")) != NULL) {
        // file exists
        fclose(file);
        TLOG(TLVL_ERROR) << "file " << Fn << " already exists, BAIL OUT";
        return;
      }
      else {
//-----------------------------------------------------------------------------
// Fn doesn't exist, open it 
//-----------------------------------------------------------------------------
        file = fopen(Fn,"w");
        if (file == nullptr) {
          TLOG(TLVL_ERROR) <<  "failed to open " << Fn << " , BAIL OUT";
          return;
        }
      }
    }
//-----------------------------------------------------------------------------
// reset per-roc error counters
//-----------------------------------------------------------------------------
    for (int i=0; i<6; i++) {
      nerr_roc    [i] = 0;
      nerr_roc_tot[i] = 0;
    }
//-----------------------------------------------------------------------------
// always read an event into the same external buffer (VSub), 
// so no problem with the memory management
//-----------------------------------------------------------------------------
    int header_printed = 0;
    while(1) {
      // sleep(1);
      DTCLib::DTC_EventWindowTag event_tag = DTCLib::DTC_EventWindowTag(ewt);
      try {
        if (PrintLevel > 0) {
//-----------------------------------------------------------------------------
// print header
// if fValidate != 0, there is a lot of printout, so it is better to print header
// for every event
//-----------------------------------------------------------------------------
          if ((Validate and PrintLevel > 1) or (header_printed == 0)) {
            std::cout << Form("      event  DTC     EW Tag nbytes   nbytes_tot  link0   nb0  link1   nb1  link2   nb2  link3   nb3  link4   nb4  link5   nb5  nerr nerr_tot\n");
            std::cout << Form("--------------------------------------------------------------------------------------------------------------------------------------------\n");
            header_printed = 1;
          }
        }
        VSub   = getDTC()->GetSubEventData(event_tag, match_ts);
        int sz = VSub.size();
        if (sz == 0) {
          if (PrintLevel > 0) {
            std::cout << Form(">>>> ------- ewt = %5li NDTCs:%2i END_OF_DATA\n",ewt,sz);
          }
          break;
        }
//-----------------------------------------------------------------------------
// a subevent contains data of a single DTC
//-----------------------------------------------------------------------------
        int rs[6];
        std::vector<uint8_t> dtc_block;
        
        for (int i=0; i<sz; i++) {
          DTCLib::DTC_SubEvent* ev  = VSub[i].get();
          uint64_t ew_tag   = ev->GetEventWindowTag().GetEventWindowTag(true);
          char*    raw_data = (char*) ev->GetRawBufferPointer();

          int      nbytes  = ev->GetSubEventByteCount();
//-----------------------------------------------------------------------------
// create a local copy of the DTC data block
//-----------------------------------------------------------------------------
          dtc_block.reserve(nbytes);
          memcpy(dtc_block.data(),raw_data,nbytes);

          nbytes_tot += nbytes;

          int nerr(0);
          
          if (Validate > 0) {
            // different readout modes - different validation
            if      ((fRocReadoutMode & 0xf) == 0) {
              nerr = Ui_ValidateVarPatterns((ushort*) dtc_block.data(),ew_tag,&offset,PrintLevel,nerr_roc);
            }
            else if ((fRocReadoutMode & 0xf) == 1) {
              nerr = Ui_ValidateDigiPatterns((ushort*) dtc_block.data(),ew_tag,&offset,PrintLevel,nerr_roc);
            }
            else if ((fRocReadoutMode & 0xf) == 2) {
              nerr = Ui_ValidateFixedPatterns((ushort*) dtc_block.data(),ew_tag,&offset,PrintLevel,nerr_roc);
            }
            
              
            nerr_tot += nerr;
            for (int ir=0; ir<6; ir++) nerr_roc_tot[ir] += nerr_roc[ir];
          }

          uint8_t* roc_data  = dtc_block.data()+0x30;

          int nb_roc[6];
          for (int roc=0; roc<6; roc++) {
            nb_roc[roc] = *((ushort*) roc_data);
            rs[roc]     = *((ushort*)(roc_data+0x0c));
            roc_data   += nb_roc[roc];
          }
        
          if (PrintLevel > 0) {
            std::cout << Form(" %10li  %2i  %10li %5i %13li 0x%04x %5i 0x%04x %5i 0x%04x %5i 0x%04x %5i 0x%04x %5i 0x%04x %5i %5i %8i %4i %4i %4i %4i %4i %4i\n",
                         ewt,i,ew_tag,nbytes,nbytes_tot,
                         rs[0],nb_roc[0],rs[1],nb_roc[1],rs[2],nb_roc[2],rs[3],nb_roc[3],rs[4],nb_roc[4],rs[5],nb_roc[5],
                         nerr,nerr_tot,
                         nerr_roc[0],nerr_roc[1],nerr_roc[2],nerr_roc[3],nerr_roc[4],nerr_roc[5] );
            if (((nerr > 0) and (PrintLevel > 1)) or (PrintLevel > 2)) {
              Ui_print_PrintBuffer(ev->GetRawBufferPointer(),ev->GetSubEventByteCount()/2);
            }
          }
          
          if (file) {
//-----------------------------------------------------------------------------
// write event to output file
//-----------------------------------------------------------------------------
            int nbb = fwrite(dtc_block.data(),1,nbytes,file);
            if (nbb == 0) {
              TLOG(TLVL_ERROR) << Form("failed to write event %10li , close file and BAIL OUT\n",ew_tag);
              fclose(file);
              return;
            }
          }
        }
        
        ewt++;                          // event in sequence
      }
      catch (...) {
        TLOG(TLVL_ERROR) << "ERROR reading event_tag:" << event_tag.GetEventWindowTag(true) << " ewt:" << ewt << std::endl;
        break;
      }
    }

    //    getDTC()->ReleaseAllBuffers(DTC_DMA_Engine_DAQ);
//-----------------------------------------------------------------------------
// print summary
//-----------------------------------------------------------------------------
    ulong nev = ewt-FirstEWT;
    TLOG(TLVL_DEBUG) << Form("nevents: %10li nbytes_tot: %13li Validate:%i\n",nev, nbytes_tot,Validate)
                     << Form("nerr_tot:%10i nerr_roc_tot: %8i %8i %8i %8i %8i %8i\n",
                             nerr_tot,
                             nerr_roc_tot[0],nerr_roc_tot[1],nerr_roc_tot[2],
                             nerr_roc_tot[3],nerr_roc_tot[4],nerr_roc_tot[5]);
//-----------------------------------------------------------------------------
// to simplify first steps, assume that in a file writing mode all events 
// are read at once, so close the file on exit
//-----------------------------------------------------------------------------
    if (file) {
      fclose(file);
    }
  } // end Ui_ReadSubevents()

//==============================================================================
///	Ui_RocBlockRead()
/// 2025-01-31: P.Murat: presently, calls to begin_dcs_transaction() and end_dcs_transaction()
/// are just TODO reminders and don't do anything useful
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_RocBlockRead(int Link, int Reg, std::vector<uint16_t>& Res, int NExpected) 
{
    int rc(0), nw(0);
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
    TLOG(TLVL_DEBUG+1) << std::format("Link:{} Reg:{:03d} NExpected:{}",Link,Reg,NExpected);
    auto link_id  = DTCLib::DTC_Link_ID(Link);

    try {
      // getDTC()->GetDevice()->begin_dcs_transaction();
    
      getDTC()->WriteROCRegister   (link_id,Reg,0x0000,false,100);
      std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));
    
      uint16_t u; 
      while ((u = getDTC()->ReadROCRegister(link_id,128,100)) != 0x8000) {}; 
      TLOG(TLVL_DEBUG+2) << std::format("reg:{:03d} val:0x{:04x}\n",128,u);
//-----------------------------------------------------------------------------
// register 129: number of words to read, currently-  (+ 4) (ask Monica)
//-----------------------------------------------------------------------------
      nw = getDTC()->ReadROCRegister(link_id,129,100);
      TLOG(TLVL_DEBUG+2) << std::format("reg:{:03d} val:0x{:04x}\n",129,nw);

      nw -= 4;
      getDTC()->ReadROCBlock(Res,link_id,Reg,nw,false,100);
    }
    catch(...) {
      TLOG(TLVL_ERROR) << "failed DCS transaction link:" << Link;
      rc = -2;
    }
    
    // getDTC()->GetDevice()->end_dcs_transaction();

    if ((rc == 0) and (NExpected > 0) and (nw != NExpected)) {
      TLOG(TLVL_ERROR) << "WRONG NUMBER OF WORDS: NExpected:" << NExpected << " nw:" << nw;
      rc = -1;
    }
//-----------------------------------------------------------------------------
// does the ROC need to be reset ? Monica says NO.
//-----------------------------------------------------------------------------
    return rc;
  } // end Ui_RocBlockRead()

//==============================================================================
///	Ui_ReadRocDDR()
/// read a given block number from ROC DDR memory
/// block size: 1 kB
/// ROC reg 15: last memory block read
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ReadRocDDR(int Link, int Block, std::ostream& Stream) 
{
    int rc(0);

    DTCLib::DTC_Link_ID link_id = DTCLib::DTC_Link_ID(Link);
  
  // write block number to reg 33
    getDTC()->WriteROCRegister(link_id,33,((Block      ) & 0xffff) ,false,1000);
    getDTC()->WriteROCRegister(link_id,34,((Block >> 16) & 0xffff) ,false,1000);
  // cycle reg 32
    getDTC()->WriteROCRegister(link_id,32, 0x01,false,1000);
    getDTC()->WriteROCRegister(link_id,32, 0x00,false,1000);
  // success: reg 20:0x8080  reg21:nwords to read (512)
    int reg_20 = getDTC()->ReadROCRegister (link_id,20,1000);         // ox8080
    int nw     = getDTC()->ReadROCRegister (link_id,21,1000);         // number of 16-bit words in a 1 kByte block (512)
//-----------------------------------------------------------------------------
// at this point, if everything was OK (nw=512), can read the data
//-----------------------------------------------------------------------------
    Stream << std::format("--- read link:{} DDR block:{:10d} reg_21(nwords):{:d} reg_20:0x{:4x}",Link,Block,nw,reg_20);

    if (nw == 512) {
      std::vector<uint16_t> v;
      getDTC()->ReadROCBlock(v,link_id,0x200,nw,false,1000);
      if (nw != 512) {
        Stream << std::format("ERROR:002 read {} instead of 512 words, try again\n",nw);
        rc = -1;
      }
      else {
        ULong64_t ewt = ULong64_t(v[0]) | (ULong64_t(v[1]) << 16) | (ULong64_t(v[2]) << 32);
        Stream << "  ewt:" << ewt << " len:" << v[3] << std::endl;
        Ui_print_PrintBuffer(v.data(),nw,0,&Stream);
      }
    }
    else {
      Stream << std::endl << "ERROR:001 wrong number of words nw:" << nw << " read (not 512)\n";
      rc = -1;
    }
    return rc;
  } // end Ui_ReadRocDDR()

//==============================================================================
///	Ui_ReadROCBlockEnsured()
/// wrapper for DTCLib::DTC::ReadROCBlock
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<DTCLib::roc_data_t> ROCTrackerInterface::Ui_ReadROCBlockEnsured(const DTCLib::DTC_Link_ID& Link, const DTCLib::roc_address_t& address)
{
    // register 129: number of words to read
    size_t nwords = static_cast<size_t>(getDTC()->ReadROCRegister(Link, 129, 1000));
    nwords -= 4; // account for low-level headers already consumed on-chip

    std::vector<DTCLib::roc_data_t> rv;
    bool increment_address = false; // read via fifo
    getDTC()->ReadROCBlock(rv, Link, address, nwords, increment_address, 10000);
    if (rv.size() != nwords){
      std::string msg = "Malformed block read";
      msg += " expected ";
      msg += std::to_string(nwords);
      msg += " words, received ";
      msg += std::to_string(rv.size());
      msg += " words";
      throw cet::exception("DtcInterface::Ui_ReadROCBlockEnsured") << msg;
    }

    // P.M. don't need to reset the DDR
    // // reset ddr memory
    // getDTC()->WriteROCRegister(Link, 14, 0x01, false, 1000);
    // std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

    // return
    return rv;
  } // end Ui_ReadROCBlockEnsured()

//==============================================================================
///	Ui_GetRocRegistersNames()
/// This is just an example, needs to be implemented for each subsystem
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<std::string> ROCTrackerInterface::Ui_GetRocRegistersNames(bool history) 
{
    std::vector<std::string> roc_var_names;
    char var_name[128];
    // Basic ROC registers
    if(history) {
      for (int k=0; k<trkdaq::TrkSpiDataNWords; k++) {
        sprintf(var_name,"%s", SpiVarName(k));
        roc_var_names.push_back(var_name);
      }
    } else {
      for(const int& reg : RocRegisters) {
        sprintf(var_name,"reg_%03i",reg);
        roc_var_names.push_back(var_name);
      }
    }
    return roc_var_names;
  } // end Ui_GetRocRegistersNames()

//==============================================================================
///	Ui_GetRocRegisters()
/// This is just an example, needs to be implemented for each subsystem
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<uint32_t> ROCTrackerInterface::Ui_GetRocRegisters(int ilink, bool history) 
{
    std::vector<uint32_t> roc_reg;
    // Basic ROC registers
    if(history) {
      try { 
        std::vector<uint16_t> spi_raw_data;
        Ui_ControlRoc_ControlRoc_ReadSpi(spi_raw_data,ilink,0);
        
        for (int iw=0; iw<trkdaq::TrkSpiDataNWords; iw++) {
          roc_reg.emplace_back(spi_raw_data[iw]);
        }
      }
      catch(...) {
        TLOG(TLVL_ERROR) << "failed to read DTC:" << fPcieAddr << " ROC:" << ilink << " SPI";
//-----------------------------------------------------------------------------
// set ROC status to -1
//-----------------------------------------------------------------------------
        // TODO
      }
    } else {
      roc_reg.reserve(RocRegisters.size());
        try {
          for (const int reg : RocRegisters) {
            // ROC registers store 16-bit words, don't know how to std::declare an array
            // of shorts for ODBXX, use uint32_t
            uint32_t dat = getDTC()->ReadROCRegister(DTCLib::DTC_Link_ID(ilink),reg,100); 
            roc_reg.emplace_back(dat);
          }
        } catch (...) {
          TLOG(TLVL_ERROR) << "failed to read DTC:" << fPcieAddr << " ROC:" << ilink << " registers";
        }
    }
    return roc_reg;
  } // end Ui_GetRocRegisters()

//==============================================================================
///	Ui_GetConvertedRocRegisters()
/// This is just an example, needs to be implemented for each subsystem
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::vector<float> ROCTrackerInterface::Ui_GetConvertedRocRegisters(int ilink, bool history) 
{
    std::vector<float> roc_reg;
    // Basic ROC registers
    if(history) {
      try { 
        std::vector<uint16_t> spi_raw_data;
        struct trkdaq::TrkSpiData_t   spi;
        Ui_ControlRoc_ControlRoc_ReadSpi(spi_raw_data,ilink,0);
        Ui_ControlRoc_ConvertSpiData    (spi_raw_data,&spi,0);
              
        std::vector<float> roc_spi;
              
        for (int iw=0; iw<trkdaq::TrkSpiDataNWords; iw++) {
          roc_spi.emplace_back(spi.Data(iw));
        }
      }
      catch(...) {
        TLOG(TLVL_ERROR) << "failed to read DTC:" << fPcieAddr << " ROC:" << ilink << " SPI";
//-----------------------------------------------------------------------------
// set ROC status to -1
//-----------------------------------------------------------------------------
        // TODO
      }
    } else {
        auto val = Ui_GetRocRegisters(ilink, history);
        return std::vector<float>(val.begin(), val.end());
    }
    return roc_reg;
  } // end Ui_GetConvertedRocRegisters()

//==============================================================================
///	Ui_GetRocID()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_GetRocID(int Link) 
{
    std::string roc_id("READ_ERROR");
    if (LinkEnabled(Link)) {
      trkdaq::ControlRoc_DeviceID_t devid;
      int rc = Ui_ControlRoc_ControlRoc_ReadDeviceID(Link,devid);
      if (rc == 0) {
        roc_id = devid.DeviceSerial;
      }
    }
    else {
      TLOG(TLVL_ERROR) << "DTC:" << fPcieAddr << " Link:" << Link << " is not enabled";
    }
    return roc_id;
  } // end Ui_GetRocID()

//==============================================================================
///	Ui_GetRocDesignInfo()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_GetRocDesignInfo(int Link) 
{
    std::string design_info("READ_ERROR");
    
    if (LinkEnabled(Link)) {
      trkdaq::ControlRoc_DeviceID_t devid;
      int rc = Ui_ControlRoc_ControlRoc_ReadDeviceID(Link,devid);
      if (rc == 0) {
        design_info = devid.DesignInfo;
      }
    }
    else {
      TLOG(TLVL_ERROR) << "DTC:" << fPcieAddr << " Link:" << Link << " is not enabled";
    }
    return design_info;
  } // end Ui_GetRocDesignInfo()

//==============================================================================
///	Ui_GetRocFwGitCommit()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
std::string ROCTrackerInterface::Ui_GetRocFwGitCommit(int Link) 
{
    std::string s("READ_ERROR");

    if (LinkEnabled(Link)) {
      Ui_ControlRoc_ControlRoc_ReadGitCommit(s,Link);
    }
    else {
      TLOG(TLVL_ERROR) << "DTC:" << fPcieAddr << " Link:" << Link << " is not enabled";
    }
    return s;
  } // end Ui_GetRocFwGitCommit()

//==============================================================================
///	Ui_ProgramAndQueryThreshold()
/// ejc
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
float ROCTrackerInterface::Ui_ProgramAndQueryThreshold(const int Link,
                                               const int ChannelID,
                                               const int PreampType,
                                               const DTCLib::roc_data_t dac)
{
    uint32_t mask_lo = 0x00000000;
    uint32_t mask_md = 0x00000000;
    uint32_t mask_hi = 0x00000000;
    if (ChannelID < 32){
      mask_lo += (1 << (ChannelID -  0));
    }
    else if (ChannelID < 64){
      mask_md += (1 << (ChannelID - 32));
    }
    else if (ChannelID < 96){
      mask_hi += (1 << (ChannelID - 64));
    }
    std::vector<float> queried;
    queried.reserve(96);
    this->Ui_ControlRoc_ControlRoc_SetThreshold(Link, ChannelID, PreampType, dac);
    this->Ui_ControlRoc_ControlRoc_ReadThresholds(Link, queried, mask_lo, mask_md, mask_hi);
    auto idx = 3*ChannelID + (1 - PreampType);
    auto rv = queried.at(idx);
    return rv;
  } // end Ui_ProgramAndQueryThreshold()

//==============================================================================
///	Ui_FindThreshold()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
bool ROCTrackerInterface::Ui_FindThreshold(const int Link,
                                   const int ChannelID,
                                   const int PreampType,
                                   const float threshold,
                                   const float tolerance,
                                   DTCLib::roc_data_t& out)
{
    DTCLib::roc_data_t lower = 0;
    DTCLib::roc_data_t upper = 1023;
    auto f = [this, Link, ChannelID, PreampType] (DTCLib::roc_data_t dac){
      this->Ui_ControlRoc_ControlRoc_SetThreshold(Link, ChannelID, PreampType, dac);
      auto rv = this->Ui_ProgramAndQueryThreshold(Link, ChannelID, PreampType, dac);
      return rv;
    };

    auto dac = bisection_search(f, -threshold, tolerance, lower, upper);
    auto measured = this->Ui_ProgramAndQueryThreshold(Link, ChannelID, PreampType, dac);

    // return whether or not the search was successful
    auto rv = false;
    if (fabs((-measured) - threshold) < tolerance){
      rv = true;
      out = dac;
    }

    return rv;
  } // end Ui_FindThreshold()

//==============================================================================
///	Ui_FindThreshold()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface.cc on Tue Dec 16 13:34:56 2025 CST
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
bool ROCTrackerInterface::Ui_FindThreshold(const int Link,
                                   const int ChannelID,
                                   const int PreampType,
                                   const float threshold,
                                   const float tolerance)
{
    DTCLib::roc_data_t tmp;
    auto rv = this->Ui_FindThreshold(Link, ChannelID, PreampType,
                                  threshold, tolerance, tmp);
    return rv;
  } // end Ui_FindThreshold()

