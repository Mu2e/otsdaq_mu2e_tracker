//-----------------------------------------------------------------------------
// interactive interface for ROOT-based GUI
// mixes high- and low-level commands
// assume everything is happening on one node
// there could be one or two DTCs and only one CFO
//-----------------------------------------------------------------------------
#ifndef __mu2edaq_dtc_interface_hh__
#define __mu2edaq_dtc_interface_hh__

#define __CLING__ 1

#include <string>
#include <vector>
#include "iostream"
#include "dtcInterfaceLib/DTC.h"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"

#include "otsdaq-mu2e-tracker/ParseAlignment/Alignment.hh"
#include "otsdaq-mu2e-tracker/ParseAlignment/PrintLegacyTable.hh"
#include "otsdaq-mu2e-tracker/Ui/ControlRocTypes.hh"
#include "TString.h"

namespace mu2edaq {

  enum {
    kTracker     = 1,
    kCalorimeter = 2,
    kCRV         = 3,
    kSTM         = 4,
  };

  struct RocInputData_t {
    TString fName;         // 
    int     fLink;
  };

  struct DtcInputData_t {
    TString    fName;                   // expect fName to be uppercased
    int        fPcieAddr;
    int        fLinkMask;               // active links, for DTC - ROCs, for CFO: nDTCs
    int        fRocReadoutMode;
    int        fRocLaneMask;
    int        fRocNHitsPerLane;
    
    int        fJAMode;
    int        fOnSpill;

    int        fDtcID;                  // 4 pieces to be written to 0x9154
    int        fEventMode;              // for CFO
    int        fPartitionID;
    int        fMacAddrByte;
    
    int        fEmulateCfo;

    RocInputData_t  fRocData[6];
    RocInputData_t* fActiveRoc;

    DtcInputData_t(const char* Name = "", int PcieAddr = 0) {
      fName            = Name;
      fPcieAddr        = PcieAddr;
      fLinkMask        = 0;             // by default, not reading anything
      fRocReadoutMode  = 0;             // 0:patterns 1:digis
      fRocLaneMask     = 0xf;
      fRocNHitsPerLane = 2;             // Monicas's default
      fJAMode          = 0;
      fOnSpill         = 0;
      fDtcID           = -1;
      fPartitionID     = -1;
      fEventMode       =  1;
      fMacAddrByte     = -1;


      fActiveRoc = nullptr;
      for (int i=0;i<6; i++) {
        fRocData[i].fName = Form("ROC%i",i);
        fRocData[i].fLink = i;
      }
    }

    int IsDtc() { return fName == "DTC"; }
    int IsCfo() { return fName == "CFO"; }
  };

  class DtcInterface { 
  public:
    static DtcInterface* fgInstance[2];

    DTCLib::DTC*         fDtc;
    int                  fEnabled;           // if comes from ODB, could be 0
    int                  fPcieAddr;          // 
    int                  fLinkMask;          // int is OK, bit 31 is never used for arithmetics
                                             // for now assume that all ROCs are doing the same
                                             // fRocReadoutMode: (fixed_length << 4) | readout_mode
    int                  fRocReadoutMode;    // 0: 'counter patterns' 1:digis 2:checkerboard patterns
    int                  fSampleEdgeMode;    // 0:force raising 1:force falling 2:auto
    int                  fEmulateCfo;        // 1: this DTC operated in the emulated CFO mode
    int                  fJAMode;            // clock_source << 4 | reset

    int                  fOnSpill;           // 1:on-spill, 0:off-spill
    int                  fEventMode;         // whatever it is, hopefully, together they make 5 bytes

    int                  fDtcID;             // unique DTC ID used by the DAQ (0x9154)
    int                  fPartitionID;
    int                  fMacAddrByte;
    //    int                  fDtcEwmDelay5ns;    // 'per-DTC' delay in units of 5ns, common for all ROCs
    int                  fRocEwmDelay5ns[6]; // 'per-ROC' delays, to be added to the common one above

    // int                  fDigitizationStart5ns;  // digitization window to be set in the DIGIs, in units of 5 ns
    // int                  fDigitizationStop5ns ;

    int                  fSubsystem;         // 1:tracker 2:calorimeter 3:CRV 4:STM (better than IsCrv)

    int                  fSleepTimeROCWrite; // the two are different 
    int                  fSleepTimeROCReset; // 
    int                  fCounter;
//-----------------------------------------------------------------------------
// functions
//-----------------------------------------------------------------------------
  protected:
    DtcInterface(int PcieAddr, uint LinkMask, bool SkipInit);
    DtcInterface(DTCLib::DTC* Dtc) { fDtc = Dtc; }
  public:
    virtual ~DtcInterface();

    static DtcInterface* Instance(int PcieAddr, uint LinkMask = 0x11, bool SkipInit = false);

    int PcieAddr() { return fPcieAddr; }

