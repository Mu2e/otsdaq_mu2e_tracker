//-----------------------------------------------------------------------------
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
#include "iostream"
#include "artdaq-core-mu2e/Overlays/DTC_Types/DTC_Link_ID.h"
#include "dtcInterfaceLib/DTC.h"
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"
#include "TString.h"
#include "TSystem.h"
// #include "TRACE/tracemf.h"
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
    int         fsize;
  };

  struct FwVersion_t {
    std::string name;
    int         index;                  // in spi_offsets and bin_offsets
    ImageData_t spi_file;
    ImageData_t bin_file;
  };
//-----------------------------------------------------------------------------
// offsets of different images - no freedom here
//-----------------------------------------------------------------------------
  // int spi_offset[5] = { 0x10000, 0x1010000, 0x2010000, 0x3010000, 0x4010000 };
  // int bin_offset[5] = {      -1, 0x5000000, 0x5040000, 0x5080000, 0x50c0000 };
//-----------------------------------------------------------------------------
// GoldenVXX should always be the first image ( index 0, offset 0x10000)
// the rest versions could be overriding each other
// name = "": end of data
// assume that the image index is incremented monotonically
// 'FwVersion_t.index' is the image index in the directory catalog
//-----------------------------------------------------------------------------
  FwVersion_t drac_fw[] = {
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
const ImageData_t* spi_get_image_data(const char* Version) {
  FwVersion_t* fw(nullptr);
  std::cout << __func__ << " emoe" << std::endl;
  
  for (int i=0; drac_fw[i].name != ""; ++i) {
    std::cout << __func__ << " i:" << i << std::endl;
    if (drac_fw[i].name == Version) {
      fw = &drac_fw[i];
      break;
    }
  }

  if (fw == nullptr) {
    std::cout << "ERROR: fw version:" << Version << " is not defined, BAIL OUT." << std::endl;
    return (const ImageData_t*) nullptr;
  }

  return (const ImageData_t*) &fw->spi_file;
}

//-----------------------------------------------------------------------------
// clears SPI memory in 64k blocks
//-----------------------------------------------------------------------------
// void spi_clear(int Link, int Address, int NBytes) {
int spi_clear_memory(trkdaq::DtcInterface* Dtc_i, int Link, int Offset, int NBytes, int DebugMode = 0) { // const ImageData_t* SpiData) {
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

  std::cout << __func__ << ": SPI_CLEAR input written " << std::endl;
  
  uint16_t u; 
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,100)) != 0x8000) {};

  if (u != 0x8000) {
    std::cout << "ERROR in " << __func__ << " : timeout detected, "
              << Form("reg:%03i val:0x%04x\n",128,u) << std::endl;
    rc = -1;
  }

  std::this_thread::sleep_for(std::chrono::microseconds(200000));
  
  // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
  std::cout << __func__ << ":END" << std::endl;
  return rc;
}

