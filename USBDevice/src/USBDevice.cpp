//==============================================================================
// USBDevice.cpp
//==============================================================================
#include "jxglib/UTF8.h"
#include "jxglib/USBDevice.h"

//-----------------------------------------------------------------------------
// USBDevice
//-----------------------------------------------------------------------------
namespace jxglib {

USBDevice* USBDevice::Instance = nullptr;

USBDevice::USBDevice(const tusb_desc_device_t& deviceDesc, uint16_t langid,
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

void USBDevice::RegisterConfigDesc(const void* configDesc, int bytes)
{
	::memcpy(configDescAccum_ + offsetConfigDesc_, configDesc, bytes);
	offsetConfigDesc_ += bytes;
}

uint8_t USBDevice::RegisterStringDesc(const char* str)
{
	if (!str) return 0;
	uint8_t iStringDesc = iStringDescCur_++;
	stringDescTbl_[iStringDesc] = str;
	return iStringDesc;
}

uint8_t USBDevice::AddInterface_CDC(CDC* pCDC)
{
	uint8_t iInstance = nInstances_CDC_++;
	if (iInstance >= CFG_TUD_CDC) {
		::panic("CFG_TUD_CDC in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pCDCTbl[iInstance] = pCDC;
	return iInstance;
}

uint8_t USBDevice::AddInterface_MSC(MSC* pMSC) {
	uint8_t iInstance = nInstances_MSC_++;
	if (iInstance >= CFG_TUD_MSC) {
		::panic("CFG_TUD_MSC in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pMSCTbl[iInstance] = pMSC;
	return iInstance;
}

uint8_t USBDevice::AddInterface_HID(USBDevice::HID* pHID)
{
	uint8_t iInstance = nInstances_HID_++;
	if (iInstance >= CFG_TUD_HID) {
		::panic("CFG_TUD_HID in tusb_config.h must be set to at least %d.", iInstance + 1);
	}
	specific_.pHIDTbl[iInstance] = pHID;
	return iInstance;
}

void USBDevice::Initialize(uint8_t rhport)
{
	uint8_t config_num = 1;
	uint8_t stridx = 0;
	uint8_t configDesc[] = {
		9, TUSB_DESC_CONFIGURATION, U16_TO_U8S_LE(offsetConfigDesc_), interfaceNumCur_, config_num,
		stridx, static_cast<uint8_t>(TU_BIT(7) | attr_), static_cast<uint8_t>(power_ma_ / 2)
	};
	::memcpy(configDescAccum_, configDesc, sizeof(configDesc));
	::tud_init(rhport);
}

uint8_t USBDevice::AssignInterfaceNum(int nInterfacesToOccupy)
{
	uint8_t interfaceNum = interfaceNumCur_;
	interfaceNumCur_ += nInterfacesToOccupy;
	return interfaceNum;
}

const uint16_t* USBDevice::On_GET_STRING_DESCRIPTOR(uint8_t idxString, uint16_t langid)
{
	//::printf("On_GET_STRING_DESCRIPTOR(%d, %04x)\n", idxString, langid);
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
	jxglib::USBDevice::Instance->OnMount();
}

void tud_umount_cb()
{
	jxglib::USBDevice::Instance->OnUmount();
}

// remote_wakeup_en : if host allow us to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
	jxglib::USBDevice::Instance->OnSuspend();
}

void tud_resume_cb()
{
	jxglib::USBDevice::Instance->OnResume();
}

// Invoked when received GET DEVICE DESCRIPTOR
const uint8_t* tud_descriptor_device_cb()
{
	return jxglib::USBDevice::Instance->On_GET_DEVICE_DESCRIPTOR();
}

// Invoked when received GET CONFIGURATION DESCRIPTOR
const uint8_t* tud_descriptor_configuration_cb(uint8_t idxConfig)
{
	return jxglib::USBDevice::Instance->On_GET_CONFIGURATION_DESCRIPTOR(idxConfig);
}

// Invoked when received GET STRING DESCRIPTOR request
const uint16_t* tud_descriptor_string_cb(uint8_t idxString, uint16_t langid)
{
	return jxglib::USBDevice::Instance->On_GET_STRING_DESCRIPTOR(idxString, langid);
}

//-----------------------------------------------------------------------------
// USBDevice::Interface
//-----------------------------------------------------------------------------
namespace jxglib {

USBDevice::Interface::Interface(USBDevice& device, int nInterfacesToOccupy, uint32_t msecTick) :
	Tickable(msecTick), device_{device}, iInstance_{0}
{
	interfaceNum_ = device.AssignInterfaceNum(nInterfacesToOccupy);
}
	
void USBDevice::Interface::RegisterConfigDesc(const void* configDesc, int bytes)
{
	device_.RegisterConfigDesc(configDesc, bytes);
}

}

#if CFG_TUD_HID > 0

//-----------------------------------------------------------------------------
// USBDevice::HID
//-----------------------------------------------------------------------------
namespace jxglib {

USBDevice::HID::HID(USBDevice& device, uint32_t msecTick, const char* str, uint8_t protocol, const uint8_t* reportDesc, uint8_t bytesReportDesc,
	uint8_t endpInterrupt, uint8_t pollingInterval) : Interface(device, 1, msecTick), reportDescSaved_{reportDesc}
{
	uint8_t configDesc[] = {
		TUD_HID_DESCRIPTOR(interfaceNum_, device.RegisterStringDesc(str), protocol,
					bytesReportDesc, endpInterrupt, CFG_TUD_HID_EP_BUFSIZE, pollingInterval)
	};
	iInstance_ = device.AddInterface_HID(this);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

// Invoked when received DESCRIPTOR_REPORT control request
const uint8_t* tud_hid_descriptor_report_cb(uint8_t iInstance)
{
	using namespace jxglib;
	USBDevice::HID* pHID = USBDevice::GetInterface_HID(iInstance);
	if (pHID) return pHID->On_DESCRIPTOR_REPORT();
	return reinterpret_cast<const uint8_t*>("");
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t iInstance, uint8_t reportID, hid_report_type_t reportType, uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib;
	USBDevice::HID* pHID = USBDevice::GetInterface_HID(iInstance);
	if (pHID) return pHID->On_GET_REPORT(reportID, reportType, report, reportLength);
	return 0;
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t iInstance, uint8_t const* report, uint16_t reportLength)
{
	using namespace jxglib;
	USBDevice::HID* pHID = USBDevice::GetInterface_HID(iInstance);
	if (pHID) pHID->On_GET_REPORT_Complete(report, reportLength);
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint (Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t iInstance, uint8_t reportID, hid_report_type_t reportType, const uint8_t* report, uint16_t reportLength)
{
	using namespace jxglib;
	USBDevice::HID* pHID = USBDevice::GetInterface_HID(iInstance);
	if (pHID) pHID->On_SET_REPORT(reportID, reportType, report, reportLength);
}

// Invoked when received SET_PROTOCOL request
// protocol is either HID_PROTOCOL_BOOT (0) or HID_PROTOCOL_REPORT (1)
void tud_hid_set_protocol_cb(uint8_t iInstance, uint8_t protocol)
{
	using namespace jxglib;
	USBDevice::HID* pHID = USBDevice::GetInterface_HID(iInstance);
	if (pHID) pHID->On_SET_PROTOCOL(protocol);
}

//-----------------------------------------------------------------------------
// USBDevice::Keyboard
//-----------------------------------------------------------------------------
namespace jxglib {

const uint8_t USBDevice::Keyboard::reportDesc_[] = { TUD_HID_REPORT_DESC_KEYBOARD() };

const USBDevice::Keyboard::KeyCodeToUsageId USBDevice::Keyboard::keyCodeToUsageIdTbl[] = {
	{ 0x00,	0x00 },	// 0x00			
	{ 0x00,	0x00 },	// 0x01: VK_LBUTTON		
	{ 0x00,	0x00 },	// 0x02: VK_RBUTTON		
	{ 0x00,	0x00 },	// 0x03: VK_CANCEL			
	{ 0x00,	0x00 },	// 0x04: VK_MBUTTON		
	{ 0x00,	0x00 },	// 0x05: VK_XBUTTON1		
	{ 0x00,	0x00 },	// 0x06: VK_XBUTTON2		
	{ 0x00,	0x00 },	// 0x07			
	{ 0x2a,	0x2a },	// 0x08: VK_BACK			
	{ 0x2b,	0x2b },	// 0x09: VK_TAB			
	{ 0x00,	0x00 },	// 0x0a			
	{ 0x00,	0x00 },	// 0x0b
	{ 0x00,	0x00 },	// 0x0c: VK_CLEAR
	{ 0x28,	0x28 },	// 0x0d: VK_RETURN
	{ 0x00,	0x00 },	// 0x0e
	{ 0x00,	0x00 },	// 0x0f
	{ 0xe1,	0xe1 },	// 0x10: VK_SHIFT .. VK_LSHIFT
	{ 0xe0,	0xe0 },	// 0x11: VK_CONTROL .. VK_LCONTROL
	{ 0xe2,	0xe2 },	// 0x12: VK_MENU .. VK_LMENU
	{ 0x00,	0x00 },	// 0x13: VK_PAUSE
	{ 0x39,	0x39 },	// 0x14: VK_CAPITAL
	{ 0x00,	0x00 },	// 0x15: VK_KANA, VK_HANGUL
	{ 0x00,	0x00 },	// 0x16: VK_IME_ON
	{ 0x00,	0x00 },	// 0x17: VK_JUNJA
	{ 0x00,	0x00 },	// 0x18: VK_FINAL
	{ 0x35,	0x35 },	// 0x19: VK_KANJI
	{ 0x00,	0x00 },	// 0x1a
	{ 0x29,	0x29 },	// 0x1b: VK_ESCAPE
	{ 0x00,	0x00 },	// 0x1c: VK_CONVERT
	{ 0x00,	0x00 },	// 0x1d: VK_NONCONVERT
	{ 0x00,	0x00 },	// 0x1e: VK_ACCEPT
	{ 0x00,	0x00 },	// 0x1f: VK_MODECHANGE
	{ 0x2c,	0x2c },	// 0x20: VK_SPACE
	{ 0x4b,	0x4b },	// 0x21: VK_PRIOR
	{ 0x4e,	0x4e },	// 0x22: VK_NEXT
	{ 0x4d,	0x4d },	// 0x23: VK_END
	{ 0x4a,	0x4a },	// 0x24: VK_HOME
	{ 0x50,	0x50 },	// 0x25: VK_LEFT
	{ 0x52,	0x52 },	// 0x26: VK_UP
	{ 0x4f,	0x4f },	// 0x27: VK_RIGHT
	{ 0x51,	0x51 },	// 0x28: VK_DOWN
	{ 0x00,	0x00 },	// 0x29: VK_SELECT
	{ 0x46,	0x46 },	// 0x2a: VK_PRINT
	{ 0x00,	0x00 },	// 0x2b: VK_EXECUTE
	{ 0x00,	0x00 },	// 0x2c: VK_SNAPSHOT
	{ 0x49,	0x49 },	// 0x2d: VK_INSERT
	{ 0x4c,	0x4c },	// 0x2e: VK_DELETE
	{ 0x00,	0x00 },	// 0x2f: VK_HELP
	{ 0x27,	0x27 },	// 0x30: '0'
	{ 0x1e,	0x1e },	// 0x31: '1'
	{ 0x1f,	0x1f },	// 0x32: '2'
	{ 0x20,	0x20 },	// 0x33: '3'
	{ 0x21,	0x21 },	// 0x34: '4'
	{ 0x22,	0x22 },	// 0x35: '5'
	{ 0x23,	0x23 },	// 0x36: '6'
	{ 0x24,	0x24 },	// 0x37: '7'
	{ 0x25,	0x25 },	// 0x38: '8'
	{ 0x26,	0x26 },	// 0x39: '9'
	{ 0x00,	0x00 },	// 0x3a
	{ 0x00,	0x00 },	// 0x3b
	{ 0x00,	0x00 },	// 0x3c
	{ 0x00,	0x00 },	// 0x3d
	{ 0x00,	0x00 },	// 0x3e
	{ 0x00,	0x00 },	// 0x3f
	{ 0x00,	0x00 },	// 0x40
	{ 0x04,	0x04 },	// 0x41: 'A'
	{ 0x05,	0x05 },	// 0x42: 'B'
	{ 0x06,	0x06 },	// 0x43: 'C'
	{ 0x07,	0x07 },	// 0x44: 'D'
	{ 0x08,	0x08 },	// 0x45: 'E'
	{ 0x09,	0x09 },	// 0x46: 'F'
	{ 0x0a,	0x0a },	// 0x47: 'G'
	{ 0x0b,	0x0b },	// 0x48: 'H'
	{ 0x0c,	0x0c },	// 0x49: 'I'
	{ 0x0d,	0x0d },	// 0x4a: 'J'
	{ 0x0e,	0x0e },	// 0x4b: 'K'
	{ 0x0f,	0x0f },	// 0x4c: 'L'
	{ 0x10,	0x10 },	// 0x4d: 'M'
	{ 0x11,	0x11 },	// 0x4e: 'N'
	{ 0x12,	0x12 },	// 0x4f: 'O'
	{ 0x13,	0x13 },	// 0x50: 'P'
	{ 0x14,	0x14 },	// 0x51: 'Q'
	{ 0x15,	0x15 },	// 0x52: 'R'
	{ 0x16,	0x16 },	// 0x53: 'S'
	{ 0x17,	0x17 },	// 0x54: 'T'
	{ 0x18,	0x18 },	// 0x55: 'U'
	{ 0x19,	0x19 },	// 0x56: 'V'
	{ 0x1a,	0x1a },	// 0x57: 'W'
	{ 0x1b,	0x1b },	// 0x58: 'X'
	{ 0x1c,	0x1c },	// 0x59: 'Y'
	{ 0x1d,	0x1d },	// 0x5a: 'Z'
	{ 0xe3,	0xe3 },	// 0x5b: VK_LWIN
	{ 0xe7,	0xe7 },	// 0x5c: VK_RWIN
	{ 0x00,	0x00 },	// 0x5d: VK_APPS
	{ 0x00,	0x00 },	// 0x5e
	{ 0x00,	0x00 },	// 0x5f: VK_SLEEP
	{ 0x62,	0x62 },	// 0x60: VK_NUMPAD0
	{ 0x59,	0x59 },	// 0x61: VK_NUMPAD1
	{ 0x5a,	0x5a },	// 0x62: VK_NUMPAD2
	{ 0x5b,	0x5b },	// 0x63: VK_NUMPAD3
	{ 0x5c,	0x5c },	// 0x64: VK_NUMPAD4
	{ 0x5d,	0x5d },	// 0x65: VK_NUMPAD5
	{ 0x5e,	0x5e },	// 0x66: VK_NUMPAD6
	{ 0x5f,	0x5f },	// 0x67: VK_NUMPAD7
	{ 0x60,	0x60 },	// 0x68: VK_NUMPAD8
	{ 0x61,	0x61 },	// 0x69: VK_NUMPAD9
	{ 0x55,	0x55 },	// 0x6a: VK_MULTIPLY
	{ 0x57,	0x57 },	// 0x6b: VK_ADD
	{ 0x00,	0x00 },	// 0x6c: VK_SEPARATOR
	{ 0x56,	0x56 },	// 0x6d: VK_SUBTRACT
	{ 0x63,	0x63 },	// 0x6e: VK_DECIMAL
	{ 0x54,	0x54 },	// 0x6f: VK_DIVIDE
	{ 0x3a,	0x3a },	// 0x70: VK_F1
	{ 0x3b,	0x3b },	// 0x71: VK_F2
	{ 0x3c,	0x3c },	// 0x72: VK_F3
	{ 0x3d,	0x3d },	// 0x73: VK_F4
	{ 0x3e,	0x3e },	// 0x74: VK_F5
	{ 0x3f,	0x3f },	// 0x75: VK_F6
	{ 0x40,	0x40 },	// 0x76: VK_F7
	{ 0x41,	0x41 },	// 0x77: VK_F8
	{ 0x42,	0x42 },	// 0x78: VK_F9
	{ 0x43,	0x43 },	// 0x79: VK_F10
	{ 0x44,	0x44 },	// 0x7a: VK_F11
	{ 0x45,	0x45 },	// 0x7b: VK_F12
	{ 0x00,	0x00 },	// 0x7c: VK_F13
	{ 0x00,	0x00 },	// 0x7d: VK_F14
	{ 0x00,	0x00 },	// 0x7e: VK_F15
	{ 0x00,	0x00 },	// 0x7f: VK_F16
	{ 0x00,	0x00 },	// 0x80: VK_F17
	{ 0x00,	0x00 },	// 0x81: VK_F18
	{ 0x00,	0x00 },	// 0x82: VK_F19
	{ 0x00,	0x00 },	// 0x83: VK_F20
	{ 0x00,	0x00 },	// 0x84: VK_F21
	{ 0x00,	0x00 },	// 0x85: VK_F22
	{ 0x00,	0x00 },	// 0x86: VK_F23
	{ 0x00,	0x00 },	// 0x87: VK_F24
	{ 0x00,	0x00 },	// 0x88
	{ 0x00,	0x00 },	// 0x89
	{ 0x00,	0x00 },	// 0x8a
	{ 0x00,	0x00 },	// 0x8b
	{ 0x00,	0x00 },	// 0x8c
	{ 0x00,	0x00 },	// 0x8d
	{ 0x00,	0x00 },	// 0x8e
	{ 0x00,	0x00 },	// 0x8f
	{ 0x53,	0x53 },	// 0x90: VK_NUMLOCK
	{ 0x47,	0x47 },	// 0x91: VK_SCROLL
	{ 0x00,	0x00 },	// 0x92
	{ 0x00,	0x00 },	// 0x93
	{ 0x00,	0x00 },	// 0x94
	{ 0x00,	0x00 },	// 0x95
	{ 0x00,	0x00 },	// 0x96
	{ 0x00,	0x00 },	// 0x97
	{ 0x00,	0x00 },	// 0x98
	{ 0x00,	0x00 },	// 0x99
	{ 0x00,	0x00 },	// 0x9a
	{ 0x00,	0x00 },	// 0x9b
	{ 0x00,	0x00 },	// 0x9c
	{ 0x00,	0x00 },	// 0x9d
	{ 0x00,	0x00 },	// 0x9e
	{ 0x00,	0x00 },	// 0x9f
	{ 0xe1,	0xe1 },	// 0xa0: VK_LSHIFT
	{ 0xe5,	0xe5 },	// 0xa1: VK_RSHIFT
	{ 0xe0,	0xe0 },	// 0xa2: VK_LCONTROL
	{ 0xe4,	0xe4 },	// 0xa3: VK_RCONTROL
	{ 0xe2,	0xe2 },	// 0xa4: VK_LMENU
	{ 0xe6,	0xe6 },	// 0xa5: VK_RMENU
	{ 0x00,	0x00 },	// 0xa6: VK_BROWSER_BACK
	{ 0x00,	0x00 },	// 0xa7: VK_BROWSER_FORWARD
	{ 0x00,	0x00 },	// 0xa8: VK_BROWSER_REFRESH
	{ 0x00,	0x00 },	// 0xa9: VK_BROWSER_STOP
	{ 0x00,	0x00 },	// 0xaa: VK_BROWSER_SEARCH
	{ 0x00,	0x00 },	// 0xab: VK_BROWSER_FAVORITES
	{ 0x00,	0x00 },	// 0xac: VK_BROWSER_HOME
	{ 0x00,	0x00 },	// 0xad: VK_VOLUME_MUTE
	{ 0x00,	0x00 },	// 0xae: VK_VOLUME_DOWN
	{ 0x00,	0x00 },	// 0xaf: VK_VOLUME_UP
	{ 0x00,	0x00 },	// 0xb0: VK_MEDIA_NEXT_TRACK
	{ 0x00,	0x00 },	// 0xb1: VK_MEDIA_PREV_TRACK
	{ 0x00,	0x00 },	// 0xb2: VK_MEDIA_STOP
	{ 0x00,	0x00 },	// 0xb3: VK_MEDIA_PLAY_PAUSE
	{ 0x00,	0x00 },	// 0xb4: VK_LAUNCH_MAIL
	{ 0x00,	0x00 },	// 0xb5: VK_LAUNCH_MEDIA_SELECT
	{ 0x00,	0x00 },	// 0xb6: VK_LAUNCH_APP1
	{ 0x00,	0x00 },	// 0xb7: VK_LAUNCH_APP2
	{ 0x00,	0x00 },	// 0xb8
	{ 0x00,	0x00 },	// 0xb9
	{ 0x33,	0x34 },	// 0xba: VK_OEM_1
	{ 0x2e,	0x33 },	// 0xbb: VK_OEM_PLUS
	{ 0x36,	0x36 },	// 0xbc: VK_OEM_COMMA
	{ 0x2d,	0x2d },	// 0xbd: VK_OEM_MINUS
	{ 0x37,	0x37 },	// 0xbe: VK_OEM_PERIOD
	{ 0x38,	0x38 },	// 0xbf: VK_OEM_2
	{ 0x35,	0x2f },	// 0xc0: VK_OEM_3
	{ 0x00,	0x00 },	// 0xc1
	{ 0x00,	0x00 },	// 0xc2
	{ 0x00,	0x00 },	// 0xc3
	{ 0x00,	0x00 },	// 0xc4
	{ 0x00,	0x00 },	// 0xc5
	{ 0x00,	0x00 },	// 0xc6
	{ 0x00,	0x00 },	// 0xc7
	{ 0x00,	0x00 },	// 0xc8
	{ 0x00,	0x00 },	// 0xc9
	{ 0x00,	0x00 },	// 0xca
	{ 0x00,	0x00 },	// 0xcb
	{ 0x00,	0x00 },	// 0xcc
	{ 0x00,	0x00 },	// 0xcd
	{ 0x00,	0x00 },	// 0xce
	{ 0x00,	0x00 },	// 0xcf
	{ 0x00,	0x00 },	// 0xd0
	{ 0x00,	0x00 },	// 0xd1
	{ 0x00,	0x00 },	// 0xd2
	{ 0x00,	0x00 },	// 0xd3
	{ 0x00,	0x00 },	// 0xd4
	{ 0x00,	0x00 },	// 0xd5
	{ 0x00,	0x00 },	// 0xd6
	{ 0x00,	0x00 },	// 0xd7
	{ 0x00,	0x00 },	// 0xd8
	{ 0x00,	0x00 },	// 0xd9
	{ 0x00,	0x00 },	// 0xda
	{ 0x2f,	0x30 },	// 0xdb: VK_OEM_4
	{ 0x31,	0x31 },	// 0xdc: VK_OEM_5
	{ 0x32,	0x32 },	// 0xdd: VK_OEM_6
	{ 0x34,	0x2e },	// 0xde: VK_OEM_7
	{ 0x00,	0x00 },	// 0xdf: VK_OEM_8
	{ 0x00,	0x00 },	// 0xe0
	{ 0x00,	0x00 },	// 0xe1
	{ 0x87,	0x87 },	// 0xe2: VK_OEM_102
	{ 0x00,	0x00 },	// 0xe3
	{ 0x00,	0x00 },	// 0xe4
	{ 0x00,	0x00 },	// 0xe5: VK_PROCESSKEY
	{ 0x00,	0x00 },	// 0xe6
	{ 0x00,	0x00 },	// 0xe7: VK_PACKET
	{ 0x00,	0x00 },	// 0xe8
	{ 0x00,	0x00 },	// 0xe9
	{ 0x00,	0x00 },	// 0xea
	{ 0x00,	0x00 },	// 0xeb
	{ 0x00,	0x00 },	// 0xec
	{ 0x00,	0x00 },	// 0xed
	{ 0x00,	0x00 },	// 0xee
	{ 0x00,	0x00 },	// 0xef
	{ 0x00,	0x00 },	// 0xf0
	{ 0x00,	0x00 },	// 0xf1
	{ 0x00,	0x00 },	// 0xf2
	{ 0x00,	0x00 },	// 0xf3
	{ 0x00,	0x00 },	// 0xf4
	{ 0x00,	0x00 },	// 0xf5
	{ 0x00,	0x00 },	// 0xf6: VK_ATTN
	{ 0x00,	0x00 },	// 0xf7: VK_CRSEL
	{ 0x00,	0x00 },	// 0xf8: VK_EXSEL
	{ 0x00,	0x00 },	// 0xf9: VK_EREOF
	{ 0x00,	0x00 },	// 0xfa: VK_PLAY
	{ 0x00,	0x00 },	// 0xfb: VK_ZOOM
	{ 0x00,	0x00 },	// 0xfc: VK_NONAME
	{ 0x00,	0x00 },	// 0xfd: VK_PA1
	{ 0x00,	0x00 },	// 0xfe: VK_OEM_CLEAR
	{ 0x00,	0x00 },	// 0xff
};

USBDevice::Keyboard::Keyboard(USBDevice& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) :
	HID(device, pollingInterval, str, HID_ITF_PROTOCOL_KEYBOARD, reportDesc_, sizeof(reportDesc_), endpInterrupt, pollingInterval)
{
}


}

//-----------------------------------------------------------------------------
// USBDevice::Mouse
//-----------------------------------------------------------------------------
namespace jxglib {

const uint8_t USBDevice::Mouse::reportDesc_[] = { TUD_HID_REPORT_DESC_MOUSE() };

USBDevice::Mouse::Mouse(USBDevice& device, const char* str, uint8_t endpInterrupt, uint8_t pollingInterval) :
	HID(device, pollingInterval, str, HID_ITF_PROTOCOL_MOUSE, reportDesc_, sizeof(reportDesc_), endpInterrupt, pollingInterval)
{
}

}

//-----------------------------------------------------------------------------
// USBDevice::HIDCustom
//-----------------------------------------------------------------------------
namespace jxglib {

USBDevice::HIDCustom::HIDCustom(USBDevice& device, const char* str, const uint8_t* reportDesc,
					uint8_t bytesReportDesc, uint8_t endpInterrupt, uint8_t pollingInterval) :
	HID(device, pollingInterval, str, HID_ITF_PROTOCOL_NONE, reportDesc, bytesReportDesc, endpInterrupt, pollingInterval)
{
}
		
}

#endif

//-----------------------------------------------------------------------------
// USBDevice::CDC
//-----------------------------------------------------------------------------
#if CFG_TUD_CDC > 0

namespace jxglib {

USBDevice::CDC::CDC(USBDevice& device, const char* str, uint8_t endpNotif, uint8_t bytesNotif, uint8_t endpBulkOut, uint8_t endpBulkIn, uint8_t bytesBulk, uint8_t pollingInterval) :
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
	using namespace jxglib;
	USBDevice::CDC* pCDC = USBDevice::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_rx();
}

// Invoked when received `wanted_char`
void tud_cdc_rx_wanted_cb(uint8_t iInstance, char wanted_char)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = USBDevice::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_rx_wanted(wanted_char);
}  

// Invoked when a TX is complete and therefore space becomes available in TX buffer
void tud_cdc_tx_complete_cb(uint8_t iInstance)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = USBDevice::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_tx_complete();
}  

// Invoked when line state DTR & RTS are changed via SET_CONTROL_LINE_STATE
void tud_cdc_line_state_cb(uint8_t iInstance, bool dtr, bool rts)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = USBDevice::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_line_state(dtr, rts);
}  

// Invoked when line coding is change via SET_LINE_CODING
void tud_cdc_line_coding_cb(uint8_t iInstance, const cdc_line_coding_t* p_line_coding)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = USBDevice::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_line_coding(p_line_coding);
}  

// Invoked when received send break
void tud_cdc_send_break_cb(uint8_t iInstance, uint16_t duration_ms)
{
	using namespace jxglib;
	USBDevice::CDC* pCDC = USBDevice::GetInterface_CDC(iInstance);
	if (pCDC) pCDC->On_cdc_send_break(duration_ms);
}  

#endif

//-----------------------------------------------------------------------------
// USBDevice::MSC
//-----------------------------------------------------------------------------
#if CFG_TUD_MSC > 0

namespace jxglib {

USBDevice::MSC::MSC(USBDevice& device, const char* str, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize) : Interface(device, 1, 0)
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
	using namespace jxglib;
	USBDevice::GetInterface_MSC()->On_msc_inquiry(lun, vendor_id, product_id, product_rev);
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	using namespace jxglib;
	return USBDevice::GetInterface_MSC()->On_msc_test_unit_ready(lun);
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	using namespace jxglib;
	return USBDevice::GetInterface_MSC()->On_msc_capacity(lun, block_count, block_size);
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	using namespace jxglib;
	return USBDevice::GetInterface_MSC()->On_msc_start_stop(lun, power_condition, start, load_eject);
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	using namespace jxglib;
	return USBDevice::GetInterface_MSC()->On_msc_read10(lun, lba, offset, buffer, bufsize);
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
	using namespace jxglib;
	return USBDevice::GetInterface_MSC()->On_msc_is_writable(lun);
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	using namespace jxglib;
	return USBDevice::GetInterface_MSC()->On_msc_write10(lun, lba, offset, buffer, bufsize);
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	using namespace jxglib;
	return USBDevice::GetInterface_MSC()->On_msc_scsi(lun, scsi_cmd, buffer, bufsize);
}

#endif

//-----------------------------------------------------------------------------
// USBDevice::MIDI
//-----------------------------------------------------------------------------
#if CFG_TUD_MIDI > 0
#endif

//-----------------------------------------------------------------------------
// USBDevice::Vendor
//-----------------------------------------------------------------------------
#if CFG_TUD_VENDOR > 0
#endif
