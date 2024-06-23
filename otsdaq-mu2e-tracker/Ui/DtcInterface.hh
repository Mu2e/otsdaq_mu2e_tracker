//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __trkdaq_dtc_interface_hh__
#define __trkdaq_dtc_interface_hh__

#define __CLING__ 1

#include "iostream"
#include "dtcInterfaceLib/DTC.h"

namespace trkdaq {

  class DtcInterface { 
  public:
    static DtcInterface* fgInstance[2];

    DTCLib::DTC*         fDtc;
    int                  fPcieAddr;
    int                  fLinkMask;     // int is OK, bit 31 is never used for arithmetics
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  private:
    DtcInterface(int PcieAddr, uint LinkMask);
  public:
    virtual ~DtcInterface();

    static DtcInterface* Instance(int PcieAddr, uint LinkMask = 0x11);

    int PcieAddr() { return fPcieAddr; }

    DTCLib::DTC* Dtc() { return fDtc; }

                                        // clock source=0: internal, clock=1: RTF (RJ45)
    
    int          ConfigureJA(int ClockSource, int Reset = 1);

                                        // EWLength - in 25 ns ticks
    void         InitEmulatedCFOReadoutMode(int EWLength, int NMarkers, int FirstEWTag);

                                        // SampleEdgeMode=0: force rising  edge
                                        //                1: force falling edge
                                        //                2: auto

    void         InitExternalCFOReadoutMode(int SampleEdgeMode);
    
                                        // launch "run plan" in emulated mode...
    void         LaunchRunPlan (int NEvents);

    int          GetLinkMask() { return fLinkMask; }

    void         PrintBuffer     (const void* ptr, int nw);
    void         PrintFireflyTemp();
    void         PrintRegister   (uint16_t Register, const char* Title = "");
    void         PrintRocStatus  (int Link);
    void         PrintStatus     ();

    uint32_t     ReadRegister    (uint16_t Register);

    void         ReadSubevents   (std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev, 
                                  ulong       FirstTS,
                                  int         PrintData,
                                  const char* OutputFn = nullptr);

    void         ResetRoc        (int Link);
    void         RocPatternConfig();
                                        // 'Value' : 0 or 1
    void         SetBit(int Register, int Bit, int Value);
    //-----------------------------------------------------------------------------
// ForceCFOEdge: bit_6 and bit_5 of the control register 0x9100
// bit_6: 1:force       0:auto
// bit_5: 0:rising edge 1:falling edge
//-----------------------------------------------------------------------------    
    void         SetupCfoInterface(int CFOEmulationMode, 
                                   int ForceCFOEdge    , 
                                   int EnableCFORxTx   , 
                                   int EnableAutogenDRP);

    
  };

};

#endif
