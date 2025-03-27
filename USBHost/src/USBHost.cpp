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

class Item {
};

// 6.2.2.4 Main Items
class Item_Input : public Item {
public:
	static const uint8_t Type = 0x80;
};

class Item_Output : public Item {
public:
	static const uint8_t Type = 0x90;
};

class Item_Feature : public Item {
public:
	static const uint8_t Type = 0xb0;
};

class Item_Collection : public Item {
public:
	static const uint8_t Type = 0xa0;
};

class Item_EndCollection : public Item {
public:
	static const uint8_t Type = 0xc0;
};

// 6.2.2.7 Global Items
class Item_UsagePage : public Item {
public:
	static const uint8_t Type = 0x04;
};

class Item_LogicalMinimum : public Item {
public:
	static const uint8_t Type = 0x14;
};

class Item_LogicalMaximum : public Item {
public:
	static const uint8_t Type = 0x24;
};

class Item_PhysicalMinimum : public Item {
public:
	static const uint8_t Type = 0x34;
};

class Item_PhysicalMaximum : public Item {
public:
	static const uint8_t Type = 0x44;
};

class Item_UnitExponent : public Item {
public:
	static const uint8_t Type = 0x54;
};

class Item_Unit : public Item {
public:
	static const uint8_t Type = 0x64;
};

class Item_ReportSize : public Item {
public:
	static const uint8_t Type = 0x74;
};

class Item_ReortID : public Item {
public:
	static const uint8_t Type = 0x84;
};

class Item_ReportCount : public Item {
public:
	static const uint8_t Type = 0x94;
};

class Item_Push : public Item {
public:
	static const uint8_t Type = 0xa4;
};

class Item_Pop : public Item {
public:
	static const uint8_t Type = 0xb4;
};

// 6.2.2.8 Local Items
class Item_Usage : public Item {
public:
	static const uint8_t Type = 0x08;
};

class Item_UsageMinimum : public Item {
public:
	static const uint8_t Type = 0x18;
};

class Item_UsageMaximum : public Item {
public:
	static const uint8_t Type = 0x28;
};

class Item_DesignatorIndex : public Item {
public:
	static const uint8_t Type = 0x38;
};

class Item_DesignatorMinimum : public Item {
public:
	static const uint8_t Type = 0x48;
};

class Item_DesignatorMaximum : public Item {
public:
	static const uint8_t Type = 0x58;
};

class Item_StringIndex : public Item {
public:
	static const uint8_t Type = 0x78;
};

class Item_StringMaximum : public Item {
public:
	static const uint8_t Type = 0x88;
};

class Item_StringMinimum : public Item {
public:
	static const uint8_t Type = 0x98;
};

class Item_Delimeter : public Item {
public:
	static const uint8_t Type = 0xa8;
};

const char* GetItemTypeName(uint8_t itemType)
{
	static const struct {
		uint8_t itemType;
		const char* name;
	} tbl[] = {
		// 6.2.2.4 Main Items
		{ Item_Input::Type,				"Input"				},
		{ Item_Output::Type,			"Output"			},
		{ Item_Feature::Type,			"Feature"			},
		{ Item_Collection::Type,		"Collection"		},
		{ Item_EndCollection::Type,		"EndCollection"		},
		// 6.2.2.7 Global Items
		{ Item_UsagePage::Type,			"UsagePage"			},
		{ Item_LogicalMinimum::Type,	"LogicalMinimum"	},
		{ Item_LogicalMaximum::Type,	"LogicalMaximum"	},
		{ Item_PhysicalMinimum::Type,	"PhysicalMinimum"	},
		{ Item_PhysicalMaximum::Type,	"PhysicalMaximum"	},
		{ Item_UnitExponent::Type,		"UnitExponent"		},
		{ Item_Unit::Type,				"Unit"				},
		{ Item_ReportSize::Type,		"ReportSize"		},
		{ Item_ReortID::Type,			"ReortID"			},
		{ Item_ReportCount::Type,		"ReportCount"		},
		{ Item_Push::Type,				"Push"				},
		{ Item_Pop::Type,				"Pop"				},
		// 6.2.2.8 Local Items
		{ Item_Usage::Type,				"Usage"				},
		{ Item_UsageMinimum::Type,		"UsageMinimum"		},
		{ Item_UsageMaximum::Type,		"UsageMaximum"		},
		{ Item_DesignatorIndex::Type,	"DesignatorIndex"	},
		{ Item_DesignatorMinimum::Type,	"DesignatorMinimum"	},
		{ Item_DesignatorMaximum::Type,	"DesignatorMaximum"	},
		{ Item_StringIndex::Type,		"StringIndex"		},
		{ Item_StringMaximum::Type,		"StringMaximum"		},
		{ Item_StringMinimum::Type,		"StringMinimum"		},
		{ Item_Delimeter::Type,			"Delimeter"			},
	};
	for (int i = 0; i < count_of(tbl); i++) {
		if (tbl[i].itemType == itemType) return tbl[i].name;
	}
	return "unknown";
}