//-----------------------------------------------------------------------------
// program IAP
//-----------------------------------------------------------------------------
void spi_program_iap_w_index(trkdaq::DtcInterface* Dtc_i, int Link, int Index) {
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
void spi_program_iap_w_address(trkdaq::DtcInterface* Dtc_i, int Link, int ImageStartAddr) {
  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);

  bool increment_address(false);
                                              // 5 words
  std::vector<uint16_t> input;

  input.push_back(PROGRAM_IAP_W_ADDRESS);                 // SPI clear
  input.push_back( ImageStartAddr        & 0xFFFF);  // 
  input.push_back((ImageStartAddr >> 16) & 0xFFFF);  // 
  input.push_back(0);  // 
  input.push_back(0);

  auto roc  = DTCLib::DTC_Link_ID(Link);
  Dtc_i->fDtc->WriteROCBlock(roc,RREG,input,false,increment_address,100);

                                        // wait till the command is executed
  uint16_t u;
  while ((u = Dtc_i->fDtc->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

  uint16_t status;
  status = Dtc_i->fDtc->ReadROCRegister(roc,REG_STATUS,1000);
  std::cout << __func__ << ":END status:" << status << std::endl;
}


//-----------------------------------------------------------------------------
// read one record, can't read more than 127 words (254 bytes) at a time
// DebugMode: bit0: print one-liner
// can't read records longer than 1K bytes, but NWords could be > 1024
//-----------------------------------------------------------------------------
int spi_read_record(trkdaq::DtcInterface* Dtc_i, int Link, uint32_t SpiOffset, int NWords, uint16_t* Res, int DebugMode = 0) {
  int rc(0);
  
  if (DebugMode & 0x1) {
    std::cout << __func__ << " START: reading nwords:" << NWords << " from address:0x" << std::hex << SpiOffset << std::dec << std::endl;
  }

  if (Dtc_i == nullptr) Dtc_i = trkdaq::DtcInterface::Instance(-1);
  auto roc  = DTCLib::DTC_Link_ID(Link);

  int nw_read = 0;
  // int const max_record_size(1024);                // in bytes
  int const max_record_size(254);                 // in bytes

  int max_nwr    = max_record_size/2;
  int spi_offset = SpiOffset;
  
  int nreads = (NWords-1)/max_nwr + 1;
  for (int i=0; i<nreads; ++i) {
    int nw      = max_nwr;
    int nw_left = NWords-nw_read;
    
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
      std::cout << "ERROR in " << __func__ << " : timeout detected, "
                << Form("reg:%03i val:0x%04x\n",128,u) << std::endl;
      rc = -1;
      return rc;
    }

    int nwr = Dtc_i->fDtc->ReadROCRegister(roc,129,100);  // should return nw+4
//-----------------------------------------------------------------------------
// now actually read the data
//-----------------------------------------------------------------------------
    std::vector<uint16_t> buf; 
    Dtc_i->RocBlockRead(Link,RREG,buf);
    int nw_read = buf.size();

    if (DebugMode != 0) {
      std::cout << "nw read:" << buf.size() << std::endl;
      if (DebugMode & 0x2) {
        Dtc_i->PrintBuffer(buf.data(),nw_read);
      }
    }

    if (Res != nullptr) {
      memcpy(Res,buf.data(),2*nw);
    }
//-----------------------------------------------------------------------------
// update counters
//-----------------------------------------------------------------------------
    spi_offset += 2*nw;
    nw_read    += nw;
  }
  
  if (DebugMode & 0x1) std::cout << __func__ << " END: nw_read:" << nw_read << std::endl;

  return nw_read;
}

//-----------------------------------------------------------------------------
// read one segment (or less) starting from 'SpiOffset' in SPI memory
// NBytes is 0x10000 or less
// actual reads from SPI memory are done using records of 'record_size'
// exit in case of an error
//-----------------------------------------------------------------------------
int spi_read_segment(trkdaq::DtcInterface* Dtc_i, int Link, int SpiOffset, int NBytes, std::vector<char>& Res) {
  int rc(0);
  
  //  int record_size(1024);              // future version, not on the station yet 
  int  record_size(254);                  // can only read 254 bytes (127 shorts) at a time
  int  nw         = record_size/2;        // 
  
  int  first_addr = SpiOffset;            // initial offset in SPI memory
  bool done      = false;
  int  loc        = 0;                    // offset in readback, same as nbytes read

  Res.clear();
  Res.reserve(NBytes);

  int report_step = record_size*10;
  int report_mark = report_step;

  std::cout << __func__ << ": ---- request to read a segment, " << NBytes << " bytes, starting from SPI offset: 0x"
            << std::hex << SpiOffset << ", report every:" << std::dec << report_mark << " bytes" << std::endl;
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
    int nw = (nb-1)/2 + 1;
    int first_addr = SpiOffset+nb_read;
    rc = spi_read_record(Dtc_i, Link, first_addr, nw, (uint16_t*) (Res.data()+nb_read));
    if (rc < 0) {
      std::cout << __func__ << " read ERROR: rc:" << rc << " . BAIL OUT" << std::endl;
    }
    
    nb_read += nb;
      
    if (nb_read >= report_mark) {
      std::cout << __func__ << " -- read record nb:" << std::dec << std::setw(4) << nb
                << " total nb_read:" << std::dec << nb_read
                << " first_address:0x" << std::hex << first_addr
                << std::endl;
      report_mark += report_step;
    }
  }
  std::cout << __func__ << " DONE, total nb_read:" << std::dec << nb_read
            << ", total number of read records:" << nreads << std::endl;
  return rc;
}

