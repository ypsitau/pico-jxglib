//==============================================================================
// jxglib/USBD.h
//==============================================================================
#ifndef PICO_JXGLIB_USBD_H
#define PICO_JXGLIB_USBD_H
#include "pico/stdlib.h"
#include "jxglib/Tickable.h"
#include "tusb.h"

namespace jxglib::USBD {

class Interface;
class CDC;
class MSC;
class HID;

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
// USBD::Device
//-----------------------------------------------------------------------------
class Device : public Tickable {
public:
	static const int nInstancesMax = CFG_TUD_CDC + CFG_TUD_MSC + CFG_TUD_HID + CFG_TUD_AUDIO +
		CFG_TUD_VIDEO + CFG_TUD_MIDI + CFG_TUD_VENDOR + CFG_TUD_BTH;
public:
	tusb_desc_device_t deviceDesc_;
	uint8_t rhport_;
	uint8_t interfaceNumCur_;
	uint8_t nInstances_CDC_;
	uint8_t nInstances_MSC_;
	uint8_t nInstances_HID_;
	union {
		Interface* interfaceTbl_[nInstancesMax];
		struct {
			CDC* pCDCTbl[CFG_TUD_CDC];
			MSC* pMSCTbl[CFG_TUD_MSC];
			HID* pHIDTbl[CFG_TUD_HID];
		} specific_;
	};
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
	const char* stringDescTbl_[256];
	uint16_t langid_;
	uint8_t iStringDescCur_;
	uint16_t stringDesc_[128];
	uint8_t attr_;
	uint16_t power_ma_;
public:
	static Device* Instance;
public:
	Device(const tusb_desc_device_t& deviceDesc, uint16_t langid,
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
	static CDC* GetInterface_CDC(uint8_t iInstance = 0) { return Instance->specific_.pCDCTbl[iInstance]; }
	static MSC* GetInterface_MSC(uint8_t iInstance = 0) { return Instance->specific_.pMSCTbl[iInstance]; }
	static HID* GetInterface_HID(uint8_t iInstance = 0) { return Instance->specific_.pHIDTbl[iInstance]; }
public:
	virtual void OnTick() { ::tud_task(); }
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
// USBD::Interface
//-----------------------------------------------------------------------------
class Interface : public Tickable {
protected:
	Device& device_;
	uint8_t interfaceNum_;
	uint8_t iInstance_;
public:
	Interface(Device& device, int nInterfacesToOccupy, uint32_t msecTaskInterval);
public:
	void RegisterConfigDesc(const void* configDesc, int bytes);
public:
	void Initialize() {}
};

#if CFG_TUD_HID > 0

//-----------------------------------------------------------------------------
// USBD::HID
//-----------------------------------------------------------------------------
class HID : public Interface {
protected:
	const uint8_t* reportDesc_;
public:
	HID(Device& device, const char* str, uint8_t protocol, const uint8_t* reportDesc, uint8_t bytesReportDesc, uint8_t endpInterrupt, uint8_t pollingInterval);
	HID(Device& device, uint32_t msecTaskInterval);
public:
	void RegisterReportDesc(const uint8_t* reportDesc) { reportDesc_ = reportDesc; }
public:
	// Check if the interface is ready to use
	bool hid_ready() { return tud_hid_n_ready(iInstance_); }
	// Get interface supported protocol (bInterfaceProtocol) check out hid_interface_protocol_enum_t for possible values
	uint8_t hid_interface_protocol() { return tud_hid_n_interface_protocol(iInstance_); }
	// Get current active protocol: HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
	uint8_t hid_get_protocol() { return tud_hid_n_get_protocol(iInstance_); }
	// Send report to host
	bool hid_report(uint8_t reportId, void const* report, uint16_t len) { return tud_hid_n_report(iInstance_, reportId, report, len); }
	// KEYBOARD: convenient helper to send keyboard report if application
	// use template layout report as defined by hid_keyboard_report_t
	bool hid_keyboard_report(uint8_t reportId, uint8_t modifier, const uint8_t keycode[6]) { return tud_hid_n_keyboard_report(iInstance_, reportId, modifier, keycode); }
	// MOUSE: convenient helper to send mouse report if application
	// use template layout report as defined by hid_mouse_report_t
	bool hid_mouse_report(uint8_t reportId, uint8_t buttons, int8_t x, int8_t y, int8_t vertical, int8_t horizontal) { return tud_hid_n_mouse_report(iInstance_, reportId, buttons, x, y, vertical, horizontal); }
	// ABSOLUTE MOUSE: convenient helper to send absolute mouse report if application
	// use template layout report as defined by hid_abs_mouse_report_t
	bool hid_abs_mouse_report(uint8_t reportId, uint8_t buttons, int16_t x, int16_t y, int8_t vertical, int8_t horizontal) { return tud_hid_n_abs_mouse_report(iInstance_, reportId, buttons, x, y, vertical, horizontal); }
	// Gamepad: convenient helper to send gamepad report if application
	// use template layout report TUD_HID_REPORT_DESC_GAMEPAD
	bool hid_gamepad_report(uint8_t reportId, int8_t x, int8_t y, int8_t z, int8_t rz, int8_t rx, int8_t ry, uint8_t hat, uint32_t buttons) { return tud_hid_n_gamepad_report(iInstance_, reportId, x, y, z, rz, rx, ry, hat, buttons); }
public:
	const uint8_t* On_DESCRIPTOR_REPORT() { return reportDesc_; }
	virtual uint16_t On_GET_REPORT(uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength) { return 0; }
	virtual void On_GET_REPORT_Complete(const uint8_t* report, uint16_t reportLength) {}
	virtual void On_SET_REPORT(uint8_t reportID, hid_report_type_t reportType, const uint8_t* report, uint16_t reportLength) {}
	virtual void On_SET_PROTOCOL(uint8_t protocol) {}
};

//-----------------------------------------------------------------------------
// USBD::Keyboard
//-----------------------------------------------------------------------------
class Keyboard : public HID {
private:
	static const uint8_t reportDesc_[];
public:
	Keyboard(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
};

//-----------------------------------------------------------------------------
// USBD::Mouse
//-----------------------------------------------------------------------------
class Mouse : public HID {
private:
	static const uint8_t reportDesc_[];
public:
	Mouse(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
};

//-----------------------------------------------------------------------------
// USBD::Gamepad
//-----------------------------------------------------------------------------
class Gamepad : public HID {
private:
	static const uint8_t reportDesc_[];
public:
	Gamepad(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval = 10);
};

#endif

#if CFG_TUD_CDC > 0

//-----------------------------------------------------------------------------
// USBD::CDC
//-----------------------------------------------------------------------------
class CDC : public Interface {
public:
	CDC(Device& device, const char* str, uint8_t endpNotif, uint8_t bytesNotif, uint8_t endpBulkOut, uint8_t endpBulkIn, uint8_t bytesBulk, uint8_t pollingInterval);
public:
	virtual void On_cdc_rx() {}
	virtual void On_cdc_rx_wanted(char wanted_char) {}
	virtual void On_cdc_tx_complete() {}
	virtual void On_cdc_line_state(bool dtr, bool rts) {}
	virtual void On_cdc_line_coding(const cdc_line_coding_t* p_line_coding) {}
	virtual void On_cdc_send_break(uint16_t duration_ms) {}
public:
	bool cdc_ready() { return ::tud_cdc_n_ready(iInstance_); }
	bool cdc_connected() { return ::tud_cdc_n_connected(iInstance_); }
	uint8_t cdc_get_line_state() { return ::tud_cdc_n_get_line_state(iInstance_); }
	void cdc_get_line_coding(cdc_line_coding_t* coding) { return ::tud_cdc_n_get_line_coding(iInstance_, coding); }
	void cdc_set_wanted_char(char wanted) { return ::tud_cdc_n_set_wanted_char(iInstance_, wanted); }
	uint32_t cdc_available() { return ::tud_cdc_n_available(iInstance_); }
	uint32_t cdc_read(void* buffer, uint32_t bufsize) { return ::tud_cdc_n_read(iInstance_, buffer, bufsize); }
	int32_t cdc_read_char() { return ::tud_cdc_n_read_char(iInstance_); }
	void cdc_read_flush() { return ::tud_cdc_n_read_flush(iInstance_); }
	bool cdc_peek(uint8_t* ui8) { return ::tud_cdc_n_peek(iInstance_, ui8); }
	uint32_t cdc_write(void const* buffer, uint32_t bufsize) { return ::tud_cdc_n_write(iInstance_, buffer, bufsize); }
	uint32_t cdc_write_char(char ch) { return ::tud_cdc_n_write_char(iInstance_, ch); }
	uint32_t cdc_write_str(char const* str) { return ::tud_cdc_n_write_str(iInstance_, str); }
	uint32_t cdc_write_flush() { return ::tud_cdc_n_write_flush(iInstance_); }
	uint32_t cdc_write_available() { return ::tud_cdc_n_write_available(iInstance_); }
	bool write_clear() { return ::tud_cdc_n_write_clear(iInstance_); }
};

#endif

//-----------------------------------------------------------------------------
// USBD::MSC
//-----------------------------------------------------------------------------
#if CFG_TUD_MSC > 0
class MSC : public Interface {
public:
	MSC(Device& device, const char* str, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize = 64);
public:
	virtual void OnTick() override {}
public:
	virtual void On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) = 0;
	virtual bool On_msc_test_unit_ready(uint8_t lun) = 0;
	virtual void On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) = 0;
	virtual bool On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) = 0;
	virtual int32_t On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) = 0;
	virtual bool On_msc_is_writable(uint8_t lun) = 0;
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) = 0;
	virtual int32_t On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) = 0;

};
#endif

//-----------------------------------------------------------------------------
// USBD::MIDI
//-----------------------------------------------------------------------------
#if CFG_TUD_MIDI > 0
class MIDI : public Interface {
};
#endif

//-----------------------------------------------------------------------------
// USBD::AUDIO
//-----------------------------------------------------------------------------
#if CFG_TUD_AUDIO > 0
#endif

//-----------------------------------------------------------------------------
// USBD::VIDEO
//-----------------------------------------------------------------------------
#if CFG_TUD_VIDEO > 0
#endif

//-----------------------------------------------------------------------------
// USBD::BTH
//-----------------------------------------------------------------------------
#if CFG_TUD_BTH > 0
#endif

//-----------------------------------------------------------------------------
// USBD::VENDOR
//-----------------------------------------------------------------------------
#if CFG_TUD_VENDOR > 0
class Vendor : public Interface {
};
#endif

}

#endif
