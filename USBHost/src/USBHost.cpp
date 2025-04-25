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
	::memset(hidTbl_, 0x00, sizeof(hidTbl_));
}

void USBHost::Initialize(uint8_t rhport, EventHandler* pEventHandler)
{
	::tuh_init(rhport);
	Instance.pEventHandler_ = pEventHandler;
}

void USBHost::MountHID(uint8_t devAddr, uint8_t iInstance, const uint8_t* descReport, uint16_t descLen)
{
	std::unique_ptr<HID::Application> pApplicationTop(HID::ParseReportDescriptor(descReport, descLen));
	if (pApplicationTop) {
		HID::Application* pApplication = pApplicationTop.get();
		HID* pHID = new HID(devAddr, iInstance, pApplicationTop.release());
		hidTbl_[iInstance] = pHID;
		pApplication->Print();
		for (HIDDriver* pHIDDriver = HIDDriver::pHIDDriverRegisteredTop; pHIDDriver; pHIDDriver = pHIDDriver->GetRegisteredListNext()) {
			if (pHIDDriver->GetUsage() == pApplication->GetUsage() && !pHIDDriver->IsMounted()) {
				pHIDDriver->AttachHID(pHID->Reference(), pApplication);
				pHID->AttachDriver(*pHIDDriver);
				break;
			}
		}
	}
}

void USBHost::UmountHID(uint8_t iInstance)
{
	HID* pHID = hidTbl_[iInstance];
	HIDDriver* pHIDDriver = pHID->GetHIDDriver();
	if (pHIDDriver) pHIDDriver->DetachHID();
	hidTbl_[iInstance] = nullptr;
	HID::Delete(pHID);
}

void USBHost::OnTick()
{
	::tuh_task();
}

extern "C" void tuh_mount_cb(uint8_t devAddr)
{
	auto pEventHandler = USBHost::Instance.GetEventHandler();
	if (pEventHandler) pEventHandler->OnMount(devAddr);
}

extern "C" void tuh_umount_cb(uint8_t devAddr)
{
	auto pEventHandler = USBHost::Instance.GetEventHandler();
	if (pEventHandler) pEventHandler->OnUmount(devAddr);
}

extern "C" void tuh_hid_mount_cb(uint8_t devAddr, uint8_t iInstance, const uint8_t* descReport, uint16_t descLen)
{
	::printf("tuh_hid_mount_cb(devAddr=%d, iInstance=%d)\n", devAddr, iInstance);
	USBHost::Instance.MountHID(devAddr, iInstance, descReport, descLen);
	::tuh_hid_receive_report(devAddr, iInstance);
}

extern "C" void tuh_hid_umount_cb(uint8_t devAddr, uint8_t iInstance)
{
	::printf("tuh_hid_umount_cb(%d, %d)\n", devAddr, iInstance);
	USBHost::Instance.UmountHID(iInstance);
}

extern "C" void tuh_hid_report_received_cb(uint8_t devAddr, uint8_t iInstance, const uint8_t* report, uint16_t len)
{
	USBHost::HID* pHID = USBHost::Instance.LookupHID(iInstance);
	if (pHID) pHID->OnReport(report, len);
	::tuh_hid_receive_report(devAddr, iInstance);
}

//------------------------------------------------------------------------------
// USBHost::HIDDriver
//------------------------------------------------------------------------------
USBHost::HIDDriver* USBHost::HIDDriver::pHIDDriverRegisteredTop = nullptr;

USBHost::HIDDriver::HIDDriver(uint32_t usage) : usage_{usage}, pApplication_{nullptr}
{
	if (pHIDDriverRegisteredTop) {
		pHIDDriverRegisteredTop->AppendRegisteredList(this);
	} else {
		pHIDDriverRegisteredTop = this;
	}
}

USBHost::HIDDriver::~HIDDriver()
{
	if (pHIDDriverRegisteredTop == this) {
		pHIDDriverRegisteredTop = GetRegisteredListNext();
	} else {
		for (HIDDriver* pHIDDriver = pHIDDriverRegisteredTop; pHIDDriver; pHIDDriver = pHIDDriver->GetRegisteredListNext()) {
			if (pHIDDriver->GetRegisteredListNext() == this) {
				pHIDDriver->SetRegisteredListNext(ReleaseRegisteredListNext());
			}
		}
	}
}

USBHost::HIDDriver* USBHost::HIDDriver::GetRegisteredListLast()
{
	HIDDriver* pHIDDriver = this;
	for ( ; pHIDDriver->GetRegisteredListNext(); pHIDDriver = pHIDDriver->GetRegisteredListNext()) ;
	return pHIDDriver;
}

