//==============================================================================
// USBD.cpp
//==============================================================================
#include "jxglib/USBD.h"

//-----------------------------------------------------------------------------
// USBD::Device
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

Device* Device::Instance = nullptr;

Device::Device(const tusb_desc_device_t& deviceDesc) :
	deviceDesc_{deviceDesc}, interfaceNumCur_{0}, pMSC_{nullptr}, offsetConfigDesc_{TUD_CONFIG_DESC_LEN}
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

uint8_t Device::AddInterface(Interface* pInterface)
{
	interfaceTbl_[interfaceNumCur_++] = pInterface;
	return interfaceNumCur_ - 1;
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
	"TinyUSB CDC",
	"TinyUSB MSC",
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
	jxglib::USBD::Device::Instance->OnMount();
}

void tud_umount_cb()
{
	jxglib::USBD::Device::Instance->OnUmount();
}

// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
	jxglib::USBD::Device::Instance->OnSuspend();
}

void tud_resume_cb()
{
	jxglib::USBD::Device::Instance->OnResume();
}

// Invoked when received GET DEVICE DESCRIPTOR
const uint8_t* tud_descriptor_device_cb()
{
	return jxglib::USBD::Device::Instance->On_GET_DEVICE_DESCRIPTOR();
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
const uint8_t* tud_descriptor_configuration_cb(uint8_t idxConfig)
{
	return jxglib::USBD::Device::Instance->On_GET_CONFIGURATION_DESCRIPTOR(idxConfig);
}

// Invoked when received GET STRING DESCRIPTOR request
const uint16_t* tud_descriptor_string_cb(uint8_t idxString, uint16_t langid)
{
	return jxglib::USBD::Device::Instance->On_GET_STRING_DESCRIPTOR(idxString, langid);
}

//-----------------------------------------------------------------------------
// USBD::Interface
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

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
// USBD::HID
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

HID::HID(Device& device, uint32_t msecTaskInterval) : Interface(device, msecTaskInterval)
{
}

}

