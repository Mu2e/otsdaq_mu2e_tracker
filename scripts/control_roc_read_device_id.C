//
#define __CLING__ 1

#include "trk_utils.C"

#include "dtcInterfaceLib/DTC.h"
#include "dtcInterfaceLib/DTCSoftwareCFO.h"

using namespace DTCLib;

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void control_roc_print_device_id(uint16_t* data, int nw)
{
	printf("DeviceSerial: 0x");
	for(int iw = 15; iw >= 0; iw--)
	{
		printf("%02x", data[iw]);
	}
	printf("\n");
	// ROC ID is the last 2 1/2 bytes
	printf("DRAC ROC ID : %02x%02x%1x\n",
	       data[15] & 0xff,
	       data[14] & 0xff,
	       (data[13] >> 4) & 0xf);
	printf("check sum   : %02x%02xROC\n", data[16] & 0xff, data[17] & 0xff);
}

//-----------------------------------------------------------------------------
// PrintLevel=0: no printout
//            1: hex dump
//            2: formatted printout
//-----------------------------------------------------------------------------
void control_roc_read_device_id(int Link, int PrintLevel = 0)
{
	//-----------------------------------------------------------------------------
	// convert into enum
	// DTC has already been initialized, don't reco
	//-----------------------------------------------------------------------------
	auto roc = DTC_Link_ID(Link);

	DtcInterface* dtc_i = trkdaq::DtcInterface::Instance(-1);
	DTCLib::DTC*  dtc   = dtc_i->Dtc();

	int roc_mask = 1 << (4 * Link);

	//  dtc->WriteROCRegister(roc,14,0x01,false,1000);  // don't need to reset the ROC
	//-----------------------------------------------------------------------------
	// write parameters into reg ***  (block write) , sleep for some time,
	// then wait till reg 128 returns 0x8000
	// chan mask always includes the first channel
	//-----------------------------------------------------------------------------
	dtc->WriteROCRegister(roc, 260, 0x0000, false, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(1000));

	uint16_t u;
	while((u = dtc->ReadROCRegister(roc, 128, 5000)) != 0x8000) {};
	if((PrintLevel & 0x1) != 0)
		printf("reg:%03i val:0x%04x\n", 128, u);
	//-----------------------------------------------------------------------------
	// register 129: number of words to read, currently-  (+ 4) (ask Monica)
	//-----------------------------------------------------------------------------
	int nw = dtc->ReadROCRegister(roc, 129, 100);
	if((PrintLevel & 0x1) != 0)
		printf("reg:%03i val:0x%04x\n", 129, nw);

	nw = nw - 4;
	vector<uint16_t> v2;
	dtc->ReadROCBlock(v2, roc, 260, nw, false, 100);
	//-----------------------------------------------------------------------------
	// print output - in two formats
	//-----------------------------------------------------------------------------
	if((PrintLevel & 0x1) != 0)
	{
		printf("------------------------------------------------nw = %i(0x%x)\n", nw, nw);
		print_buffer(v2.data(), nw);
	}
	if((PrintLevel & 0x2) != 0)
	{
		control_roc_print_device_id(v2.data(), nw);
	}
}
