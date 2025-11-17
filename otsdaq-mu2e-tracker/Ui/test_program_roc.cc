//-----------------------------------------------------------------------------
// TLVL_DEBUG  : standard begin and end 
// TLVL_DEBUG+1: spi_read_record end printouts (they come often)
// TLVL_DEBUG+2: spi_read_record start printouts (normally don't need both)
// bit 0x10000 : validate
//-----------------------------------------------------------------------------
#include "otsdaq-mu2e-tracker/Ui/test_program_roc.hh"

#include <thread>
#include "TRACE/tracemf.h"
#define  TRACE_NAME "test_program_roc"

//-----------------------------------------------------------------------------
// GoldenVXX should always be the first image ( index 0, offset 0x10000)
// the rest versions could be overriding each other
// name = "": end of data
// assume that the image index is incremented monotonically
// 'FwVersion_t.index' is the image index in the directory catalog
//-----------------------------------------------------------------------------
test_program_roc::FwVersion_t drac_fw[] = {
    { "GoldenV11",  0,
      { 1,    0x10000, "/home/mu2etrk/test_stand/spi_files/GoldenV11.spi"          , 9530672 },
      {-1,         -1, ""                                                          ,      -1 }
    },
    { "ROCV14",     1,
      { 1,  0x1010000, "/home/mu2etrk/test_stand/spi_files/ROCV14.spi"             , 9482832 },
      { 1,  0x5000000, "/home/mu2etrk/test_stand/spi_files/ROCV14_stage3init.bin"  ,   86384 }
    },
    { "ROCV15",     2,
      { 1,  0x2010000, "/home/mu2etrk/test_stand/spi_files/ROCV15.spi"             , 9482832 },
      { 1,  0x5040000, "/home/mu2etrk/test_stand/spi_files/ROCV15_stage3init.bin"  ,   86384 }
    },
                                        // end of data marker 
    { "",          -1,
      {-1,         -1, ""                                                          ,      -1 },
      {-1,         -1, ""                                                          ,      -1 }
    }
  };

//-----------------------------------------------------------------------------
// if Spi=1, return SPI image, otherwise - bin
//----------------------------------------------------------------------------
const test_program_roc::ImageData_t* test_program_roc::get_image_data(const std::string& Version, const std::string Spi) {
  FwVersion_t* fw(nullptr);
  //  std::cout << __func__ << " emoe" << std::endl;
  
  for (int i=0; drac_fw[i].name != ""; ++i) {
    // std::cout << __func__ << " i:" << i << std::endl;
    if (drac_fw[i].name == Version) {
      fw = &drac_fw[i];
      break;
    }
  }

  if (fw == nullptr) {
    std::cout << "ERROR: fw version:" << Version << " is not defined, BAIL OUT." << std::endl;
    return (const ImageData_t*) nullptr;
  }

  if (Spi == "spi") return (const ImageData_t*) &fw->spi_file;
  else              return (const ImageData_t*) &fw->bin_file;
}

//-----------------------------------------------------------------------------
// if Spi=1, return SPI image, otherwise - bin
//----------------------------------------------------------------------------
const test_program_roc::FwVersion_t* test_program_roc::get_version(const std::string& Version) {
  FwVersion_t* fw(nullptr);
  
  for (int i=0; drac_fw[i].name != ""; ++i) {
    if (drac_fw[i].name == Version) {
      fw = &drac_fw[i];
      break;
    }
  }

  if (fw == nullptr) TLOG(TLVL_ERROR) << "fw version:" << Version << " is not defined, return NULL pointer.";

  return fw;
}

//-----------------------------------------------------------------------------
// clears SPI memory in 64k blocks
//-----------------------------------------------------------------------------
// void spi_clear(int Link, int Address, int NBytes) {
int test_program_roc::spi_clear_memory(trkdaq::DtcInterface* Dtc_i, int Link, int Offset, int NBytes, int DebugMode) { // const ImageData_t* SpiData) {
  int rc(0);
  
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  // int nbytes = SpiData->fsize;
  // int offset = SpiData->offset;

  if (DebugMode & 0x1) {
    std::cout << __func__ << ": clearing nbytes:" << NBytes << " starting from address 0x:" << std::hex << Offset << std::dec << std::endl;
  }

  input.push_back(SPI_CLEAR);                 // SPI clear
  input.push_back( Offset         & 0xFFFF);  // 
  input.push_back((Offset  >> 16) & 0xFFFF);  // 
  input.push_back( NBytes         & 0xFFFF);  // 
  input.push_back((NBytes  >> 16) & 0xFFFF);  // 

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  if (DebugMode & 0x1) {
    std::cout << __func__ << ": SPI_CLEAR input written " << std::endl;
  }
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,100)) != 0x8000) {};

  if (u != 0x8000) {
    std::cout << "ERROR in " << __func__ << " : timeout detected, "
              << Form("reg:%03i val:0x%04x\n",128,u) << std::endl;
    rc = -1;
  }

  std::this_thread::sleep_for(std::chrono::microseconds(200000));
  
  // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
  if (DebugMode & 0x1) {
    std::cout << __func__ << ":END" << std::endl;
  }
  return rc;
}

