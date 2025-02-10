//==============================================================================
// USBD.cpp
//==============================================================================
#include "jxglib/UTF8Decoder.h"
#include "jxglib/USBD.h"

//-----------------------------------------------------------------------------
// USBD::Device
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

Device* Device::Instance = nullptr;

Device::Device(const tusb_desc_device_t& deviceDesc, uint16_t langid,
		const char* strManufacturer, const char* strProduct, const char* strSerialNumber) :
	deviceDesc_{deviceDesc}, langid_{langid}, interfaceNumCur_{0}, pMSC_{nullptr},
	offsetConfigDesc_{TUD_CONFIG_DESC_LEN}, iStringDescCur_{4}
{
	Instance = this;
	deviceDesc_.bLength = sizeof(tusb_desc_device_t);
	deviceDesc_.bDescriptorType		= TUSB_DESC_DEVICE;
	deviceDesc_.iManufacturer		= 1;
	deviceDesc_.iProduct			= 2;
	deviceDesc_.iSerialNumber		= 3;
	deviceDesc_.bNumConfigurations	= 1;
	stringDescTbl_[deviceDesc_.iManufacturer] = strManufacturer;
	stringDescTbl_[deviceDesc_.iProduct] = strProduct;
	stringDescTbl_[deviceDesc_.iSerialNumber] = strSerialNumber;
}

void Device::RegisterConfigDesc(const void* configDesc, int bytes)
{
	::memcpy(configDescAccum_ + offsetConfigDesc_, configDesc, bytes);
	offsetConfigDesc_ += bytes;
}

uint8_t Device::RegisterStringDesc(const char* str)
{
	if (!str) return 0;
	uint8_t iStringDesc = iStringDescCur_++;
	stringDescTbl_[iStringDesc] = str;
	return iStringDesc;
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

const uint16_t* Device::On_GET_STRING_DESCRIPTOR(uint8_t idxString, uint16_t langid)
{
	::printf("On_GET_STRING_DESCRIPTOR(%d, %04x)\n", idxString, langid);
	int bytesDst = 0;
	if (idxString == 0) {
		stringDesc_[1] = langid_;
		bytesDst = 2;
	} else {
		UTF8Decoder decoder;
		uint8_t* pDst = reinterpret_cast<uint8_t*>(&stringDesc_[1]);
		for (const char* pSrc = stringDescTbl_[idxString]; *pSrc; pSrc++) {
			uint32_t codeUTF32;
			if (decoder.FeedChar(*pSrc, &codeUTF32)) {
				if (bytesDst + 2 > sizeof(stringDesc_) - 2) break;
				*pDst++ = static_cast<uint8_t>(codeUTF32 & 0xff);
				*pDst++ = static_cast<uint8_t>((codeUTF32 >> 8) & 0xff);
				bytesDst += 2;
			}
		}
	}
	stringDesc_[0] = static_cast<uint16_t>((TUSB_DESC_STRING << 8) | static_cast<uint8_t>(bytesDst + 2));
	return stringDesc_;
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

Keyboard::Keyboard(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), device.RegisterStringDesc(str), HID_ITF_PROTOCOL_KEYBOARD,
			sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Mouse
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

const uint8_t Mouse::descriptor[] = { TUD_HID_REPORT_DESC_MOUSE() };

Mouse::Mouse(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), device.RegisterStringDesc(str), HID_ITF_PROTOCOL_MOUSE,
			sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Consumer
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

const uint8_t Consumer::descriptor[] = { TUD_HID_REPORT_DESC_GAMEPAD() };

Consumer::Consumer(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), device.RegisterStringDesc(str), HID_ITF_PROTOCOL_NONE,
			sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Gamepad
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

const uint8_t Gamepad::descriptor[] = { TUD_HID_REPORT_DESC_GAMEPAD() };

Gamepad::Gamepad(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(GetInterfaceNum(), device.RegisterStringDesc(str), HID_ITF_PROTOCOL_NONE,
			sizeof(descriptor), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

#endif

//-----------------------------------------------------------------------------
// USBD::CDC
//-----------------------------------------------------------------------------
#if CFG_TUD_CDC > 0

namespace jxglib::USBD {

CDC::CDC(Device& device, const char* str, uint8_t endpNotif, uint8_t bytesNotif, uint8_t endpBulkOut, uint8_t endpBulkIn, uint8_t bytesBulk, uint8_t pollingInterval) :
				Interface(device, pollingInterval)
{
	uint8_t configDesc[] = {
		// Interface number, string index, EP notification address and size, EP data address (out, in) and size.
		TUD_CDC_DESCRIPTOR(GetInterfaceNum(), device.RegisterStringDesc(str), endpNotif, bytesNotif, endpBulkOut, endpBulkIn, bytesBulk),
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

void tud_cdc_line_state_cb(uint8_t interfaceNum, bool dtr, bool rts)
{
	using namespace jxglib::USBD;
	Device::GetInterface<CDC>(interfaceNum).On_line_state(dtr, rts);
}  

void tud_cdc_rx_cb(uint8_t interfaceNum)
{
	using namespace jxglib::USBD;
	Device::GetInterface<CDC>(interfaceNum).On_rx();
}

#endif

//-----------------------------------------------------------------------------
// USBD::MSC
//-----------------------------------------------------------------------------
#if CFG_TUD_MSC > 0

namespace jxglib::USBD {

MSC::MSC(Device& device, const char* str, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize) : Interface(device, 0)
{
	uint8_t configDesc[] = {
		// Interface number, string index, EP Out & EP In address, EP size
		TUD_MSC_DESCRIPTOR(GetInterfaceNum(), device.RegisterStringDesc(str), endpBulkOut, endpBulkIn, endpSize),
	};
	RegisterConfigDesc(configDesc, sizeof(configDesc));
	device.SetInterfaceMSC(this);
}

}

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