//------------------------------------------------------------------------------
// USBHost::Keyboard
//------------------------------------------------------------------------------
const USBHost::Keyboard::UsageIdToKeyCode USBHost::Keyboard::usageIdToKeyCodeTbl[256] = {
	{ 0,				0,				},	// 0x00
	{ 0,				0,				},	// 0x01
	{ 0,				0,				},	// 0x02
	{ 0,				0,				},	// 0x03
	{ VK_A,				VK_A,			},	// 0x04
	{ VK_B,				VK_B,			},	// 0x05
	{ VK_C,				VK_C,			},	// 0x06
	{ VK_D,				VK_D,			},	// 0x07
	{ VK_E,				VK_E,			},	// 0x08
	{ VK_F,				VK_F,			},	// 0x09
	{ VK_G,				VK_G,			},	// 0x0a
	{ VK_H,				VK_H,			},	// 0x0b
	{ VK_I,				VK_I,			},	// 0x0c
	{ VK_J,				VK_J,			},	// 0x0d
	{ VK_K,				VK_K,			},	// 0x0e
	{ VK_L,				VK_L,			},	// 0x0f
	{ VK_M,				VK_M,			},	// 0x10
	{ VK_N,				VK_N,			},	// 0x11
	{ VK_O,				VK_O,			},	// 0x12
	{ VK_P,				VK_P,			},	// 0x13
	{ VK_Q,				VK_Q,			},	// 0x14
	{ VK_R,				VK_R,			},	// 0x15
	{ VK_S,				VK_S,			},	// 0x16
	{ VK_T,				VK_T,			},	// 0x17
	{ VK_U,				VK_U,			},	// 0x18
	{ VK_V,				VK_V,			},	// 0x19
	{ VK_W,				VK_W,			},	// 0x1a
	{ VK_X,				VK_X,			},	// 0x1b
	{ VK_Y,				VK_Y,			},	// 0x1c
	{ VK_Z,				VK_Z,			},	// 0x1d
	{ VK_1,				VK_1,			},	// 0x1e
	{ VK_2,				VK_2,			},	// 0x1f
	{ VK_3,				VK_3,			},	// 0x20
	{ VK_4,				VK_4,			},	// 0x21
	{ VK_5,				VK_5,			},	// 0x22
	{ VK_6,				VK_6,			},	// 0x23
	{ VK_7,				VK_7,			},	// 0x24
	{ VK_8,				VK_8,			},	// 0x25
	{ VK_9,				VK_9,			},	// 0x26
	{ VK_0,				VK_0,			},	// 0x27
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
	{ VK_APPS,			VK_APPS,		},	// 0x65
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
	{ VK_OEM_COPY,		VK_OEM_COPY,	},	// 0x88 kana
	{ VK_OEM_5,			VK_OEM_5,		},	// 0x89
	{ VK_CONVERT,		VK_CONVERT,		},	// 0x8a
	{ VK_NONCONVERT,	VK_NONCONVERT,	},	// 0x8b
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

USBHost::Keyboard::Keyboard() : HIDDriver(0x00010006), capsLockAsCtrlFlag_{false}
{
	::memset(&reportCaptured_, 0x00, sizeof(reportCaptured_));
}

Keyboard& USBHost::Keyboard::SetCapsLockAsCtrl(bool capsLockAsCtrlFlag)
{
	capsLockAsCtrlFlag_ = capsLockAsCtrlFlag;
	return *this;
}

void USBHost::Keyboard::OnReport(const uint8_t* report, uint16_t len)
{
	const hid_keyboard_report_t& reportEx = *reinterpret_cast<const hid_keyboard_report_t*>(report);
	::memset(&reportCaptured_, 0x00, sizeof(reportCaptured_));
	reportCaptured_.modifier = reportEx.modifier;
	int iDst = 0;
	for (int iSrc = 0; iSrc < count_of(reportEx.keycode); iSrc++) {
		uint8_t usageId = reportEx.keycode[iSrc];
		const UsageIdToKeyCode& usageIdToKeyCode = usageIdToKeyCodeTbl[usageId];
		uint8_t keyCode = GetKeyLayout().IsNonUS()? usageIdToKeyCode.keyCodeNonUS : usageIdToKeyCode.keyCodeUS;
		if (capsLockAsCtrlFlag_ && keyCode == VK_CAPITAL) {
			reportCaptured_.modifier |= KEYBOARD_MODIFIER_LEFTCTRL;
		} else {
			reportCaptured_.keyCodeTbl[iDst++] = keyCode;
		}
	}
	if (reportCaptured_.keyCodeTbl[0] == 0) {
		GetRepeater().Invalidate();
	} else {
		for (int i = 0; i < count_of(reportCaptured_.keyCodeTbl); i++) {
			if (GetRepeater().SignalFirst(reportCaptured_.keyCodeTbl[i], reportCaptured_.modifier)) break;
		}
	}
}

int USBHost::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax, bool includeModifiers)
{
	int nKeys = 0;
	for (int i = 0; i < ChooseMin(static_cast<int>(count_of(reportCaptured_.keyCodeTbl)), nKeysMax); i++) {
		uint8_t keyCode = reportCaptured_.keyCodeTbl[i];
		if (keyCode) keyCodeTbl[nKeys++] = keyCode;
	}
	uint8_t modifier = reportCaptured_.modifier;
	if (includeModifiers) {
		if (nKeys < nKeysMax && (modifier & Mod::CtrlL))	keyCodeTbl[nKeys++] = VK_LCONTROL;
		if (nKeys < nKeysMax && (modifier & Mod::ShiftL)) 	keyCodeTbl[nKeys++] = VK_LSHIFT;
		if (nKeys < nKeysMax && (modifier & Mod::AltL))		keyCodeTbl[nKeys++] = VK_LMENU;
		if (nKeys < nKeysMax && (modifier & Mod::WindowsL))	keyCodeTbl[nKeys++] = VK_LWIN;
		if (nKeys < nKeysMax && (modifier & Mod::CtrlR)) 	keyCodeTbl[nKeys++] = VK_RCONTROL;
		if (nKeys < nKeysMax && (modifier & Mod::ShiftR))	keyCodeTbl[nKeys++] = VK_RSHIFT;
		if (nKeys < nKeysMax && (modifier & Mod::AltR))		keyCodeTbl[nKeys++] = VK_RMENU;
		if (nKeys < nKeysMax && (modifier & Mod::WindowsR))	keyCodeTbl[nKeys++] = VK_RWIN;
	}
	return nKeys;
}