//-----------------------------------------------------------------------------
// program IAP
//-----------------------------------------------------------------------------
void test_program_roc::program_roc(trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Version) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  const FwVersion_t* ver = get_version(Version);
  if (ver == nullptr) {
    TLOG(TLVL_ERROR) << std::format("image {}not found. BAIL OUT",Version);
    return -1;
  }

  TLOG(TLVL_INFO) << std::format("-- START programming PCIE:{} Link:{} Version:{} index:{}\n",Dtc_i->PcieAddr(),Link,Version,ver->index);
  
  
  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_IAP_W_INDEX);                 // SPI clear
  input.push_back( ver->index        & 0xFFFF);  // 
  input.push_back((ver->index >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);
//-----------------------------------------------------------------------------
// 25 sec was not enough on the tower, 30 sec was OK.
// may need to increase the sleep time in the future
//-----------------------------------------------------------------------------
  sleep(30);
  TLOG(TLVL_INFO) << std::format("after sleep\n");
  
                                        // the fw has been reloaded to FPGA, reset the DTC
  Dtc_i->fDtc->SoftReset();
  Dtc_i->InitReadout(-1,1);
                                        // no point in testing - just wait long enough
  // uint16_t u;
  // while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  // uint16_t status;
  // status = Dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  // TLOG(TLVL_INFO) << "-- END status:" << status << std::endl;
}



//-----------------------------------------------------------------------------
void test_program_roc::spi_program_iap_w_index(trkdaq::DtcInterface* Dtc_i, int Link, int Index) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_IAP_W_INDEX);                 // SPI clear
  input.push_back( Index        & 0xFFFF);  // 
  input.push_back((Index >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  sleep(40);
                                        // wait till the command is executed
  uint16_t u;
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  uint16_t status;
  status = Dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  std::cout << __func__ << ":END status:" << status << std::endl;
}


//-----------------------------------------------------------------------------
// program IAP by address - not really needed
//-----------------------------------------------------------------------------
void test_program_roc::spi_program_iap_w_address(trkdaq::DtcInterface* Dtc_i, int Link, int ImageOffset) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  TLOG(TLVL_DEBUG) << std::format("START PCIE:{} Link:{}",Dtc_i->PcieAddr(),Link);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_IAP_W_ADDRESS);                 // SPI clear
  input.push_back((ImageOffset      ) & 0xFFFF);  // 
  input.push_back((ImageOffset >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

                                        // wait till the command is executed
  uint16_t u;
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  uint16_t status;
  status = Dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  TLOG(TLVL_DEBUG) << std::format("END  PCIE:{} Link:{} status:{}",Dtc_i->PcieAddr(),Link,status);
}


//-----------------------------------------------------------------------------
// read one record, can't read more than 127 words (254 bytes) at a time
// DebugMode: bit0: print one-liner
// can't read records longer than 1K bytes, but NWords could be > 1024
// upon success returns rc=0, otherwise rc<0
//-----------------------------------------------------------------------------
int test_program_roc::spi_read_record(trkdaq::DtcInterface* Dtc_i, int Link, uint32_t SpiOffset, int NWords, uint16_t* Res, int DebugMode) {
  int rc(0);
  
  TLOG(TLVL_DEBUG+2) << std::format("START PCIE:{} Link:{} SpiOffset:0x{:08x} NWords:{}",Dtc_i->PcieAddr(),Link,SpiOffset,NWords);

  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);
  auto roc  = DTCLib::DTC_Link_ID(Link);

  int nw_read_tot = 0;
  // int const max_record_size(1024);                // in bytes
  int const max_record_size(254);                 // in bytes

  int max_nwr    = max_record_size/2;
  int spi_offset = SpiOffset;                     // offset in SPI memory of the record to be read out
  
  int nreads = (NWords-1)/max_nwr + 1;
  for (int i=0; i<nreads; ++i) {
    int nw      = max_nwr;
    int nw_left = NWords-nw_read_tot;
    
    if (nw_left < nw) nw = nw_left;
    
    bool increment_address(false);
    std::vector<uint16_t> input;

    input.push_back(SPI_FLASH_READ);              // SPI flash read
    input.push_back((spi_offset      ) & 0xFFFF); // starting address LSB
    input.push_back((spi_offset >> 16) & 0xFFFF); // starting address MSB
    input.push_back(nw*2);                        // number of bytes to read
    input.push_back(0);                           // 5 words total

    Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

    uint16_t u; 
    while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,100)) != 0x8000) {};
    if (u != 0x8000) {
      TLOG(TLVL_ERROR) << std::format(" : timeout detected: reg:{:03d} val:0x{:04x}",128,u);
      rc = -1;
      return rc;
    }

    int nwr = Dtc_i->fDtc->ReadROCRegister(roc,129,100);  // should return nw+4
    if (DebugMode & 0x2) {
      TLOG(TLVL_DEBUG+1) << std::format("[{}:{}] nw:{} nwr:{}",__func__,__LINE__,nw,nwr);
    }
