//==============================================================================
// USBHost.cpp
//==============================================================================
#include "jxglib/USBHost.h"

namespace jxglib {

//------------------------------------------------------------------------------
// USBHost
//------------------------------------------------------------------------------
USBHost USBHost::Instance;

USBHost::USBHost() : pEventHandler_{nullptr}
{
}

void USBHost::Initialize(uint8_t rhport, EventHandler* pEventHandler)
{
	::tuh_init(rhport);
	Instance.pEventHandler_ = pEventHandler;
}

void USBHost::OnTick()
{
	::tuh_task();
}

extern "C" {

void tuh_mount_cb(uint8_t devAddr)
{
	auto pEventHandler = USBHost::Instance.GetEventHandler();
	if (pEventHandler) pEventHandler->OnMount(devAddr);
}

void tuh_umount_cb(uint8_t devAddr)
{
	auto pEventHandler = USBHost::Instance.GetEventHandler();
	if (pEventHandler) pEventHandler->OnUmount(devAddr);
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
const USBHost::Keyboard::ConvEntry USBHost::Keyboard::convEntryTbl_101Keyboard[256] = {
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
	{VK_RETURN,		'\r',		'\r',		0x00}, // 0x28
	{VK_ESCAPE,		'\x1b',		'\x1b',		0x00}, // 0x29
	{VK_BACK,		'\b',		'\b',		0x00}, // 0x2a
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
	{VK_RETURN,		'\r',		'\r',		0x00}, // 0x58
	{VK_NUMPAD1,	'1',		'1',		0x00}, // 0x59
	{VK_NUMPAD2,	'2',		'2',		0x00}, // 0x5a
	{VK_NUMPAD3,	'3',		'3',		0x00}, // 0x5b
	{VK_NUMPAD4,	'4',		'4',		0x00}, // 0x5c
	{VK_NUMPAD5,	'5',		'5',		0x00}, // 0x5d
	{VK_NUMPAD6,	'6',		'6',		0x00}, // 0x5e
	{VK_NUMPAD7,	'7',		'7',		0x00}, // 0x5f
	{VK_NUMPAD8,	'8',		'8',		0x00}, // 0x60
	{VK_NUMPAD9,	'9',		'9',		0x00}, // 0x61
	{VK_NUMPAD0,	'0',		'0',		0x00}, // 0x62
	{VK_DECIMAL,	'.',		'.',		0x00}, // 0x63
	{0,				0,			0,			0x00}, // 0x64
	{0,				0,			0,			0x00}, // 0x65
	{0,				0,			0,			0x00}, // 0x66
	{0,				'=',		'=',		0x00}, // 0x67
	{0,				0,			0,			0x00}, // 0x68
	{0,				0,			0,			0x00}, // 0x69
	{0,				0,			0,			0x00}, // 0x6a
	{0,				0,			0,			0x00}, // 0x6b
	{0,				0,			0,			0x00}, // 0x6c
	{0,				0,			0,			0x00}, // 0x6d
	{0,				0,			0,			0x00}, // 0x6e
	{0,				0,			0,			0x00}, // 0x6f
	{0,				0,			0,			0x00}, // 0x70
	{0,				0,			0,			0x00}, // 0x71
	{0,				0,			0,			0x00}, // 0x72
	{0,				0,			0,			0x00}, // 0x73
	{0,				0,			0,			0x00}, // 0x74
	{0,				0,			0,			0x00}, // 0x75
	{0,				0,			0,			0x00}, // 0x76
	{0,				0,			0,			0x00}, // 0x77
	{0,				0,			0,			0x00}, // 0x78
	{0,				0,			0,			0x00}, // 0x79
	{0,				0,			0,			0x00}, // 0x7a
	{0,				0,			0,			0x00}, // 0x7b
	{0,				0,			0,			0x00}, // 0x7c
	{0,				0,			0,			0x00}, // 0x7d
	{0,				0,			0,			0x00}, // 0x7e
	{0,				0,			0,			0x00}, // 0x7f
	{0,				0,			0,			0x00}, // 0x80
	{0,				0,			0,			0x00}, // 0x81
	{0,				0,			0,			0x00}, // 0x82
	{0,				0,			0,			0x00}, // 0x83
	{0,				0,			0,			0x00}, // 0x84
	{0,				0,			0,			0x00}, // 0x85
	{0,				0,			0,			0x00}, // 0x86
	{0,				0,			0,			0x00}, // 0x87
	{0,				0,			0,			0x00}, // 0x88
	{0,				0,			0,			0x00}, // 0x89
	{0,				0,			0,			0x00}, // 0x8a
	{0,				0,			0,			0x00}, // 0x8b
	{0,				0,			0,			0x00}, // 0x8c
	{0,				0,			0,			0x00}, // 0x8d
	{0,				0,			0,			0x00}, // 0x8e
	{0,				0,			0,			0x00}, // 0x8f
	{0,				0,			0,			0x00}, // 0x90
	{0,				0,			0,			0x00}, // 0x91
	{0,				0,			0,			0x00}, // 0x92
	{0,				0,			0,			0x00}, // 0x93
	{0,				0,			0,			0x00}, // 0x94
	{0,				0,			0,			0x00}, // 0x95
	{0,				0,			0,			0x00}, // 0x96
	{0,				0,			0,			0x00}, // 0x97
	{0,				0,			0,			0x00}, // 0x98
	{0,				0,			0,			0x00}, // 0x99
	{0,				0,			0,			0x00}, // 0x9a
	{0,				0,			0,			0x00}, // 0x9b
	{0,				0,			0,			0x00}, // 0x9c
	{0,				0,			0,			0x00}, // 0x9d
	{0,				0,			0,			0x00}, // 0x9e
	{0,				0,			0,			0x00}, // 0x9f
	{0,				0,			0,			0x00}, // 0xa0
	{0,				0,			0,			0x00}, // 0xa1
	{0,				0,			0,			0x00}, // 0xa2
	{0,				0,			0,			0x00}, // 0xa3
	{0,				0,			0,			0x00}, // 0xa4
	{0,				0,			0,			0x00}, // 0xa5
	{0,				0,			0,			0x00}, // 0xa6
	{0,				0,			0,			0x00}, // 0xa7
	{0,				0,			0,			0x00}, // 0xa8
	{0,				0,			0,			0x00}, // 0xa9
	{0,				0,			0,			0x00}, // 0xaa
	{0,				0,			0,			0x00}, // 0xab
	{0,				0,			0,			0x00}, // 0xac
	{0,				0,			0,			0x00}, // 0xad
	{0,				0,			0,			0x00}, // 0xae
	{0,				0,			0,			0x00}, // 0xaf
	{0,				0,			0,			0x00}, // 0xb0
	{0,				0,			0,			0x00}, // 0xb1
	{0,				0,			0,			0x00}, // 0xb2
	{0,				0,			0,			0x00}, // 0xb3
	{0,				0,			0,			0x00}, // 0xb4
	{0,				0,			0,			0x00}, // 0xb5
	{0,				0,			0,			0x00}, // 0xb6
	{0,				0,			0,			0x00}, // 0xb7
	{0,				0,			0,			0x00}, // 0xb8
	{0,				0,			0,			0x00}, // 0xb9
	{0,				0,			0,			0x00}, // 0xba
	{0,				0,			0,			0x00}, // 0xbb
	{0,				0,			0,			0x00}, // 0xbc
	{0,				0,			0,			0x00}, // 0xbd
	{0,				0,			0,			0x00}, // 0xbe
	{0,				0,			0,			0x00}, // 0xbf
	{0,				0,			0,			0x00}, // 0xc0
	{0,				0,			0,			0x00}, // 0xc1
	{0,				0,			0,			0x00}, // 0xc2
	{0,				0,			0,			0x00}, // 0xc3
	{0,				0,			0,			0x00}, // 0xc4
	{0,				0,			0,			0x00}, // 0xc5
	{0,				0,			0,			0x00}, // 0xc6
	{0,				0,			0,			0x00}, // 0xc7
	{0,				0,			0,			0x00}, // 0xc8
	{0,				0,			0,			0x00}, // 0xc9
	{0,				0,			0,			0x00}, // 0xca
	{0,				0,			0,			0x00}, // 0xcb
	{0,				0,			0,			0x00}, // 0xcc
	{0,				0,			0,			0x00}, // 0xcd
	{0,				0,			0,			0x00}, // 0xce
	{0,				0,			0,			0x00}, // 0xcf
	{0,				0,			0,			0x00}, // 0xd0
	{0,				0,			0,			0x00}, // 0xd1
	{0,				0,			0,			0x00}, // 0xd2
	{0,				0,			0,			0x00}, // 0xd3
	{0,				0,			0,			0x00}, // 0xd4
	{0,				0,			0,			0x00}, // 0xd5
	{0,				0,			0,			0x00}, // 0xd6
	{0,				0,			0,			0x00}, // 0xd7
	{0,				0,			0,			0x00}, // 0xd8
	{0,				0,			0,			0x00}, // 0xd9
	{0,				0,			0,			0x00}, // 0xda
	{0,				0,			0,			0x00}, // 0xdb
	{0,				0,			0,			0x00}, // 0xdc
	{0,				0,			0,			0x00}, // 0xdd
	{0,				0,			0,			0x00}, // 0xde
	{0,				0,			0,			0x00}, // 0xdf
	{0,				0,			0,			0x00}, // 0xe0
	{0,				0,			0,			0x00}, // 0xe1
	{0,				0,			0,			0x00}, // 0xe2
	{0,				0,			0,			0x00}, // 0xe3
	{0,				0,			0,			0x00}, // 0xe4
	{0,				0,			0,			0x00}, // 0xe5
	{0,				0,			0,			0x00}, // 0xe6
	{0,				0,			0,			0x00}, // 0xe7
	{0,				0,			0,			0x00}, // 0xe8
	{0,				0,			0,			0x00}, // 0xe9
	{0,				0,			0,			0x00}, // 0xea
	{0,				0,			0,			0x00}, // 0xeb
	{0,				0,			0,			0x00}, // 0xec
	{0,				0,			0,			0x00}, // 0xed
	{0,				0,			0,			0x00}, // 0xee
	{0,				0,			0,			0x00}, // 0xef
	{0,				0,			0,			0x00}, // 0xf0
	{0,				0,			0,			0x00}, // 0xf1
	{0,				0,			0,			0x00}, // 0xf2
	{0,				0,			0,			0x00}, // 0xf3
	{0,				0,			0,			0x00}, // 0xf4
	{0,				0,			0,			0x00}, // 0xf5
	{0,				0,			0,			0x00}, // 0xf6
	{0,				0,			0,			0x00}, // 0xf7
	{0,				0,			0,			0x00}, // 0xf8
	{0,				0,			0,			0x00}, // 0xf9
	{0,				0,			0,			0x00}, // 0xfa
	{0,				0,			0,			0x00}, // 0xfb
	{0,				0,			0,			0x00}, // 0xfc
	{0,				0,			0,			0x00}, // 0xfd
	{0,				0,			0,			0x00}, // 0xfe
	{0,				0,			0,			0x00}, // 0xff
};

const USBHost::Keyboard::ConvEntry USBHost::Keyboard::convEntryTbl_106Keyboard[256] = {
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
	{'2',			'2',		'\"',		0x00}, // 0x1f
	{'3',			'3',		'#',		0x00}, // 0x20
	{'4',			'4',		'$',		0x00}, // 0x21
	{'5',			'5',		'%',		0x00}, // 0x22
	{'6',			'6',		'&',		0x00}, // 0x23
	{'7',			'7',		'\'',		0x00}, // 0x24
	{'8',			'8',		'(',		0x00}, // 0x25
	{'9',			'9',		')',		0x00}, // 0x26
	{'0',			'0',		'0',		0x00}, // 0x27
	{VK_RETURN,		0,			0,			0x00}, // 0x28
	{VK_ESCAPE,		'\x1b',		'\x1b',		0x00}, // 0x29
	{VK_BACK,		0,			0,			0x00}, // 0x2a
	{VK_TAB,		'\t',		'\t',		0x00}, // 0x2b
	{VK_SPACE,		' ',		' ',		0x00}, // 0x2c
	{VK_OEM_MINUS,	'-',		'=',		0x00}, // 0x2d
	{VK_OEM_PLUS,	'^',		'~',		0x00}, // 0x2e
	{VK_OEM_4,		'@',		'`',		0x00}, // 0x2f
	{VK_OEM_6,		'[',		'{',		0x00}, // 0x30
	{VK_OEM_5,		'\\',		'|',		0x00}, // 0x31
	{VK_OEM_3,		']',		'}',		0x00}, // 0x32
	{VK_OEM_1,		';',		'+',		0x00}, // 0x33
	{VK_OEM_7,		':',		'*',		0x00}, // 0x34
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
	{0,				0,			0,			0x00}, // 0x68
	{0,				0,			0,			0x00}, // 0x69
	{0,				0,			0,			0x00}, // 0x6a
	{0,				0,			0,			0x00}, // 0x6b
	{0,				0,			0,			0x00}, // 0x6c
	{0,				0,			0,			0x00}, // 0x6d
	{0,				0,			0,			0x00}, // 0x6e
	{0,				0,			0,			0x00}, // 0x6f
	{0,				0,			0,			0x00}, // 0x70
	{0,				0,			0,			0x00}, // 0x71
	{0,				0,			0,			0x00}, // 0x72
	{0,				0,			0,			0x00}, // 0x73
	{0,				0,			0,			0x00}, // 0x74
	{0,				0,			0,			0x00}, // 0x75
	{0,				0,			0,			0x00}, // 0x76
	{0,				0,			0,			0x00}, // 0x77
	{0,				0,			0,			0x00}, // 0x78
	{0,				0,			0,			0x00}, // 0x79
	{0,				0,			0,			0x00}, // 0x7a
	{0,				0,			0,			0x00}, // 0x7b
	{0,				0,			0,			0x00}, // 0x7c
	{0,				0,			0,			0x00}, // 0x7d
	{0,				0,			0,			0x00}, // 0x7e
	{0,				0,			0,			0x00}, // 0x7f
	{0,				0,			0,			0x00}, // 0x80
	{0,				0,			0,			0x00}, // 0x81
	{0,				0,			0,			0x00}, // 0x82
	{0,				0,			0,			0x00}, // 0x83
	{0,				0,			0,			0x00}, // 0x84
	{0,				0,			0,			0x00}, // 0x85
	{0,				0,			0,			0x00}, // 0x86
	{VK_OEM_102,	'\\',		'_',		0x00}, // 0x87
	{0,				0,			0,			0x00}, // 0x88
	{VK_OEM_5,		'\\',		'|',		0x00}, // 0x89
	{0,				0,			0,			0x00}, // 0x8a
	{0,				0,			0,			0x00}, // 0x8b
	{0,				0,			0,			0x00}, // 0x8c
	{0,				0,			0,			0x00}, // 0x8d
	{0,				0,			0,			0x00}, // 0x8e
	{0,				0,			0,			0x00}, // 0x8f
	{0,				0,			0,			0x00}, // 0x90
	{0,				0,			0,			0x00}, // 0x91
	{0,				0,			0,			0x00}, // 0x92
	{0,				0,			0,			0x00}, // 0x93
	{0,				0,			0,			0x00}, // 0x94
	{0,				0,			0,			0x00}, // 0x95
	{0,				0,			0,			0x00}, // 0x96
	{0,				0,			0,			0x00}, // 0x97
	{0,				0,			0,			0x00}, // 0x98
	{0,				0,			0,			0x00}, // 0x99
	{0,				0,			0,			0x00}, // 0x9a
	{0,				0,			0,			0x00}, // 0x9b
	{0,				0,			0,			0x00}, // 0x9c
	{0,				0,			0,			0x00}, // 0x9d
	{0,				0,			0,			0x00}, // 0x9e
	{0,				0,			0,			0x00}, // 0x9f
	{0,				0,			0,			0x00}, // 0xa0
	{0,				0,			0,			0x00}, // 0xa1
	{0,				0,			0,			0x00}, // 0xa2
	{0,				0,			0,			0x00}, // 0xa3
	{0,				0,			0,			0x00}, // 0xa4
	{0,				0,			0,			0x00}, // 0xa5
	{0,				0,			0,			0x00}, // 0xa6
	{0,				0,			0,			0x00}, // 0xa7
	{0,				0,			0,			0x00}, // 0xa8
	{0,				0,			0,			0x00}, // 0xa9
	{0,				0,			0,			0x00}, // 0xaa
	{0,				0,			0,			0x00}, // 0xab
	{0,				0,			0,			0x00}, // 0xac
	{0,				0,			0,			0x00}, // 0xad
	{0,				0,			0,			0x00}, // 0xae
	{0,				0,			0,			0x00}, // 0xaf
	{0,				0,			0,			0x00}, // 0xb0
	{0,				0,			0,			0x00}, // 0xb1
	{0,				0,			0,			0x00}, // 0xb2
	{0,				0,			0,			0x00}, // 0xb3
	{0,				0,			0,			0x00}, // 0xb4
	{0,				0,			0,			0x00}, // 0xb5
	{0,				0,			0,			0x00}, // 0xb6
	{0,				0,			0,			0x00}, // 0xb7
	{0,				0,			0,			0x00}, // 0xb8
	{0,				0,			0,			0x00}, // 0xb9
	{0,				0,			0,			0x00}, // 0xba
	{0,				0,			0,			0x00}, // 0xbb
	{0,				0,			0,			0x00}, // 0xbc
	{0,				0,			0,			0x00}, // 0xbd
	{0,				0,			0,			0x00}, // 0xbe
	{0,				0,			0,			0x00}, // 0xbf
	{0,				0,			0,			0x00}, // 0xc0
	{0,				0,			0,			0x00}, // 0xc1
	{0,				0,			0,			0x00}, // 0xc2
	{0,				0,			0,			0x00}, // 0xc3
	{0,				0,			0,			0x00}, // 0xc4
	{0,				0,			0,			0x00}, // 0xc5
	{0,				0,			0,			0x00}, // 0xc6
	{0,				0,			0,			0x00}, // 0xc7
	{0,				0,			0,			0x00}, // 0xc8
	{0,				0,			0,			0x00}, // 0xc9
	{0,				0,			0,			0x00}, // 0xca
	{0,				0,			0,			0x00}, // 0xcb
	{0,				0,			0,			0x00}, // 0xcc
	{0,				0,			0,			0x00}, // 0xcd
	{0,				0,			0,			0x00}, // 0xce
	{0,				0,			0,			0x00}, // 0xcf
	{0,				0,			0,			0x00}, // 0xd0
	{0,				0,			0,			0x00}, // 0xd1
	{0,				0,			0,			0x00}, // 0xd2
	{0,				0,			0,			0x00}, // 0xd3
	{0,				0,			0,			0x00}, // 0xd4
	{0,				0,			0,			0x00}, // 0xd5
	{0,				0,			0,			0x00}, // 0xd6
	{0,				0,			0,			0x00}, // 0xd7
	{0,				0,			0,			0x00}, // 0xd8
	{0,				0,			0,			0x00}, // 0xd9
	{0,				0,			0,			0x00}, // 0xda
	{0,				0,			0,			0x00}, // 0xdb
	{0,				0,			0,			0x00}, // 0xdc
	{0,				0,			0,			0x00}, // 0xdd
	{0,				0,			0,			0x00}, // 0xde
	{0,				0,			0,			0x00}, // 0xdf
	{VK_LCONTROL,	0,			0,			0x00}, // 0xe0
	{VK_LSHIFT,		0,			0,			0x00}, // 0xe1
	{VK_LMENU,		0,			0,			0x00}, // 0xe2
	{VK_LWIN,		0,			0,			0x00}, // 0xe3
	{VK_RCONTROL,	0,			0,			0x00}, // 0xe4
	{VK_RSHIFT,		0,			0,			0x00}, // 0xe5
	{VK_RMENU,		0,			0,			0x00}, // 0xe6
	{VK_RWIN,		0,			0,			0x00}, // 0xe7
	{0,				0,			0,			0x00}, // 0xe8
	{0,				0,			0,			0x00}, // 0xe9
	{0,				0,			0,			0x00}, // 0xea
	{0,				0,			0,			0x00}, // 0xeb
	{0,				0,			0,			0x00}, // 0xec
	{0,				0,			0,			0x00}, // 0xed
	{0,				0,			0,			0x00}, // 0xee
	{0,				0,			0,			0x00}, // 0xef
	{0,				0,			0,			0x00}, // 0xf0
	{0,				0,			0,			0x00}, // 0xf1
	{0,				0,			0,			0x00}, // 0xf2
	{0,				0,			0,			0x00}, // 0xf3
	{0,				0,			0,			0x00}, // 0xf4
	{0,				0,			0,			0x00}, // 0xf5
	{0,				0,			0,			0x00}, // 0xf6
	{0,				0,			0,			0x00}, // 0xf7
	{0,				0,			0,			0x00}, // 0xf8
	{0,				0,			0,			0x00}, // 0xf9
	{0,				0,			0,			0x00}, // 0xfa
	{0,				0,			0,			0x00}, // 0xfb
	{0,				0,			0,			0x00}, // 0xfc
	{0,				0,			0,			0x00}, // 0xfd
	{0,				0,			0,			0x00}, // 0xfe
	{0,				0,			0,			0x00}, // 0xff
};

const uint8_t USBHost::Keyboard::virtualKeyCodeTbl[256] = {
	0,				// 0x00
	0,				// 0x01
	0,				// 0x02
	0,				// 0x03
	'A',			// 0x04
	'B',			// 0x05
	'C',			// 0x06
	'D',			// 0x07
	'E',			// 0x08
	'F',			// 0x09
	'G',			// 0x0a
	'H',			// 0x0b
	'I',			// 0x0c
	'J',			// 0x0d
	'K',			// 0x0e
	'L',			// 0x0f
	'M',			// 0x10
	'N',			// 0x11
	'O',			// 0x12
	'P',			// 0x13
	'Q',			// 0x14
	'R',			// 0x15
	'S',			// 0x16
	'T',			// 0x17
	'U',			// 0x18
	'V',			// 0x19
	'W',			// 0x1a
	'X',			// 0x1b
	'Y',			// 0x1c
	'Z',			// 0x1d
	'1',			// 0x1e
	'2',			// 0x1f
	'3',			// 0x20
	'4',			// 0x21
	'5',			// 0x22
	'6',			// 0x23
	'7',			// 0x24
	'8',			// 0x25
	'9',			// 0x26
	'0',			// 0x27
	VK_RETURN,		// 0x28
	VK_ESCAPE,		// 0x29
	VK_BACK,		// 0x2a
	VK_TAB,			// 0x2b
	VK_SPACE,		// 0x2c
	VK_OEM_MINUS,	// 0x2d
	VK_OEM_PLUS,	// 0x2e
	VK_OEM_4,		// 0x2f
	VK_OEM_6,		// 0x30
	VK_OEM_5,		// 0x31
	VK_OEM_3,		// 0x32
	VK_OEM_1,		// 0x33
	VK_OEM_7,		// 0x34
	VK_OEM_3,		// 0x35
	VK_OEM_COMMA,	// 0x36
	VK_OEM_PERIOD,	// 0x37
	VK_OEM_2,		// 0x38
	VK_CAPITAL,		// 0x39
	VK_F1,			// 0x3a
	VK_F2,			// 0x3b
	VK_F3,			// 0x3c
	VK_F4,			// 0x3d
	VK_F5,			// 0x3e
	VK_F6,			// 0x3f
	VK_F7,			// 0x40
	VK_F8,			// 0x41
	VK_F9,			// 0x42
	VK_F10,			// 0x43
	VK_F11,			// 0x44
	VK_F12,			// 0x45
	VK_PRINT,		// 0x46
	VK_SCROLL,		// 0x47
	VK_PAUSE,		// 0x48
	VK_INSERT,		// 0x49
	VK_HOME,		// 0x4a
	VK_PRIOR,		// 0x4b
	VK_DELETE,		// 0x4c
	VK_END,			// 0x4d
	VK_NEXT,		// 0x4e
	VK_RIGHT,		// 0x4f
	VK_LEFT,		// 0x50
	VK_DOWN,		// 0x51
	VK_UP,			// 0x52
	VK_NUMLOCK,		// 0x53
	VK_DIVIDE,		// 0x54
	VK_MULTIPLY,	// 0x55
	VK_SUBTRACT,	// 0x56
	VK_ADD,			// 0x57
	VK_RETURN,		// 0x58
	VK_NUMPAD1,		// 0x59
	VK_NUMPAD2,		// 0x5a
	VK_NUMPAD3,		// 0x5b
	VK_NUMPAD4,		// 0x5c
	VK_NUMPAD5,		// 0x5d
	VK_NUMPAD6,		// 0x5e
	VK_NUMPAD7,		// 0x5f
	VK_NUMPAD8,		// 0x60
	VK_NUMPAD9,		// 0x61
	VK_NUMPAD0,		// 0x62
	VK_DECIMAL,		// 0x63
	0,				// 0x64
	0,				// 0x65
	0,				// 0x66
	0,				// 0x67
	0,				// 0x68
	0,				// 0x69
	0,				// 0x6a
	0,				// 0x6b
	0,				// 0x6c
	0,				// 0x6d
	0,				// 0x6e
	0,				// 0x6f
	0,				// 0x70
	0,				// 0x71
	0,				// 0x72
	0,				// 0x73
	0,				// 0x74
	0,				// 0x75
	0,				// 0x76
	0,				// 0x77
	0,				// 0x78
	0,				// 0x79
	0,				// 0x7a
	0,				// 0x7b
	0,				// 0x7c
	0,				// 0x7d
	0,				// 0x7e
	0,				// 0x7f
	0,				// 0x80
	0,				// 0x81
	0,				// 0x82
	0,				// 0x83
	0,				// 0x84
	0,				// 0x85
	0,				// 0x86
	VK_OEM_102,		// 0x87
	0,				// 0x88
	VK_OEM_5,		// 0x89
	0,				// 0x8a
	0,				// 0x8b
	0,				// 0x8c
	0,				// 0x8d
	0,				// 0x8e
	0,				// 0x8f
	0,				// 0x90
	0,				// 0x91
	0,				// 0x92
	0,				// 0x93
	0,				// 0x94
	0,				// 0x95
	0,				// 0x96
	0,				// 0x97
	0,				// 0x98
	0,				// 0x99
	0,				// 0x9a
	0,				// 0x9b
	0,				// 0x9c
	0,				// 0x9d
	0,				// 0x9e
	0,				// 0x9f
	0,				// 0xa0
	0,				// 0xa1
	0,				// 0xa2
	0,				// 0xa3
	0,				// 0xa4
	0,				// 0xa5
	0,				// 0xa6
	0,				// 0xa7
	0,				// 0xa8
	0,				// 0xa9
	0,				// 0xaa
	0,				// 0xab
	0,				// 0xac
	0,				// 0xad
	0,				// 0xae
	0,				// 0xaf
	0,				// 0xb0
	0,				// 0xb1
	0,				// 0xb2
	0,				// 0xb3
	0,				// 0xb4
	0,				// 0xb5
	0,				// 0xb6
	0,				// 0xb7
	0,				// 0xb8
	0,				// 0xb9
	0,				// 0xba
	0,				// 0xbb
	0,				// 0xbc
	0,				// 0xbd
	0,				// 0xbe
	0,				// 0xbf
	0,				// 0xc0
	0,				// 0xc1
	0,				// 0xc2
	0,				// 0xc3
	0,				// 0xc4
	0,				// 0xc5
	0,				// 0xc6
	0,				// 0xc7
	0,				// 0xc8
	0,				// 0xc9
	0,				// 0xca
	0,				// 0xcb
	0,				// 0xcc
	0,				// 0xcd
	0,				// 0xce
	0,				// 0xcf
	0,				// 0xd0
	0,				// 0xd1
	0,				// 0xd2
	0,				// 0xd3
	0,				// 0xd4
	0,				// 0xd5
	0,				// 0xd6
	0,				// 0xd7
	0,				// 0xd8
	0,				// 0xd9
	0,				// 0xda
	0,				// 0xdb
	0,				// 0xdc
	0,				// 0xdd
	0,				// 0xde
	0,				// 0xdf
	VK_LCONTROL,	// 0xe0
	VK_LSHIFT,		// 0xe1
	VK_LMENU,		// 0xe2
	VK_LWIN,		// 0xe3
	VK_RCONTROL,	// 0xe4
	VK_RSHIFT,		// 0xe5
	VK_RMENU,		// 0xe6
	VK_RWIN,		// 0xe7
	0,				// 0xe8
	0,				// 0xe9
	0,				// 0xea
	0,				// 0xeb
	0,				// 0xec
	0,				// 0xed
	0,				// 0xee
	0,				// 0xef
	0,				// 0xf0
	0,				// 0xf1
	0,				// 0xf2
	0,				// 0xf3
	0,				// 0xf4
	0,				// 0xf5
	0,				// 0xf6
	0,				// 0xf7
	0,				// 0xf8
	0,				// 0xf9
	0,				// 0xfa
	0,				// 0xfb
	0,				// 0xfc
	0,				// 0xfd
	0,				// 0xfe
	0,				// 0xff
};

uint8_t tbl[256][2] = {
{0,		0 },	// 0x00			
{' ',	' '},	// 0x01: VK_LBUTTON		
{' ',	' '},	// 0x02: VK_RBUTTON		
{' ',	' '},	// 0x03: VK_CANCEL			
{' ',	' '},	// 0x04: VK_MBUTTON		
{' ',	' '},	// 0x05: VK_XBUTTON1		
{' ',	' '},	// 0x06: VK_XBUTTON2		
{0,		0 },	// 0x07			
{' ',	' '},	// 0x08: VK_BACK			
{' ',	' '},	// 0x09: VK_TAB			
{0,		0 },	// 0x0a			
{0,		0 },	// 0x0b
{' ',	' '},	// 0x0c: VK_CLEAR
{' ',	' '},	// 0x0d: VK_RETURN
{' ',	' '},	// 0x10: VK_SHIFT
{' ',	' '},	// 0x11: VK_CONTROL
{' ',	' '},	// 0x12: VK_MENU
{' ',	' '},	// 0x13: VK_PAUSE
{' ',	' '},	// 0x14: VK_CAPITAL
{' ',	' '},	// 0x15: VK_KANA
{' ',	' '},	// 0x15: VK_HANGUL
{' ',	' '},	// 0x16: VK_IME_ON
{' ',	' '},	// 0x17: VK_JUNJA
{' ',	' '},	// 0x18: VK_FINAL
{' ',	' '},	// 0x19: VK_KANJI
{' ',	' '},	// 0x1b: VK_ESCAPE
{' ',	' '},	// 0x1c: VK_CONVERT
{' ',	' '},	// 0x1d: VK_NONCONVERT
{' ',	' '},	// 0x1e: VK_ACCEPT
{' ',	' '},	// 0x1f: VK_MODECHANGE
{' ',	' '},	// 0x20: VK_SPACE
{' ',	' '},	// 0x21: VK_PRIOR
{' ',	' '},	// 0x22: VK_NEXT
{' ',	' '},	// 0x23: VK_END
{' ',	' '},	// 0x24: VK_HOME
{' ',	' '},	// 0x25: VK_LEFT
{' ',	' '},	// 0x26: VK_UP
{' ',	' '},	// 0x27: VK_RIGHT
{' ',	' '},	// 0x28: VK_DOWN
{' ',	' '},	// 0x29: VK_SELECT
{' ',	' '},	// 0x2a: VK_PRINT
{' ',	' '},	// 0x2b: VK_EXECUTE
{' ',	' '},	// 0x2c: VK_SNAPSHOT
{' ',	' '},	// 0x2d: VK_INSERT
{' ',	' '},	// 0x2e: VK_DELETE
{' ',	' '},	// 0x2f: VK_HELP
{' ',	' '},	// 0x5b: VK_LWIN
{' ',	' '},	// 0x5c: VK_RWIN
{' ',	' '},	// 0x5d: VK_APPS
{' ',	' '},	// 0x5f: VK_SLEEP
{' ',	' '},	// 0x60: VK_NUMPAD0
{' ',	' '},	// 0x61: VK_NUMPAD1
{' ',	' '},	// 0x62: VK_NUMPAD2
{' ',	' '},	// 0x63: VK_NUMPAD3
{' ',	' '},	// 0x64: VK_NUMPAD4
{' ',	' '},	// 0x65: VK_NUMPAD5
{' ',	' '},	// 0x66: VK_NUMPAD6
{' ',	' '},	// 0x67: VK_NUMPAD7
{' ',	' '},	// 0x68: VK_NUMPAD8
{' ',	' '},	// 0x69: VK_NUMPAD9
{' ',	' '},	// 0x6a: VK_MULTIPLY
{' ',	' '},	// 0x6b: VK_ADD
{' ',	' '},	// 0x6c: VK_SEPARATOR
{' ',	' '},	// 0x6d: VK_SUBTRACT
{' ',	' '},	// 0x6e: VK_DECIMAL
{' ',	' '},	// 0x6f: VK_DIVIDE
{' ',	' '},	// 0x70: VK_F1
{' ',	' '},	// 0x71: VK_F2
{' ',	' '},	// 0x72: VK_F3
{' ',	' '},	// 0x73: VK_F4
{' ',	' '},	// 0x74: VK_F5
{' ',	' '},	// 0x75: VK_F6
{' ',	' '},	// 0x76: VK_F7
{' ',	' '},	// 0x77: VK_F8
{' ',	' '},	// 0x78: VK_F9
{' ',	' '},	// 0x79: VK_F10
{' ',	' '},	// 0x7a: VK_F11
{' ',	' '},	// 0x7b: VK_F12
{' ',	' '},	// 0x7c: VK_F13
{' ',	' '},	// 0x7d: VK_F14
{' ',	' '},	// 0x7e: VK_F15
{' ',	' '},	// 0x7f: VK_F16
{' ',	' '},	// 0x80: VK_F17
{' ',	' '},	// 0x81: VK_F18
{' ',	' '},	// 0x82: VK_F19
{' ',	' '},	// 0x83: VK_F20
{' ',	' '},	// 0x84: VK_F21
{' ',	' '},	// 0x85: VK_F22
{' ',	' '},	// 0x86: VK_F23
{' ',	' '},	// 0x87: VK_F24
{' ',	' '},	// 0x90: VK_NUMLOCK
{' ',	' '},	// 0x91: VK_SCROLL
{' ',	' '},	// 0xa0: VK_LSHIFT
{' ',	' '},	// 0xa1: VK_RSHIFT
{' ',	' '},	// 0xa2: VK_LCONTROL
{' ',	' '},	// 0xa3: VK_RCONTROL
{' ',	' '},	// 0xa4: VK_LMENU
{' ',	' '},	// 0xa5: VK_RMENU
{' ',	' '},	// 0xa6: VK_BROWSER_BACK
{' ',	' '},	// 0xa7: VK_BROWSER_FORWARD
{' ',	' '},	// 0xa8: VK_BROWSER_REFRESH
{' ',	' '},	// 0xa9: VK_BROWSER_STOP
{' ',	' '},	// 0xaa: VK_BROWSER_SEARCH
{' ',	' '},	// 0xab: VK_BROWSER_FAVORITES
{' ',	' '},	// 0xac: VK_BROWSER_HOME
{' ',	' '},	// 0xad: VK_VOLUME_MUTE
{' ',	' '},	// 0xae: VK_VOLUME_DOWN
{' ',	' '},	// 0xaf: VK_VOLUME_UP
{' ',	' '},	// 0xb0: VK_MEDIA_NEXT_TRACK
{' ',	' '},	// 0xb1: VK_MEDIA_PREV_TRACK
{' ',	' '},	// 0xb2: VK_MEDIA_STOP
{' ',	' '},	// 0xb3: VK_MEDIA_PLAY_PAUSE
{' ',	' '},	// 0xb4: VK_LAUNCH_MAIL
{' ',	' '},	// 0xb5: VK_LAUNCH_MEDIA_SELECT
{' ',	' '},	// 0xb6: VK_LAUNCH_APP1
{' ',	' '},	// 0xb7: VK_LAUNCH_APP2
{' ',	' '},	// 0xba: VK_OEM_1
{' ',	' '},	// 0xbb: VK_OEM_PLUS
{' ',	' '},	// 0xbc: VK_OEM_COMMA
{' ',	' '},	// 0xbd: VK_OEM_MINUS
{' ',	' '},	// 0xbe: VK_OEM_PERIOD
{' ',	' '},	// 0xbf: VK_OEM_2
{' ',	' '},	// 0xc0: VK_OEM_3
{' ',	' '},	// 0xdb: VK_OEM_4
{' ',	' '},	// 0xdc: VK_OEM_5
{' ',	' '},	// 0xdd: VK_OEM_6
{' ',	' '},	// 0xde: VK_OEM_7
{' ',	' '},	// 0xdf: VK_OEM_8
{' ',	' '},	// 0xe2: VK_OEM_102
{' ',	' '},	// 0xe5: VK_PROCESSKEY
{' ',	' '},	// 0xe7: VK_PACKET
{' ',	' '},	// 0xf6: VK_ATTN
{' ',	' '},	// 0xf7: VK_CRSEL
{' ',	' '},	// 0xf8: VK_EXSEL
{' ',	' '},	// 0xf9: VK_EREOF
{' ',	' '},	// 0xfa: VK_PLAY
{' ',	' '},	// 0xfb: VK_ZOOM
{' ',	' '},	// 0xfc: VK_NONAME
{' ',	' '},	// 0xfd: VK_PA1
{' ',	' '},	// 0xfe: VK_OEM_CLEAR
};

USBHost::Keyboard::Keyboard() : capsLockAsCtrlFlag_{false},
	repeat_{keycode: 0, modifier: 0, consumedFlag: true, msecDelay: 300, msecRate: 100}
{
	::memset(&reportCaptured_, 0x00, sizeof(reportCaptured_));
	Suspend();
}

USBHost::Keyboard& USBHost::Keyboard::SetRepeatTime(uint32_t msecDelay, uint32_t msecRate)
{
	repeat_.msecDelay = msecDelay;
	repeat_.msecRate = msecRate;
	return *this;
}

void USBHost::Keyboard::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report)
{
	reportCaptured_ = report;
	if (capsLockAsCtrlFlag_) {
		for (int i = 0; i < count_of(reportCaptured_.keycode); i++) {
			uint8_t keycode = reportCaptured_.keycode[i];
			if (keycode == HID_KEY_CAPS_LOCK) {
				::memmove(&reportCaptured_.keycode[i], &reportCaptured_.keycode[i + 1],
												count_of(reportCaptured_.keycode) - i - 1);
				reportCaptured_.keycode[count_of(reportCaptured_.keycode) - 1] = 0;
				reportCaptured_.modifier |= KEYBOARD_MODIFIER_LEFTCTRL;
				break;
			}
		}
	}
	if (reportCaptured_.keycode[0] == 0) {
		repeat_.keycode = 0;
		repeat_.modifier = 0;
		repeat_.consumedFlag = true;
		return;
	}
	for (int i = 0; i < count_of(reportCaptured_.keycode); i++) {
		uint8_t keycode = reportCaptured_.keycode[i];
		if (repeat_.keycode != keycode) {
			repeat_.keycode = keycode;
			repeat_.modifier = reportCaptured_.modifier;
			repeat_.consumedFlag = false;
			ResetTick(repeat_.msecDelay);
			Resume();
			break;
		}
	}
}

bool USBHost::Keyboard::GetKeyDataNB(KeyData* pKeyData)
{
	if (repeat_.consumedFlag) return false;
	*pKeyData = CreateKeyData(repeat_.keycode, repeat_.modifier);
	repeat_.consumedFlag = true;
	return pKeyData->IsValid();
}

int USBHost::Keyboard::SenseKeyData(KeyData keyDataTbl[], int nKeysMax)
{
	int nKeys = 0;
	for (int i = 0; i < ChooseMin(static_cast<int>(count_of(reportCaptured_.keycode)), nKeysMax); i++) {
		uint8_t keycode = reportCaptured_.keycode[i];
		if (keycode) {
			KeyData keyData = CreateKeyData(keycode, reportCaptured_.modifier);
			if (keyData.IsValid()) keyDataTbl[nKeys++] = keyData;
		}
	}
	return nKeys;
}

void USBHost::Keyboard::OnTick()
{
	for (int i = 0; i < count_of(reportCaptured_.keycode); i++) {
		uint8_t keycode = reportCaptured_.keycode[i];
		if (repeat_.keycode == keycode) {
			repeat_.consumedFlag = false;
			if (GetTick() != repeat_.msecRate) SetTick(repeat_.msecRate);
			return;
		}
	}
	Suspend();
}

KeyData USBHost::Keyboard::CreateKeyData(uint8_t keycode, uint8_t modifier)
{
	bool isShiftDown = !!(modifier & (KEYBOARD_MODIFIER_LEFTSHIFT | KEYBOARD_MODIFIER_RIGHTSHIFT));
	bool isCtrlDown = !!(modifier & (KEYBOARD_MODIFIER_LEFTCTRL | KEYBOARD_MODIFIER_RIGHTCTRL));
	const ConvEntry& convEntry = convEntryTbl_106Keyboard[keycode];
	char charCode = isCtrlDown? convEntry.charCodeCtrl : isShiftDown? convEntry.charCodeShift : convEntry.charCode;
	return (charCode == 0)? KeyData(convEntry.keyCode, true, modifier) : KeyData(charCode, false, modifier);
}

//------------------------------------------------------------------------------
// USBHost::Mouse
//------------------------------------------------------------------------------
USBHost::Mouse::Mouse() : sensibility_{.6}
{
	SetStage({0, 0, 320, 240});
}

USBHost::Mouse& USBHost::Mouse::SetStage(const Rect& rcStage)
{
	rcStage_ = rcStage;
	UpdateStage();
	return *this;
}

USBHost::Mouse& USBHost::Mouse::SetSensibility(float sensibility)
{
	sensibility_ = sensibility;
	UpdateStage();
	return *this;
}

void USBHost::Mouse::UpdateStage()
{
	rcStageRaw_.width = static_cast<int>(rcStage_.width / sensibility_);
	rcStageRaw_.height = static_cast<int>(rcStage_.height / sensibility_);
	xRaw_ = rcStageRaw_.width / 2, yRaw_ = rcStageRaw_.height / 2;
	status_.SetPoint(CalcPoint());	
}

Point USBHost::Mouse::CalcPoint() const
{
	return Point(xRaw_ * (rcStage_.width - 1) / (rcStageRaw_.width - 1) + rcStage_.x,
			yRaw_ * (rcStage_.height - 1) / (rcStageRaw_.height - 1) + rcStage_.y);
}

void USBHost::Mouse::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report)
{
	int xDiff = report.x, yDiff = report.y;
	xRaw_ = ChooseMin(ChooseMax(xRaw_ + xDiff, 0), rcStageRaw_.width - 1);
	yRaw_ = ChooseMin(ChooseMax(yRaw_ + yDiff, 0), rcStageRaw_.height - 1);
	status_.Update(CalcPoint(), report.x, report.y, report.wheel, report.pan, report.buttons);
}


}