//------------------------------------------------------------------------------
// USBHost::Mouse
//------------------------------------------------------------------------------
USBHost::Mouse::Mouse() : HIDDriver(0x00010002), sensibility_{.6}
{
	SetStage({0, 0, 320, 240});
}

Mouse& USBHost::Mouse::SetStage(const Rect& rcStage)
{
	rcStage_ = rcStage;
	UpdateStage();
	return *this;
}

Mouse& USBHost::Mouse::SetSensibility(float sensibility)
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

void USBHost::Mouse::OnReport(const uint8_t* report, uint16_t len)
{
	const hid_mouse_report_t& reportEx = *reinterpret_cast<const hid_mouse_report_t*>(report);
	int xDiff = reportEx.x, yDiff = reportEx.y;
	xRaw_ = ChooseMin(ChooseMax(xRaw_ + xDiff, 0), rcStageRaw_.width - 1);
	yRaw_ = ChooseMin(ChooseMax(yRaw_ + yDiff, 0), rcStageRaw_.height - 1);
	status_.Update(CalcPoint(), reportEx.x, reportEx.y, reportEx.wheel, reportEx.pan, reportEx.buttons);
}

//------------------------------------------------------------------------------
// USBHost::GamePad
//------------------------------------------------------------------------------
USBHost::GamePad::GamePad() : HIDDriver(0x00010005)
{
}

float USBHost::GamePad::GetAxis(uint32_t usage) const
{
	if (!IsMounted()) return 0.;
	const auto& UsageAccessor = GetApplication().GetCollection().FindUsageAccessor(usage);
	if (!UsageAccessor.IsValid()) return 0.;
	int32_t valueMin = UsageAccessor.GetLogicalMinimum();
	int32_t valueMax = UsageAccessor.GetLogicalMaximum();
	int32_t value = UsageAccessor.GetVariable(GetHID().GetReport(), GetHID().GetReportLen());
	if (valueMin <= 0) {
		if (valueMin == 0) {
			int32_t valueMid = (valueMax + 1) / 2;
			valueMin -= valueMid;
			valueMax -= valueMid;
			value -= valueMid;
			if (valueMin == 0) return 0.;
		}
		return (value < 0)? -static_cast<float>(value) / valueMin :
			(valueMax > 0)? static_cast<float>(value) / valueMax : 0;
	} else if (valueMax > 0) {
		return static_cast<float>(value) / valueMax;
	} else {
		return 0.;
	}
}

//------------------------------------------------------------------------------
// USBHost::HID
//------------------------------------------------------------------------------
USBHost::HID::HID(uint8_t devAddr, uint8_t iInstance, HID::Application* pApplication) :
	devAddr_{devAddr}, iInstance_{iInstance}, pApplication_{pApplication}, reportLen_{0}, pHIDDriver_{nullptr}
{
}

void USBHost::HID::OnReport(const uint8_t* report, uint16_t len)
{
	if (len <= sizeof(report_)) {
		::memcpy(report_, report, len);
		reportLen_ = len;
	}
	if (pHIDDriver_) pHIDDriver_->OnReport(report, len);
}