//-----------------------------------------------------------------------------
// now actually read the data
//-----------------------------------------------------------------------------
    std::vector<uint16_t> buf; 
    Dtc_i->RocBlockRead(Link,RREG,buf);

    if (DebugMode & 0x8) {
      int nw_read = buf.size();
      std::cout << std::format("[{}:{}] read number {} nw:{} nw_read:{}\n",__func__,__LINE__,i,nw,nw_read);
      Dtc_i->PrintBuffer(buf.data(),nw_read,spi_offset);
    }

    if (Res != nullptr) {
      memcpy(Res,buf.data(),2*nw);
    }
//-----------------------------------------------------------------------------
// update counters
//-----------------------------------------------------------------------------
    spi_offset  += 2*nw;
    nw_read_tot += nw;
  }
  
  TLOG(TLVL_DEBUG+1) << std::format("END   PCIE:{} Link:{} SpiOffset:0x{:08x} NWords:{} nw_read_tot:{} rc:{}",
                                    Dtc_i->PcieAddr(),Link,SpiOffset,NWords,nw_read_tot,rc);

  return rc;
}

//-----------------------------------------------------------------------------
// read one segment (or less) starting from 'SpiOffset' in SPI memory
// NBytes is 0x10000 or less
// actual reads from SPI memory are done using records of 'record_size'
// exit in case of an error
//-----------------------------------------------------------------------------
int test_program_roc::spi_read_segment(trkdaq::DtcInterface* Dtc_i, int Link, uint32_t SpiOffset, int NBytes, std::vector<char>& Res, int DebugMode) {
  int rc(0);
  //   std::mutex mtx; // For thread-safe output

  //  int record_size(1024);              // future version, not on the station yet 
  int  record_size(254);                  // can only read 254 bytes (127 shorts) at a time
  
  //  int  first_addr = SpiOffset;            // initial offset in SPI memory

  Res.clear();
  Res.reserve(NBytes);

  int report_step = record_size*40;
  int report_mark = report_step;

  TLOG(TLVL_INFO) << std::format("request to read a segment: PCIE:{} Link:{} NBytes:{} SpiOffset:0x{:08x} report_step:{} bytes\n",
                                 Dtc_i->PcieAddr(),Link,NBytes,SpiOffset,report_step);
//-----------------------------------------------------------------------------
// can read only 254 bytes (127 shorts) at a time
//-----------------------------------------------------------------------------
  int  nreads     = (NBytes-1)/record_size + 1;
  int  nb_read    = 0;
  for (int i=0; i<nreads; i++) {
    int nb      = record_size;
    int nb_left = NBytes-nb_read;
    if (nb_left < record_size) nb = nb_left;
                                        // read nb_left bytes
//-----------------------------------------------------------------------------
// read next record
//-----------------------------------------------------------------------------
    int      nw         = (nb-1)/2 + 1;
    uint32_t first_addr = SpiOffset+nb_read;
    {
      rc = spi_read_record(Dtc_i, Link, first_addr, nw, (uint16_t*) (Res.data()+nb_read),DebugMode);
    }
    if (rc < 0) {
      TLOG(TLVL_ERROR) << std::format(" read ERROR rc:{} . BAIL OUT",__func__,__LINE__,rc);
      return rc;
    }

    nb_read += nb;

    if (nb_read >= report_mark) {
      TLOG(TLVL_INFO) << std::format("-- PCIE:{} Link:{} read record nb:{}  total nb_read:{} first_address:0x{:08x}",
                                     Dtc_i->PcieAddr(),Link,nb,nb_read,first_addr);
      report_mark += report_step;
    }
  }

  TLOG(TLVL_INFO) << std::format("DONE, PCIE:{} Link:{} rc:{} total nb_read:{} nreads:{}",Dtc_i->PcieAddr(),Link,rc,nb_read,nreads);
  return rc;
}

//-----------------------------------------------------------------------------
// validate image already writen to SPI memory
//-----------------------------------------------------------------------------
int test_program_roc::spi_validate_segment(trkdaq::DtcInterface* Dtc_i, int Link, const ImageData_t* Image, int Segment, int DebugMode) {
  int rc = 0;
//-----------------------------------------------------------------------------
// open input file and determine its size
//-----------------------------------------------------------------------------
  std::ifstream file(Image->fn, std::ios::binary);
  if (not file.is_open()) {
    TLOG(TLVL_ERROR) << std::format("failed to open file:{} . BAIL OUT",Image->fn);
    return -1;
  }

  file.seekg(0, std::ios::end);
  int fsize = file.tellg();
  file.seekg(0, std::ios::beg);
//-----------------------------------------------------------------------------
// clear the spi memory for image at index *** ###
//-----------------------------------------------------------------------------
  TLOG(TLVL_DEBUG) << std::format("fn:{} fsize:{}",Image->fn,fsize);
 //-----------------------------------------------------------------------------
// read the input file and upload its content to the SPI memory
// have to do everything in 64K blocks - this is the unit in which the SPI memory
// is getting reset
//-----------------------------------------------------------------------------
  int const segment_size(0x10000); // the SPI memory is cleared in 64K segments 
  
  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);

  int nb_read_tot(0);
                                                 // offset in the image file
  int segment_offset  = Segment*segment_size;
    
  int nb_to_read     = segment_size;
                                                 // handle last, potentially shorter, block
  if (segment_offset+segment_size > fsize) {
    nb_to_read = fsize-segment_offset;
  }
                                                 // first address in SPI memory
  int spi_offset   = Image->offset+segment_offset;
