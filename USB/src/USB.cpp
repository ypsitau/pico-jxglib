//==============================================================================
// USB.cpp
//==============================================================================
#include "jxglib/USB.h"

//-----------------------------------------------------------------------------
// USB::Device
//-----------------------------------------------------------------------------
namespace jxglib::USB {

Device* Device::Instance = nullptr;

Device::Device(const tusb_desc_device_t& deviceDesc) :
		deviceDesc_{deviceDesc}, interfaceNumCur_{0}, offsetConfigDesc_{TUD_CONFIG_DESC_LEN}
{
	Instance = this;
	deviceDesc_.bLength = sizeof(tusb_desc_device_t);
	deviceDesc_.bDescriptorType = TUSB_DESC_DEVICE;
	deviceDesc_.bNumConfigurations = 1;
}

void Device::RegisterConfigDesc(const void* configDesc, int bytes)
{
	::memcpy(configDescAccum_ + offsetConfigDesc_, configDesc, bytes);
	offsetConfigDesc_ += bytes;
}

void Device::Initialize(uint8_t rhport)
{
	uint8_t configDesc[] = {
		TUD_CONFIG_DESCRIPTOR(1, interfaceNumCur_, 0, offsetConfigDesc_, TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP, 100)
	};
	::memcpy(configDescAccum_, configDesc, sizeof(configDesc));
	for (int interfaceNum = 0; interfaceNum < interfaceNumCur_; interfaceNum++) {
		interfaceTbl_[interfaceNum]->InitTimer();
	}
	::tud_init(rhport);
}

void Device::Task()
{
	::tud_task();
	for (int interfaceNum = 0; interfaceNum < interfaceNumCur_; interfaceNum++) {
		Interface* pInterface = interfaceTbl_[interfaceNum];
		if (pInterface->IsTimerElapsed()) pInterface->OnTask();
	}
}

enum {
	STRID_LANGID = 0,
	STRID_MANUFACTURER,
	STRID_PRODUCT,
	STRID_SERIAL,
};

// array of pointer to string descriptors
char const *string_desc_arr[] =
{
	(const char[]) { 0x09, 0x04 }, // 0: is supported language is English (0x0409)
	"TinyUSB",                     // 1: Manufacturer
	"TinyUSB Device",              // 2: Product
	NULL,                          // 3: Serials will use unique ID if possible
};

static uint16_t _desc_str[32 + 1];

static inline size_t board_usb_get_serial(uint16_t desc_str1[], size_t max_chars) {
	uint8_t uid[16] TU_ATTR_ALIGNED(4);
	size_t uid_len;

	// TODO work with make, but not working with esp32s3 cmake
	//if ( board_get_unique_id ) {
	//  uid_len = board_get_unique_id(uid, sizeof(uid));
	{
		// fixed serial string is 01234567889ABCDEF
		uint32_t* uid32 = (uint32_t*) (uintptr_t) uid;
		uid32[0] = 0x67452301;
		uid32[1] = 0xEFCDAB89;
		uid_len = 8;
	}

	if ( uid_len > max_chars / 2 ) uid_len = max_chars / 2;

	for ( size_t i = 0; i < uid_len; i++ ) {
		for ( size_t j = 0; j < 2; j++ ) {
			const char nibble_to_hex[16] = {
					'0', '1', '2', '3', '4', '5', '6', '7',
					'8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
			};
			uint8_t const nibble = (uid[i] >> (j * 4)) & 0xf;
			desc_str1[i * 2 + (1 - j)] = nibble_to_hex[nibble]; // UTF-16-LE
		}
	}

	return 2 * uid_len;
}

const uint16_t* Device::On_GET_STRING_DESCRIPTOR(uint8_t idxString, uint16_t langid)
{
	::printf("On_GET_STRING_DESCRIPTOR(%d, %04x)\n", idxString, langid);
	size_t chr_count;

	switch ( idxString ) {
		case STRID_LANGID:
			memcpy(&_desc_str[1], string_desc_arr[0], 2);
			chr_count = 1;
			break;

		case STRID_SERIAL:
			chr_count = board_usb_get_serial(_desc_str + 1, 32);
			break;

		default:
			// Note: the 0xEE index string is a Microsoft OS 1.0 Descriptors.
			// https://docs.microsoft.com/en-us/windows-hardware/drivers/usbcon/microsoft-defined-usb-descriptors

			if ( !(idxString < sizeof(string_desc_arr) / sizeof(string_desc_arr[0])) ) return NULL;

			const char *str = string_desc_arr[idxString];

			// Cap at max char
			chr_count = strlen(str);
			size_t const max_count = sizeof(_desc_str) / sizeof(_desc_str[0]) - 1; // -1 for string type
			if ( chr_count > max_count ) chr_count = max_count;

			// Convert ASCII string into UTF-16
			for ( size_t i = 0; i < chr_count; i++ ) {
				_desc_str[1 + i] = str[i];
			}
			break;
	}

	// first byte is length (including header), second byte is string type
	_desc_str[0] = (uint16_t) ((TUSB_DESC_STRING << 8) | (2 * chr_count + 2));

	return _desc_str;
}

}

void tud_mount_cb()
{
	jxglib::USB::Device::Instance->OnMount();
}

void tud_umount_cb()
{
	jxglib::USB::Device::Instance->OnUmount();
}

// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
	jxglib::USB::Device::Instance->OnSuspend();
}

void tud_resume_cb()
{
	jxglib::USB::Device::Instance->OnResume();
}

