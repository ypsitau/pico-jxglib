//==============================================================================
// USBHost.cpp
//==============================================================================
#include "jxglib/USBHost.h"

namespace jxglib {

//------------------------------------------------------------------------------
// USBHost
//------------------------------------------------------------------------------
USBHost* USBHost::Instance = nullptr;

USBHost::USBHost()
{
	Instance = this;
}

void USBHost::Initialize(uint8_t rhport)
{
	::tuh_init(rhport);
}

void USBHost::OnTick()
{
	::tuh_task();
}

extern "C" {

void tuh_mount_cb(uint8_t devAddr)
{
	USBHost::Instance->OnMount(devAddr);
}

void tuh_umount_cb(uint8_t devAddr)
{
	USBHost::Instance->OnUmount(devAddr);
}

#define MAX_REPORT  4

static struct {
	uint8_t report_count;
	tuh_hid_report_info_t report_info[MAX_REPORT];
} hid_info[CFG_TUH_HID];

void tuh_hid_mount_cb(uint8_t devAddr, uint8_t iInstance, const uint8_t* descReport, uint16_t descLen)
{
	::printf("tuh_hid_mount_cb(%d, %d)\n", devAddr, iInstance);
	uint8_t itfProtocol = ::tuh_hid_interface_protocol(devAddr, iInstance);
	::printf("HID Interface Protocol = %s\r\n",
		(itfProtocol == HID_ITF_PROTOCOL_NONE)? "None" :
		(itfProtocol == HID_ITF_PROTOCOL_KEYBOARD)? "Keyboard" :
		(itfProtocol == HID_ITF_PROTOCOL_MOUSE)? "Mouse" : "unknown");
	// By default host stack will use activate boot protocol on supported interface.
	// Therefore for this simple example, we only need to parse generic report descriptor (with built-in parser)
	if (itfProtocol == HID_ITF_PROTOCOL_NONE) {
		hid_info[iInstance].report_count = ::tuh_hid_parse_report_descriptor(
							hid_info[iInstance].report_info, MAX_REPORT, descReport, descLen);
		::printf("HID has %u reports \r\n", hid_info[iInstance].report_count);
	}
	// request to receive report
	// tuh_hid_report_received_cb() will be invoked when report is available
	if (!::tuh_hid_receive_report(devAddr, iInstance)) {
		::printf("Error: cannot request to receive report\r\n");
	}
}

void tuh_hid_umount_cb(uint8_t devAddr, uint8_t iInstance)
{
	::printf("tuh_hid_umount_cb(%d, %d)\n", devAddr, iInstance);
}

void tuh_hid_report_received_cb(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len)
{
	uint8_t const itfProtocol = ::tuh_hid_interface_protocol(devAddr, iInstance);
	switch (itfProtocol) {
	case HID_ITF_PROTOCOL_NONE: {
		break;
	}
	case HID_ITF_PROTOCOL_KEYBOARD: {
		USBHost::GetKeyboard().OnReport(devAddr, iInstance, *reinterpret_cast<const hid_keyboard_report_t*>(report));
		break;
	}
	case HID_ITF_PROTOCOL_MOUSE: {
		USBHost::GetMouse().OnReport(devAddr, iInstance, *reinterpret_cast<const hid_mouse_report_t*>(report));
		break;
	}
	default: break;
	}
	if (!::tuh_hid_receive_report(devAddr, iInstance)) {
		printf("Error: cannot request to receive report\r\n");
	}
}

}

//------------------------------------------------------------------------------
// USBHost::Keyboard
//------------------------------------------------------------------------------
const USBHost::Keyboard::ConvEntry USBHost::Keyboard::convEntryTbl_101Keyboard[128] = {
	{0,				0,			0,			0x00}, // 0x00
	{0,				0,			0,			0x00}, // 0x01
	{0,				0,			0,			0x00}, // 0x02
	{0,				0,			0,			0x00}, // 0x03
	{'A',			'a',		'A',		0x01}, // 0x04
	{'B',			'b',		'B',		0x02}, // 0x05
	{'C',			'c',		'C',		0x03}, // 0x06
	{'D',			'd',		'D',		0x04}, // 0x07
	{'E',			'e',		'E',		0x05}, // 0x08
	{'F',			'f',		'F',		0x06}, // 0x09
	{'G',			'g',		'G',		0x07}, // 0x0a
	{'H',			'h',		'H',		0x08}, // 0x0b
	{'I',			'i',		'I',		0x09}, // 0x0c
	{'J',			'j',		'J',		0x0a}, // 0x0d
	{'K',			'k',		'K',		0x0b}, // 0x0e
	{'L',			'l',		'L',		0x0c}, // 0x0f
	{'M',			'm',		'M',		0x0d}, // 0x10
	{'N',			'n',		'N',		0x0e}, // 0x11
	{'O',			'o',		'O',		0x0f}, // 0x12
	{'P',			'p',		'P',		0x10}, // 0x13
	{'Q',			'q',		'Q',		0x11}, // 0x14
	{'R',			'r',		'R',		0x12}, // 0x15
	{'S',			's',		'S',		0x13}, // 0x16
	{'T',			't',		'T',		0x14}, // 0x17
	{'U',			'u',		'U',		0x15}, // 0x18
	{'V',			'v',		'V',		0x16}, // 0x19
	{'W',			'w',		'W',		0x17}, // 0x1a
	{'X',			'x',		'X',		0x18}, // 0x1b
	{'Y',			'y',		'Y',		0x19}, // 0x1c
	{'Z',			'z',		'Z',		0x1a}, // 0x1d
	{'1',			'1',		'!',		0x00}, // 0x1e
	{'2',			'2',		'@',		0x00}, // 0x1f
	{'3',			'3',		'#',		0x00}, // 0x20
	{'4',			'4',		'$',		0x00}, // 0x21
	{'5',			'5',		'%',		0x00}, // 0x22
	{'6',			'6',		'^',		0x00}, // 0x23
	{'7',			'7',		'&',		0x00}, // 0x24
	{'8',			'8',		'*',		0x00}, // 0x25
	{'9',			'9',		'(',		0x00}, // 0x26
	{'0',			'0',		')',		0x00}, // 0x27
	{VK_RETURN,		0,			0,			0x00}, // 0x28
	{VK_ESCAPE,		'\x1b',		'\x1b',		0x00}, // 0x29
	{VK_BACK,		0,			0,			0x00}, // 0x2a
	{VK_TAB,		'\t',		'\t',		0x00}, // 0x2b
	{VK_SPACE,		' ',		' ',		0x00}, // 0x2c
	{VK_OEM_MINUS,	'-',		'_',		0x00}, // 0x2d
	{VK_OEM_PLUS,	'=',		'+',		0x00}, // 0x2e
	{VK_OEM_4,		'[',		'{',		0x00}, // 0x2f
	{VK_OEM_6,		']',		'}',		0x00}, // 0x30
	{VK_OEM_5,		'\\',		'|',		0x00}, // 0x31
	{VK_OEM_3,		'#',		'~',		0x00}, // 0x32
	{VK_OEM_1,		';',		':',		0x00}, // 0x33
	{VK_OEM_7,		'\'',		'\"',		0x00}, // 0x34
	{VK_OEM_3,		'`',		'~',		0x00}, // 0x35
	{VK_OEM_COMMA,	',',		'<',		0x00}, // 0x36
	{VK_OEM_PERIOD,	'.',		'>',		0x00}, // 0x37
	{VK_OEM_2,		'/',		'?',		0x00}, // 0x38
	{VK_CAPITAL,	0,			0,			0x00}, // 0x39
	{VK_F1,			0,			0,			0x00}, // 0x3a
	{VK_F2,			0,			0,			0x00}, // 0x3b
	{VK_F3,			0,			0,			0x00}, // 0x3c
	{VK_F4,			0,			0,			0x00}, // 0x3d
	{VK_F5,			0,			0,			0x00}, // 0x3e
	{VK_F6,			0,			0,			0x00}, // 0x3f
	{VK_F7,			0,			0,			0x00}, // 0x40
	{VK_F8,			0,			0,			0x00}, // 0x41
	{VK_F9,			0,			0,			0x00}, // 0x42
	{VK_F10,		0,			0,			0x00}, // 0x43
	{VK_F11,		0,			0,			0x00}, // 0x44
	{VK_F12,		0,			0,			0x00}, // 0x45
	{VK_PRINT,		0,			0,			0x00}, // 0x46
	{VK_SCROLL,		0,			0,			0x00}, // 0x47
	{VK_PAUSE,		0,			0,			0x00}, // 0x48
	{VK_INSERT,		0,			0,			0x00}, // 0x49
	{VK_HOME,		0,			0,			0x00}, // 0x4a
	{VK_PRIOR,		0,			0,			0x00}, // 0x4b
	{VK_DELETE,		0,			0,			0x00}, // 0x4c
	{VK_END,		0,			0,			0x00}, // 0x4d
	{VK_NEXT,		0,			0,			0x00}, // 0x4e
	{VK_RIGHT,		0,			0,			0x00}, // 0x4f
	{VK_LEFT,		0,			0,			0x00}, // 0x50
	{VK_DOWN,		0,			0,			0x00}, // 0x51
	{VK_UP,			0,			0,			0x00}, // 0x52
	{VK_NUMLOCK,	0,			0,			0x00}, // 0x53
	{VK_DIVIDE,		'/',		'/',		0x00}, // 0x54
	{VK_MULTIPLY,	'*',		'*',		0x00}, // 0x55
	{VK_SUBTRACT,	'-',		'-',		0x00}, // 0x56
	{VK_ADD,		'+',		'+',		0x00}, // 0x57
	{VK_RETURN,		0,			0,			0x00}, // 0x58
	{VK_NUMPAD1,	'1',		0,			0x00}, // 0x59
	{VK_NUMPAD2,	'2',		0,			0x00}, // 0x5a
	{VK_NUMPAD3,	'3',		0,			0x00}, // 0x5b
	{VK_NUMPAD4,	'4',		0,			0x00}, // 0x5c
	{VK_NUMPAD5,	'5',		0,			0x00}, // 0x5d
	{VK_NUMPAD6,	'6',		0,			0x00}, // 0x5e
	{VK_NUMPAD7,	'7',		0,			0x00}, // 0x5f
	{VK_NUMPAD8,	'8',		0,			0x00}, // 0x60
	{VK_NUMPAD9,	'9',		0,			0x00}, // 0x61
	{VK_NUMPAD0,	'0',		0,			0x00}, // 0x62
	{VK_DECIMAL,	'.',		0,			0x00}, // 0x63
	{0,				0,			0,			0x00}, // 0x64
	{0,				0,			0,			0x00}, // 0x65
	{0,				0,			0,			0x00}, // 0x66
	{0,				'=',		'=',		0x00}, // 0x67
};

USBHost::Keyboard::Keyboard() : Tickable(100), cntHold_{0}
{
	::memset(&report_, 0x00, sizeof(report_));
}

void USBHost::Keyboard::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report)
{
	report_ = report;
	if (report_.keycode[0]) {
		fifoKeyData_.WriteData(CreateKeyData(report_.keycode[0], report_.modifier));
		cntHold_ = 5;
	}
}