//-----------------------------------------------------------------------------
// read one segment, 64K or less
//-----------------------------------------------------------------------------
  std::vector<char> readback;
  rc = spi_read_segment(Dtc_i,Link,spi_offset,nb_to_read,readback);
  if (rc == 0) {
    nb_read_tot += nb_to_read;
                                                 // compare to the original
    for (int i=0; i<nb_to_read; ++i) {
      if (readback[i] != fileData[segment_offset+i]) {
//-----------------------------------------------------------------------------
// if an inconsistency has been detected, print diagnostics and try to rewrite up to 3 times
//-----------------------------------------------------------------------------
        TLOG(TLVL_ERROR) << std::format("PCIE:{} Link:{} segment SPI offset:0x{:08x}\n",Dtc_i->PcieAddr(),Link,spi_offset)
                         << std::format(" segment number {:5d} byte at file offset:0x{:08x}",Segment,segment_offset+i)
                         << std::format(" is:0x{:2x}",(uint8_t)fileData[segment_offset+i])
                         << std::format(" readback byte offset:0x{:08x} value:0x{:x}",i, (uint8_t) readback[i]);
        rc = -2;
        break;
      }
    }
    if (rc == 0) {
      TLOG(TLVL_INFO) << std::format("PCIE:{} Link:{} segment:{} : validation succeeded",Dtc_i->PcieAddr(),Link,Segment);
    }
  }
//-----------------------------------------------------------------------------
// if rc = 0, the readback and the validation succeeded, nothing more to do
//-----------------------------------------------------------------------------
  if (rc != 0) {
    TLOG(TLVL_ERROR) << std::format("validation failed, rc:{} . BAIL OUT",rc);
    return rc;
  }
  
  TLOG(TLVL_DEBUG) << std::format("END rc:{} nb_read_tot:{}",rc,nb_read_tot);
  return rc;
}

//-----------------------------------------------------------------------------
// validate image already writen to SPI memory
//-----------------------------------------------------------------------------
int test_program_roc::spi_validate_image(trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Version, const std::string& Type, int DebugMode) {
  int rc = 0;
//-----------------------------------------------------------------------------
// open input file and determine its size
//-----------------------------------------------------------------------------
  TLOG(TLVL_INFO) << std::format("-- START validating PCIE:{} Link:{} Version:{} Type:{}\n",Dtc_i->PcieAddr(),Link,Version,Link);
  
  const ImageData_t* image = get_image_data(Version, Type);
  if (image == nullptr) {
    TLOG(TLVL_ERROR) << std::format("image {}:{} not found. BAIL OUT",Version,Type);
    return -1;
  }
  
  std::ifstream file(image->fn, std::ios::binary);
  if (not file.is_open()) {
    TLOG(TLVL_ERROR) << std::format("failed to open file:{} . BAIL OUT",image->fn);
    return -1;
  }

  file.seekg(0, std::ios::end);
  int fsize = file.tellg();
  file.seekg(0, std::ios::beg);
//-----------------------------------------------------------------------------
// clear the spi memory for image at index *** ###
//-----------------------------------------------------------------------------
  TLOG(TLVL_DEBUG) << std::format("fn:{} fsize:{}",image->fn,fsize);
 //-----------------------------------------------------------------------------
// read the input file and upload its content to the SPI memory
// have to do everything in 64K blocks - this is the unit in which the SPI memory
// is getting reset
//-----------------------------------------------------------------------------
  int const segment_size(0x10000); // the SPI memory is cleared in 64K segments 
  
  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);

  int n64k_segments = (fsize-1)/0x10000 + 1;

  int nb_read_tot(0);
  int first_addr    = image->offset;
    
  for (int iseg=0; iseg<n64k_segments; ++iseg) {
    int segment_offset = segment_size*iseg;
    int nb_to_read     = segment_size;
                                        // handle last, potentially shorter, block
    if (segment_offset+segment_size > fsize) {
      nb_to_read = fsize-segment_offset;
    }

    int spi_offset   = first_addr+segment_offset;
    // const char* data = fileData.data()+segment_offset;
//-----------------------------------------------------------------------------
// read one segment, 64K or less
//-----------------------------------------------------------------------------
    std::vector<char> readback;
    rc = spi_read_segment(Dtc_i,Link,spi_offset,nb_to_read,readback);
    if (rc == 0) {
      nb_read_tot += nb_to_read;
                                        // compare to the original
      for (int i=0; i<nb_to_read; ++i) {
        if (readback[i] != fileData[segment_offset+i]) {
//-----------------------------------------------------------------------------
// if an inconsistency has been detected, print diagnostics and try to rewrite up to 3 times
//-----------------------------------------------------------------------------
          TLOG(TLVL_ERROR) << std::format("PCIE:{} Link:{} segment SPI offset:0x{:08x}\n",Dtc_i->PcieAddr(),Link,spi_offset)
                           << std::format(" segment number {:5d} byte at file offset:0x{:08x}",iseg,segment_offset+i)
                           << std::format(" is:0x{:2x}",(uint8_t)fileData[segment_offset+i])
                           << std::format(" readback byte offset:0x{:08x} value:0x{:x}",i, (uint8_t) readback[i]);
          rc = -2;
          break;
        }
      }
      if (rc == 0) {
        TLOG(TLVL_INFO) << std::format("PCIE:{} Link:{} segment:{} out of {}: validation succeeded",Dtc_i->PcieAddr(),Link,iseg,n64k_segments);
      }
    }
//-----------------------------------------------------------------------------
// if rc = 0, the readback and the validation succeeded, nothing more to do
//-----------------------------------------------------------------------------
    if (rc != 0) {
      TLOG(TLVL_ERROR) << std::format("validation failed, rc:{} segment:{}. BAIL OUT",rc,iseg);
      return rc;
    }
  }
  
  TLOG(TLVL_INFO) << std::format("-- END rc:{} nb_read_tot:{}",rc,nb_read_tot);
  return rc;
}