// Invoked when received GET DEVICE DESCRIPTOR
const uint8_t* tud_descriptor_device_cb()
{
	return jxglib::USB::Device::Instance->On_GET_DEVICE_DESCRIPTOR();
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
const uint8_t* tud_descriptor_configuration_cb(uint8_t idxConfig)
{
	return jxglib::USB::Device::Instance->On_GET_CONFIGURATION_DESCRIPTOR(idxConfig);
}

// Invoked when received GET STRING DESCRIPTOR request
const uint16_t* tud_descriptor_string_cb(uint8_t idxString, uint16_t langid)
{
	return jxglib::USB::Device::Instance->On_GET_STRING_DESCRIPTOR(idxString, langid);
}

//-----------------------------------------------------------------------------
// USB::Interface
//-----------------------------------------------------------------------------
namespace jxglib::USB {

Interface::Interface(Device& device, uint32_t msecTaskInterval) :
			device_{device}, msecTaskInterval_{msecTaskInterval}
{
	interfaceNum_ = device.AddInterface(this);
}
	
void Interface::RegisterConfigDesc(const void* configDesc, int bytes)
{
	device_.RegisterConfigDesc(configDesc, bytes);
}

bool Interface::IsTimerElapsed()
{
	if (GetAbsoluteTimeMSec() - msecStart_ < msecTaskInterval_) return false;
	msecStart_ += msecTaskInterval_;
	return true;
}

}

#if CFG_TUD_HID > 0

//-----------------------------------------------------------------------------
// USB::HID
//-----------------------------------------------------------------------------
namespace jxglib::USB {

HID::HID(Device& device, uint32_t msecTaskInterval) : Interface(device, msecTaskInterval)
{
}

}

// Invoked when received DESCRIPTOR_REPORT control request
uint8_t const * tud_hid_descriptor_report_cb(uint8_t interfaceNum)
{
	using namespace jxglib::USB;
	return Device::GetInterface<HID>(interfaceNum).On_DESCRIPTOR_REPORT();
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t interfaceNum, uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib::USB;
	return Device::GetInterface<HID>(interfaceNum).On_GET_REPORT(reportID, reportType, report, reportLength);
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t interfaceNum, uint8_t const* report, uint16_t reportLength)
{
	using namespace jxglib::USB;
	Device::GetInterface<HID>(interfaceNum).On_GET_REPORT_Complete(report, reportLength);
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint (Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t interfaceNum, uint8_t reportID, hid_report_type_t reportType, const uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib::USB;
	Device::GetInterface<HID>(interfaceNum).On_SET_REPORT(reportID, reportType, report, reportLength);
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t interfaceNum, uint8_t protocol)
{
	using namespace jxglib::USB;
	Device::GetInterface<HID>(interfaceNum).On_SET_PROTOCOL(protocol);
}

//-----------------------------------------------------------------------------
// USB::Keyboard
//-----------------------------------------------------------------------------
namespace jxglib::USB {

const uint8_t Keyboard::descriptor[] = { TUD_HID_REPORT_DESC_KEYBOARD() };

Keyboard::Keyboard(Device& device, uint8_t endpNum, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(descriptor), endpNum, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USB::Mouse
//-----------------------------------------------------------------------------
namespace jxglib::USB {

const uint8_t Mouse::descriptor[] = { TUD_HID_REPORT_DESC_MOUSE() };

Mouse::Mouse(Device& device, uint8_t endpNum, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_MOUSE, sizeof(descriptor), endpNum, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USB::Consumer
//-----------------------------------------------------------------------------
namespace jxglib::USB {

const uint8_t Consumer::descriptor[] = { TUD_HID_REPORT_DESC_GAMEPAD() };

Consumer::Consumer(Device& device, uint8_t endpNum, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_NONE, sizeof(descriptor), endpNum, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USB::Gamepad
//-----------------------------------------------------------------------------
namespace jxglib::USB {

const uint8_t Gamepad::descriptor[] = { TUD_HID_REPORT_DESC_GAMEPAD() };

Gamepad::Gamepad(Device& device, uint8_t endpNum, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_NONE, sizeof(descriptor), endpNum, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

#endif

//-----------------------------------------------------------------------------
// USB::CDC
//-----------------------------------------------------------------------------
#if CFG_TUD_CDC > 0

const uint8_t CDC::descriptor[] = { TUD_HID_REPORT_DESC_KEYBOARD() };

CDC::CDC(Device& device, uint8_t endpNum, uint8_t pollingInterval) : Interface(device, pollingInterval)
{
	uint8_t EPNUM_CDC_NOTIF = 0x81;
	uint8_t EPNUM_CDC_OUT   = 0x02;
	uint8_t EPNUM_CDC_IN    = 0x82;
	uint8_t configDesc[] = {
	    // Interface number, string index, EP notification address and size, EP data address (out, in) and size.
		TUD_CDC_DESCRIPTOR(ITF_NUM_CDC, 4, EPNUM_CDC_NOTIF, 8, EPNUM_CDC_OUT, EPNUM_CDC_IN, 64),
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

#endif

//-----------------------------------------------------------------------------
// USB::MSC
//-----------------------------------------------------------------------------
#if CFG_TUD_MSC > 0
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	return 0;
}

int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	return 0;
}

void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
}

bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	return false;
}

void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
}

int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	return 0;
}

// Optional
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	return false;
}

#endif

//-----------------------------------------------------------------------------
// USB::MIDI
//-----------------------------------------------------------------------------
#if CFG_TUD_MIDI > 0
#endif

//-----------------------------------------------------------------------------
// USB::Vendor
//-----------------------------------------------------------------------------
#if CFG_TUD_VENDOR > 0
#endif
