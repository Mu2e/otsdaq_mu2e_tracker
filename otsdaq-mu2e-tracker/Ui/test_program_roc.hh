///////////////////////////////////////////////////////////////////////////////
// 1. spi_write_directory
// 2. spi_clear - done by spi_load_image .. Make sure the right address is used
// 3. spi_program_aip_w_index for the .spi file
// 4. wait for the upload to complete
// this is how the offsets should be:
//
// 0x0010000      : spi#0 ("Golden", no microcontroller code - why ?)
// 0x1010000      : spi#1
// 0x2010000      : spi#2
// 0x3010000      : spi#3
// 0x4010000      : spi#4
// 
// 0x5000000      : bin#1 (no bin#0)
// 0x5040000      : bin#2
// 0x5080000      : bin#2
// 0x50c0000      : bin#2
// 0x6000000      : bin#2
//-----------------------------------------------------------------------------
#ifndef __test_program_roc_hh__
#define __test_program_roc_hh__

#include "iostream"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"
#include "dtcInterfaceLib/DTC.h"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"
#include "TString.h"
#include "TSystem.h"

class test_program_roc {
public:
                                        // registers
  int RREG                  = 384;
  int REG_STATUS            = 132;
                                        // commands
  int SPI_CLEAR             = 3;
  int PROGRAM_IAP_W_INDEX   = 4;
  int PROGRAM_IAP_W_ADDRESS = 5;
  int IAP_AUTO_UPDATE       = 6;
  int SPI_FLASH_READ        = 7;
  int SPI_WRITE_RECORD      = 8;
  int SPI_WRITE_DIRECTORY   = 9;

                                        // description of the image
  struct ImageData_t {
    int         load_flag;
    int         offset;
    const char* fn;
    //    int         fsize;
  };

  struct FwVersion_t {
    std::string name;
    int         index;                  // in spi_offsets and bin_offsets
    ImageData_t spi_file;
    ImageData_t bin_file;
  };

  int kSPI_CLEAR_SLEEP_US;

  test_program_roc() {
    kSPI_CLEAR_SLEEP_US = 200000;       // 0.2 sec
  }

  const test_program_roc::ImageData_t* get_image_data(const std::string& Version, const std::string Spi="spi");
  const test_program_roc::FwVersion_t* get_version   (const std::string& Version);
  
  int  spi_clear_memory         (trkdaq::DtcInterface* Dtc_i, int Link, int Offset, int NBytes, int DebugMode = 0);

                                        // the next three functions do the FPGA programming, use the first one
  
  int  spi_program_roc          (trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Version);
  void spi_program_iap_w_index  (trkdaq::DtcInterface* Dtc_i, int Link, int Index);
  void spi_program_iap_w_address(trkdaq::DtcInterface* Dtc_i, int Link, int ImageStartAddr);

  int  spi_read_record          (trkdaq::DtcInterface* Dtc_i, int Link, uint32_t SpiOffset, int NWords, uint16_t*          Res, int DebugMode = 0);
  int  spi_read_segment         (trkdaq::DtcInterface* Dtc_i, int Link, uint32_t SpiOffset, int NBytes, std::vector<char>& Res, int DebugMode = 0);

  int  spi_validate_segment     (trkdaq::DtcInterface* Dtc_i, int Link, const test_program_roc::ImageData_t* SpiData, int Segment, int DebugMode = 0);
  int  spi_validate_image       (trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Version, const std::string& Type, int DebugMode = 0);

                                        // if Type = "", validate both images, otherwise use the file extention: "spi" or "bin"
  
  int  spi_validate_version     (trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Version, const std::string& Type = "", int DebugMode = 0);

  int  spi_write_directory      (trkdaq::DtcInterface* Dtc_i, int Link);

  int  spi_write_record         (trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr   , int NWords, uint16_t* Data, int DebugMode = 0);
  int  spi_write_segment        (trkdaq::DtcInterface* Dtc_i, int Link, const char* Data, int NBytes, int SpiOffset , int DebugMode = 0);
  int  spi_write_image          (trkdaq::DtcInterface* Dtc_i, int Link, const test_program_roc::ImageData_t* SpiData, int DebugMode = 0);

                                        // this one only uploads image to SPI memory, but doesn't program the FPGA
  
  int  spi_write_version        (trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Version, int DebugMode = 0);

                                        // tests
  
  int  test_spi_write_record    (trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords, int DelayUs = 0);
  int  test_spi_read_record     (trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords);

  
};
#endif
