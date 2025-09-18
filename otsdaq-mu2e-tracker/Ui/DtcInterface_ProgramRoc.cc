//
#include <vector>
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterface_ProgramRoc"

#include "TString.h"     // includes ROOT's Form

using namespace DTCLib;

namespace  trkdaq {
//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  int DtcInterface::ProgramRoc(int Link, const char* Version, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiWriteDirectory(int Link, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiWriteRecord(int Link, int FirstAddr, int NWords, const uint16_t* Data,
                                   int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiClearMemory(int Link, int Index, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiReadFlash(int Link, int Address, int NWords, std::vector<uint16_t>* Res,
                                 int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiLoadImage(int Link, int Address, int NWords, const uint16_t* Data,
                                 int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiIapIndex(int Link, int Index, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiIapAddress(int Link, int Index, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }
  
};