    DTCLib::DTC* Dtc() { return fDtc; }
//-----------------------------------------------------------------------------
// if 'ClockSource' and 'Reset' are set to -1, use fJAMode
// clock source= 0:internal, 1:RTF (RJ45)
//-----------------------------------------------------------------------------    
    int          ConfigureJA(int ClockSource = -1, int Reset = -1);

    int          Enabled   () { return fEnabled;    }
    int          EmulateCfo() { return fEmulateCfo; }

    int64_t      EventMode () { return (((int64_t) fOnSpill) << 32) | ((int64_t) fEventMode); }

    int          DtcID     () { return fDtcID; }
    //    int          IsCrv     () { return fIsCrv; }

    int          InitReadout        (int EmulateCfo = -1, int RocReadoutMode = -1, std::ostream* Stream = nullptr);
    virtual int  InitRocReadoutMode(std::ostream* Stream = nullptr); 
    
    int          InitEmulatedCFOReadoutMode();
                                        // SampleEdgeMode=0: force rising  edge
                                        //                1: force falling edge
                                        //                2: auto
                                        // -1 means use the pre-fetched one
                                        // success: returns rc=0
                                        // if rc < 0, can't continue
    int          InitExternalCFOReadoutMode(int SampleEdgeMode = -1);

    // read configuration data from a file and store them in 'DtcData'
    static int   InitConfiguration(const char* ConfigName, int DeviceID, DtcInputData_t* DtcData);

                                        // EWLength - in 25 ns ticks
                                        // to be executed on the emulated CFO side
    
    void         LaunchRunPlanEmulatedCfo  (int EWLength, int NMarkers, int FirstEWTag);

    int          LinkEnabled(int Link) { return (fLinkMask >> 4*Link) & 0x1 ; }
    int          LinkLocked (int Link);
                                        // this EWM delay is common for all ROCs,
                                        // on top of that, each separate ROC has its own delay
    
    //    int          GetDtcEwmDelay5ns()         { return fDtcEwmDelay5ns; }
    int          GetRocEwmDelay5ns(int Link) { return fRocEwmDelay5ns[Link]; }
    
    int          GetLinkMask() { return fLinkMask; }
    void         PrintFireflyTemp(std::ostream& Stream = std::cout);
    
    void         PrintDtcLinkRegisters(uint     FirstReg, const char* Desc, std::ostream& Stream = std::cout);
    void         PrintRegister        (uint16_t Register, const char* Title = "",
                                       std::ostream& Stream = std::cout);
    void         PrintStatus          (std::ostream& Stream = std::cout);
    virtual int  PrintRocStatus       (uint32_t Format = 1, int Link = -1, std::ostream& Stream = std::cout);

    uint32_t     ReadRegister         (uint16_t Register);

//-----------------------------------------------------------------------------
// ROC functions
// if LinkMask=0, use fLinkMask
//-----------------------------------------------------------------------------
    int          ResetLinks             (int LinkMask = 0, int SetNewMask = 0);

    virtual int  ResetLink              (int Link);                              // no defaults here !
    int          RocReadoutMode         ()  { return fRocReadoutMode; }
    void         SetRocReadoutMode      (int Mode ) { fRocReadoutMode  = Mode ; }
    void         SetOnSpill             (int OnSpill) { fOnSpill        = OnSpill; }
    
                                        // 'Value' : 0 or 1
    void         SetBit        (int Register, int Bit, int Value);

    //    void         SetDtcEwmDelay5ns(int Delay5ns  ) { fDtcEwmDelay5ns = Delay5ns  ; }
    void         SetEmulateCfo    (int EmulateCfo) { fEmulateCfo     = EmulateCfo; }
//-----------------------------------------------------------------------------
// event mode is specified in the heartbeat packet, non-zero
// event mode=0 is reserved, last packet of the train
//-----------------------------------------------------------------------------
    void         SetEventMode (int Mode      ) { fEventMode  = Mode      ; }
    
                                        // just cache the DTC ID for future, to evolve

    void         SetJAMode    (int Mode      ) { fJAMode     = Mode;       }

    void         SetLinkMask  (int Mask = 0);

                                        // a simple setter, for now, no range check..
                                        // assume everyone knows that Link in [0,5]
    void         SetRocEwmDelay5ns(int Link, int Delay5ns) {
      fRocEwmDelay5ns[Link] = Delay5ns  ;
    }
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
// to be redefined in the derived classes
//-----------------------------------------------------------------------------
    virtual std::vector<std::string> GetRocRegistersNames     (           bool history = false);
    virtual std::vector<uint32_t>    GetRocRegisters          (int ilink, bool history = false);
    virtual std::vector<float>       GetConvertedRocRegisters (int ilink, bool history = false);

//-----------------------------------------------------------------------------
// ROC ID, firmware version ID, and the corresponding git commit
//-----------------------------------------------------------------------------
    virtual std::string              GetRocID         (int Link);
    virtual std::string              GetRocDesignInfo (int Link);
    virtual std::string              GetRocFwGitCommit(int Link);
  };

};

#endif