//-----------------------------------------------------------------------------
// mu2etrk@mu2edaq22:~/test_stand/daquser_001>cat ../monica_008/flash_nmap 
// 0x10000 
// 0x1010000
// 0x5000000
// works - can read back
// what's written is defined by the config file
//-----------------------------------------------------------------------------
int test_program_roc::spi_write_directory(trkdaq::DtcInterface* Dtc_i, int Link) {
  int rc(0);
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);

  int nimages = 0;
  for (int i=0; drac_fw[i].name != ""; ++i) {
    if (drac_fw[i].spi_file.load_flag >= 0) ++nimages;
  }

  std::cout << __func__ << ": nimages:" << nimages << std::endl;

  std::vector<uint16_t> input;
                                           // 5 words
  input.push_back(SPI_WRITE_DIRECTORY);    // SPI flash read
  input.push_back(nimages & 0xFFFF);       // nimages LSB
  input.push_back(0               );       // nimages MSB
  input.push_back(0               );       // 2nd command : 2 16-bit words - uunused
  input.push_back(0               );       // unused
//-----------------------------------------------------------------------------
// to form the record to be written, assume that the image index is incremented monotonically
// directory catalog only contains offsets of the SPI images
//-----------------------------------------------------------------------------
  for (int i=0; drac_fw[i].name != ""; ++i) {
    FwVersion_t* fw = &drac_fw[i];
    if (fw->spi_file.load_flag >= 0) {
      int offset = fw->spi_file.offset;
      input.push_back( offset        & 0xFFFF);                             //
      input.push_back((offset >> 16) & 0xFFFF);                             //
    }
  }

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  // std::cout << __func__ << ": SPI_DIRECTORY_WRITE input written " << std::endl;
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {};

                                        // read return code
  rc = Dtc_i->fDtc->ReadROCRegister(roc,132,1000);

  std::cout << __func__ << ":END rc:" << rc << std::endl;

  return rc;
}

//-----------------------------------------------------------------------------
// returns 0 if OK and an error code otherwise
//-----------------------------------------------------------------------------
int test_program_roc::spi_write_record(trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords, uint16_t* Data, int DebugMode) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(SPI_WRITE_RECORD);            // SPI clear
  input.push_back( FirstAddr        & 0xFFFF);  // 
  input.push_back((FirstAddr >> 16) & 0xFFFF);  // 
  input.push_back( NWords           & 0xFFFF);  // 
  input.push_back((NWords    >> 16) & 0xFFFF);  // 

  for (int i=0; i<NWords; ++i) {
    input.push_back(Data[i  ]);
  }

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

  // std::cout << " data written, nb:" << NWords*2 << std::endl;
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  int rc = Dtc_i->fDtc->ReadROCRegister(roc,132,1000);

  if (rc != 0) {
    std::cout << std::format("[{}:{}] ERROR writing NWords:{} to SPI address 0x{:08x}, rc=:{}\n",
                             __func__,__LINE__,NWords,FirstAddr,rc);
    if (DebugMode & 0x2) {
      Dtc_i->PrintBuffer(Data,NWords,FirstAddr);
    }
  }

  if (DebugMode & 0x1) {
    std::cout << __func__ << ":END rc:" << rc << std::endl;
  }
  return rc;
}

