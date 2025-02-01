//==============================================================================
// jxglib/USB.h
//==============================================================================
#ifndef PICO_JXGLIB_USB_H
#define PICO_JXGLIB_USB_H
#include "pico/stdlib.h"
#include "tusb.h"

namespace jxglib::USB {

class Interface;
class MSC;

//-----------------------------------------------------------------------------
// Utility function
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// USB::Device
//-----------------------------------------------------------------------------
class Device {
public:
	static const int nInterfaces = CFG_TUD_CDC + CFG_TUD_MSC + CFG_TUD_HID + CFG_TUD_AUDIO +
		CFG_TUD_VIDEO + CFG_TUD_MIDI + CFG_TUD_VENDOR + CFG_TUD_BTH;
public:
	tusb_desc_device_t deviceDesc_;
	uint8_t rhport_;
	uint8_t interfaceNumCur_;
	Interface* interfaceTbl_[nInterfaces];
	MSC* pMSC_;
	int offsetConfigDesc_;
	uint8_t configDescAccum_[
		TUD_CONFIG_DESC_LEN +
		TUD_CDC_DESC_LEN *			CFG_TUD_CDC +
		TUD_MSC_DESC_LEN *			CFG_TUD_MSC +
		TUD_HID_DESC_LEN *			CFG_TUD_HID +
	//	TUD_AUDIO_DESC_LEN *		CFG_TUD_AUDIO +
	//	TUD_VIDEO_DESC_LEN *		CFG_TUD_VIDEO +
		TUD_MIDI_DESC_LEN *			CFG_TUD_MIDI +
		TUD_VENDOR_DESC_LEN *		CFG_TUD_VENDOR
	//	TUD_USBTMC_DESC_LEN *		CFG_TUD_USBTMC +
	//	TUD_DFU_RUNTIME_DESC_LEN *	CFG_TUD_DFU_RUNTIME_DESC +
	//	TUD_DFU_DESC_LEN *		 	CFG_TUD_DFU +
	//	TUD_ECM_RNDIS_DESC_LEN *	CFG_TUD_ECM_RNDIS +
	//	TUD_NCM_DESC_LEN *			CFG_TUD_NCM +
	//	TUD_BTH_DESC_LEN *			CFG_TUD_BTH
	];
public:
	static Device* Instance;
public:
	Device(const tusb_desc_device_t& deviceDesc);
public:
	void RegisterConfigDesc(const void* configDesc, int bytes);
	void Initialize(uint8_t rhport = 0);
public:
	uint8_t AddInterface(Interface* pInterface);
	template<typename T> static T& GetInterface(int interfaceNum) {
		return *reinterpret_cast<T*>(Instance->interfaceTbl_[interfaceNum]);
	}
	void Task();
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

//-----------------------------------------------------------------------------
// USB::Interface
//-----------------------------------------------------------------------------
class Interface {
protected:
	Device& device_;
	uint8_t interfaceNum_;
	uint32_t msecStart_;
	uint32_t msecTaskInterval_;
public:
	Interface(Device& device, uint32_t msecTaskInterval);
public:
	void InitTimer() { msecStart_ = GetAbsoluteTimeMSec(); }
	bool IsTimerElapsed();
public:
	uint8_t GetInterfaceNum() const { return interfaceNum_; }
public:
	virtual bool IsMSC() const { return false; }
	virtual void OnTask() = 0;
public:
	void RegisterConfigDesc(const void* configDesc, int bytes);
public:
	uint32_t GetAbsoluteTimeMSec() { return ::to_ms_since_boot(::get_absolute_time()); }
};

#if CFG_TUD_HID > 0

//-----------------------------------------------------------------------------
// USB::HID
//-----------------------------------------------------------------------------
class HID : public Interface {
public:
	HID(Device& device, uint32_t msecTaskInterval);
public:
	// Check if the interface is ready to use
	bool ready() { return tud_hid_n_ready(GetInterfaceNum()); }
	// Get interface supported protocol (bInterfaceProtocol) check out hid_interface_protocol_enum_t for possible values
	uint8_t interface_protocol() { return tud_hid_n_interface_protocol(GetInterfaceNum()); }
	// Get current active protocol: HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
	uint8_t get_protocol() { return tud_hid_n_get_protocol(GetInterfaceNum()); }
	// Send report to host
	bool report(uint8_t report_id, void const* report, uint16_t len) { return tud_hid_n_report(GetInterfaceNum(), report_id, report, len); }
	// KEYBOARD: convenient helper to send keyboard report if application
	// use template layout report as defined by hid_keyboard_report_t
	bool keyboard_report(uint8_t report_id, uint8_t modifier, const uint8_t keycode[6]) { return tud_hid_n_keyboard_report(GetInterfaceNum(), report_id, modifier, keycode); }
	// MOUSE: convenient helper to send mouse report if application
	// use template layout report as defined by hid_mouse_report_t
	bool mouse_report(uint8_t report_id, uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal) { return tud_hid_n_mouse_report(GetInterfaceNum(), report_id, buttons, x, y, vertical, horizontal); }
	// ABSOLUTE MOUSE: convenient helper to send absolute mouse report if application
	// use template layout report as defined by hid_abs_mouse_report_t
	bool abs_mouse_report(uint8_t report_id, uint8_t buttons, int16_t x, int16_t y, int8_t vertical, int8_t horizontal) { return tud_hid_n_abs_mouse_report(GetInterfaceNum(), report_id, buttons, x, y, vertical, horizontal); }
	// Gamepad: convenient helper to send gamepad report if application
	// use template layout report TUD_HID_REPORT_DESC_GAMEPAD
	bool gamepad_report(uint8_t report_id, int8_t x, int8_t y, int8_t z, int8_t rz, int8_t rx, int8_t ry, uint8_t hat, uint32_t buttons) { return tud_hid_n_gamepad_report(GetInterfaceNum(), report_id, x, y, z, rz, rx, ry, hat, buttons); }
public:
	virtual const uint8_t* On_DESCRIPTOR_REPORT() = 0;
	virtual uint16_t On_GET_REPORT(uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength) { return 0; }
	virtual void On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength) {}
	virtual void On_SET_REPORT(uint8_t reportID, hid_report_type_t report_type, const uint8_t* report, uint16_t reportLength) {}
	virtual void On_SET_PROTOCOL(uint8_t protocol) {}
};

//-----------------------------------------------------------------------------
// USB::Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public HID {
private:
	static const uint8_t descriptor[];
public:
	Keyboard(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
public:
	virtual const uint8_t* On_DESCRIPTOR_REPORT() override { return descriptor; }
};

//-----------------------------------------------------------------------------
// USB::Mouse
//-----------------------------------------------------------------------------
class Mouse : public HID {
private:
	static const uint8_t descriptor[];
public:
	Mouse(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
public:
	virtual const uint8_t* On_DESCRIPTOR_REPORT() override { return descriptor; }
};

//-----------------------------------------------------------------------------
// USB::Gamepad
//-----------------------------------------------------------------------------
class Gamepad : public HID {
private:
	static const uint8_t descriptor[];
public:
	Gamepad(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
public:
	virtual const uint8_t* On_DESCRIPTOR_REPORT() override { return descriptor; }
};

//-----------------------------------------------------------------------------
// USB::Consumer
//-----------------------------------------------------------------------------
class Consumer : public HID {
private:
	static const uint8_t descriptor[];
public:
	Consumer(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
public:
	virtual const uint8_t* On_DESCRIPTOR_REPORT() override { return descriptor; }
};

#endif

#if CFG_TUD_CDC > 0

//-----------------------------------------------------------------------------
// USB::CDC
//-----------------------------------------------------------------------------
class CDC : public Interface {
private:
	static const uint8_t descriptor[];
public:
	CDC(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
public:
	virtual const uint8_t* On_DESCRIPTOR_REPORT() override { return descriptor; }
};

#endif

//-----------------------------------------------------------------------------
// USB::MSC
//-----------------------------------------------------------------------------
#if CFG_TUD_MSC > 0
class MSC : public Interface {
public:
	MSC(Device& device, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize = 64);
public:
	virtual bool IsMSC() const override { return true; }
	virtual void OnTask() override {}
public:
	virtual void inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) = 0;
	virtual bool test_unit_ready(uint8_t lun) = 0;
	virtual void capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) = 0;
	virtual bool start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) = 0;
	virtual int32_t read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) = 0;
	virtual bool is_writable(uint8_t lun) = 0;
	virtual int32_t write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) = 0;
	virtual int32_t scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) = 0;

};
#endif

//-----------------------------------------------------------------------------
// USB::MIDI
//-----------------------------------------------------------------------------
#if CFG_TUD_MIDI > 0
class MIDI : public Interface {
};
#endif

//-----------------------------------------------------------------------------
// USB::AUDIO
//-----------------------------------------------------------------------------
#if CFG_TUD_AUDIO > 0
#endif

//-----------------------------------------------------------------------------
// USB::VIDEO
//-----------------------------------------------------------------------------
#if CFG_TUD_VIDEO > 0
#endif

//-----------------------------------------------------------------------------
// USB::BTH
//-----------------------------------------------------------------------------
#if CFG_TUD_BTH > 0
#endif

//-----------------------------------------------------------------------------
// USB::VENDOR
//-----------------------------------------------------------------------------
#if CFG_TUD_VENDOR > 0
class Vendor : public Interface {
};
#endif

}

#endif
