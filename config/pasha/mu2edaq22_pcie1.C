///////////////////////////////////////////////////////////////////////////////
// mu2edaq22 :
// init_run_configuration : the name and the call signature are is fixed
//                          and can't be changed
///////////////////////////////////////////////////////////////////////////////
#include "otsdaq-mu2e-tracker/Gui/DtcGui.hh"
int init_run_configuration(DtcGui* X)
{
	int rc(0);

	printf("[init_run_configuration] : host:%s\n", gSystem->Getenv("HOSTNAME"));

	DtcGui::DtcData_t* dtc = (DtcGui::DtcData_t*)X->fDtcData;

	X->fNDtcs = 1;  // 2;              // installed on a machine

	// dtc[0].fName        = "CFO";
	// dtc[0].fPcieAddr    = 0;
	// dtc[0].fLinkMask    = 0x2;            // 2 DTCs on link0
	// gSystem->Setenv("CFOLIB_CFO","0");

	dtc[0].fName     = "DTC";
	dtc[0].fPcieAddr = 1;
	dtc[0].fLinkMask = 0x010;  // TS2 (0x10) +TS1
	//  dtc[0].fLinkMask    = 0x01;           // TS1
	dtc[0].fRocReadoutMode  = 1;    // 0:patterns 1:digis 2:ROC checkerboard
	dtc[0].fRocLaneMask     = 0xf;  // 0:patterns 1:digis
	dtc[0].fRocNHitsPerLane = 10;   // for mode=2

	dtc[0].fJAMode = 0x01;

	dtc[0].fDtcID       = 45;  // for 1 node, make it the same as PcieAddr
	dtc[0].fPartitionID = 0;
	dtc[0].fMacAddrByte = 0;

	dtc[0].fOnSpill   = 1;
	dtc[0].fEventMode = 1;

	gSystem->Setenv("DTCLIB_DTC", "1");

	return rc;
}