//-----------------------------------------------------------------------------
// write a single 64K byte segment (or less) . Return in case of an error
//-----------------------------------------------------------------------------
int test_program_roc::spi_write_segment(trkdaq::DtcInterface* Dtc_i, int Link, const char* Data, int NBytes, int SpiOffset, int DebugMode) {
  const int record_size (0x400);   // write 1Kbytes blocks
  int rc(0);

  TLOG(TLVL_DEBUG+1) << std::format("START: writing NBytes:{} starting from offset 0x:{:08x} in SPI memory\n",NBytes,SpiOffset);
//-----------------------------------------------------------------------------
// can write only 1024 bytes (512 shorts) at a time
//-----------------------------------------------------------------------------
  int first_addr     = SpiOffset;
  int nb_written     = 0;

  bool done(false);
  while (not done) {
    int nb      = record_size;         // however, write in 2-byte words
    int nb_left = NBytes-nb_written;   // n bytes left to write within the segment
    if (nb_left < record_size) nb  = nb_left;
    
    if (nb > 0) {
//-----------------------------------------------------------------------------
// write next record
//-----------------------------------------------------------------------------
      TLOG(TLVL_DEBUG+1) << std::format(": nb:{:4d} first_addr:0x{:08x}",nb,first_addr)
                         << std::format(" nb_written:{:8d}\n",nb_written);

      // std::this_thread::sleep_for(std::chrono::microseconds(1000));
      int spi_offset = first_addr+nb_written;             // offset in bytes
      uint16_t* data = (uint16_t*) (Data+nb_written);
      int nw         = (nb-1)/2 + 1;
      rc      = spi_write_record(Dtc_i,Link,spi_offset,nw,data);

      std::this_thread::sleep_for(std::chrono::microseconds(1000));
      if (rc != 0) {
        TLOG(TLVL_ERROR) << std::format("ERROR to write to SPI offset:0x{:08x} . BAIL OUT\n",spi_offset);
        return rc;
      }
//-----------------------------------------------------------------------------
// write succeeded
//-----------------------------------------------------------------------------
      nb_written += nb;
    }
    else {
      done = true;
    }
  }
  TLOG(TLVL_DEBUG) << std::format("END rc:{} written_bytes:0x{:05x} starting_offset:0x{:08x}\n",rc,nb_written,SpiOffset);
  return rc;
}