void PrintReportDescriptor(const uint8_t* descReport, uint16_t descLen)
{
	enum class Stat {
		Prefix, Data,
	} stat = Stat::Prefix;
	//Dump(descReport, descLen);
	int iData = 0;
	int nData = 0;
	uint8_t itemType = 0x00;
	uint8_t data[4];
	for (uint16_t i = 0; i < descLen; i++) {
		uint8_t src = descReport[i];
		switch (stat) {
		case Stat::Prefix: {
			itemType = src & 0xfc;
			nData = src & 0x03;
			if (nData == 3) nData = 4;
			if (nData == 0) {
				::printf("%02x:%s\n", itemType, GetItemTypeName(itemType));
			} else {
				iData = 0;
				stat = Stat::Data;
			}
			break;
		}
		case Stat::Data: {
			data[iData++] = src;
			if (iData == nData) {
				::printf("%02x:%s\n", itemType, GetItemTypeName(itemType));
				stat = Stat::Prefix;
			}
			break;
		}
		default: break;
		}
	}
}

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
	PrintReportDescriptor(descReport, descLen);
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
const USBHost::Keyboard::ReportIdToKeyCode USBHost::Keyboard::reportIdToKeyCodeTbl[256] = {
	{ 0,				0,				},	// 0x00
	{ 0,				0,				},	// 0x01
	{ 0,				0,				},	// 0x02
	{ 0,				0,				},	// 0x03
	{ 'A',				'A',			},	// 0x04
	{ 'B',				'B',			},	// 0x05
	{ 'C',				'C',			},	// 0x06
	{ 'D',				'D',			},	// 0x07
	{ 'E',				'E',			},	// 0x08
	{ 'F',				'F',			},	// 0x09
	{ 'G',				'G',			},	// 0x0a
	{ 'H',				'H',			},	// 0x0b
	{ 'I',				'I',			},	// 0x0c
	{ 'J',				'J',			},	// 0x0d
	{ 'K',				'K',			},	// 0x0e
	{ 'L',				'L',			},	// 0x0f
	{ 'M',				'M',			},	// 0x10
	{ 'N',				'N',			},	// 0x11
	{ 'O',				'O',			},	// 0x12
	{ 'P',				'P',			},	// 0x13
	{ 'Q',				'Q',			},	// 0x14
	{ 'R',				'R',			},	// 0x15
	{ 'S',				'S',			},	// 0x16
	{ 'T',				'T',			},	// 0x17
	{ 'U',				'U',			},	// 0x18
	{ 'V',				'V',			},	// 0x19
	{ 'W',				'W',			},	// 0x1a
	{ 'X',				'X',			},	// 0x1b
	{ 'Y',				'Y',			},	// 0x1c
	{ 'Z',				'Z',			},	// 0x1d
	{ '1',				'1',			},	// 0x1e
	{ '2',				'2',			},	// 0x1f
	{ '3',				'3',			},	// 0x20
	{ '4',				'4',			},	// 0x21
	{ '5',				'5',			},	// 0x22
	{ '6',				'6',			},	// 0x23
	{ '7',				'7',			},	// 0x24
	{ '8',				'8',			},	// 0x25
	{ '9',				'9',			},	// 0x26
	{ '0',				'0',			},	// 0x27
	{ VK_RETURN,		VK_RETURN,		},	// 0x28
	{ VK_ESCAPE,		VK_ESCAPE,		},	// 0x29
	{ VK_BACK,			VK_BACK,		},	// 0x2a
	{ VK_TAB,			VK_TAB,			},	// 0x2b
	{ VK_SPACE,			VK_SPACE,		},	// 0x2c
	{ VK_OEM_MINUS,		VK_OEM_MINUS,	},	// 0x2d
	{ VK_OEM_PLUS,		VK_OEM_7,		},	// 0x2e
	{ VK_OEM_4,			VK_OEM_3,		},	// 0x2f
	{ VK_OEM_6,			VK_OEM_4,		},	// 0x30
	{ VK_OEM_5,			VK_OEM_5,		},	// 0x31
	{ VK_OEM_6,			VK_OEM_6,		},	// 0x32
	{ VK_OEM_1,			VK_OEM_PLUS,	},	// 0x33
	{ VK_OEM_7,			VK_OEM_1,		},	// 0x34
	{ VK_OEM_3,			VK_KANJI,		},	// 0x35
	{ VK_OEM_COMMA,		VK_OEM_COMMA,	},	// 0x36
	{ VK_OEM_PERIOD,	VK_OEM_PERIOD,	},	// 0x37
	{ VK_OEM_2,			VK_OEM_2,		},	// 0x38
	{ VK_CAPITAL,		VK_CAPITAL,		},	// 0x39
	{ VK_F1,			VK_F1,			},	// 0x3a
	{ VK_F2,			VK_F2,			},	// 0x3b
	{ VK_F3,			VK_F3,			},	// 0x3c
	{ VK_F4,			VK_F4,			},	// 0x3d
	{ VK_F5,			VK_F5,			},	// 0x3e
	{ VK_F6,			VK_F6,			},	// 0x3f
	{ VK_F7,			VK_F7,			},	// 0x40
	{ VK_F8,			VK_F8,			},	// 0x41
	{ VK_F9,			VK_F9,			},	// 0x42
	{ VK_F10,			VK_F10,			},	// 0x43
	{ VK_F11,			VK_F11,			},	// 0x44
	{ VK_F12,			VK_F12,			},	// 0x45
	{ VK_PRINT,			VK_PRINT,		},	// 0x46
	{ VK_SCROLL,		VK_SCROLL,		},	// 0x47
	{ VK_PAUSE,			VK_PAUSE,		},	// 0x48
	{ VK_INSERT,		VK_INSERT,		},	// 0x49
	{ VK_HOME,			VK_HOME,		},	// 0x4a
	{ VK_PRIOR,			VK_PRIOR,		},	// 0x4b
	{ VK_DELETE,		VK_DELETE,		},	// 0x4c
	{ VK_END,			VK_END,			},	// 0x4d
	{ VK_NEXT,			VK_NEXT,		},	// 0x4e
	{ VK_RIGHT,			VK_RIGHT,		},	// 0x4f
	{ VK_LEFT,			VK_LEFT,		},	// 0x50
	{ VK_DOWN,			VK_DOWN,		},	// 0x51
	{ VK_UP,			VK_UP,			},	// 0x52
	{ VK_NUMLOCK,		VK_NUMLOCK,		},	// 0x53
	{ VK_DIVIDE,		VK_DIVIDE,		},	// 0x54
	{ VK_MULTIPLY,		VK_MULTIPLY,	},	// 0x55
	{ VK_SUBTRACT,		VK_SUBTRACT,	},	// 0x56
	{ VK_ADD,			VK_ADD,			},	// 0x57
	{ VK_RETURN,		VK_RETURN,		},	// 0x58
	{ VK_NUMPAD1,		VK_NUMPAD1,		},	// 0x59
	{ VK_NUMPAD2,		VK_NUMPAD2,		},	// 0x5a
	{ VK_NUMPAD3,		VK_NUMPAD3,		},	// 0x5b
	{ VK_NUMPAD4,		VK_NUMPAD4,		},	// 0x5c
	{ VK_NUMPAD5,		VK_NUMPAD5,		},	// 0x5d
	{ VK_NUMPAD6,		VK_NUMPAD6,		},	// 0x5e
	{ VK_NUMPAD7,		VK_NUMPAD7,		},	// 0x5f
	{ VK_NUMPAD8,		VK_NUMPAD8,		},	// 0x60
	{ VK_NUMPAD9,		VK_NUMPAD9,		},	// 0x61
	{ VK_NUMPAD0,		VK_NUMPAD0,		},	// 0x62
	{ VK_DECIMAL,		VK_DECIMAL,		},	// 0x63
	{ 0,				0,				},	// 0x64
	{ 0,				0,				},	// 0x65
	{ 0,				0,				},	// 0x66
	{ 0,				0,				},	// 0x67
	{ 0,				0,				},	// 0x68
	{ 0,				0,				},	// 0x69
	{ 0,				0,				},	// 0x6a
	{ 0,				0,				},	// 0x6b
	{ 0,				0,				},	// 0x6c
	{ 0,				0,				},	// 0x6d
	{ 0,				0,				},	// 0x6e
	{ 0,				0,				},	// 0x6f
	{ 0,				0,				},	// 0x70
	{ 0,				0,				},	// 0x71
	{ 0,				0,				},	// 0x72
	{ 0,				0,				},	// 0x73
	{ 0,				0,				},	// 0x74
	{ 0,				0,				},	// 0x75
	{ 0,				0,				},	// 0x76
	{ 0,				0,				},	// 0x77
	{ 0,				0,				},	// 0x78
	{ 0,				0,				},	// 0x79
	{ 0,				0,				},	// 0x7a
	{ 0,				0,				},	// 0x7b
	{ 0,				0,				},	// 0x7c
	{ 0,				0,				},	// 0x7d
	{ 0,				0,				},	// 0x7e
	{ 0,				0,				},	// 0x7f
	{ 0,				0,				},	// 0x80
	{ 0,				0,				},	// 0x81
	{ 0,				0,				},	// 0x82
	{ 0,				0,				},	// 0x83
	{ 0,				0,				},	// 0x84
	{ 0,				0,				},	// 0x85
	{ 0,				0,				},	// 0x86
	{ VK_OEM_102,		VK_OEM_102,		},	// 0x87
	{ 0,				0,				},	// 0x88
	{ VK_OEM_5,			VK_OEM_5,		},	// 0x89
	{ 0,				0,				},	// 0x8a
	{ 0,				0,				},	// 0x8b
	{ 0,				0,				},	// 0x8c
	{ 0,				0,				},	// 0x8d
	{ 0,				0,				},	// 0x8e
	{ 0,				0,				},	// 0x8f
	{ 0,				0,				},	// 0x90
	{ 0,				0,				},	// 0x91
	{ 0,				0,				},	// 0x92
	{ 0,				0,				},	// 0x93
	{ 0,				0,				},	// 0x94
	{ 0,				0,				},	// 0x95
	{ 0,				0,				},	// 0x96
	{ 0,				0,				},	// 0x97
	{ 0,				0,				},	// 0x98
	{ 0,				0,				},	// 0x99
	{ 0,				0,				},	// 0x9a
	{ 0,				0,				},	// 0x9b
	{ 0,				0,				},	// 0x9c
	{ 0,				0,				},	// 0x9d
	{ 0,				0,				},	// 0x9e
	{ 0,				0,				},	// 0x9f
	{ 0,				0,				},	// 0xa0
	{ 0,				0,				},	// 0xa1
	{ 0,				0,				},	// 0xa2
	{ 0,				0,				},	// 0xa3
	{ 0,				0,				},	// 0xa4
	{ 0,				0,				},	// 0xa5
	{ 0,				0,				},	// 0xa6
	{ 0,				0,				},	// 0xa7
	{ 0,				0,				},	// 0xa8
	{ 0,				0,				},	// 0xa9
	{ 0,				0,				},	// 0xaa
	{ 0,				0,				},	// 0xab
	{ 0,				0,				},	// 0xac
	{ 0,				0,				},	// 0xad
	{ 0,				0,				},	// 0xae
	{ 0,				0,				},	// 0xaf
	{ 0,				0,				},	// 0xb0
	{ 0,				0,				},	// 0xb1
	{ 0,				0,				},	// 0xb2
	{ 0,				0,				},	// 0xb3
	{ 0,				0,				},	// 0xb4
	{ 0,				0,				},	// 0xb5
	{ 0,				0,				},	// 0xb6
	{ 0,				0,				},	// 0xb7
	{ 0,				0,				},	// 0xb8
	{ 0,				0,				},	// 0xb9
	{ 0,				0,				},	// 0xba
	{ 0,				0,				},	// 0xbb
	{ 0,				0,				},	// 0xbc
	{ 0,				0,				},	// 0xbd
	{ 0,				0,				},	// 0xbe
	{ 0,				0,				},	// 0xbf
	{ 0,				0,				},	// 0xc0
	{ 0,				0,				},	// 0xc1
	{ 0,				0,				},	// 0xc2
	{ 0,				0,				},	// 0xc3
	{ 0,				0,				},	// 0xc4
	{ 0,				0,				},	// 0xc5
	{ 0,				0,				},	// 0xc6
	{ 0,				0,				},	// 0xc7
	{ 0,				0,				},	// 0xc8
	{ 0,				0,				},	// 0xc9
	{ 0,				0,				},	// 0xca
	{ 0,				0,				},	// 0xcb
	{ 0,				0,				},	// 0xcc
	{ 0,				0,				},	// 0xcd
	{ 0,				0,				},	// 0xce
	{ 0,				0,				},	// 0xcf
	{ 0,				0,				},	// 0xd0
	{ 0,				0,				},	// 0xd1
	{ 0,				0,				},	// 0xd2
	{ 0,				0,				},	// 0xd3
	{ 0,				0,				},	// 0xd4
	{ 0,				0,				},	// 0xd5
	{ 0,				0,				},	// 0xd6
	{ 0,				0,				},	// 0xd7
	{ 0,				0,				},	// 0xd8
	{ 0,				0,				},	// 0xd9
	{ 0,				0,				},	// 0xda
	{ 0,				0,				},	// 0xdb
	{ 0,				0,				},	// 0xdc
	{ 0,				0,				},	// 0xdd
	{ 0,				0,				},	// 0xde
	{ 0,				0,				},	// 0xdf
	{ VK_LCONTROL,		VK_LCONTROL,	},	// 0xe0
	{ VK_LSHIFT,		VK_LSHIFT,		},	// 0xe1
	{ VK_LMENU,			VK_LMENU,		},	// 0xe2
	{ VK_LWIN,			VK_LWIN,		},	// 0xe3
	{ VK_RCONTROL,		VK_RCONTROL,	},	// 0xe4
	{ VK_RSHIFT,		VK_RSHIFT,		},	// 0xe5
	{ VK_RMENU,			VK_RMENU,		},	// 0xe6
	{ VK_RWIN,			VK_RWIN,		},	// 0xe7
	{ 0,				0,				},	// 0xe8
	{ 0,				0,				},	// 0xe9
	{ 0,				0,				},	// 0xea
	{ 0,				0,				},	// 0xeb
	{ 0,				0,				},	// 0xec
	{ 0,				0,				},	// 0xed
	{ 0,				0,				},	// 0xee
	{ 0,				0,				},	// 0xef
	{ 0,				0,				},	// 0xf0
	{ 0,				0,				},	// 0xf1
	{ 0,				0,				},	// 0xf2
	{ 0,				0,				},	// 0xf3
	{ 0,				0,				},	// 0xf4
	{ 0,				0,				},	// 0xf5
	{ 0,				0,				},	// 0xf6
	{ 0,				0,				},	// 0xf7
	{ 0,				0,				},	// 0xf8
	{ 0,				0,				},	// 0xf9
	{ 0,				0,				},	// 0xfa
	{ 0,				0,				},	// 0xfb
	{ 0,				0,				},	// 0xfc
	{ 0,				0,				},	// 0xfd
	{ 0,				0,				},	// 0xfe
	{ 0,				0,				},	// 0xff
};

