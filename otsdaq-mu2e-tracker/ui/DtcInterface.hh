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

                                        // EWLength - in 25 ns ticks
    void         InitEmulatedCFOMode(int EWLength, int NMarkers, int FirstEWTag);
    void         InitExternalCFOMode();
                                        // launch "run plan" in emulated mode...
    void         LaunchRunPlan (int NEvents );

    void         PrintBuffer   (const void* ptr, int nw);
    void         PrintRegister (uint16_t Register, const char* Title = "");
    void         PrintRocStatus(int Link);
    void         PrintStatus   ();

    uint32_t     ReadRegister    (uint16_t Register);
    void         ReadSubevents   (std::vector<std::unique_ptr<DTCLib::DTC_SubEvent>>& Vsev, 
                                  ulong      FirstTS,
                                  int        PrintData );

    void         ResetRoc        (int Link);
    void         RocPatternConfig(int LinkMask);

    void         SetupCfoInterface(int CFOEmulationMode, 
                                   int ForceCFOEdge    , 
                                   int EnableCFORxTx   , 
                                   int EnableAutogenDRP);

    
  };

};

#endif