//-----------------------------------------------------------------------------
// attn: not compete yet!
//
//  /home/mu2etrk/test_stand/spi_files:
//  drwxr-x---  2 mu2etrk mu2e    4096 Aug 26 16:20 .
//  drwxr-xr-x 60 mu2etrk mu2e    4096 Aug 20 16:47 ..
//  -rw-r-----  1 mu2etrk mu2e 9524032 Aug 20 16:47 GoldenV10.spi
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROC.spi
//  -rw-r-----  1 mu2etrk mu2e   82272 Aug 20 16:48 ROCV12-3_stage3init.bin
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROCV12.spi
//  -rw-r-----  1 mu2etrk mu2e 9480352 Aug 20 16:47 ROCV13.spi
//
// Index       : index in the spi_data table of the image to load
// TestMode = 1: don't write to memory, just clear the memory and count
// NWrites     : number of records to write, -1: write all
//-----------------------------------------------------------------------------
int test_program_roc::spi_load_image(trkdaq::DtcInterface* Dtc_i, int Link, const ImageData_t* SpiData, int DebugMode) {
  int rc = 0;
//-----------------------------------------------------------------------------
// open input file and determine its size
//-----------------------------------------------------------------------------
  std::cout << __func__ << ": fn:" << SpiData->fn << std::endl;
                         
  std::ifstream file(SpiData->fn, std::ios::binary);
  if (not file.is_open()) {
    std::cout << "ERROR in " << __func__ << " failed to open file: " << SpiData->fn << " . BAIL OUT" << std::endl;
    return -1;
  }

  file.seekg(0, std::ios::end);
  int fsize = file.tellg();
  file.seekg(0, std::ios::beg);
//-----------------------------------------------------------------------------
// clear the spi memory for image at index *** ###
//-----------------------------------------------------------------------------
  // std::cout << std::format("[{}:{}] -- before spi_clear: fn:{} fsize:{:d}\n",__func__,__LINE__,SpiData->fn,fsize);
 
  // rc = spi_clear_memory(Dtc_i, Link, SpiData->offset,fsize,DebugMode);
  // if (rc < 0) {
  //   std::cout << std::format("[{}:{}]: ERROR: rc:{} in spi_clear_memory, BAIL OUT\n",__func__,__LINE__,rc);
  //   return rc;
  // }

  if (DebugMode & 0x1) {
    std::cout << __func__ << ": after spi_clear_memory" << std::endl;
  }
//-----------------------------------------------------------------------------
// read the input file and upload its content to the SPI memory
// have to do everything in 64K blocks - this is the unit in which the SPI memory
// is getting reset
//-----------------------------------------------------------------------------
  int const segment_size(0x10000); // the SPI memory is cleared in 64K segments 
  
  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);

  int n64k_segments = (fsize-1)/0x10000 + 1;

  int first_addr    = SpiData->offset;
  int nb_written    = 0;
    
  if (DebugMode & 0x1) {
    std::cout << std::format("[{}:{}]: n64k_segments:{} first_addr:0x{:08x}\n",__func__,__LINE__,n64k_segments,first_addr);
  }

  for (int ib=0; ib<n64k_segments; ++ib) {
    int segment_offset = segment_size*ib;
    int nb_to_write    = segment_size;
                                        // handle last, potentially shorter, block
    if (segment_offset+segment_size > fsize) {
      nb_to_write = fsize-segment_offset;
    }

    int spi_offset   = first_addr+segment_offset;
    const char* data = fileData.data()+segment_offset;
//-----------------------------------------------------------------------------
// writing one segment, 64K or less
//-----------------------------------------------------------------------------
    int attempt(1);
    while (attempt < 4) {
//-----------------------------------------------------------------------------
// for the first time, the memory is reset by the microcontroller, when re-trying,
// need to clear the memory here
//-----------------------------------------------------------------------------
      if (DebugMode & 0x1) {
        std::cout << std::format("{}:{}: attempt {} to write segment {:d}\n",__func__,__LINE__,attempt, ib);
      }
      
      spi_clear_memory(Dtc_i,Link,spi_offset,0x10000,DebugMode);
      rc = spi_write_segment(Dtc_i,Link,data,nb_to_write,spi_offset,DebugMode);
      if (rc != 0) {
                                        // detected error
        std::cout << std::format("{}:{}  attempt:{} ERROR writing segment number {:4d} rc:{}\n",
                                 __func__,__LINE__,attempt,ib,rc);
      }
      else {
//-----------------------------------------------------------------------------
// segment has been written w/o detected errors, validate if requested
//-----------------------------------------------------------------------------
        // compare to the original
        if (DebugMode & 0x10000) {
          std::cout << std::format("{}:{}: validation: read segment:{} back\n",__func__,__LINE__,ib);

          std::vector<char> readback;
          rc = spi_read_segment(Dtc_i,Link,spi_offset,nb_to_write,readback,DebugMode);
          if (rc == 0) {
            for (int i=0; i<nb_to_write; ++i) {
              if (readback[i] != fileData[segment_offset+i]) {
//-----------------------------------------------------------------------------
// if an inconsistency has been detected, print diagnostics and try to rewrite up to 3 times
//-----------------------------------------------------------------------------
                int ic1 = readback[i] & 0xff;
                int ic2 = fileData[segment_offset+i] &0xff;
                
                std::cout << std::format("[{}:{}] ERROR validating written segment starting from SPI offset:0x{:x}\n",
                                         __func__,__LINE__,spi_offset)
                          << std::format("i:{} readback[i]:0x{:x} fileData[segment_offset+i]:0x{:x}\n",
                                         i,ic1,ic2);
                Dtc_i->PrintBuffer((void*) readback.data(),64,0);
                Dtc_i->PrintBuffer((void*) (fileData.data()+segment_offset),64,0);
                rc = -2;
                break;
              }
            }
          }
        }
        if (DebugMode & 0x1) {
          std::cout << std::format("{:s}:{:d}: end validation, rc:{}\n",__func__,__LINE__,rc);
        }
      }
//-----------------------------------------------------------------------------
// if rc = 0, the readback and the validation succeeded, nothing more to do
//-----------------------------------------------------------------------------
      if (rc == 0) break;
      attempt++;
    }
//-----------------------------------------------------------------------------
// 3 attempts to rewrite have failed
//-----------------------------------------------------------------------------
    if (rc != 0) {
      printf("[%s] 3 attempts to write segment failed, rc: %i. BAIL OUT\n",__func__,rc);
      return rc;
    }
    nb_written += nb_to_write;
  }

  std::cout << std::format("[{}:{}]: END: rc:{} total nb_writen:{}\n",__func__,__LINE__,rc,nb_written);
  return rc;
}


