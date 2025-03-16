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

/*
    0,              // 0x00 (No event)
    0,              // 0x01 (Reserved)
    0,              // 0x02 (Reserved)
    0,              // 0x03 (Reserved)
    'A',            // 0x04
    'B',            // 0x05
    'C',            // 0x06
    'D',            // 0x07
    'E',            // 0x08
    'F',            // 0x09
    'G',            // 0x0A
    'H',            // 0x0B
    'I',            // 0x0C
    'J',            // 0x0D
    'K',            // 0x0E
    'L',            // 0x0F
    'M',            // 0x10
    'N',            // 0x11
    'O',            // 0x12
    'P',            // 0x13
    'Q',            // 0x14
    'R',            // 0x15
    'S',            // 0x16
    'T',            // 0x17
    'U',            // 0x18
    'V',            // 0x19
    'W',            // 0x1A
    'X',            // 0x1B
    'Y',            // 0x1C
    'Z',            // 0x1D
    '1',            // 0x1E
    '2',            // 0x1F
    '3',            // 0x20
    '4',            // 0x21
    '5',            // 0x22
    '6',            // 0x23
    '7',            // 0x24
    '8',            // 0x25
    '9',            // 0x26
    '0',            // 0x27
    VK_RETURN,      // 0x28 (Enter)
    VK_ESCAPE,      // 0x29 (Escape)
    VK_BACK,        // 0x2A (Backspace)
    VK_TAB,         // 0x2B (Tab)
    VK_SPACE,       // 0x2C (Space)
    VK_OEM_MINUS,   // 0x2D (-)
    VK_OEM_PLUS,    // 0x2E (=)
    VK_OEM_4,       // 0x2F ([)
    VK_OEM_6,       // 0x30 (])
    VK_OEM_5,       // 0x31 (\)
    VK_OEM_3,       // 0x32 (`)
    VK_OEM_1,       // 0x33 (;)
    VK_OEM_7,       // 0x34 (')
    VK_OEM_COMMA,   // 0x36 (,)
    VK_OEM_PERIOD,  // 0x37 (.)
    VK_OEM_2,       // 0x38 (/)
    VK_CAPITAL,     // 0x39 (Caps Lock)
    VK_F1,          // 0x3A
    VK_F2,          // 0x3B
    VK_F3,          // 0x3C
    VK_F4,          // 0x3D
    VK_F5,          // 0x3E
    VK_F6,          // 0x3F
    VK_F7,          // 0x40
    VK_F8,          // 0x41
    VK_F9,          // 0x42
    VK_F10,         // 0x43
    VK_F11,         // 0x44
    VK_F12,         // 0x45
    VK_PRINT,       // 0x46 (Print Screen)
    VK_SCROLL,      // 0x47 (Scroll Lock)
    VK_PAUSE,       // 0x48 (Pause)
    VK_INSERT,      // 0x49
    VK_HOME,        // 0x4A
    VK_PRIOR,       // 0x4B (Page Up)
    VK_DELETE,      // 0x4C
    VK_END,         // 0x4D
    VK_NEXT,        // 0x4E (Page Down)
    VK_RIGHT,       // 0x4F
    VK_LEFT,        // 0x50
    VK_DOWN,        // 0x51
    VK_UP,          // 0x52
    VK_NUMLOCK,     // 0x53 (Num Lock)
    VK_NUMPAD0,     // 0x54
    VK_NUMPAD1,     // 0x55
    VK_NUMPAD2,     // 0x56
    VK_NUMPAD3,     // 0x57
    VK_NUMPAD4,     // 0x58
    VK_NUMPAD5,     // 0x59
    VK_NUMPAD6,     // 0x5A
    VK_NUMPAD7,     // 0x5B
    VK_NUMPAD8,     // 0x5C
    VK_NUMPAD9,     // 0x5D
    VK_DECIMAL,     // 0x5E (Keypad .)
    VK_DIVIDE,      // 0x5F (Keypad /)
    VK_MULTIPLY,    // 0x60 (Keypad *)
    VK_SUBTRACT,    // 0x61 (Keypad -)
    VK_ADD,         // 0x62 (Keypad +)
    VK_RETURN,      // 0x63 (Keypad Enter)
*/

