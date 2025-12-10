//==============================================================================
// jxglib/USBDevice.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_H
#define PICO_JXGLIB_USBDEVICE_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "tusb.h"

#define TUD_VIDEO_DESC_LEN 1024

namespace jxglib::USBDevice {

class Controller;

class Interface {
protected:
	Controller& deviceController_;
	uint8_t interfaceNum_;
	uint8_t iInstance_;
public:
	Interface(Controller& deviceController, int nInterfacesToOccupy);
	virtual ~Interface() {}
public:
	void RegisterConfigDesc(const void* configDesc, int bytes);
public:
	void Initialize() {}
};

#if CFG_TUD_HID == 0
using HID = Interface;
#else
class HID;
#endif

#if CFG_TUD_CDC == 0
using CDC = Interface;
#else
class CDC;
#endif

#if CFG_TUD_MSC == 0
using MSC = Interface;
#else
class MSC;
#endif

#if CFG_TUD_MIDI == 0
using MIDI = Interface;
#else
class MIDI;
#endif

#if CFG_TUD_AUDIO == 0
using Audio = Interface;
#else
class Audio;
#endif

#if CFG_TUD_VIDEO == 0
using Video = Interface;
#else
class Video;
#endif

#if CFG_TUD_BTH == 0
using BTH = Interface;
#else
class BTH;
#endif

#if CFG_TUD_VENDOR == 0
using Vendor = Interface;
#else
class Vendor;
#endif

//-----------------------------------------------------------------------------
// Controller
//-----------------------------------------------------------------------------
class Controller : public Tickable {
public:
	static const int nInstancesMax = CFG_TUD_CDC + CFG_TUD_MSC + CFG_TUD_HID + CFG_TUD_AUDIO +
		CFG_TUD_VIDEO + CFG_TUD_MIDI + CFG_TUD_VENDOR + CFG_TUD_BTH;
private:
	tusb_desc_device_t deviceDesc_;
	uint8_t rhport_;
	uint8_t interfaceNumCur_;
	uint8_t nInstances_CDC_;
	uint8_t nInstances_MSC_;
	uint8_t nInstances_HID_;
	uint8_t nInstances_Video_;
	union {
		Interface* interfaceTbl_[nInstancesMax];
		struct {
			CDC* pCDCTbl[CFG_TUD_CDC];
			MSC* pMSCTbl[CFG_TUD_MSC];
			HID* pHIDTbl[CFG_TUD_HID];
			Video* pVideoTbl[CFG_TUD_VIDEO];
			MIDI* pMIDITbl[CFG_TUD_MIDI];
			Audio* pAudioTbl[CFG_TUD_AUDIO];
			Vendor* pVendorTbl[CFG_TUD_VENDOR];
			BTH* pBTHTbl[CFG_TUD_BTH];
		} specific_;
	};
	int offsetConfigDesc_;
	uint8_t configDescAccum_[
		TUD_CONFIG_DESC_LEN +
		TUD_CDC_DESC_LEN *			CFG_TUD_CDC +
		TUD_MSC_DESC_LEN *			CFG_TUD_MSC +
		TUD_HID_DESC_LEN *			CFG_TUD_HID +
		TUD_VIDEO_DESC_LEN *		CFG_TUD_VIDEO +
	//	TUD_AUDIO_DESC_LEN *		CFG_TUD_AUDIO +
		TUD_MIDI_DESC_LEN *			CFG_TUD_MIDI +
		TUD_VENDOR_DESC_LEN *		CFG_TUD_VENDOR
	//	TUD_USBTMC_DESC_LEN *		CFG_TUD_USBTMC +
	//	TUD_DFU_RUNTIME_DESC_LEN *	CFG_TUD_DFU_RUNTIME_DESC +
	//	TUD_DFU_DESC_LEN *		 	CFG_TUD_DFU +
	//	TUD_ECM_RNDIS_DESC_LEN *	CFG_TUD_ECM_RNDIS +
	//	TUD_NCM_DESC_LEN *			CFG_TUD_NCM +
	//	TUD_BTH_DESC_LEN *			CFG_TUD_BTH
	];
	const char* stringDescTbl_[256];
	uint16_t langid_;
	uint8_t iStringDescCur_;
	uint16_t stringDesc_[128];
	uint8_t attr_;
	uint16_t power_ma_;
public:
	static Controller* Instance;
public:
	Controller(const tusb_desc_device_t& deviceDesc, uint16_t langid,
		const char* strManufacturer, const char* strProduct, const char* strSerialNumber,
		uint8_t attr = 0, uint16_t power_ma = 100);
public:
	void RegisterConfigDesc(const void* configDesc, int bytes);
	uint8_t RegisterStringDesc(const char* str);
	void Initialize(uint8_t rhport = 0);
public:
	uint8_t AssignInterfaceNum(int nInterfacesToOccupy);
	uint8_t AddInterface_CDC(CDC* pCDC);
	uint8_t AddInterface_MSC(MSC* pMSC);
	uint8_t AddInterface_HID(HID* pHID);
	uint8_t AddInterface_Video(Video* pVideo);
	static CDC* GetInterface_CDC(uint8_t iInstance = 0) { return Instance->specific_.pCDCTbl[iInstance]; }
	static MSC* GetInterface_MSC(uint8_t iInstance = 0) { return Instance->specific_.pMSCTbl[iInstance]; }
	static HID* GetInterface_HID(uint8_t iInstance = 0) { return Instance->specific_.pHIDTbl[iInstance]; }
	static Video* GetInterface_Video(uint8_t iInstance = 0) { return Instance->specific_.pVideoTbl[iInstance]; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "USBDevice::Controller"; }
	virtual void OnTick() override { ::tud_task(); }
public:
	virtual void OnMount() {}
	virtual void OnUmount() {}
	virtual void OnSuspend() {}
	virtual void OnResume() {}
public:
	const uint8_t* On_GET_DEVICE_DESCRIPTOR() const { return reinterpret_cast<const uint8_t*>(&deviceDesc_); }
	const uint8_t* On_GET_CONFIGURATION_DESCRIPTOR(uint8_t idxConfig) { return configDescAccum_; }
	const uint16_t* On_GET_STRING_DESCRIPTOR(uint8_t idxString, uint16_t langid);
};

// A combination of interfaces must have a unique product id, since PC will save device driver after the first plug.
// Same VID/PID with different interface e.g MSC (first), then CDC (later) will possibly cause system error on PC.
constexpr uint16_t GenerateSpecificProductId(uint16_t base)
{
	return base
		+ (CFG_TUD_CDC?		(1 << 0) : 0)
		+ (CFG_TUD_MSC?		(1 << 1) : 0)
		+ (CFG_TUD_HID?		(1 << 2) : 0)
		+ (CFG_TUD_AUDIO?	(1 << 3) : 0)
		+ (CFG_TUD_VIDEO?	(1 << 4) : 0)
		+ (CFG_TUD_MIDI?	(1 << 5) : 0)
		+ (CFG_TUD_VENDOR?	(1 << 6) : 0)
		+ (CFG_TUD_BTH?		(1 << 7) : 0);
}

}

#endif
