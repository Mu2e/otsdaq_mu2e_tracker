//
#include <vector>
#include "otsdaq-mu2e-tracker/Ui/DtcInterface.hh"

#include "TRACE/tracemf.h"
#define TRACE_NAME "DtcInterface_ControlRoc"

#include "TString.h"  // includes ROOT's Form

using namespace DTCLib;

namespace trkdaq
{
//-----------------------------------------------------------------------------
// a boilerplate for a generic control_ROC.py CLI command - do we need it at  all ?
//-----------------------------------------------------------------------------
int DtcInterface::ControlRoc(const char* Command, void* Par) { return 0; }

//-----------------------------------------------------------------------------
// digi_rw over the fiber: reg 263
//-----------------------------------------------------------------------------
int DtcInterface::ControlRoc_DigiRW(ControlRoc_DigiRW_Input_t*  Input,
                                    ControlRoc_DigiRW_Output_t* Output,
                                    int                         LinkMask,
                                    int                         PrintLevel)
{
	//-----------------------------------------------------------------------------
	const int reg(263);  // for digi_rw

	std::vector<uint16_t> vec;

	vec.push_back(Input->rw);
	vec.push_back(Input->hvcal);
	vec.push_back(Input->address);
	vec.push_back(Input->data[0]);
	vec.push_back(Input->data[1]);

	bool increment_address(false);
	//-----------------------------------------------------------------------------
	// if LinkMask != -1, use it, but don't redefine fLinkMask - that would be wa-a-ay too
	// smart !
	//-----------------------------------------------------------------------------
	int link_mask = fLinkMask;
	if(LinkMask != -1)
	{
		link_mask = LinkMask;
	}
	//-----------------------------------------------------------------------------
	// loop over the links and execute
	//-----------------------------------------------------------------------------
	for(int i = 0; i < 6; i++)
	{
		int used = (link_mask >> 4 * i) & 0x1;
		if(not used)
			continue;
		auto roc = DTC_Link_ID(i);
		fDtc->WriteROCBlock(roc, reg, vec, false, increment_address, 100);
		std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

		uint16_t u;
		while((u = fDtc->ReadROCRegister(roc, 128, 1000)) != 0x8000) {};
		TLOG(TLVL_DEBUG + 1) << Form("reg:%03i val:0x%04x\n", 128, u);
		//-----------------------------------------------------------------------------
		// register 129: number of words to read, currently-  (+ 4) (ask Monica)
		//-----------------------------------------------------------------------------
		int nw = fDtc->ReadROCRegister(roc, 129, 100);
		TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 129, nw);

		nw = nw - 4;
		std::vector<uint16_t> v2;
		fDtc->ReadROCBlock(v2, roc, reg, nw, false, 100);

		if(PrintLevel > 0)
		{
			printf(" ---------------- link %i\n", i);
			PrintBuffer(v2.data(), nw);
			if(PrintLevel > 1)
			{
				trkdaq::ControlRoc_DigiRW_Output_t* o =
				    (trkdaq::ControlRoc_DigiRW_Output_t*)v2.data();

				printf("rw           : %i\n", o->rw);
				printf("hvcal        : 0x%04x\n", o->hvcal);
				printf("address      : 0x%04x\n", o->address);
				printf("data[32 bit] : 0x%02x%02x\n", o->data[1], o->data[0]);
				printf("adc_num      : 0x%04x\n", o->adc_num);
				printf("adc_mask     : 0x%04x\n", o->adc_mask);
			}
		}
	}
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	ResetRoc();
	return 0;
}

