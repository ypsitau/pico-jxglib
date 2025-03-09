//==============================================================================
// USBD.cpp
//==============================================================================
#include "jxglib/UTF8.h"
#include "jxglib/USBD.h"

//-----------------------------------------------------------------------------
// USBD::Device
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

Device* Device::Instance = nullptr;

Device::Device(const tusb_desc_device_t& deviceDesc, uint16_t langid,
		const char* strManufacturer, const char* strProduct, const char* strSerialNumber,
		uint8_t attr, uint16_t power_ma) : Tickable(0),
	deviceDesc_{deviceDesc}, langid_{langid}, attr_{attr}, power_ma_{power_ma}, interfaceNumCur_{0},
	offsetConfigDesc_{TUD_CONFIG_DESC_LEN}, iStringDescCur_{1},
	nInstances_CDC_{0}, nInstances_MSC_{0}, nInstances_HID_{0}
{
	for (int iInstance = 0; iInstance < nInstancesMax; iInstance++) {
		interfaceTbl_[iInstance] = nullptr;
	}
	Instance = this;
	deviceDesc_.bLength = sizeof(tusb_desc_device_t);
	deviceDesc_.bDescriptorType		= TUSB_DESC_DEVICE;
	deviceDesc_.iManufacturer		= RegisterStringDesc(strManufacturer);
	deviceDesc_.iProduct			= RegisterStringDesc(strProduct);
	deviceDesc_.iSerialNumber		= RegisterStringDesc(strSerialNumber);
	deviceDesc_.bNumConfigurations	= 1;
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

uint8_t Device::AddInterface_CDC(CDC* pCDC)
{
	uint8_t iInstance = nInstances_CDC_++;
	if (iInstance >= CFG_TUD_CDC) {
		::panic("CFG_TUD_CDC in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pCDCTbl[iInstance] = pCDC;
	return iInstance;
}

uint8_t Device::AddInterface_MSC(MSC* pMSC) {
	uint8_t iInstance = nInstances_MSC_++;
	if (iInstance >= CFG_TUD_MSC) {
		::panic("CFG_TUD_MSC in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pMSCTbl[iInstance] = pMSC;
	return iInstance;
}

uint8_t Device::AddInterface_HID(HID* pHID)
{
	uint8_t iInstance = nInstances_HID_++;
	if (iInstance >= CFG_TUD_HID) {
		::panic("CFG_TUD_HID in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pHIDTbl[iInstance] = pHID;
	return iInstance;
}

void Device::Initialize(uint8_t rhport)
{
	uint8_t config_num = 1;
	uint8_t stridx = 0;
	uint8_t configDesc[] = {
		9, TUSB_DESC_CONFIGURATION, U16_TO_U8S_LE(offsetConfigDesc_), interfaceNumCur_, config_num,
		stridx, static_cast<uint8_t>(TU_BIT(7) | attr_), static_cast<uint8_t>(power_ma_ / 2)
	};
	::memcpy(configDescAccum_, configDesc, sizeof(configDesc));
	::tud_init(rhport);
	AddTickable(*this);
}

uint8_t Device::AssignInterfaceNum(int nInterfacesToOccupy)
{
	uint8_t interfaceNum = interfaceNumCur_;
	interfaceNumCur_ += nInterfacesToOccupy;
	return interfaceNum;
}

const uint16_t* Device::On_GET_STRING_DESCRIPTOR(uint8_t idxString, uint16_t langid)
{
	::printf("On_GET_STRING_DESCRIPTOR(%d, %04x)\n", idxString, langid);
	int bytesDst = 0;
	if (idxString == 0) {
		stringDesc_[1] = langid_;
		bytesDst = 2;
	} else {
		UTF8::Decoder decoder;
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

Interface::Interface(Device& device, int nInterfacesToOccupy, uint32_t msecTaskInterval) :
	Tickable(msecTaskInterval), device_{device}, iInstance_{0}
{
	interfaceNum_ = device.AssignInterfaceNum(nInterfacesToOccupy);
}
	
void Interface::RegisterConfigDesc(const void* configDesc, int bytes)
{
	device_.RegisterConfigDesc(configDesc, bytes);
}

}

#if CFG_TUD_HID > 0

//-----------------------------------------------------------------------------
// USBD::HID
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

HID::HID(Device& device, uint32_t msecTaskInterval) : Interface(device, 1, msecTaskInterval), reportDesc_{nullptr}
{
	iInstance_ = device.AddInterface_HID(this);
}

}

// Invoked when received DESCRIPTOR_REPORT control request
const uint8_t* tud_hid_descriptor_report_cb(uint8_t iInstance)
{
	using namespace jxglib::USBD;
	HID* pHID = Device::GetInterface_HID(iInstance);
	if (pHID) return pHID->On_DESCRIPTOR_REPORT();
	return reinterpret_cast<const uint8_t*>("");
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t iInstance, uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib::USBD;
	HID* pHID = Device::GetInterface_HID(iInstance);
	if (pHID) return pHID->On_GET_REPORT(reportID, reportType, report, reportLength);
	return 0;
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t iInstance, uint8_t const* report, uint16_t reportLength)
{
	using namespace jxglib::USBD;
	HID* pHID = Device::GetInterface_HID(iInstance);
	if (pHID) pHID->On_GET_REPORT_Complete(report, reportLength);
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint (Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t iInstance, uint8_t reportID, hid_report_type_t reportType, const uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib::USBD;
	HID* pHID = Device::GetInterface_HID(iInstance);
	if (pHID) pHID->On_SET_REPORT(reportID, reportType, report, reportLength);
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t iInstance, uint8_t protocol)
{
	using namespace jxglib::USBD;
	HID* pHID = Device::GetInterface_HID(iInstance);
	if (pHID) pHID->On_SET_PROTOCOL(protocol);
}

//-----------------------------------------------------------------------------
// USBD::Keyboard
//-----------------------------------------------------------------------------
namespace jxglib::USBD {


Keyboard::Keyboard(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	static const uint8_t reportDesc[] = { TUD_HID_REPORT_DESC_KEYBOARD() };
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(interfaceNum_, device.RegisterStringDesc(str), HID_ITF_PROTOCOL_KEYBOARD,
			sizeof(reportDesc), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterReportDesc(reportDesc);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Mouse
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

Mouse::Mouse(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	static const uint8_t reportDesc[] = { TUD_HID_REPORT_DESC_MOUSE() };
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(interfaceNum_, device.RegisterStringDesc(str), HID_ITF_PROTOCOL_MOUSE,
			sizeof(reportDesc), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterReportDesc(reportDesc);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Consumer
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

Consumer::Consumer(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	static const uint8_t reportDesc[] = { TUD_HID_REPORT_DESC_GAMEPAD() };
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(interfaceNum_, device.RegisterStringDesc(str), HID_ITF_PROTOCOL_NONE,
			sizeof(reportDesc), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterReportDesc(reportDesc);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

//-----------------------------------------------------------------------------
// USBD::Gamepad
//-----------------------------------------------------------------------------
namespace jxglib::USBD {

Gamepad::Gamepad(Device& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) : HID(device, pollingInterval)
{
	static const uint8_t reportDesc[] = { TUD_HID_REPORT_DESC_GAMEPAD() };
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(interfaceNum_, device.RegisterStringDesc(str), HID_ITF_PROTOCOL_NONE,
			sizeof(reportDesc), endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	RegisterReportDesc(reportDesc);
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
				Interface(device, 2, pollingInterval)
{
	uint8_t configDesc[] = {
		// Interface number, string index, EP notification address and size, EP data address (out, in) and size.
		TUD_CDC_DESCRIPTOR(interfaceNum_, device.RegisterStringDesc(str), endpNotif, bytesNotif, endpBulkOut, endpBulkIn, bytesBulk),
	};
	iInstance_ = device.AddInterface_CDC(this);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

// Invoked when received new data
void tud_cdc_rx_cb(uint8_t iInstance)
{
	using namespace jxglib::USBD;
	CDC* pCDC = Device::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_rx();
}

// Invoked when received `wanted_char`
void tud_cdc_rx_wanted_cb(uint8_t iInstance, char wanted_char)
{
	using namespace jxglib::USBD;
	CDC* pCDC = Device::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_rx_wanted(wanted_char);
}  

// Invoked when a TX is complete and therefore space becomes available in TX buffer
void tud_cdc_tx_complete_cb(uint8_t iInstance)
{
	using namespace jxglib::USBD;
	CDC* pCDC = Device::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_tx_complete();
}  

// Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
void tud_cdc_line_state_cb(uint8_t iInstance, bool dtr, bool rts)
{
	using namespace jxglib::USBD;
	CDC* pCDC = Device::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_line_state(dtr, rts);
}  

// Invoked when line coding is change via SET_LINE_CODING
void tud_cdc_line_coding_cb(uint8_t iInstance, const cdc_line_coding_t* p_line_coding)
{
	using namespace jxglib::USBD;
	CDC* pCDC = Device::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_line_coding(p_line_coding);
}  

// Invoked when received send break
void tud_cdc_send_break_cb(uint8_t iInstance, uint16_t duration_ms)
{
	using namespace jxglib::USBD;
	CDC* pCDC = Device::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_send_break(duration_ms);
}  

#endif

//-----------------------------------------------------------------------------
// USBD::MSC
//-----------------------------------------------------------------------------
#if CFG_TUD_MSC > 0

namespace jxglib::USBD {

MSC::MSC(Device& device, const char* str, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize) : Interface(device, 1, 0)
{
	uint8_t configDesc[] = {
		// Interface number, string index, EP Out & EP In address, EP size
		TUD_MSC_DESCRIPTOR(interfaceNum_, device.RegisterStringDesc(str), endpBulkOut, endpBulkIn, endpSize),
	};
	iInstance_ = device.AddInterface_MSC(this);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	using namespace jxglib::USBD;
	Device::GetInterface_MSC()->On_msc_inquiry(lun, vendor_id, product_id, product_rev);
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	using namespace jxglib::USBD;
	return Device::GetInterface_MSC()->On_msc_test_unit_ready(lun);
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	using namespace jxglib::USBD;
	return Device::GetInterface_MSC()->On_msc_capacity(lun, block_count, block_size);
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	using namespace jxglib::USBD;
	return Device::GetInterface_MSC()->On_msc_start_stop(lun, power_condition, start, load_eject);
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	using namespace jxglib::USBD;
	return Device::GetInterface_MSC()->On_msc_read10(lun, lba, offset, buffer, bufsize);
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
	using namespace jxglib::USBD;
	return Device::GetInterface_MSC()->On_msc_is_writable(lun);
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	using namespace jxglib::USBD;
	return Device::GetInterface_MSC()->On_msc_write10(lun, lba, offset, buffer, bufsize);
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	using namespace jxglib::USBD;
	return Device::GetInterface_MSC()->On_msc_scsi(lun, scsi_cmd, buffer, bufsize);
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