USBHost::HID::Application* USBHost::HID::ParseReportDescriptor(const uint8_t* descReport, uint16_t descLen)
{
	uint32_t usagePage = 0;
	uint32_t reportOffset_Input = 0;
	uint32_t reportOffset_Output = 0;
	uint32_t reportOffset_Feature = 0;
	std::unique_ptr<GlobalItemList> pGlobalItemStack(new GlobalItemList());
	std::unique_ptr<LocalItem> pLocalItem(new LocalItem());
	//Dump(descReport, descLen);
	uint8_t itemTypePrev = 0;
	std::unique_ptr<Application> pApplicationTop;
	Collection* pCollectionCur = nullptr;
	Application* pApplicationCur = nullptr;
	GlobalItem* pGlobalItemCur = &pGlobalItemStack->globalItem;
	for (uint16_t descOffset = 0; descOffset < descLen; ) {
		uint8_t src = descReport[descOffset++];
		uint8_t itemType = src & 0xfc;
		int bytesItemData = src & 0x03;
		if (bytesItemData == 3) bytesItemData = 4;
		if (descOffset + bytesItemData > descLen) {
			// illegal format
			return nullptr;
		}
		uint32_t itemData = 0;
		for (int iByte = 0; iByte < bytesItemData; iByte++, descOffset++) {
			itemData = itemData | static_cast<uint32_t>(descReport[descOffset]) << (iByte * 8);
		}
		if (itemType == 0xfc) {
			// Long Items
			descOffset += itemData & 0xff;
			itemTypePrev = itemType;
			continue;
		}
		//::printf("%02x:%s (0x%0*x)\n", itemType, GetItemTypeName(itemType), bytesItemData * 2, itemData);
		switch (itemType) {
		// 6.2.2.4 Main Items
		case ItemType::Input: {
			pGlobalItemCur->itemType = itemType;
			pGlobalItemCur->mainItemData.itemData = itemData;
			if (pApplicationCur) pApplicationCur->AddMainItem(*pCollectionCur, *pGlobalItemCur, *pLocalItem, reportOffset_Input);
			pLocalItem->Clear();
			break;
		}
		case ItemType::Output: {
			pGlobalItemCur->itemType = itemType;
			pGlobalItemCur->mainItemData.itemData = itemData;
			if (pApplicationCur) pApplicationCur->AddMainItem(*pCollectionCur, *pGlobalItemCur, *pLocalItem, reportOffset_Output);
			pLocalItem->Clear();
			break;
		}
		case ItemType::Feature: {
			pGlobalItemCur->itemType = itemType;
			pGlobalItemCur->mainItemData.itemData = itemData;
			if (pApplicationCur) pApplicationCur->AddMainItem(*pCollectionCur, *pGlobalItemCur, *pLocalItem, reportOffset_Feature);
			pLocalItem->Clear();
			break;
		}
		case ItemType::Collection: {
			CollectionType collectionType = static_cast<CollectionType>(itemData);
			uint32_t usage = (pLocalItem->nUsage > 0)? pLocalItem->usageTbl[0].minimum : (usagePage << 16);
			if (pCollectionCur) {
				Collection* pCollection = new Collection(collectionType, usage, pCollectionCur);
				pCollectionCur->AppendCollectionChild(pCollection);
				pCollectionCur = pCollection;
			} else {
				pApplicationCur = new Application(usage);
				pCollectionCur = &pApplicationCur->GetCollection();
				if (pApplicationTop) {
					pApplicationTop->AppendList(pApplicationCur);
				} else {
					pApplicationTop.reset(pApplicationCur);
				}
			}
			pLocalItem->Clear();
			break;
		}
		case ItemType::EndCollection: {
			pCollectionCur = pCollectionCur->GetCollectionParent();
			break;
		}
		// 6.2.2.7 Global Items
		case ItemType::UsagePage: {
			usagePage = itemData;
			break;
		}
		case ItemType::LogicalMinimum: {
			pGlobalItemCur->logicalMinimum = SignExtend(itemData, bytesItemData * 8);
			break;
		}
		case ItemType::LogicalMaximum: {
			pGlobalItemCur->logicalMaximum = SignExtend(itemData, bytesItemData * 8);
			break;
		}
		case ItemType::PhysicalMinimum: {
			pGlobalItemCur->physicalMinimum = SignExtend(itemData, bytesItemData * 8);
			break;
		}
		case ItemType::PhysicalMaximum: {
			pGlobalItemCur->physicalMaximum = SignExtend(itemData, bytesItemData * 8);
			break;
		}
		case ItemType::UnitExponent: {
			pGlobalItemCur->unitExponent = SignExtend(itemData, bytesItemData * 8);
			break;
		}
		case ItemType::Unit: {
			pGlobalItemCur->unit = SignExtend(itemData, bytesItemData * 8);
			break;
		}
		case ItemType::ReportSize: {
			pGlobalItemCur->reportSize = itemData;
			break;
		}
		case ItemType::ReportID: {
			if (pApplicationCur) pApplicationCur->SetReportID(static_cast<uint8_t>(itemData));
			reportOffset_Input += 8;
			break;
		}
		case ItemType::ReportCount: {
			pGlobalItemCur->reportCount = itemData;
			break;
		}
		case ItemType::Push: {
			GlobalItemList* pGlobalItemList = new GlobalItemList(*pGlobalItemCur);
			pGlobalItemStack->AppendList(pGlobalItemList);
			pGlobalItemCur = &pGlobalItemList->globalItem;
			break;
		}
		case ItemType::Pop: {
			for (GlobalItemList* pGlobalItemList = pGlobalItemStack.get(); pGlobalItemList;
															pGlobalItemList = pGlobalItemList->GetListNext()) {
				if (pGlobalItemCur == &pGlobalItemList->GetListNext()->globalItem) {
					pGlobalItemList->RemoveListNext();
					pGlobalItemCur = &pGlobalItemList->globalItem;
					break;
				}
			}
			break;
		}
		// 6.2.2.8 Local Items
		case ItemType::Usage: {
			if (pLocalItem->nUsage < count_of(pLocalItem->usageTbl)) {
				uint32_t usage = ((bytesItemData < 4)? (usagePage << 16) : 0) | itemData;
				pLocalItem->usageTbl[pLocalItem->nUsage++] = Range(usage, usage);
			}
			break;
		}
		case ItemType::UsageMinimum: {
			uint32_t usage = ((bytesItemData < 4)? (usagePage << 16) : 0) | itemData;
			if (itemTypePrev == ItemType::UsageMaximum) {
				pLocalItem->usageTbl[pLocalItem->nUsage - 1].minimum = usage;
			} else if (pLocalItem->nUsage < count_of(pLocalItem->usageTbl)) {
				pLocalItem->usageTbl[pLocalItem->nUsage++] = Range(usage, usage);
			}
			break;
		}		
		case ItemType::UsageMaximum: {
			uint32_t usage = ((bytesItemData < 4)? (usagePage << 16) : 0) | itemData;
			if (itemTypePrev == ItemType::UsageMinimum) {
				pLocalItem->usageTbl[pLocalItem->nUsage - 1].maximum = usage;
			} else if (pLocalItem->nUsage < count_of(pLocalItem->usageTbl)) {
				pLocalItem->usageTbl[pLocalItem->nUsage++] = Range(usage, usage);
			}
			break;
		}
		case ItemType::DesignatorIndex: case ItemType::DesignatorMinimum: {
			if (pLocalItem->nDesignatorIndex < count_of(pLocalItem->designatorIndexTbl)) {
				pLocalItem->designatorIndexTbl[pLocalItem->nDesignatorIndex++] = Range(itemData, itemData);
			}
			break;
		}
		case ItemType::DesignatorMaximum: {
			if (pLocalItem->nDesignatorIndex > 0) {
				pLocalItem->designatorIndexTbl[pLocalItem->nDesignatorIndex - 1].maximum = itemData;
			}
			break;
		}
		case ItemType::StringIndex: case ItemType::StringMinimum: {
			if (pLocalItem->nStringIndex < count_of(pLocalItem->stringIndexTbl)) {
				pLocalItem->stringIndexTbl[pLocalItem->nStringIndex++] = Range(itemData, itemData);
			}
			break;
		}
		case ItemType::StringMaximum: {
			if (pLocalItem->nStringIndex > 0) {
				pLocalItem->stringIndexTbl[pLocalItem->nStringIndex - 1].maximum = itemData;
			}
			break;
		}
		case ItemType::Delimeter: {
			pLocalItem->delimeter = itemData;
			break;
		}
		}
		itemTypePrev = itemType;
	}
	return pApplicationTop.release();
}

