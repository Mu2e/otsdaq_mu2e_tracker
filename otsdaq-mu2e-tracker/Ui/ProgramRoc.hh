///////////////////////////////////////////////////////////////////////////////
// PM: defaults for remote ROC programming
///////////////////////////////////////////////////////////////////////////////
#ifndef __DtcInterface_ProgramRoc_hh__
#define __DtcInterface_ProgramRoc_hh__

namespace trkdaq {
//-----------------------------------------------------------------------------
// GoldenV10 should always be there in the beginning
// the rest versions could be overriding each other
// name = nullptr: end of data, to avoid hardcoded constants
// assume that the image index is incremented monotonically
//-----------------------------------------------------------------------------
                                        // offsets come in pairs, an FPGA image assumes certain offset
                                        // of the corresponding microcontroller image. i.e.
                                        // 0x0010000 and  nothing ("Golden" ?)
                                        // 0x1010000 and 0x5000000
                                        // 0x2010000 and 0x5040000
                                        // 0x3010000 and 0x5080000
                                        // 0x4010000 and 0x50c0000
  struct roc_fw_data_t {
    int         index;                  // image offset index in SPI directory
    int         offset;                 // image offset in memory
    const char* fn;                     // image filename
  };

  struct roc_fw_version_t {
    std::string name;
    int         index_spi;       //
    int         index_bin;
  };

  struct RocFwData_t {
    roc_fw_data_t       spi_directory[10];
    roc_fw_version_t    version      [10];
  };
  
}
#endif
