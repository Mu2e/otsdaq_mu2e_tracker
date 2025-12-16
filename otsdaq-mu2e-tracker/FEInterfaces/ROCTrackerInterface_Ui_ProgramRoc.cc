
// clang-format off

//-----------------------------------------------------------------------------
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
//-----------------------------------------------------------------------------


#include "otsdaq-mu2e-tracker/FEInterfaces/ROCTrackerInterface.h"


#include <TString.h>  // includes ROOT's Form

#include <filesystem>


using namespace ots;

#undef __MF_SUBJECT__
#define __MF_SUBJECT__ "FE-ROCTrackerInterface"


//==============================================================================
///	Ui_ProgramRoc_SpiClearMemory()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_SpiClearMemory(int Link, const trkdaq::roc_fw_data_t* Dir, int PrintLevel, std::ostream& Stream) 
{
    int rc(0);
    std::vector<uint16_t> input;
                                                  // determine file size
    int nbytes(0);
    try {
      nbytes = std::filesystem::file_size(Dir->fn);
    }
    catch (...) {
      Stream << __func__ << "ERROR: wrong file name:" << Dir->fn << "  ?" << std::endl;
      return -1;
    }
    
    int offset = Dir->offset;

    input.push_back(SPI_CLEAR);                 // SPI clear
    input.push_back( offset         & 0xFFFF);  // 
    input.push_back((offset  >> 16) & 0xFFFF);  // 
    input.push_back( nbytes         & 0xFFFF);  // 
    input.push_back((nbytes  >> 16) & 0xFFFF);  // 5 words in total

    bool increment_address(false);
    auto roc  = DTCLib::DTC_Link_ID(Link);
    getDTC()->WriteROCBlock(roc,RREG,input,false,increment_address,100);

    if (PrintLevel != 0) {
      Stream << __func__ << ": SPI_CLEAR input written " << std::endl;
    }
  
    uint16_t u; 
    while ((u = getDTC()->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
    // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
    if (PrintLevel != 0) {
      Stream << __func__ << ":END" << std::endl;
    }
    return rc;
  } // end Ui_ProgramRoc_SpiClearMemory()

//==============================================================================
///	Ui_ProgramRoc_SpiIapIndex()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_SpiIapIndex(int Link, const trkdaq::roc_fw_data_t* DirEntry, int PrintLevel, std::ostream& Stream) 
{
    std::vector<uint16_t> input;

    input.push_back(PROGRAM_IAP_W_INDEX);               // SPI clear
    input.push_back( DirEntry->index        & 0xFFFF);  // 
    input.push_back((DirEntry->index >> 16) & 0xFFFF);  // 
    input.push_back(0);                                 // 
    input.push_back(0);                                 // 5 words total

    bool increment_address(false);
    auto roc  = DTCLib::DTC_Link_ID(Link);
    getDTC()->WriteROCBlock(roc,RREG,input,false,increment_address,100);
    // while the image is being uploaded, there is nothing to check. may need less than 40 sec
    sleep(40);
                                                        // wait till the command is executed
    uint16_t u;
    while ((u = getDTC()->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

                                        // images loaded, do soft reset before checking the ROC
    getDTC()->SoftReset();
    uint16_t status = getDTC()->ReadROCRegister(roc,REG_STATUS,1000);
    if (PrintLevel != 0) Stream << __func__ << ":END status:" << status << std::endl;
    return (int) status;
  } // end Ui_ProgramRoc_SpiIapIndex()

//==============================================================================
///	Ui_ProgramRoc_SpiIapAddress()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_SpiIapAddress(int Link, const trkdaq::roc_fw_data_t* DirEntry, int PrintLevel, std::ostream& Stream) 
{
    std::vector<uint16_t> input;

    input.push_back(PROGRAM_IAP_W_ADDRESS);              // SPI clear
    input.push_back( DirEntry->offset        & 0xFFFF);  // 
    input.push_back((DirEntry->offset >> 16) & 0xFFFF);  // 
    input.push_back(0);                                  // 
    input.push_back(0);                                  // 5 words in total

    bool increment_address(false);
    auto roc  = DTCLib::DTC_Link_ID(Link);
    getDTC()->WriteROCBlock(roc,RREG,input,false,increment_address,100);
    // while the image is being uploaded, there is no one to talk to, just sleep
    // may need less than 40 sec
    sleep(40);
                                                          // wait till the command is executed
    uint16_t u;
    while ((u = getDTC()->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

                                        // images loaded, do soft reset before checking the ROC
    getDTC()->SoftReset();
    uint16_t status = getDTC()->ReadROCRegister(roc,REG_STATUS,1000);
    if (PrintLevel != 0) Stream << __func__ << ":END status:" << status << std::endl;
    return (int) status;
  } // end Ui_ProgramRoc_SpiIapAddress()

//==============================================================================
///	Ui_ProgramRoc_SpiLoadImage()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_SpiLoadImage(int Link, const trkdaq::roc_fw_data_t* DirEntry, int Doit, int NWrites, int PrintLevel, std::ostream& Stream) 
{
    //   int rc(0);
//-----------------------------------------------------------------------------
// open input file and determine its size
//-----------------------------------------------------------------------------
    std::ifstream file(DirEntry->fn, std::ios::binary);
    file.seekg(0, std::ios::end);
    int fsize = file.tellg();
    file.seekg(0, std::ios::beg);
//-----------------------------------------------------------------------------
// clear the spi memory for image at index *** ###
//-----------------------------------------------------------------------------
    Stream << __func__ << ": -- before spi_clear : index:" << DirEntry->index
           << " fn:" << DirEntry->fn
           << " fsize:" << std::dec << fsize << std::endl;
 
    //  spi_clear(Link, spi_data[Index].offset, fsize);
    Ui_ProgramRoc_SpiClearMemory(Link, DirEntry);

    Stream << "-- after clear" << std::endl;
//-----------------------------------------------------------------------------
// read the input file and upload its content to the SPI memory
//-----------------------------------------------------------------------------
    std::vector<char> fileData(fsize);
    file.read((char*) &fileData[0], fsize);

    int first_addr = DirEntry->offset;
    int nw         = 512;
    int done       = 0;
    int loc        = 0;
    int nwrites    = 0;
    int nerrors    = 0;
//-----------------------------------------------------------------------------
// can write only 1024 bytes (512 shorts) at a time
//-----------------------------------------------------------------------------
    while (not done) {
                                        // last record could be shorter
      if (loc + 2*nw > fsize) {
                                        // assume the file is written in 2-byte words
        nw   = (fsize-loc)/2;
        done = 1;
      }
    
      if (nw > 0) {
//-----------------------------------------------------------------------------
// write next record
//-----------------------------------------------------------------------------
        Stream << __func__ << ": nwrites" << std::setw(6) << nwrites << " nw:" << std::setw(4) << nw
               << " first_addr:0x" << std::hex << first_addr
               << " loc:" << std::dec << loc
               << std::endl;

        if (Doit) {
          uint16_t* data = (uint16_t*) &fileData[loc];
          int rc         = Ui_ProgramRoc_SpiWriteRecord(Link,first_addr,nw,data);
          if (rc != 0) {
            Stream << "-- ERROR: nwrites:" << nwrites << " rc:0x" << std::hex << rc << std::endl;
            nerrors++;
          }
        }
        
        loc        += 2*nw;
        first_addr += 2*nw;
        
        nwrites ++;
        if ((NWrites > 0) and (nwrites > NWrites)) break;
      }
    }

    Stream << "END nwrites:" << nwrites << " fsize:" << fsize << " nerrors:" << nerrors << std::endl;
//-----------------------------------------------------------------------------
// validate writing (not debugged yet)
//-----------------------------------------------------------------------------
    int ierror(0);
//   if (Validate) {
//     first_addr  = spi_data[Index].offset;   // offset in SPI memory
//     nw          = 127;
//     done        = 0;
//     loc         = 0;                    // offset in the 'file'
//     int nreads  = 0;
// //-----------------------------------------------------------------------------
// // can read only 254 bytes (127 shorts) at a time
// //-----------------------------------------------------------------------------
//     while (not done) {
//       if (loc + 2*nw > fsize) {
//         nw   = (fsize-loc)/2;
//       }
      
//       if (nw <= 0) {
//         done = 1;
//       }
//       else {
// //-----------------------------------------------------------------------------
// // read next record
// //-----------------------------------------------------------------------------
//         std::cout << "-- nw:" << std::setw(4) << nw
//                   << " first_addr:0x" << std::hex << first_addr
//                   << " loc:" << std::dec << loc
//                   << std::endl;
        
//         std::vector<uint16_t> res;
//         spi_flash_read(DtcInterface, Link, first_addr, nw, &res);
// //-----------------------------------------------------------------------------
// // compare to the original
// //-----------------------------------------------------------------------------
//         for (int iw=0; iw<nw; iw++) {
//           if (res[iw] != fileData[loc+iw]) {
//             std::cout << "ERROR at loc:" << loc << " iw:" << iw << std::endl;
//             ierror = 1;
//           }
//         }

//         if (ierror != 0) break;
//         loc        += 2*nw;
//         first_addr += nw;
      
//         nreads ++;
//       }
//     }
//  }
  
    Stream << __func__ << ":END ierror:" << ierror << std::endl;
    return nerrors;
  } // end Ui_ProgramRoc_SpiLoadImage()

//==============================================================================
///	Ui_ProgramRoc_SpiReadFlash()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_SpiReadFlash(int Link, int Address, int NWords, std::vector<uint16_t>* Res,
                                 int PrintLevel, std::ostream& Stream) 
{

    std::vector<uint16_t> input;

    input.push_back(SPI_FLASH_READ);            // SPI flash read
    input.push_back((Address      ) & 0xFFFF);  // starting address LSB
    input.push_back((Address >> 16) & 0xFFFF);  // starting address MSB
    input.push_back(NWords*2);                  // number of bytes to read (max 254 bytes = 127 words)
    input.push_back(0);                         // 5 words total

    bool increment_address(false);
    auto roc  = DTCLib::DTC_Link_ID(Link);
    getDTC()->WriteROCBlock(roc,RREG,input,false,increment_address,100);

    if (PrintLevel & 0x1) {
      std::cout << " input written " << std::endl;
    }
  
    uint16_t u; 
    while ((u = getDTC()->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

    int nw (-1);
    nw = getDTC()->ReadROCRegister(roc,129,1000);  // should return NWords+4
//-----------------------------------------------------------------------------
// validation: reading back and comparing
//-----------------------------------------------------------------------------
    Ui_RocBlockRead(Link,RREG,*Res);
    int nw_read = Res->size();
    
    if (PrintLevel != 0) {
      std::cout << "nw:" << nw << " nw read:" << nw_read << std::endl;
      if (PrintLevel & 0x2) {
        Ui_print_PrintBuffer(Res->data(),nw_read);
      }
    }
    
    return nw_read;
  } // end Ui_ProgramRoc_SpiReadFlash()

//==============================================================================
///	Ui_ProgramRoc_SpiWriteDirectory()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_SpiWriteDirectory(int Link, const trkdaq::roc_fw_data_t* Dir, int PrintLevel, std::ostream& Stream) 
{
    int rc(0);
    
    int nimages = 0;
    for (int i=0; Dir[i].offset >= 0; ++i) nimages++;

    std::cout << __func__ << ": nimages:" << nimages << std::endl;

    std::vector<uint16_t> input;
                                          // 5 words
    input.push_back(SPI_WRITE_DIRECTORY); // SPI flash read
    input.push_back(nimages & 0xFFFF);    // nimages LSB
    input.push_back(0               );    // nimages MSB
    input.push_back(0               );    // 2nd command : 2 16-bit words - uunused
    input.push_back(0               );    // unused
  
    for (int i=0; i<nimages; ++i) {
      int offset = Dir[i].offset;
      input.push_back( offset        & 0xFFFF);                             //
      input.push_back((offset >> 16) & 0xFFFF);                             //
    }
    
    bool increment_address(false);

    auto roc  = DTCLib::DTC_Link_ID(Link);
    getDTC()->WriteROCBlock(roc,RREG,input,false,increment_address,100);
    
    std::cout << __func__ << ": SPI_WRITE_DIRECTORY input written " << std::endl;
    
    uint16_t u; 
    while ((u = getDTC()->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 
    // TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n",128,u);
  

    // int nw (-1);
    // nw = dtc_i->getDTC()->ReadROCRegister(roc,129,1000);
  
    // std::vector<uint16_t> res;

    // dtc_i->Ui_RocBlockRead(Link,RREG,res);
    // int nw = res.size();
    
    // std::cout << "nw read:" << nw << std::endl;
    // dtc_i->Ui_print_PrintBuffer(res.data(),nw);
    
    //  return nw;
    std::cout << __func__ << ":END" << std::endl;
    return rc;
  } // end Ui_ProgramRoc_SpiWriteDirectory()

//==============================================================================
///	Ui_ProgramRoc_SpiWriteRecord()
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_SpiWriteRecord(int Link, int FirstAddr, int NWords, const uint16_t* Data, int PrintLevel, std::ostream& Stream) 
{
    int rc(0);

    std::vector<uint16_t> input;

    input.push_back(SPI_WRITE_RECORD);            // SPI clear
    input.push_back( FirstAddr        & 0xFFFF);  // 
    input.push_back((FirstAddr >> 16) & 0xFFFF);  // 
    input.push_back( NWords           & 0xFFFF);  // 
    input.push_back((NWords    >> 16) & 0xFFFF);  // 5 words in total

    for (int i=0; i<NWords; ++i) {
      input.push_back(Data[i]);
    }

    bool increment_address(false);
    auto roc  = DTCLib::DTC_Link_ID(Link);
    getDTC()->WriteROCBlock(roc,RREG,input,false,increment_address,100);

    // if (PrintLevel != 0) Stream << " data written, nb:" << NWords*2 << std::endl;
  
    uint16_t u; 
    while ((u = getDTC()->ReadROCRegister(roc,128,1000)) != 0x8000) {}; 

    rc = getDTC()->ReadROCRegister(roc,132,1000);

    if (PrintLevel != 0) {
      Stream << __func__ << ":END rc:" << rc << std::endl;
    }
    return rc;
  } // end Ui_ProgramRoc_SpiWriteRecord()

//==============================================================================
///	Ui_ProgramRoc_ProgramRoc()
/// 
/// This file was auto-generated from otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc
/// Do not modify this file directly.
///
/// To modify, edit otsdaq-mu2e-tracker/Ui//DtcInterface_ProgramRoc.cc and re-run the import tool:
///
///   otsdaq_import_tracker_test_stand   otsdaq-mu2e-tracker/Ui/   otsdaq-mu2e-tracker/FEInterfaces/
///
int ROCTrackerInterface::Ui_ProgramRoc_ProgramRoc(int Link, const trkdaq::RocFwData_t* Fw, const char* Version, int Doit, int PrintLevel, std::ostream& Stream) 
{
    int rc(0);
                                        // offset=-1 flags the end
    int nimages(0);
    for (int i=0; Fw->spi_directory[i].offset>=0; i++) nimages++;
    
                                        // 2. find if the requested firmware version is available
    const trkdaq::roc_fw_version_t* fw(nullptr);
  
    for (int i=0; Fw->version[i].name != ""; ++i) {
      const trkdaq::roc_fw_version_t* fww = &Fw->version[i];
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
    Stream << "--- writing SPI directory" << std::endl;
    Ui_ProgramRoc_SpiWriteDirectory(Link,Fw->spi_directory,PrintLevel,Stream);

                                        // upload the .spi image
    
    Stream << "--- loading " << Fw->spi_directory[fw->index_spi].fn << std::endl;
    rc = Ui_ProgramRoc_SpiLoadImage(Link,&Fw->spi_directory[fw->index_spi],Doit);
    if (rc != 0) return rc;

                                        // upload the .bin image, if defined
    if (fw->index_bin >= 0) {       
      Stream << "--- loading image:" << Fw->spi_directory[fw->index_bin].fn << std::endl;
      rc = Ui_ProgramRoc_SpiLoadImage(Link,&Fw->spi_directory[fw->index_bin],Doit);
      if (rc != 0) return rc;
    }
//-----------------------------------------------------------------------------
// activate the image. Activation itself is a trivial step - internal logic,
// however it requires the images to be actually written
//-----------------------------------------------------------------------------
    if (Doit) {
      Stream << "--- activating SPI image" << std::endl;
      rc = Ui_ProgramRoc_SpiIapIndex(Link,&Fw->spi_directory[fw->index_spi]);
    }
    
    return rc;
  } // end Ui_ProgramRoc_ProgramRoc()

// clang-format on