// Invoked when received DESCRIPTOR_REPORT control request
uint8_t const * tud_hid_descriptor_report_cb(uint8_t interfaceNum)
{
	using namespace jxglib::USBD;
	return Device::GetInterface<HID>(interfaceNum).On_DESCRIPTOR_REPORT();
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t interfaceNum, uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib::USBD;
	return Device::GetInterface<HID>(interfaceNum).On_GET_REPORT(reportID, reportType, report, reportLength);
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t interfaceNum, uint8_t const* report, uint16_t reportLength)
{
	using namespace jxglib::USBD;
	Device::GetInterface<HID>(interfaceNum).On_GET_REPORT_Complete(report, reportLength);
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint (Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t interfaceNum, uint8_t reportID, hid_report_type_t reportType, const uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib::USBD;
	Device::GetInterface<HID>(interfaceNum).On_SET_REPORT(reportID, reportType, report, reportLength);
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t interfaceNum, uint8_t protocol)
{
	using namespace jxglib::USBD;
	Device::GetInterface<HID>(interfaceNum).On_SET_PROTOCOL(protocol);
}

//-----------------------------------------------------------------------------
// USBD::Keyboard
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

const uint8_t Keyboard::descriptor[] = { TUD_HID_REPORT_DESC_KEYBOARD() };

Keyboard::Keyboard(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_KEYBOARD, sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Mouse
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

const uint8_t Mouse::descriptor[] = { TUD_HID_REPORT_DESC_MOUSE() };

Mouse::Mouse(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_MOUSE, sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Consumer
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

const uint8_t Consumer::descriptor[] = { TUD_HID_REPORT_DESC_GAMEPAD() };

Consumer::Consumer(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_NONE, sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Gamepad
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

const uint8_t Gamepad::descriptor[] = { TUD_HID_REPORT_DESC_GAMEPAD() };

Gamepad::Gamepad(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), 0, HID_ITF_PROTOCOL_NONE, sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

#endif

//-----------------------------------------------------------------------------
// USBD::CDC
//-----------------------------------------------------------------------------
#if CFG_TUD_CDC > 0

CDC::CDC(Device& device, uint8_t endpInterrupt, uint8_t pollingInterval) : Interface(device, pollingInterval)
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
// USBD::MSC
//-----------------------------------------------------------------------------
#if CFG_TUD_MSC > 0

namespace jxglib::USBD {

MSC::MSC(Device& device, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize) : Interface(device, 0)
{
	uint8_t configDesc[] = {
		// Interface number, string index, EP Out & EP In address, EP size
		TUD_MSC_DESCRIPTOR(GetInterfaceNum(), 5, endpBulkOut, endpBulkIn, endpSize),
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
	device.SetInterfaceMSC(this);
}

}

#if 0
// whether host does safe-eject
static bool ejected = false;

#define README_CONTENTS \
"hoge This is tinyusb's MassStorage Class demo.\r\n\r\n\
If you find any bugs or get any questions, feel free to file an\r\n\
issue at github.com/hathach/tinyusb"

enum {
	DISK_BLOCK_NUM  = 16, // 8KB is the smallest size that windows allow to mount
	DISK_BLOCK_SIZE = 512
};

uint8_t msc_disk[DISK_BLOCK_NUM][DISK_BLOCK_SIZE] =
{
	//------------- Block0: Boot Sector -------------//
	// byte_per_sector    = DISK_BLOCK_SIZE; fat12_sector_num_16  = DISK_BLOCK_NUM;
	// sector_per_cluster = 1; reserved_sectors = 1;
	// fat_num            = 1; fat12_root_entry_num = 16;
	// sector_per_fat     = 1; sector_per_track = 1; head_num = 1; hidden_sectors = 0;
	// drive_number       = 0x80; media_type = 0xf8; extended_boot_signature = 0x29;
	// filesystem_type    = "FAT12   "; volume_serial_number = 0x1234; volume_label = "TinyUSB MSC";
	// FAT magic code at offset 510-511
	{
		0xEB, 0x3C, 0x90, 0x4D, 0x53, 0x44, 0x4F, 0x53, 0x35, 0x2E, 0x30, 0x00, 0x02, 0x01, 0x01, 0x00,
		0x01, 0x10, 0x00, 0x10, 0x00, 0xF8, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x29, 0x34, 0x12, 0x00, 0x00, 'T' , 'i' , 'n' , 'y' , 'U' ,
		'S' , 'B' , ' ' , 'M' , 'S' , 'C' , 0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, 0x00, 0x00,
		// Zero up to 2 last bytes of FAT magic code
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xAA
	},
	//------------- Block1: FAT12 Table -------------//
	{
		0xF8, 0xFF, 0xFF, 0xFF, 0x0F // // first 2 entries must be F8FF, third entry is cluster end of readme file
	},
	//------------- Block2: Root Directory -------------//
	{
		// first entry is volume label
		'T' , 'i' , 'n' , 'y' , 'U' , 'S' , 'B' , ' ' , 'M' , 'S' , 'C' , 0x08, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4F, 0x6D, 0x65, 0x43, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		// second entry is readme file
		'R' , 'E' , 'A' , 'D' , 'M' , 'E' , ' ' , ' ' , 'T' , 'X' , 'T' , 0x20, 0x00, 0xC6, 0x52, 0x6D,
		0x65, 0x43, 0x65, 0x43, 0x00, 0x00, 0x88, 0x6D, 0x65, 0x43, 0x02, 0x00,
		sizeof(README_CONTENTS)-1, 0x00, 0x00, 0x00 // readme's files size (4 Bytes)
	},
	//------------- Block3: Readme Content -------------//
	README_CONTENTS
};

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	const char vid[] = "TinyUSB";
	const char pid[] = "Mass Storage";
	const char rev[] = "1.0";
	memcpy(vendor_id  , vid, strlen(vid));
	memcpy(product_id , pid, strlen(pid));
	memcpy(product_rev, rev, strlen(rev));
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	// RAM disk is ready until ejected
	if (ejected) {
	  // Additional Sense 3A-00 is NOT_FOUND
		tud_msc_set_sense(lun, SCSI_SENSE_NOT_READY, 0x3a, 0x00);
		return false;
	}
	return true;
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	*block_count = DISK_BLOCK_NUM;
	*block_size  = DISK_BLOCK_SIZE;
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	if (load_eject) {
		if (start) {
			// load disk storage
		} else {
			// unload disk storage
			ejected = true;
		}
	}
	return true;
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	if (lba >= DISK_BLOCK_NUM) return -1;
	uint8_t const* addr = msc_disk[lba] + offset;
	memcpy(buffer, addr, bufsize);
	return bufsize;
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
	return true;
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	if ( lba >= DISK_BLOCK_NUM ) return -1;
	uint8_t* addr = msc_disk[lba] + offset;
	memcpy(addr, buffer, bufsize);
	return bufsize;
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	// read10 & write10 has their own callback and MUST not be handled here
	void const* response = NULL;
	int32_t resplen = 0;
	// most scsi handled is input
	bool in_xfer = true;
	switch (scsi_cmd[0]) {
	default:
		// Set Sense = Invalid Command Operation
		tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
		// negative means error -> tinyusb could stall and/or response with failed status
		resplen = -1;
		break;
	}
	// return resplen must not larger than bufsize
	if (resplen > bufsize) resplen = bufsize;
	if (response && (resplen > 0)) {
		if(in_xfer) {
			memcpy(buffer, response, (size_t) resplen);
		} else {
			// SCSI output
		}
	}
	return resplen;
}

#else

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	using namespace jxglib::USBD;
	Device::GetInterfaceMSC().On_inquiry(lun, vendor_id, product_id, product_rev);
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	using namespace jxglib::USBD;
	return Device::GetInterfaceMSC().On_test_unit_ready(lun);
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	using namespace jxglib::USBD;
	return Device::GetInterfaceMSC().On_capacity(lun, block_count, block_size);
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	using namespace jxglib::USBD;
	return Device::GetInterfaceMSC().On_start_stop(lun, power_condition, start, load_eject);
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	using namespace jxglib::USBD;
	return Device::GetInterfaceMSC().On_read10(lun, lba, offset, buffer, bufsize);
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
	using namespace jxglib::USBD;
	return Device::GetInterfaceMSC().On_is_writable(lun);
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	using namespace jxglib::USBD;
	return Device::GetInterfaceMSC().On_write10(lun, lba, offset, buffer, bufsize);
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	using namespace jxglib::USBD;
	return Device::GetInterfaceMSC().On_scsi(lun, scsi_cmd, buffer, bufsize);
}

#endif

#endif

//-----------------------------------------------------------------------------
// USBD::MIDI
//-----------------------------------------------------------------------------
#if CFG_TUD_MIDI > 0
#endif

//-----------------------------------------------------------------------------
// USBD::Vendor
//-----------------------------------------------------------------------------
#if CFG_TUD_VENDOR > 0
#endif