//-----------------------------------------------------------------------------
// validate image already writen to SPI memory
//-----------------------------------------------------------------------------
int spi_validate_image(trkdaq::DtcInterface* Dtc_i, int Link, const ImageData_t* SpiData) {
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
  std::cout << "-- before spi_clear :"
            << " fn:" << SpiData->fn
            << " fsize:" << std::dec << fsize << std::endl;
 //-----------------------------------------------------------------------------
// read the input file and upload its content to the SPI memory
// have to do everything in 64K blocks - this is the unit in which the SPI memory
// is getting reset
//-----------------------------------------------------------------------------
  int const segment_size(0x10000); // the SPI memory is cleared in 64K segments 
  
  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);

  int n64k_segments = (fsize-1)/0x10000;

  int nrecords_tot  = (fsize-1) / 512 + 1; // total number of records to be written
  int first_addr    = SpiData->offset;
    
  for (int ib=0; ib<n64k_segments; ++ib) {
    int segment_offset = segment_size*ib;
    int nb_to_read     = segment_size;
                                        // handle last, potentially shorter, block
    if (segment_offset+segment_size > fsize) {
      nb_to_read = fsize-segment_offset;
    }

    int spi_offset   = first_addr+segment_offset;
    const char* data = fileData.data()+segment_offset;
//-----------------------------------------------------------------------------
// read one segment, 64K or less
//-----------------------------------------------------------------------------
    std::vector<char> readback;
    rc = spi_read_segment(Dtc_i,Link,spi_offset,nb_to_read,readback);
    if (rc == 0) {
                                        // compare to the original
      for (int i=0; i<nb_to_read; ++i) {
        if (readback[i] != fileData[segment_offset+i]) {
//-----------------------------------------------------------------------------
// if an inconsistency has been detected, print diagnostics and try to rewrite up to 3 times
//-----------------------------------------------------------------------------
          std::cout << __func__ << " ERROR in validating written segment starting from SPI offset:0x"
                    << std::hex << spi_offset << " . TRY AGAIN" << std::dec << std::endl;
          rc = -2;
          break;
        }
      }
      if (rc == 0) {
        std::cout << __func__ << ": segment:" << ib << " out of " << n64k_segments << " : validation succeeded" << std::endl;
      }
    }
//-----------------------------------------------------------------------------
// if rc = 0, the readback and the validation succeeded, nothing more to do
//-----------------------------------------------------------------------------
    if (rc != 0) {
      std::cout << __func__ << ": validation failed, rc:" << rc << " . BAIL OUT" << std::endl;
      return rc;
    }
  }
  
  std::cout << __func__ << ":END rc:" << rc << std::endl;
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
int spi_write_directory(trkdaq::DtcInterface* Dtc_i, int Link) {
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
int spi_write_record(trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords, uint16_t* Data) {
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

  std::cout << __func__ << ":END rc:" << rc << std::endl;
  return rc;
}

//-----------------------------------------------------------------------------
// write a single 64K byte segment (or less) . Return in case of an error
//-----------------------------------------------------------------------------
int spi_write_segment(trkdaq::DtcInterface* Dtc_i, int Link, const char* Data, int NBytes, int SpiOffset) {
  const int record_size (0x400);   // write 1Kbytes blocks
  int rc(0);

  std::cout << __func__ << " START: writing NBytes:0x" << std::hex << NBytes
            << " starting from SPI offset:0x" << SpiOffset << std::dec << std::endl;
//-----------------------------------------------------------------------------
// can write only 1024 bytes (512 shorts) at a time
//-----------------------------------------------------------------------------
  int first_addr     = SpiOffset;
  int loc            = 0;
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
      std::cout << __func__ << ": nb:" << std::setw(4) << nb
                << " first_addr:0x" << std::hex << first_addr
                << " loc:" << std::dec << loc
                << std::endl;
//-----------------------------------------------------------------------------
// test mode: don't write
//-----------------------------------------------------------------------------
      // std::this_thread::sleep_for(std::chrono::microseconds(1000));
      int spi_offset = first_addr+nb_written;             // offset in bytes
      uint16_t* data = (uint16_t*) (Data+nb_written);
      int nw         = (nb-1)/2 + 1;
      rc      = spi_write_record(Dtc_i,Link,spi_offset,nw,data);
      if (rc != 0) {
        std::cout << __func__ << " ERROR to write to SPI offset:0x" << std::hex <<  spi_offset
                  << std::dec << " . BAIL OUT" << std::endl;
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
  std::cout << __func__ << " END nb_written :" << nb_written << " rc:" << rc << std::endl;
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
int spi_load_image(trkdaq::DtcInterface* Dtc_i, int Link, const ImageData_t* SpiData, int TestMode = 1, int NWrites = -1, int Validate = 0) {
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
  std::cout << "-- before spi_clear :"
            << " fn:" << SpiData->fn
            << " fsize:" << std::dec << fsize << std::endl;
 
  //  spi_clear(Link, spi_data[Index].offset, fsize);
  rc = spi_clear_memory(Dtc_i, Link, SpiData->offset,fsize);
  if (rc < 0) {
    std::cout << " ERROR in " << __func__ << " rc:" << rc << " detected in spi_clear_memory, BAIL OUT" << std::endl;
    return rc;
  }

  std::cout << __func__ << ": after spi_clear_memory" << std::endl;
//-----------------------------------------------------------------------------
// read the input file and upload its content to the SPI memory
// have to do everything in 64K blocks - this is the unit in which the SPI memory
// is getting reset
//-----------------------------------------------------------------------------
  int const segment_size(0x10000); // the SPI memory is cleared in 64K segments 
  
  std::vector<char> fileData(fsize);
  file.read((char*) &fileData[0], fsize);

  int n64k_segments = (fsize-1)/0x10000;

  int nrecords_tot  = (fsize-1) / 512 + 1; // total number of records to be written
  int first_addr    = SpiData->offset;
    
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
    int attempt(0);
    while (1) {
      if (attempt > 3) break;                   // rc < 0
      
      rc = spi_write_segment(Dtc_i,Link,data,nb_to_write,spi_offset);
      attempt++;
      if (rc == 0) {
//-----------------------------------------------------------------------------
// segment has been written w/o detected errors, validate if requested
//-----------------------------------------------------------------------------
        if (Validate) {
          std::vector<char> readback;
          rc = spi_read_segment(Dtc_i,Link,spi_offset,nb_to_write,readback);
          if (rc == 0) {
                                        // compare to the original
            for (int i=0; i<nb_to_write; ++i) {
              if (readback[i] != fileData[segment_offset+i]) {
//-----------------------------------------------------------------------------
// if an inconsistency has been detected, print diagnostics and try to rewrite up to 3 times
//-----------------------------------------------------------------------------
                std::cout << __func__ << " ERROR in validating written segment starting from SPI offset:0x"
                          << std::hex << spi_offset << " . TRY AGAIN" << std::dec << std::endl;
                rc = -2;
                break;
              }
            }
          }
        }
      }
//-----------------------------------------------------------------------------
// if rc = 0, the readback and the validation succeeded, nothing more to do
//-----------------------------------------------------------------------------
      if (rc == 0) break;
//-----------------------------------------------------------------------------
// try again
// for the first time, the memory is reset by the microcontroller, when re-trying,
// need to clear the memory here
//-----------------------------------------------------------------------------
      spi_clear_memory(Dtc_i,Link,segment_offset,0x10000);
    }
//-----------------------------------------------------------------------------
// 3 attempts to rewrite have failed
//-----------------------------------------------------------------------------
    if (rc < 0) {
      std::cout << __func__ << " 3 attempts to write segment failed, rc:" << rc << " . BAIL OUT" << std::endl;
      return rc;
    }
  }
  
  std::cout << __func__ << ":END rc:" << rc << std::endl;
  return rc;
}


//-----------------------------------------------------------------------------
// upload images into SPI memory, but do not program the FPGA
// keep the programming step - program_iap_w_index - separate, as it needs validation
// Version defines one or two images to uploas
//-----------------------------------------------------------------------------
int dtc_reprogram_roc(trkdaq::DtcInterface* Dtc_i, int Link, const char* Version) {
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
  int  link_mask = (1 << 4*Link);
  bool skip_init(false);

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

  int test_mode = 0;                    // load for real
  
                                        // upload the .spi image

  rc = spi_load_image(dtc_i,Link,&fw->spi_file,test_mode);

  if (rc < 0) {
    return rc;
  }

                                        // upload the .bin image, if needed
  if (fw->bin_file.load_flag >= 0) {
    spi_load_image(dtc_i,Link,&fw->bin_file,test_mode);
  }
                                        // activate the image as a separate step after validating
                                        // the memory, thus the following is commented out
  // spi_program_iap_w_index(dtc_i,Link,fw->spi_file.index);
  
  return rc;
}

//-----------------------------------------------------------------------------
int test_write_record(trkdaq::DtcInterface* Dtc_i, int Link, int FirstAddr, int NWords, int DelayUs = 0) {
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
int test_spi_read(int Link, int FirstAddr, int NWords, trkdaq::DtcInterface* Dtc_i = nullptr) {

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
