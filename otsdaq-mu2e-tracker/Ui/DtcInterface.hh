//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_interface_hh__
#define __trkdaq_dtc_interface_hh__

#define __CLING__ 1

#include <string>
#include <vector>
#include <sstream>
#include "iostream"
#include "dtcInterfaceLib/DTC.h"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"

#include "otsdaq-mu2e-tracker/ParseAlignment/Alignment.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/PrintLegacyTable.hh"
#include "otsdaq-mu2e-tracker/Ui/ControlRocTypes.hh"
#include "otsdaq-mu2e-tracker/Ui/DtcInterfaceBase.hh"

namespace trkdaq {
  using roc_serial_t = std::string;

  class DtcInterface : public mu2edaq::DtcInterface { 
    private:
      DtcInterface(int PcieAddr, uint LinkMask, bool SkipInit);
    public:
      roc_serial_t                    ReadSerialNumber(const DTCLib::DTC_Link_ID& Link);

//-----------------------------------------------------------------------------
// ROC functions
// if LinkMask=0, use fLinkMask
//-----------------------------------------------------------------------------
    void         ResetRoc               (int LinkMask = 0, int SetNewMask = 0);
    void         RocConfigurePatternMode(int LinkMask = 0);
    void         RocSetDataVersion      (int Version, int LinkMask=0);

    static const char*   fgSpiVarName[TrkSpiDataNWords]; //
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  public:
    static const char*  SpiVarName(int I) { return fgSpiVarName[I]; }
    static DtcInterface* Instance(int PcieAddr, uint LinkMask = 0x11, bool SkipInit = false);
    static const char*  SpiVarNaPrintBufferme(int I) { return fgSpiVarName[I]; }

    std::vector<DTCLib::roc_data_t> ReadDeviceID        (const DTCLib::DTC_Link_ID& Link);
//-----------------------------------------------------------------------------
// generic interface to control_ROC.py commands.
// When/if we figure how to do it better, we'll implement a better solution
//-----------------------------------------------------------------------------
    int          ControlRoc(const char* Command, void* Parameters);

    // need: digi_rw -h 0 -w 1 -a 0x82 -d 0x1388
    int          ControlRoc_DigiRW(ControlRoc_DigiRW_Input_t*  Input          ,
                                   ControlRoc_DigiRW_Output_t* Output         ,
                                   int                         LinkMask   = -1,
                                   int                         PrintLevel =  0);
    
    int          ControlRoc_Read(ControlRoc_Read_Input_t* Par               ,
                                 int                      LinkMask   = -1   ,
                                 bool                     UpdateMask = false,
                                 int                      PrintLevel = 0    ,
                                 std::ostream&            Stream     = std::cout);
//-----------------------------------------------------------------------------
// measure thresholds returns an array of thresholds, which needs to be parsed
// so far, do it internally
//-----------------------------------------------------------------------------
    int          ControlRoc_MeasureThresholds(int      Link,
                                              uint32_t MaskC = 0xFFFFFFFF,
                                              uint32_t MaskD = 0xFFFFFFFF,
                                              uint32_t MaskE = 0xFFFFFFFF);
//-----------------------------------------------------------------------------
// PreampType: 0:HV 1:CAL, or vice versa
// do one channel at a time
// shall we think of a block operation ? or not ? - channels could be masked OFFx
//-----------------------------------------------------------------------------
    int          ControlRoc_SetGain     (int Link, int ChannelID, int PreampType, int Gain     );
    int          ControlRoc_SetThreshold(int Link, int ChannelID, int PreampType, int Threshold);

    int          ConvertSpiData(const std::vector<uint16_t>& RawData, TrkSpiData_t* Data, int PrintLevel = 0);

    void         InitRocReadoutMode() override;

//-----------------------------------------------------------------------------
// assume that to be printed are 'nw' uint16_t words , in hex
// if Stream == nullptr, PrintBuffer uses TRACE's TLOG
//-----------------------------------------------------------------------------    
    void         PrintBuffer     (const void* ptr, int nw, std::ostream* Stream = nullptr);
    
//-----------------------------------------------------------------------------
// Format = 0 : for each register, print a register and its value
// Format = 1 : add short description of each register
// if Link = -1, print a line per register for each ROC
//-----------------------------------------------------------------------------
    void         PrintRocRegister (uint Reg, std::string& Desc, int Format = 1, int LinkMask = -1, std::ostream& Stream = std::cout);
    void         PrintRocRegister2(uint Reg, std::string& Desc, int Format = 1, int LinkMask = -1, std::ostream& Stream = std::cout);
    void         PrintRocStatus   (int Format = 1, int LinkMask = -1, std::ostream& Stream = std::cout);