const char* USBHost::HID::GetCollectionTypeName(CollectionType collectionType)
{
	return
		(collectionType == CollectionType::Physical)?		"Physical" :
		(collectionType == CollectionType::Application)?	"Application" :
		(collectionType == CollectionType::Logical)?		"Logical" :
		(collectionType == CollectionType::Report)?			"Report" :
		(collectionType == CollectionType::NamedArray)?		"NamedArray" :
		(collectionType == CollectionType::UsageSwitch)?	"UsageSwitch" :
		(collectionType == CollectionType::UsageModifier)?	"UsageModifier" : "none";
}

const char* USBHost::HID::GetItemTypeName(uint8_t itemType)
{
	static const struct {
		uint8_t itemType;
		const char* name;
	} tbl[] = {
		// 6.2.2.4 Main Items
		{ ItemType::Input,				"Input"				},
		{ ItemType::Output,				"Output"			},
		{ ItemType::Feature,			"Feature"			},
		{ ItemType::Collection,			"Collection"		},
		{ ItemType::EndCollection,		"EndCollection"		},
		// 6.2.2.7 Global Items
		{ ItemType::UsagePage,			"UsagePage"			},
		{ ItemType::LogicalMinimum,		"LogicalMinimum"	},
		{ ItemType::LogicalMaximum,		"LogicalMaximum"	},
		{ ItemType::PhysicalMinimum,	"PhysicalMinimum"	},
		{ ItemType::PhysicalMaximum,	"PhysicalMaximum"	},
		{ ItemType::UnitExponent,		"UnitExponent"		},
		{ ItemType::Unit,				"Unit"				},
		{ ItemType::ReportSize,			"ReportSize"		},
		{ ItemType::ReportID,			"ReportID"			},
		{ ItemType::ReportCount,		"ReportCount"		},
		{ ItemType::Push,				"Push"				},
		{ ItemType::Pop,				"Pop"				},
		// 6.2.2.8 Local Items
		{ ItemType::Usage,				"Usage"				},
		{ ItemType::UsageMinimum,		"UsageMinimum"		},
		{ ItemType::UsageMaximum,		"UsageMaximum"		},
		{ ItemType::DesignatorIndex,	"DesignatorIndex"	},
		{ ItemType::DesignatorMinimum,	"DesignatorMinimum"	},
		{ ItemType::DesignatorMaximum,	"DesignatorMaximum"	},
		{ ItemType::StringIndex,		"StringIndex"		},
		{ ItemType::StringMaximum,		"StringMaximum"		},
		{ ItemType::StringMinimum,		"StringMinimum"		},
		{ ItemType::Delimeter,			"Delimeter"			},
	};
	for (int i = 0; i < count_of(tbl); i++) {
		if (tbl[i].itemType == itemType) return tbl[i].name;
	}
	return "unknown";
}

