//
#define __CLING__ 1

#include "srcs/otsdaq_mu2e_tracker/scripts/trk_utils.C"

#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTC.h"
#include "srcs/mu2e_pcie_utils/dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
// read_dtc_registers: read some registers
//-----------------------------------------------------------------------------
void read_dtc_registers(int NEvents=1) {

  uint16_t reg[] = { 
    0x9004, 0,
    0x9100, 0, 
    0x9140, 0,
    0x9144, 0, 
    0x9158, 0,
    0x9188, 0,
    0x9190, 1,   // bits 7-0 could be ignored, 
    0x9194, 1,   // bits 23-16 could be ignored 
    0x9198, 1,
    0x919c, 1,
    0x91a8, 0,
    0x91ac, 0,
    0x91bc, 0,
    0x91c0, 0,
    0x91c4, 0,
    0x91c8, 0,
    0x91f4, 0,
    0x91f8, 0,
    0x9374, 1,
    0x9378, 1,
    0x9380, 1,    // Link 0 Error Flags
    0x9384, 1,    // Link 1 Error Flags
    0x9388, 1,    // Link 2 Error Flags
    0x93b0, 1,
    0x93b4, 1,
    0x93b8, 1,
    0x93d0, 1,   // CFO Link EventStart Character Error Count
    0x93d8, 1,   // Input Buffer Fragment Dump Count
    0x93dc, 1,   //  Output Buffer Fragment Dump Count
    0x93e0, 0,
    0x9560, 1,   // SERDES RX CRC Error Count Link 0
    0x9564, 1,   // SERDES RX CRC Error Count Link 1
    0x9568, 1,   // SERDES RX CRC Error Count Link 2
    0x9630, 1,   // TX Data Request Packet Count Link 0
    0x9634, 1,   // TX Data Request Packet Count Link 1
    0x9638, 1,   // TX Data Request Packet Count Link 2
    0x9650, 1,   // TX Heartbeat Packet Count Link 0
    0x9654, 1,   // TX Heartbeat Packet Count Link 1
    0x9658, 1,   // TX Heartbeat Packet Count Link 2
    0x9670, 1,   // RX Data Header Packet Count Link 0
    0x9674, 1,   // RX Data Header Packet Count Link 1
    0x9678, 1,   // RX Data Header Packet Count Link 2
    0x9690, 1,   // RX Data Packet Count Link 0                             //  link 2 reset - write 0
    0x9694, 1,   // RX Data Packet Count Link 1                             //  link 2 reset - write 0
    0x9698, 1,   // RX Data Packet Count Link 2                             //  link 2 reset - write 0
    //-----------------------------------------------------------------------------
    // 2023-09-14 - new counters by Rick - in DTC2023Sep02_22_22.1
    //-----------------------------------------------------------------------------
    0x9720, 1,   // rxlink0           
    0x9724, 1,   // rxlink1           
    0x9728, 1,   // rxlink2           
    0x972C, 1,   // rxlink3           
    0x9730, 1,   // rxlink4           
    0x9734, 1,   // rxlink5           
    0x9740, 1,   // rxinputbufferin   
    0x9744, 1,   // DDRWrite          
    0x9748, 1,   // DDRRead           
    0x974C, 1,   // DMAtoPCI          
      
    0xffff
  };
//-----------------------------------------------------------------------------
// convert into enum
//-----------------------------------------------------------------------------
  auto roc  = DTC_Link_ID(0);

  int Link(0);
  int roc_mask = 1 << (4*Link);
  DTC dtc(DTC_SimMode_NoCFO,-1,roc_mask,"");
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

//-----------------------------------------------------------------------------
// reset counters
//-----------------------------------------------------------------------------
  int nreg = 0;
  int i    = 0;

  do {
    ushort r    = reg[2*i  ];
    int    flag = reg[2*i+1];
//-----------------------------------------------------------------------------
// flag=1 : a counter, reset counters
// according to Rick, to clear a counter one writes 0xffffffff to it
//-----------------------------------------------------------------------------
    if (flag != 0) dtc.GetDevice()->write_register(r,100,0xffffffff);
    nreg  += 1;
    i     += 1;
  } while (reg[2*i] != 0xffff) ;

//-----------------------------------------------------------------------------
// read registers N times
//-----------------------------------------------------------------------------
  uint res;
  for (int ievent=0; ievent<NEvents; ievent++) {
    printf("---------------------------------------------- event %i\n",ievent);
    for (int i=0; i<nreg; i++) {
      try   { 
        int rc   = dtc.GetDevice()->read_register(reg[2*i],100,&res); 
        printf("reg 0x%04x: contains 0x%08x rc=%i\n",reg[2*i],res,rc);
      }
      catch (...) {
        printf("ERROR event: %8i register %i \n", ievent, reg[2*i]);
        break;
      }
    }
  }
}
