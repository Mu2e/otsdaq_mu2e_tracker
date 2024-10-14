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
#include "iostream"
#include "dtcInterfaceLib/DTC.h"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"

#include "otsdaq-mu2e-tracker/ParseAlignment/Alignment.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/PrintLegacyTable.hh"
#include "otsdaq-mu2e-tracker/Ui/TrkSpiData.hh"
#include "otsdaq-mu2e-tracker/Ui/ControlRoc_Read_Par_t.hh"

namespace trkdaq {
  using roc_serial_t = std::string;

  class DtcInterface { 
  public:
    static DtcInterface* fgInstance[2];

    DTCLib::DTC*         fDtc;
    int                  fPcieAddr;
    int                  fLinkMask;       // int is OK, bit 31 is never used for arithmetics
                                          // for now assume that all ROCs are doing the same
    int                  fReadoutMode;    // 0: patterns 1:digis
    int                  fSampleEdgeMode; // 0:force raising 1:force falling 2:auto
    int                  fEmulateCfo;     // 1: this DTC operated in the emulated CFO mode
    int                  fJAMode;         // clock_source << 4 | reset

    int                  fSleepTimeROCWrite; // the two are different 
    int                  fSleepTimeROCReset;
    int                  fPrintLevel;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  private:
    DtcInterface(int PcieAddr, uint LinkMask, bool SkipInit);
  public:
    virtual ~DtcInterface();

    static DtcInterface* Instance(int PcieAddr, uint LinkMask = 0x11, bool SkipInit = false);

    int PcieAddr() { return fPcieAddr; }

    DTCLib::DTC* Dtc() { return fDtc; }
                                        // clock source=0: internal, clock=1: RTF (RJ45)
    
    int          ConfigureJA(int ClockSource, int Reset = 1);
//-----------------------------------------------------------------------------
// generic interface to control_ROC.py commands.
// When/if we figure how to do it better, we'll implement a better solution
//-----------------------------------------------------------------------------
    int          ControlRoc(const char* Command, void* Parameters);
    
    int          ControlRoc_Read(ControlRoc_Read_Par_t* Par,
                                 int                    LinkMask   = -1   ,
                                 bool                   UpdateMask = false,
                                 int                    PrintLevel = 0    );

    int          EmulateCfo() { return fEmulateCfo; }

                                        // EWLength - in 25 ns ticks
                                        // to be executed on the emulated CFO side
    
    void         InitEmulatedCFOReadoutMode();
    void         LaunchRunPlanEmulatedCfo  (int EWLength, int NMarkers, int FirstEWTag);

                                        // SampleEdgeMode=0: force rising  edge
                                        //                1: force falling edge
                                        //                2: auto
                                        // -1 means use the pre-fetched one

    void         InitExternalCFOReadoutMode(int SampleEdgeMode = -1);

    void         InitReadout       (int EmulateCfo = -1, int RocReadoutMode = -1);
    void         InitRocReadoutMode();
    
    int          GetLinkMask() { return fLinkMask; }

    int          ConvertSpiData  (const std::vector<uint16_t>& RawData, TrkSpiData_t* Data, int PrintLevel = 0);
//-----------------------------------------------------------------------------
// assume that to be printed are 'nw' uint16_t words , in hex
//-----------------------------------------------------------------------------    
    void         PrintBuffer     (const void* ptr, int nw);
    void         PrintFireflyTemp();
    void         PrintRegister   (uint16_t Register, const char* Title = "");
//-----------------------------------------------------------------------------
// Format = 0 : for each register, print a register and its value
// Format = 1 : add short description of each register
// if Link = -1, print a line per register for each ROC
//-----------------------------------------------------------------------------
    void         PrintRocRegister (uint Reg, std::string& Desc, int Format = 1, int LinkMask = -1);
    void         PrintRocRegister2(uint Reg, std::string& Desc, int Format = 1, int LinkMask = -1);
    void         PrintRocStatus   (int Format = 1, int LinkMask = -1);
    void         PrintStatus      ();

    uint32_t     ReadRegister    (uint16_t Register);

    int          ReadSpiData     (int Link, std::vector<uint16_t>& SpiRawData, int PrintLevel = 0);

    void         ReadSubevents   (std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev, 
                                  ulong       FirstTS,
                                  int         PrintData,
                                  int         Validate = 0      , 
                                  const char* OutputFn = nullptr);

    std::vector<DTCLib::roc_data_t> ReadROCBlockEnsured(const DTCLib::DTC_Link_ID& Link,
                                                        const DTCLib::roc_address_t& address);
    std::vector<DTCLib::roc_data_t> ReadDeviceID        (const DTCLib::DTC_Link_ID& Link);
    roc_serial_t                    ReadSerialNumber(const DTCLib::DTC_Link_ID& Link);

    Alignment    FindAlignment(DTCLib::DTC_Link_ID Link);
    void         FindAlignments(bool print=false, int LinkMask=0);
//-----------------------------------------------------------------------------
// ROC functions
// if LinkMask=0, use fLinkMask
//-----------------------------------------------------------------------------
    void         ResetRoc               (int LinkMask = 0, int SetNewMask = 0);

    int          RocReadoutMode         ()  { return fReadoutMode; }
    
    void         RocConfigurePatternMode(int LinkMask = 0);
    void         RocSetDataVersion      (int Version, int LinkMask=0);

    void         SetRocReadoutMode      (int Mode) { fReadoutMode = Mode; }
    
                                        // 'Value' : 0 or 1
    void         SetBit     (int Register, int Bit, int Value);

    void         SetJAMode  (int Mode) { fJAMode = Mode; }

    void         SetLinkMask(int Mask = 0);
//-----------------------------------------------------------------------------
// ForceCFOEdge: bit_6 and bit_5 of the control register 0x9100
// bit_6: 1:force       0:auto
// bit_5: 0:rising edge 1:falling edge
//-----------------------------------------------------------------------------    
    void         SetupCfoInterface(int CFOEmulationMode, 
                                   int ForceCFOEdge    , 
                                   int EnableCFORxTx   , 
                                   int EnableAutogenDRP);
//-----------------------------------------------------------------------------
// return number of found errors
//-----------------------------------------------------------------------------
    int          ValidateDtcBlock(ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel, int* NErrRoc);
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
    
    int          MonicaVarPatternConfig(int LinkMask = 0);
  };

};

#endif