//-----------------------------------------------------------------------------
// USBHost::HID::GlobalItem
//-----------------------------------------------------------------------------
const USBHost::HID::GlobalItem USBHost::HID::GlobalItem::None {
	itemType:			0,
	mainItemData:		0,
	logicalMinimum:		0,
	logicalMaximum:		1,
	physicalMinimum:	0,
	physicalMaximum:	1,
	unitExponent:		0,
	unit:				0,
	reportSize:			0,
	reportCount:		0,
};

USBHost::HID::GlobalItemList* USBHost::HID::GlobalItemList::GetListLast()
{
	auto* pGlobalItemList = this;
	for ( ; pGlobalItemList->GetListNext(); pGlobalItemList = pGlobalItemList->GetListNext()) ;
	return pGlobalItemList;
}

void USBHost::HID::GlobalItem::Print(Printable& printable, int indentLevel) const
{
	printable.Printf("%*slogicalMinimum:  %d\n", indentLevel * 2, "", logicalMinimum);
	printable.Printf("%*slogicalMaximum:  %d\n", indentLevel * 2, "", logicalMaximum);
	printable.Printf("%*sphysicalMinimum: %d\n", indentLevel * 2, "", physicalMinimum);
	printable.Printf("%*sphysicalMaximum: %d\n", indentLevel * 2, "", physicalMaximum);
	printable.Printf("%*sunitExponent:    %d\n", indentLevel * 2, "", unitExponent);
	printable.Printf("%*sunit:            %d\n", indentLevel * 2, "", unit);
	printable.Printf("%*sreportSize:      %d\n", indentLevel * 2, "", reportSize);
	printable.Printf("%*sreportCount:     %d\n", indentLevel * 2, "", reportCount);
}

//-----------------------------------------------------------------------------
// USBHost::HID::UsageAccessor
//-----------------------------------------------------------------------------
const USBHost::HID::UsageAccessor USBHost::HID::UsageAccessor::None(0, &GlobalItem::None, 0);

USBHost::HID::UsageAccessor* USBHost::HID::UsageAccessor::GetListLast()
{
	auto* pUsageAccessor = this;
	for ( ; pUsageAccessor->GetListNext(); pUsageAccessor = pUsageAccessor->GetListNext()) ;
	return pUsageAccessor;
}

int32_t USBHost::HID::UsageAccessor::GetVariable(const uint8_t* report, uint16_t len, int idx) const
{
	if (!IsValid()) return 0;
	uint32_t reportOffset = GetReportOffset() + GetReportSize() * idx;
	uint32_t byteOffset = reportOffset / 8;
	uint32_t nBytes = (GetReportSize() + 7) / 8;
	uint32_t value = 0;
	for (uint32_t iByte = 0; iByte < nBytes; iByte++) {
		value = value | (static_cast<uint32_t>(report[byteOffset + iByte]) << (iByte * 8));
	}
	value = (value >> (reportOffset % 8)) & ((1 << GetReportSize()) - 1);
	return (GetLogicalMinimum() >= 0)? static_cast<int32_t>(value) : SignExtend(value, GetReportSize());
}