int DtcInterface::ControlRoc_Read(ControlRoc_Read_Input_t* Par,
                                  int                      LinkMask,
                                  bool                     UpdateMask,
                                  int                      PrintLevel)
{
	//-----------------------------------------------------------------------------
	// write parameters into reg 266 (via block write), sleep for some time,
	// then wait till reg 128 returns 0x8000
	/*
	            adc_mode = dtcbuffer[0];                                  // -a
	            tdc_mode = dtcbuffer[1];                                  // -t
	            num_lookback = dtcbuffer[2];                              // -l
	            num_triggers = (dtcbuffer[4] << 16) + dtcbuffer[3];         // -T
	            channel_mask[0] = (dtcbuffer[6] << 16) + dtcbuffer[5];    // -C
	            channel_mask[1] = (dtcbuffer[8] << 16) + dtcbuffer[7];    // -D
	            channel_mask[2] = (dtcbuffer[10] << 16) + dtcbuffer[9];    // -E
	            num_samples = dtcbuffer[11];                     // -s
	            enable_pulser = (uint8_t) dtcbuffer[12];         // -p
	            max_total_delay = dtcbuffer[13];                 // -d (def 1)
	            marker_clock = (uint8_t) dtcbuffer[14];          // -m
	*/
	//-----------------------------------------------------------------------------
	int version(1);  // v2: 20w ewaponse version, v1: 18w version
	TLOG(TLVL_DEBUG) << "version: " << version;

	const int             reg(265);  // for control_ROC.py(read)
	std::vector<uint16_t> vec;

	TLOG(TLVL_DEBUG) << "LinkMask: 0x" << std::hex << LinkMask << std::dec
	                 << " PrintLevel:" << PrintLevel;

#if READ_ROC_VERSION == 1
	vec.push_back(Par->adc_mode);
	vec.push_back(Par->tdc_mode);
	vec.push_back(Par->num_lookback);

	uint16_t w1 = Par->num_triggers[0];
	uint16_t w2 = Par->num_triggers[1];

	vec.push_back(w1);
	vec.push_back(w2);

	for(int i = 0; i < 6; i++)
		vec.push_back(Par->ch_mask[i]);

	if(Par->num_samples > 63)
	{
		TLOG(TLVL_WARNING) << "num_samples:" << Par->num_samples
		                   << " gt 63, truncate to 63";
		Par->num_samples = 63;
	}

	vec.push_back(Par->num_samples);

	vec.push_back(Par->enable_pulser);
	vec.push_back(1);  // max_total_delay (unused)
	vec.push_back(Par->marker_clock);
	// vec.push_back(0 );
	// vec.push_back(99);
#elif READ_ROC_VERSION == 2
	vec.push_back(Par->adc_mode);
	vec.push_back(Par->tdc_mode);
	vec.push_back(Par->num_lookback);

	if(Par->num_samples > 63)
	{
		TLOG(TLVL_WARNING) << "num_samples:" << Par->num_samples
		                   << " gt 63, truncate to 63";
		Par->num_samples = 63;
	}

	vec.push_back(Par->num_samples);

	uint16_t w1 = Par->num_triggers[0];
	uint16_t w2 = Par->num_triggers[1];

	vec.push_back(w1);
	vec.push_back(w2);

	for(int i = 0; i < 6; i++)
		vec.push_back(Par->ch_mask[i]);

	vec.push_back(Par->enable_pulser);
	// vec.push_back(1 );                  // max_total_delay (unused)
	vec.push_back(Par->marker_clock);
	vec.push_back(Par->mode);
	vec.push_back(Par->clock);
	// vec.push_back(0 );
	// vec.push_back(99);
#endif

	bool increment_address(false);
	//-----------------------------------------------------------------------------
	// if LinkMask != -1, use it
	// in addition, if UpdateMask=true, update the DTC link mask (fLinkMask)
	//-----------------------------------------------------------------------------
	int link_mask = fLinkMask;
	if(LinkMask != -1)
	{
		link_mask = LinkMask;
		if(UpdateMask)
			fLinkMask = LinkMask;
	}

	for(int i = 0; i < 6; i++)
	{
		int used = (link_mask >> 4 * i) & 0x1;
		if(not used)
			continue;
		auto roc = DTC_Link_ID(i);
		fDtc->WriteROCBlock(roc, reg, vec, false, increment_address, 100);
		std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

		// 0x86 = 0x82 + 4
		uint16_t u;
		while((u = fDtc->ReadROCRegister(roc, 128, 1000)) != 0x8000) {};
		TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 128, u);
		//-----------------------------------------------------------------------------
		// register 129: number of words to read, currently-  (+ 4) (ask Monica)
		//-----------------------------------------------------------------------------
		int nw = fDtc->ReadROCRegister(roc, 129, 100);
		TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 129, nw);

		nw = nw - 4;
		std::vector<uint16_t> v2;
		fDtc->ReadROCBlock(v2, roc, reg, nw, false, 100);

		if(PrintLevel & 0x1)
		{
			PrintBuffer(v2.data(), nw);
		}

		if(PrintLevel & 0x2)
		{
			trkdaq::ControlRoc_Read_Output_t* o =
			    (trkdaq::ControlRoc_Read_Output_t*)v2.data();

#if READ_ROC_VERSION == 1
			printf("enable_pulser   : %i\n", o->enable_pulser);
			printf("num_samples     : %i\n", o->num_samples);
			printf("num_lookback    : %i\n", o->num_lookback);
			printf("ch_mask         : 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x\n",
			       o->ch_mask[0],
			       o->ch_mask[1],
			       o->ch_mask[2],
			       o->ch_mask[3],
			       o->ch_mask[4],
			       o->ch_mask[5]);
			printf("adc_mode        : %i\n", o->adc_mode);
			printf("tdc_mode        : %i\n", o->tdc_mode);
			printf("num_triggers    : %5i %5i\n", o->num_triggers[0], o->num_triggers[1]);
			printf("digi_read_0xb   : 0x%04x\n", o->digi_read_0xb);
			printf("digi_read_0xe   : 0x%04x\n", o->digi_read_0xe);
			printf("digi_read_0xd   : 0x%04x\n", o->digi_read_0xd);
			printf("digi_read_0xc   : 0x%04x\n", o->digi_read_0xc);
			printf("mode            : %i\n", o->mode);
			printf("clock           : %i\n", o->clock);
			printf("marker_clock    : %i\n", o->marker_clock);
#elif READ_ROC_VERSION == 2
			printf("adc_mode     : %i\n", o->adc_mode);
			printf("tdc_mode     : %i\n", o->tdc_mode);
			printf("num_lookback : %i\n", o->num_lookback);
			printf("num_triggers : %5i %5i\n", o->num_triggers[0], o->num_triggers[1]);
			printf("ch_mask      : 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x 0x%04x\n",
			       o->ch_mask[0],
			       o->ch_mask[1],
			       o->ch_mask[2],
			       o->ch_mask[3],
			       o->ch_mask[4],
			       o->ch_mask[5]);
			printf("num_samples  : %i\n", o->num_samples);
			printf("enable_pulser : %i\n", o->enable_pulser);
			printf("marker_clock  : %i\n", o->marker_clock);
			printf("mode          : %i\n", o->mode);
			printf("clock         : %i\n", o->clock);
			printf("digi_read_0xb : 0x%04x\n", o->digi_read_0xb);
			printf("digi_read_0xe : 0x%04x\n", o->digi_read_0xe);
			printf("digi_read_0xd : 0x%04x\n", o->digi_read_0xd);
			printf("digi_read_0xc : 0x%04x\n", o->digi_read_0xc);
#endif
		}
	}
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	ResetRoc();
	return 0;
}