//-----------------------------------------------------------------------------
// upload images into SPI memory, but do not program the FPGA
// keep the programming step - program_iap_w_index - separate, as it needs validation
// Version defines one or two images to uploas
//-----------------------------------------------------------------------------
int test_program_roc::dtc_program_roc(trkdaq::DtcInterface* Dtc_i, int Link, const char* Version, int DebugMode) {
                                        // find firmware version
  int rc(0);
  
  FwVersion_t* fw(nullptr);
  
  for (int i=0; drac_fw[i].name != ""; ++i) {
    if (drac_fw[i].name == Version) {
      fw = &drac_fw[i];
      break;
    }
  }

  if (fw == nullptr) {
    std::cout << "ERROR: fw version:" << Version << " is not defined, BAIL OUT." << std::endl;
    return -1;
  }
//-----------------------------------------------------------------------------
// firmware to be uploaded found, proceed with the upload.
// 1. initialize the DTC
//-----------------------------------------------------------------------------
  // int  link_mask = (1 << 4*Link);
  //  bool skip_init(false);

  trkdaq::DtcInterface* dtc_i = Dtc_i;
  if (dtc_i == nullptr) {
    dtc_i = trkdaq::DtcInterface::Instance(-1);
  }

  gSystem->Setenv("DTCLIB_DTC",Form("%i",dtc_i->PcieAddr()));
//-----------------------------------------------------------------------------
// 2. spi directory has always to be mapped in full , 
//    as the directory doesn't need to be rewritten every time, make it a separate step
//    this is why the next line is commented out
//-----------------------------------------------------------------------------
//  spi_write_directory(dtc_i,Link);

                                        // upload the .spi image

  rc = spi_load_image(dtc_i,Link,&fw->spi_file,DebugMode);

  if (rc < 0) {
    return rc;
  }

                                        // upload the .bin image, if needed
  if (fw->bin_file.load_flag >= 0) {
    spi_load_image(dtc_i,Link,&fw->bin_file,DebugMode);
  }
                                        // activate the image as a separate step after validating
                                        // the memory, thus the following is commented out
  // spi_program_iap_w_index(dtc_i,Link,fw->spi_file.index);
  
  return rc;
}
//-----------------------------------------------------------------------------
int test_program_roc::dtc_validate_version(trkdaq::DtcInterface* Dtc_i, int Link, const std::string& Version, const std::string& Type, int DebugMode) {
  // find firmware version
  int rc(0);
  
  FwVersion_t* fw(nullptr);
  
  for (int i=0; drac_fw[i].name != ""; ++i) {
    if (drac_fw[i].name == Version) {
      fw = &drac_fw[i];
      break;
    }
  }

  if (fw == nullptr) {
    TLOG(TLVL_ERROR) << std::format("fw version:{} is not defined, BAIL OUT.\n",Version);
    return -1;
  }
//-----------------------------------------------------------------------------
// firmware to be uploaded found, proceed with the upload.
// 1. initialize the DTC
//-----------------------------------------------------------------------------
  trkdaq::DtcInterface* dtc_i = Dtc_i;
  if (dtc_i == nullptr) {
    dtc_i = trkdaq::DtcInterface::Instance(-1);
  }

  gSystem->Setenv("DTCLIB_DTC",Form("%i",dtc_i->PcieAddr()));

                                        // validate the .spi image
  if (Type != "bin") {
    rc = spi_validate_image(dtc_i,Link,Version,Type,DebugMode);
    
    if (rc < 0) {
                                        // don't proceed in case of any failure
      return rc;
    }
  }
                                        // upload the .bin image, if needed
  if ((Type != "spi") and (fw->bin_file.load_flag >= 0)) {
    spi_validate_image(dtc_i,Link,Version,Type,DebugMode);
  }
  
  return rc;
}

//-----------------------------------------------------------------------------
int test_program_roc::test_write_record(trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords, int DelayUs) {
  int rc(0);

  trkdaq::DtcInterface* dtc_i = Dtc_i;
  if (dtc_i == nullptr) {
    dtc_i = trkdaq::DtcInterface::Instance(-1);
    if (dtc_i == nullptr) {
      std::cout << "%i ERROR: dtc_i not defined, EXIT" << __LINE__ << std::endl;
      return -1;
    }
  }

  std::vector<uint16_t> dat;
  for (int i=0; i<NWords; i++) dat.push_back(i+3);

  // memory is organized in 4K (0x1000) sectors,
  // according to ChatGPT, a sector corresponding to the record being written needs to be erased
  // assume that the 'FirstAddr' starts on a 4K boundary and for testing purposes clear just one sector
  
  // spi_clear_memory(Dtc_i, Link, FirstAddr,0x1000);
  // rc = spi_clear_memory(Dtc_i, Link, FirstAddr,0x10000);
  
  if (rc < 0) {
                                        // the diagnostics printed in the function where the error was first detected
    return -1;
  }

  if (DelayUs != 0) {
    std::this_thread::sleep_for(std::chrono::microseconds(DelayUs));
  }

  rc = spi_write_record(Dtc_i, Link, FirstAddr, NWords, dat.data());

  std::cout << " after write rc:" << rc << std::endl;

  dat.clear();

  if (DelayUs != 0) {
    std::this_thread::sleep_for(std::chrono::microseconds(DelayUs));
  }

  // dat.reserve(NWords);
  // spi_read_record(Dtc_i,Link,FirstAddr,NWords,dat.data(),0x3);
  
  return 0;
}


//-----------------------------------------------------------------------------
int test_program_roc::test_read_record(trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords) {

  trkdaq::DtcInterface* dtc_i = Dtc_i;
  if (dtc_i == nullptr) {
    dtc_i = trkdaq::DtcInterface::Instance(-1);
    if (dtc_i == nullptr) {
      std::cout << "ERROR: dtc_i not defined, EXIT" << std::endl;
    }
  }

  std::vector<uint16_t> dat(NWords);

  spi_read_record(dtc_i,Link,FirstAddr,NWords,dat.data(),0x3);
  
  return 0;
}