void USBHost::HID::UsageAccessor::Print(Printable& printable, int indentLevel) const
{
	printable.Printf("%*s%s(%04x'%04x)%s %s offset:%d size:%d LMin:%d LMax:%d PMin:%d PMax:%d UnitExp:%d\n",
		indentLevel * 2, "", GetItemTypeName(GetItemType()),
		GetUsagePage(), GetUsageId(), GetMainItemData().IsArray()? ":array" : "", IsAbsolute()? "Abs" : "Rel",
		GetReportOffset(), GetReportSize(),
		GetLogicalMinimum(), GetLogicalMaximum(), GetPhysicalMinimum(), GetPhysicalMaximum(), GetUnitExponent());
}

//-----------------------------------------------------------------------------
// USBHost::HID::Collection
//-----------------------------------------------------------------------------
const USBHost::HID::Collection USBHost::HID::Collection::None(CollectionType::None, 0, nullptr);

USBHost::HID::Collection* USBHost::HID::Collection::GetListLast()
{
	auto* pCollection = this;
	for ( ; pCollection->GetListNext(); pCollection = pCollection->GetListNext()) ;
	return pCollection;
}

int32_t USBHost::HID::Collection::GetArrayItem(const uint8_t* report, uint16_t len, int idx) const
{
	return pUsageAccessorArray_? pUsageAccessorArray_->GetVariable(report, len, idx) : 0;
}

void USBHost::HID::Collection::AppendUsageAccessor(UsageAccessor* pUsageAccessor)
{
	if (pUsageAccessorTop_) {
		pUsageAccessorTop_->AppendList(pUsageAccessor);
	} else {
		pUsageAccessorTop_.reset(pUsageAccessor);
	}
}

void USBHost::HID::Collection::AppendCollectionChild(Collection* pCollection)
{
	if (pCollectionChildTop_) {
		pCollectionChildTop_->AppendList(pCollection);
	} else {
		pCollectionChildTop_.reset(pCollection);
	}
}

void USBHost::HID::Collection::AddMainItem(GlobalItem* pGlobalItem, const LocalItem& localItem, uint32_t& reportOffset)
{
	//uint32_t mainItemData = pGlobalItem->mainItemData;
	//::printf("%s,%s,%s ReportSize:%d ReportCount:%d\n", MainItemData::IsData(mainItemData)? "Data" : "Constant",
	//	MainItemData::IsArray(mainItemData)? "Array" : "Variable", MainItemData::IsAbsolute(mainItemData)? "Absolute" : "Relative",
	//	pGlobalItem->reportSize, pGlobalItem->reportCount);
	//globalItem.Print(1);
	if (pGlobalItem->mainItemData.IsConstant()) {
		reportOffset += pGlobalItem->reportSize * pGlobalItem->reportCount;
	} else if (pGlobalItem->mainItemData.IsVariable()) {
		for (int i = 0; i < localItem.nUsage; i++) {
			const Range& range = localItem.usageTbl[i];
			for (uint32_t usage = range.minimum; usage <= range.maximum; usage++) {
				auto pUsageAccessor = new UsageAccessor(usage, pGlobalItem, reportOffset);
				if (pUsageAccessorTop_) {
					pUsageAccessorTop_->AppendList(pUsageAccessor);
				} else {
					pUsageAccessorTop_.reset(pUsageAccessor);
				}
				reportOffset += pGlobalItem->reportSize;
			}
		}
	} else { // Array
		pUsageAccessorArray_.reset(new UsageAccessor(0, pGlobalItem, reportOffset));
		reportOffset += pGlobalItem->reportSize * pGlobalItem->reportCount;
	}
}

const USBHost::HID::UsageAccessor& USBHost::HID::Collection::FindUsageAccessor(uint32_t usage) const
{
	for (auto pUsageAccessor = pUsageAccessorTop_.get(); pUsageAccessor; pUsageAccessor = pUsageAccessor->GetListNext()) {
		if (pUsageAccessor->GetUsage() == usage) return *pUsageAccessor;
	}
	return UsageAccessor::None;
}

const USBHost::HID::Collection& USBHost::HID::Collection::FindCollection(uint32_t usage) const
{
	for (auto pCollection = pCollectionChildTop_.get(); pCollection; pCollection = pCollection->GetListNext()) {
		if (pCollection->GetUsage() == usage) return *pCollection;
	}
	return Collection::None;
}

