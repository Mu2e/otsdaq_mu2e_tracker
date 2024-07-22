//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_interface_hh__
#define __trkdaq_dtc_interface_hh__

#define __CLING__ 1

#include <vector>
#include "iostream"
#include "dtcInterfaceLib/DTC.h"

#include "otsdaq-mu2e-tracker/Ui/TrkSpiData.hh"

namespace trkdaq {

  class DtcInterface { 
  public:
    static DtcInterface* fgInstance[2];

    DTCLib::DTC*         fDtc;
    int                  fPcieAddr;
    int                  fLinkMask;       // int is OK, bit 31 is never used for arithmetics
                                          // for now assume that all ROCs are doing the same
    int                  fReadoutMode;    // 0: patterns 1:data
    int                  fSampleEdgeMode; // 0:force raising 1:force falling 2:auto
    int                  fEmulatesCfo;    // 1: this DTC operated in the emulated CFO mode
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

    int          EmulatesCfo() { return fEmulatesCfo; }


    int          MonicaDigiClear(int LinkMask=0);

                                        // EWLength - in 25 ns ticks
    
    void         InitEmulatedCFOReadoutMode(int EWLength, int NMarkers, int FirstEWTag);

                                        // SampleEdgeMode=0: force rising  edge
                                        //                1: force falling edge
                                        //                2: auto

    void         InitExternalCFOReadoutMode(int SampleEdgeMode);
    
    int          GetLinkMask() { return fLinkMask; }

    int          ConvertSpiData  (const std::vector<uint16_t>& RawData, TrkSpiData_t* Data, int PrintLevel = 0);

    // assume that printed are uint16_t words , in hex
    void         PrintBuffer     (const void* ptr, int nw);
    void         PrintFireflyTemp();
    void         PrintRegister   (uint16_t Register, const char* Title = "");
    void         PrintRocStatus  (int Link);
    void         PrintStatus     ();

    uint32_t     ReadRegister    (uint16_t Register);

    int          ReadSpiData     (int Link, std::vector<uint16_t>& SpiRawData, int PrintLevel = 0);

    void         ReadSubevents   (std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev, 
                                  ulong       FirstTS,
                                  int         PrintData,
                                  int         Validate = 0      , 
                                  const char* OutputFn = nullptr);
//-----------------------------------------------------------------------------
// ROC functions
// if LinkMask=0, use fLinkMask
//-----------------------------------------------------------------------------
    void         ResetRoc               (int LinkMask = 0);

    int          RocReadoutMode         ()  { return fReadoutMode; }
    
    void         RocConfigurePatternMode(int LinkMask = 0);
    void         RocSetDataVersion      (int Version, int LinkMask=0);

    void         SetRocReadoutMode      (int Mode) { fReadoutMode = Mode; }
    
                                        // 'Value' : 0 or 1
    void         SetBit(int Register, int Bit, int Value);

    void         SetLinkMask(int Mask);
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
    int          ValidateDtcBlock(ushort* Data, ulong EwTag, ulong* Offset, int PrintLevel);

//-----------------------------------------------------------------------------
// ROC has 4 lanes: 2 CAl lanes (0x5) and 2 HV lanes (0xa)
//-----------------------------------------------------------------------------
    int          MonicaVarLinkConfig   (int LinkMask = 0, int LaneMask = 0xf);
//-----------------------------------------------------------------------------
// VarPatternConfig = RocConfigurePatternMode
//-----------------------------------------------------------------------------
    
    int          MonicaVarPatternConfig(int LinkMask = 0);
  };

};

#endif