USBHost::Keyboard::Keyboard() : capsLockAsCtrlFlag_{false}
{
	::memset(&reportCaptured_, 0x00, sizeof(reportCaptured_));
}

Keyboard& USBHost::Keyboard::SetCapsLockAsCtrl(bool capsLockAsCtrlFlag)
{
	capsLockAsCtrlFlag_ = capsLockAsCtrlFlag;
	return *this;
}

void USBHost::Keyboard::OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report)
{
	::memset(&reportCaptured_, 0x00, sizeof(reportCaptured_));
	reportCaptured_.modifier = report.modifier;
	for (int iSrc = 0, iDst = 0; iSrc < count_of(report.keycode); iSrc++) {
		const ReportIdToKeyCode& reportIdToKeyCode = reportIdToKeyCodeTbl[report.keycode[iSrc]];
		uint8_t keyCode = GetKeyLayout().IsNonUS()? reportIdToKeyCode.keyCodeNonUS : reportIdToKeyCode.keyCodeUS;
		if (capsLockAsCtrlFlag_ && keyCode == VK_CAPITAL) {
			reportCaptured_.modifier |= KEYBOARD_MODIFIER_LEFTCTRL;
		} else {
			reportCaptured_.keyCodeTbl[iDst++] = keyCode;
		}
	}
	if (reportCaptured_.keyCodeTbl[0] == 0) {
		GetRepeat().Invalidate();
	} else {
		for (int i = 0; i < count_of(reportCaptured_.keyCodeTbl); i++) {
			if (GetRepeat().SignalFirst(reportCaptured_.keyCodeTbl[i], reportCaptured_.modifier)) break;
		}
	}
}

int USBHost::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax)
{
	int nKeys = 0;
	for (int i = 0; i < ChooseMin(static_cast<int>(count_of(reportCaptured_.keyCodeTbl)), nKeysMax); i++) {
		uint8_t keyCode = reportCaptured_.keyCodeTbl[i];
		if (keyCode) keyCodeTbl[nKeys++] = keyCode;
	}
	return nKeys;
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