void USBHost::HID::Collection::PrintUsage(Printable& printable, int indentLevel) const
{
	for (auto pUsageAccessor = pUsageAccessorTop_.get(); pUsageAccessor; pUsageAccessor = pUsageAccessor->GetListNext()) {
		pUsageAccessor->Print(printable, indentLevel);
	}
	if (pUsageAccessorArray_) pUsageAccessorArray_->Print(printable, indentLevel);
	for (auto pCollection = pCollectionChildTop_.get(); pCollection; pCollection = pCollection->GetListNext()) {
		printable.Printf("%*s%s(%04x'%04x) {\n", indentLevel * 2, "",
			GetCollectionTypeName(pCollection->GetCollectionType()), pCollection->GetUsagePage(), pCollection->GetUsageId());
		pCollection->PrintUsage(printable, indentLevel + 1);
		printable.Printf("%*s}\n", indentLevel * 2, "");
	}
}

//------------------------------------------------------------------------------
// USBHost::HID::Application
//------------------------------------------------------------------------------
USBHost::HID::Application::Application(uint32_t usage) : reportID_{0}, collection_(CollectionType::Application, usage, nullptr)
{
}

USBHost::HID::Application* USBHost::HID::Application::GetListLast()
{
	auto* pApplication = this;
	for ( ; pApplication->GetListNext(); pApplication = pApplication->GetListNext()) ;
	return pApplication;
}

void USBHost::HID::Application::AddMainItem(Collection& collection, const GlobalItem& globalItem, const LocalItem& localItem, uint32_t& reportOffset)
{
	GlobalItemList* pGlobalItemList = new GlobalItemList(globalItem);
	if (pGlobalItemListTop_) {
		pGlobalItemListTop_->AppendList(pGlobalItemList);
	} else {
		pGlobalItemListTop_.reset(pGlobalItemList);
	}
	collection.AddMainItem(&pGlobalItemList->globalItem, localItem, reportOffset);
}

int32_t USBHost::HID::Application::GetVariable(const uint8_t* report, uint16_t len, uint32_t usage) const
{
	return GetCollection().FindUsageAccessor(usage).GetVariable(report, len);
}

int32_t USBHost::HID::Application::GetVariable(const uint8_t* report, uint16_t len, uint32_t usageCollection, uint32_t usage) const
{
	return GetCollection().FindCollection(usageCollection).FindUsageAccessor(usage).GetVariable(report, len);
}

int32_t USBHost::HID::Application::GetVariable(const uint8_t* report, uint16_t len, uint32_t usageCollection1, uint32_t usageCollection2, uint32_t usage) const
{
	return GetCollection().FindCollection(usageCollection1).FindCollection(usageCollection2).FindUsageAccessor(usage).GetVariable(report, len);
}

int32_t USBHost::HID::Application::GetVariableWithDefault(const uint8_t* report, uint16_t len, uint32_t usage, int32_t valueDefault) const
{
	return GetCollection().FindUsageAccessor(usage).GetVariableWithDefault(report, len, valueDefault);
}

int32_t USBHost::HID::Application::GetVariableWithDefault(const uint8_t* report, uint16_t len, uint32_t usageCollection, uint32_t usage, int32_t valueDefault) const
{
	return GetCollection().FindCollection(usageCollection).FindUsageAccessor(usage).GetVariableWithDefault(report, len, valueDefault);
}

int32_t USBHost::HID::Application::GetVariableWithDefault(const uint8_t* report, uint16_t len, uint32_t usageCollection1, uint32_t usageCollection2, uint32_t usage, int32_t valueDefault) const
{
	return GetCollection().FindCollection(usageCollection1).FindCollection(usageCollection2)
		.FindUsageAccessor(usage).GetVariableWithDefault(report, len, valueDefault);
}

int32_t USBHost::HID::Application::GetArrayItem(const uint8_t* report, uint16_t len, int idx) const
{
	return GetCollection().GetArrayItem(report, len, idx);
}

int32_t USBHost::HID::Application::GetArrayItem(const uint8_t* report, uint16_t len, uint32_t usageCollection, int idx) const
{
	return GetCollection().FindCollection(usageCollection).GetArrayItem(report, len, idx);
}

int32_t USBHost::HID::Application::GetArrayItem(const uint8_t* report, uint16_t len, uint32_t usageCollection1, uint32_t usageCollection2, int idx) const
{
	return GetCollection().FindCollection(usageCollection1).FindCollection(usageCollection2).GetArrayItem(report, len, idx);
}

void USBHost::HID::Application::Print(Printable& printable, int indentLevel) const
{
	printable.Printf("%*sApplication(%08x) {\n", indentLevel * 2, "", GetUsage());
	GetCollection().PrintUsage(printable, indentLevel + 1);
	printable.Printf("%*s}\n", indentLevel * 2, "");
}

void USBHost::HID::Application::PrintAll(Printable& printable, int indentLevel) const
{
	for (const Application* pApplication = this; pApplication; pApplication = pApplication->GetListNext()) {
		pApplication->Print(printable, indentLevel);
	}
}

}
