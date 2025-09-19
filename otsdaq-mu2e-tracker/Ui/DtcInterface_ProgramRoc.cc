//
#include <vector>
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "TRACE/tracemf.h"
#define  TRACE_NAME "DtcInterface_ProgramRoc"

#include "TString.h"     // includes ROOT's Form

using namespace DTCLib;

namespace  trkdaq {
  RocFwData_t RocFw = {
                                        // spi_directory
    {
      { 0,   0x10000, "/home/mu2etrk/test_stand/spi_files/GoldenV10.spi"          }, // 9524032 },
      { 1, 0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV12.spi"             }, // 9480352 },
      { 2, 0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV12-3_stage3init.bin"}, //   82272 },
      { 3, 0x2010000, "/home/mu2etrk/test_stand/spi_files/ROCV14.spi"             }, // 9482832 },
      { 4, 0x5040000, "/home/mu2etrk/test_stand/spi_files/ROCV14_stage3init.bin"  }, //   86384 },
      {-1,        -1, ""                                                          },  //      -1 }
      {-1,        -1, ""                                                          },  //      -1 }
      {-1,        -1, ""                                                          },  //      -1 }
      {-1,        -1, ""                                                          },  //      -1 }
      {-1,        -1, ""                                                          }   //      -1 }
    },
                                        // firmware versions = pairs of images
    {
      { "GoldenV10",  0, -1 },
      { "ROCV12",     1,  2 },
      { "ROCV14",     3,  4 },
      { "",          -1, -1 },
      { "",          -1, -1 },
      { "",          -1, -1 },
      { "",          -1, -1 },
      { "",          -1, -1 },
      { "",          -1, -1 },
      { "",          -1, -1 }
    }
  };

//-----------------------------------------------------------------------------
  int DtcInterface::SpiClearMemory(int Link, int Index, std::ostream& Stream) {
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
  
//-----------------------------------------------------------------------------
  int DtcInterface::SpiWriteDirectory(int Link, const roc_fw_data_t* SpiDirectory, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
  int DtcInterface::SpiLoadImage(int Link, int Index, const roc_fw_data_t* SpiDirectory, int PrintLevel, std::ostream& Stream) {
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
  int DtcInterface::SpiWriteRecord(int Link, int FirstAddr, int NWords, const uint16_t* Data, int PrintLevel, std::ostream& Stream) {
    int rc(0);
    return rc;
  }

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
  int DtcInterface::ProgramRoc(int Link, const char* Version, const RocFwData_t* Fw, int PrintLevel, std::ostream& Stream) {
    int rc(0);
                                        // offset=-1 flags the end
    int nimages(0);
    for (int i=0; Fw->spi_directory[i].offset>=0; i++) nimages++;
    
                                        // 2. find if the requested firmware version is available
    const roc_fw_version_t* fw(nullptr);
  
    for (int i=0; Fw->version[i].name != ""; ++i) {
      const roc_fw_version_t* fww = &Fw->version[i];
      if (fww->name == Version) {
                                        // protect against trivial mistakes
        if ((fww->index_spi < nimages) and (fww->index_bin < nimages)) { 
          fw = fww;
        }
        break;
      }
    }

    if (fw == nullptr) {
      Stream << "ERROR: fw version:" << Version << " is not defined, BAIL OUT." << std::endl;
      return -1;
    }
//-----------------------------------------------------------------------------
// 3. every time, fully rewrite the SPI directory
//-----------------------------------------------------------------------------
    SpiWriteDirectory(Link,Fw->spi_directory,PrintLevel,Stream);

    int test_mode = 0;                  // 0: do it for for real, not testing
  
                                        // upload the .spi image
    
    SpiLoadImage(Link,fw->index_spi,test_mode);

                                        // upload the .bin image, if defined
    if (fw->index_bin >= 0) {       
      SpiLoadImage(Link,fw->index_bin,test_mode);
    }
                                        // activate the image
    SpiIapIndex(Link,fw->index_spi);
    
    return rc;
  }

};