//-----------------------------------------------------------------------------
int DtcInterface::ControlRoc_SetGain(int Link, int ChannelID, int PreampType, int Gain)
{
	//-----------------------------------------------------------------------------
	// convert into enum
	//-----------------------------------------------------------------------------
	auto roc = DTC_Link_ID(Link);
	//-----------------------------------------------------------------------------
	// write parameters into reg 266 (block write) , sleep for some time,
	// then wait till reg 128 returns 0x8000
	//-----------------------------------------------------------------------------
	std::vector<uint16_t> vec;
	vec.push_back(uint16_t(ChannelID));
	vec.push_back(uint16_t(Gain));
	vec.push_back(uint16_t(PreampType));

	bool increment_address(false);
	fDtc->WriteROCBlock(roc, 266, vec, false, increment_address, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	// 0x86 = 0x82 + 4
	uint16_t u;
	while((u = fDtc->ReadROCRegister(roc, 128, 100)) != 0x8000) {};
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 128, u);
	//-----------------------------------------------------------------------------
	// register 129: number of words to read, currently-  (+ 4) (ask Monica)
	//-----------------------------------------------------------------------------
	int nw = fDtc->ReadROCRegister(roc, 129, 100);
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 129, nw);

	nw = nw - 4;
	std::vector<uint16_t> v2;
	fDtc->ReadROCBlock(v2, roc, 266, nw, false, 100);

	PrintBuffer(v2.data(), nw);
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	ResetRoc(Link);
	return 0;
}