bool USBHost::Keyboard::GetKeyData(KeyData& keyData)
{
	if (fifoKeyData_.HasData()) {
		keyData = fifoKeyData_.ReadData();
		return true;
	}
	return false;
}

void USBHost::Keyboard::OnTick()
{
	if (cntHold_ > 0) {
		cntHold_--;
		return;
	}
	if (report_.keycode[0]) {
		fifoKeyData_.WriteData(CreateKeyData(report_.keycode[0], report_.modifier));
	}
}

KeyData USBHost::Keyboard::CreateKeyData(uint8_t keycode, uint8_t modifier)
{
	bool isShiftDown = !!(modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT));
	bool isCtrlDown = !!(modifier & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL));
	const ConvEntry& convEntry = convEntryTbl_101Keyboard[keycode];
	char charCode = isCtrlDown? convEntry.charCodeCtrl : isShiftDown? convEntry.charCodeShift : convEntry.charCode;
	return (charCode == 0)? KeyData(convEntry.keyCode, true, modifier) : KeyData(charCode, false, modifier);
}

//------------------------------------------------------------------------------
// USBHost::Mouse
//------------------------------------------------------------------------------
USBHost::Mouse::Mouse()
{
}

void USBHost::Mouse::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report)
{
	status_.Update(report);
}

USBHost::Mouse::Status USBHost::Mouse::CaptureStatus()
{
	Status statusRtn = status_;
	status_.Clear();
	return statusRtn;
}

//------------------------------------------------------------------------------
// USBHost::Mouse::Report
//------------------------------------------------------------------------------
void USBHost::Mouse::Status::Update(const hid_mouse_report_t& report)
{
	deltaX_ += report.x;
	deltaY_ += report.y;
	deltaWheel_ += report.wheel;
	deltaPan_ += report.pan;
	buttonsPrev_ = buttons_;
	buttons_ = report.buttons;
}

void USBHost::Mouse::Status::Clear()
{
	deltaX_ = 0;
	deltaY_ = 0;
	deltaWheel_ = 0;
	deltaPan_ = 0;
}

}