    int          ReadSpiData     (int Link, std::vector<uint16_t>& SpiRawData, int PrintLevel = 0);

    void         ReadSubevents   (std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev, 
                                  ulong       FirstTS,
                                  int         PrintData,
                                  int         Validate = 0      , 
                                  const char* OutputFn = nullptr);

    std::vector<DTCLib::roc_data_t> ReadROCBlockEnsured(const DTCLib::DTC_Link_ID& Link,
                                                        const DTCLib::roc_address_t& address);

    Alignment    FindAlignment(DTCLib::DTC_Link_ID Link);
    void         FindAlignments(bool print=false, int LinkMask=0);

    void         SetRocLaneMask    (int Mask ) { fRocLaneMask     = Mask ; }
    void         SetRocNHitsPerLane(int NHits) { fRocNHitsPerLane = NHits; }

//-----------------------------------------------------------------------------
// return number of found errors
//-----------------------------------------------------------------------------
    int          ValidateDigiPatterns (ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
    int          ValidateFixedPatterns(ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
    int          ValidateVarPatterns  (ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
//-----------------------------------------------------------------------------
// reset digitizers .. to be called in the beginning of each event 
//-----------------------------------------------------------------------------
    int          MonicaDigiClear(int LinkMask = 0);
//-----------------------------------------------------------------------------
// ROC has 4 lanes: 2 CAL lanes (0x5) and 2 HV lanes (0xa)
//-----------------------------------------------------------------------------
    int          MonicaVarLinkConfig   (int LinkMask = 0, int LaneMask = 0xf);
//-----------------------------------------------------------------------------
// VarPatternConfig = RocConfigurePatternMode
//-----------------------------------------------------------------------------
    int          MonicaVarPatternConfig(int LinkMask = 0, int LaneMask = -1, int NHits = -1);
  };

  struct RocDataHeaderPacket_t {        // 8 16-byte words in total
                                        // 16-bit word 0
    uint16_t            byteCount    : 16;
                                        // 16-bit word 1
    uint16_t            unused       : 4;
    uint16_t            packetType   : 4;
    uint16_t            linkID       : 3;
    uint16_t            DtcErrors    : 4;
    uint16_t            valid        : 1;
                                        // 16-bit word 2
    uint16_t            packetCount  : 11;
    uint16_t            unused2      : 2;
    uint16_t            subsystemID  : 3;
                                        // 16-bit words 3-5
    uint16_t            eventTag[3];
                                        // 16-bit word 6
    uint8_t             status       : 8;
    uint8_t             version      : 8;
                                        // 16-bit word 7
    uint8_t             dtcID        : 8;
    uint8_t             onSpill      : 1;
    uint8_t             subrun       : 2;
    uint8_t             eventMode    : 5;

    ulong ewtag() {
      ulong x1 = eventTag[0];
      ulong x2 = eventTag[1];
      ulong x3 = eventTag[2];
      ulong ewt = x1 | (x2 << 16) | (x3 << 32);
      return ewt;
    }
    
                                        // decoding status
      
    int                 empty     () { return (status & 0x01) == 0; }
    int                 invalid_dr() { return (status & 0x02); }
    int                 corrupt   () { return (status & 0x04); }
    int                 timeout   () { return (status & 0x08); }
    int                 overflow  () { return (status & 0x10); }
      
    int                 error_code() { return (status & 0x1e); }
  };
  
  struct RocData_t {                    // 8 16-byte words in total
    RocDataHeaderPacket_t header;
    uint16_t              data[1];
  };
  
  // struct RocData_t {
  // ushort  nb;
  // ushort  header;
  // ushort  n_data_packets;  // n data packets, 16 bytes each
  // ushort  ewt[3];
  // ushort  status;
  // ushort  xxx2;
  // ushort  data; // array, use it juxsst for memory mapping
  
  // int                 empty     () { return (status & 0x01) == 0; }
  // int                 invalid_dr() { return (status & 0x02); }
  // int                 corrupt   () { return (status & 0x04); }
  // int                 timeout   () { return (status & 0x08); }
  // int                 overflow  () { return (status & 0x10); }
      
  // int                 error_code() { return (status & 0x1e); }
  //};

};

#endif