//-----------------------------------------------------------------------------
int DtcInterface::ControlRoc_SetThreshold(int Link,
                                          int ChannelID,
                                          int PreampType,
                                          int Threshold)
{
	//-----------------------------------------------------------------------------
	// convert into enum
	//-----------------------------------------------------------------------------
	auto roc = DTC_Link_ID(Link);
	//-----------------------------------------------------------------------------
	// write parameters into reg 267 (block write) , sleep for some time,
	// then wait till reg 128 returns 0x8000
	//-----------------------------------------------------------------------------
	std::vector<uint16_t> vec;
	vec.push_back(uint16_t(ChannelID));
	vec.push_back(uint16_t(Threshold));
	vec.push_back(uint16_t(PreampType));

	bool increment_address(false);
	fDtc->WriteROCBlock(roc, 267, vec, false, increment_address, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	// 0x86 = 0x82 + 4
	uint16_t u;
	while((u = fDtc->ReadROCRegister(roc, 128, 100)) != 0x8000) {};
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 128, u);
	//-----------------------------------------------------------------------------
	// register 129: number of words to read, currently-  (+ 4) (ask Monica)
	//-----------------------------------------------------------------------------
	int nw = fDtc->ReadROCRegister(roc, 129, 100);
	TLOG(TLVL_DEBUG) << Form("reg:%03i val:0x%04x\n", 129, nw);

	nw = nw - 4;
	std::vector<uint16_t> v2;
	fDtc->ReadROCBlock(v2, roc, 267, nw, false, 100);

	PrintBuffer(v2.data(), nw);
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	ResetRoc(Link);
	return 0;
}

//-----------------------------------------------------------------------------
int DtcInterface::ControlRoc_MeasureThresholds(int      Link,
                                               uint32_t MaskC,
                                               uint32_t MaskD,
                                               uint32_t MaskE)
{
	//-----------------------------------------------------------------------------
	// convert into enum
	//-----------------------------------------------------------------------------
	auto roc = DTC_Link_ID(Link);

	// int roc_mask = 1 << (4*Link);
	//-----------------------------------------------------------------------------
	// write parameters into reg 264 (block write) , sleep for some time,
	// then wait till reg 128 returns 0x8000
	//-----------------------------------------------------------------------------
	// uint16_t chan_mask[6] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

	std::vector<uint16_t> vec;

	vec.push_back((MaskC) & 0xffff);
	vec.push_back((MaskC >> 16) & 0xffff);
	vec.push_back((MaskD) & 0xffff);
	vec.push_back((MaskD >> 16) & 0xffff);
	vec.push_back((MaskE) & 0xffff);
	vec.push_back((MaskE >> 16) & 0xffff);

	bool increment_address(false);
	fDtc->WriteROCBlock(roc, 270, vec, false, increment_address, 100);
	std::this_thread::sleep_for(std::chrono::microseconds(fSleepTimeROCWrite));

	// 0x86 = 0x82 + 4
	uint16_t u;
	while((u = fDtc->ReadROCRegister(roc, 128, 100)) != 0x8000) {};
	printf("reg:%03i val:0x%04x\n", 128, u);
	//-----------------------------------------------------------------------------
	// register 129: number of words to read, currently-  (+ 4) (ask Monica)
	//-----------------------------------------------------------------------------
	int nw = fDtc->ReadROCRegister(roc, 129, 100);
	printf("reg:%03i val:0x%04x\n", 129, nw);

	nw = nw - 4;
	std::vector<uint16_t> v2;
	fDtc->ReadROCBlock(v2, roc, 270, nw, false, 100);
	//-----------------------------------------------------------------------------
	//
	//-----------------------------------------------------------------------------
	fDtc->WriteROCRegister(roc, 14, 0x01, false, 1000);

	PrintBuffer(v2.data(), nw);
	// expect nw=288 = 96*3, if not - in trouble

	for(int i = 0; i < 96; i++)
	{
		float hw  = (-1000. + v2[i] * 2000. / 1024.) / 10.;
		float cal = (-1000. + v2[96 + i] * 2000. / 1024.) / 10.;
		float tot = (-1000. + v2[192 + i] * 2000. / 1024.) / 10.;

		printf(" i, hw, cal, tot : %3i %10.3f %10.3f %10.3f\n", i, hw, cal, tot);
	}
	return 0;
}

};  // namespace trkdaq