//------------------------------------------------------------------------------
// USBHost::Keyboard
//------------------------------------------------------------------------------
const uint8_t USBHost::Keyboard::convTbl_101Keyboard[128][3] = {
	{0,			0,			0x00}, // 0x00
	{0,			0,			0x00}, // 0x01
	{0,			0,			0x00}, // 0x02
	{0,			0,			0x00}, // 0x03
	{'a',		'A',		0x01}, // 0x04
	{'b',		'B',		0x02}, // 0x05
	{'c',		'C',		0x03}, // 0x06
	{'d',		'D',		0x04}, // 0x07
	{'e',		'E',		0x05}, // 0x08
	{'f',		'F',		0x06}, // 0x09
	{'g',		'G',		0x07}, // 0x0a
	{'h',		'H',		0x08}, // 0x0b
	{'i',		'I',		0x09}, // 0x0c
	{'j',		'J',		0x0a}, // 0x0d
	{'k',		'K',		0x0b}, // 0x0e
	{'l',		'L',		0x0c}, // 0x0f
	{'m',		'M',		0x0d}, // 0x10
	{'n',		'N',		0x0e}, // 0x11
	{'o',		'O',		0x0f}, // 0x12
	{'p',		'P',		0x10}, // 0x13
	{'q',		'Q',		0x11}, // 0x14
	{'r',		'R',		0x12}, // 0x15
	{'s',		'S',		0x13}, // 0x16
	{'t',		'T',		0x14}, // 0x17
	{'u',		'U',		0x15}, // 0x18
	{'v',		'V',		0x16}, // 0x19
	{'w',		'W',		0x17}, // 0x1a
	{'x',		'X',		0x18}, // 0x1b
	{'y',		'Y',		0x19}, // 0x1c
	{'z',		'Z',		0x1a}, // 0x1d
	{'1',		'!',		0x00}, // 0x1e
	{'2',		'@',		0x00}, // 0x1f
	{'3',		'#',		0x00}, // 0x20
	{'4',		'$',		0x00}, // 0x21
	{'5',		'%',		0x00}, // 0x22
	{'6',		'^',		0x00}, // 0x23
	{'7',		'&',		0x00}, // 0x24
	{'8',		'*',		0x00}, // 0x25
	{'9',		'(',		0x00}, // 0x26
	{'0',		')',		0x00}, // 0x27
	{'\r',		'\r',		0x00}, // 0x28
	{'\x1b',	'\x1b',		0x00}, // 0x29
	{'\b',		'\b',		0x00}, // 0x2a
	{'\t',		'\t',		0x00}, // 0x2b
	{' ',		' ',		0x00}, // 0x2c
	{'-',		'_',		0x00}, // 0x2d
	{'=',		'+',		0x00}, // 0x2e
	{'[',		'{',		0x00}, // 0x2f
	{']',		'}',		0x00}, // 0x30
	{'\\',		'|',		0x00}, // 0x31
	{'#',		'~',		0x00}, // 0x32
	{';',		':',		0x00}, // 0x33
	{'\'',		'\"',		0x00}, // 0x34
	{'`',		'~',		0x00}, // 0x35
	{',',		'<',		0x00}, // 0x36
	{'.',		'>',		0x00}, // 0x37
	{'/',		'?',		0x00}, // 0x38
	{0,			0,			0x00}, // 0x39
	{0,			0,			0x00}, // 0x3a
	{0,			0,			0x00}, // 0x3b
	{0,			0,			0x00}, // 0x3c
	{0,			0,			0x00}, // 0x3d
	{0,			0,			0x00}, // 0x3e
	{0,			0,			0x00}, // 0x3f
	{0,			0,			0x00}, // 0x40
	{0,			0,			0x00}, // 0x41
	{0,			0,			0x00}, // 0x42
	{0,			0,			0x00}, // 0x43
	{0,			0,			0x00}, // 0x44
	{0,			0,			0x00}, // 0x45
	{0,			0,			0x00}, // 0x46
	{0,			0,			0x00}, // 0x47
	{0,			0,			0x00}, // 0x48
	{0,			0,			0x00}, // 0x49
	{0,			0,			0x00}, // 0x4a
	{0,			0,			0x00}, // 0x4b
	{0,			0,			0x00}, // 0x4c
	{0,			0,			0x00}, // 0x4d
	{0,			0,			0x00}, // 0x4e
	{0,			0,			0x00}, // 0x4f
	{0,			0,			0x00}, // 0x50
	{0,			0,			0x00}, // 0x51
	{0,			0,			0x00}, // 0x52
	{0,			0,			0x00}, // 0x53
	{'/',		'/',		0x00}, // 0x54
	{'*',		'*',		0x00}, // 0x55
	{'-',		'-',		0x00}, // 0x56
	{'+',		'+',		0x00}, // 0x57
	{'\r',		'\r',		0x00}, // 0x58
	{'1',		0,			0x00}, // 0x59
	{'2',		0,			0x00}, // 0x5a
	{'3',		0,			0x00}, // 0x5b
	{'4',		0,			0x00}, // 0x5c
	{'5',		'5',		0x00}, // 0x5d
	{'6',		0,			0x00}, // 0x5e
	{'7',		0,			0x00}, // 0x5f
	{'8',		0,			0x00}, // 0x60
	{'9',		0,			0x00}, // 0x61
	{'0',		0,			0x00}, // 0x62
	{'.',		0,			0x00}, // 0x63
	{0,			0,			0x00}, // 0x64
	{0,			0,			0x00}, // 0x65
	{0,			0,			0x00}, // 0x66
	{'=',		'=',		0x00}, // 0x67
};

USBHost::Keyboard::Keyboard() : Tickable(100), modifier_{0}, cntHold_{0}
{
	::memset(keycode_, 0x00, sizeof(keycode_));
}

void USBHost::Keyboard::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report)
{
	modifier_ = report.modifier;
	::memcpy(keycode_, report.keycode, sizeof(keycode_));
	if (keycode_[0]) {
		fifoKeyData_.WriteData(CreateKeyData(keycode_[0], modifier_));
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
	if (keycode_[0]) {
		fifoKeyData_.WriteData(CreateKeyData(keycode_[0], modifier_));
	}
}

KeyData USBHost::Keyboard::CreateKeyData(uint8_t keycode, uint8_t modifier)
{
	bool isShiftDown = !!(modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT));
	bool isCtrlDown = !!(modifier & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL));
	char charCode = (keycode >= 128)? 0 : convTbl_101Keyboard[keycode][isCtrlDown? 2 : isShiftDown? 1 : 0];
	return (charCode == 0)? KeyData(keycode, true, modifier) : KeyData(charCode, false, modifier);
}

//------------------------------------------------------------------------------
// USBHost::Mouse
//------------------------------------------------------------------------------
USBHost::Mouse::Mouse()
{
}

void USBHost::Mouse::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report)
{
	report.buttons;
	report.pan;
	report.wheel;
	report.x;
	report.y;
}

}
