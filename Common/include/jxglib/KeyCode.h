//==============================================================================
// jxglib/KeyCode.h
// https://learn.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
//==============================================================================
#ifndef PICO_JXGLIB_KEYCODE_h
#define PICO_JXGLIB_KEYCODE_H

namespace jxglib {

//------------------------------------------------------------------------------
// KeyCode
//------------------------------------------------------------------------------
static const int VK_LBUTTON				= 0x01;
static const int VK_RBUTTON				= 0x02;
static const int VK_CANCEL				= 0x03;
static const int VK_MBUTTON				= 0x04;
static const int VK_XBUTTON1			= 0x05;
static const int VK_XBUTTON2			= 0x06;
static const int VK_BACK				= 0x08;
static const int VK_TAB					= 0x09;
static const int VK_CLEAR				= 0x0c;
static const int VK_RETURN				= 0x0d;
static const int VK_SHIFT				= 0x10;
static const int VK_CONTROL				= 0x11;
static const int VK_MENU				= 0x12;
static const int VK_PAUSE				= 0x13;
static const int VK_CAPITAL				= 0x14;
static const int VK_KANA				= 0x15;
static const int VK_HANGUL				= 0x15;
static const int VK_IME_ON				= 0x16;
static const int VK_JUNJA				= 0x17;
static const int VK_FINAL				= 0x18;
static const int VK_KANJI				= 0x19;
static const int VK_ESCAPE				= 0x1b;
static const int VK_CONVERT				= 0x1c;
static const int VK_NONCONVERT			= 0x1d;
static const int VK_ACCEPT				= 0x1e;
static const int VK_MODECHANGE			= 0x1f;
static const int VK_SPACE				= 0x20;
static const int VK_PRIOR				= 0x21;
static const int VK_NEXT				= 0x22;
static const int VK_END					= 0x23;
static const int VK_HOME				= 0x24;
static const int VK_LEFT				= 0x25;
static const int VK_UP					= 0x26;
static const int VK_RIGHT				= 0x27;
static const int VK_DOWN				= 0x28;
static const int VK_SELECT				= 0x29;
static const int VK_PRINT				= 0x2a;
static const int VK_EXECUTE				= 0x2b;
static const int VK_SNAPSHOT			= 0x2c;
static const int VK_INSERT				= 0x2d;
static const int VK_DELETE				= 0x2e;
static const int VK_HELP				= 0x2f;
static const int VK_LWIN				= 0x5b;
static const int VK_RWIN				= 0x5c;
static const int VK_APPS				= 0x5d;
static const int VK_SLEEP				= 0x5f;
static const int VK_NUMPAD0				= 0x60;
static const int VK_NUMPAD1				= 0x61;
static const int VK_NUMPAD2				= 0x62;
static const int VK_NUMPAD3				= 0x63;
static const int VK_NUMPAD4				= 0x64;
static const int VK_NUMPAD5				= 0x65;
static const int VK_NUMPAD6				= 0x66;
static const int VK_NUMPAD7				= 0x67;
static const int VK_NUMPAD8				= 0x68;
static const int VK_NUMPAD9				= 0x69;
static const int VK_MULTIPLY			= 0x6a;
static const int VK_ADD					= 0x6b;
static const int VK_SEPARATOR			= 0x6c;
static const int VK_SUBTRACT			= 0x6d;
static const int VK_DECIMAL				= 0x6e;
static const int VK_DIVIDE				= 0x6f;
static const int VK_F1					= 0x70;
static const int VK_F2					= 0x71;
static const int VK_F3					= 0x72;
static const int VK_F4					= 0x73;
static const int VK_F5					= 0x74;
static const int VK_F6					= 0x75;
static const int VK_F7					= 0x76;
static const int VK_F8					= 0x77;
static const int VK_F9					= 0x78;
static const int VK_F10					= 0x79;
static const int VK_F11					= 0x7a;
static const int VK_F12					= 0x7b;
static const int VK_F13					= 0x7c;
static const int VK_F14					= 0x7d;
static const int VK_F15					= 0x7e;
static const int VK_F16					= 0x7f;
static const int VK_F17					= 0x80;
static const int VK_F18					= 0x81;
static const int VK_F19					= 0x82;
static const int VK_F20					= 0x83;
static const int VK_F21					= 0x84;
static const int VK_F22					= 0x85;
static const int VK_F23					= 0x86;
static const int VK_F24					= 0x87;
static const int VK_NUMLOCK				= 0x90;
static const int VK_SCROLL				= 0x91;
static const int VK_LSHIFT				= 0xa0;
static const int VK_RSHIFT				= 0xa1;
static const int VK_LCONTROL			= 0xa2;
static const int VK_RCONTROL			= 0xa3;
static const int VK_LMENU				= 0xa4;
static const int VK_RMENU				= 0xa5;
static const int VK_BROWSER_BACK		= 0xa6;
static const int VK_BROWSER_FORWARD		= 0xa7;
static const int VK_BROWSER_REFRESH		= 0xa8;
static const int VK_BROWSER_STOP		= 0xa9;
static const int VK_BROWSER_SEARCH		= 0xaa;
static const int VK_BROWSER_FAVORITES	= 0xab;
static const int VK_BROWSER_HOME		= 0xac;
static const int VK_VOLUME_MUTE			= 0xad;
static const int VK_VOLUME_DOWN			= 0xae;
static const int VK_VOLUME_UP			= 0xaf;
static const int VK_MEDIA_NEXT_TRACK	= 0xb0;
static const int VK_MEDIA_PREV_TRACK	= 0xb1;
static const int VK_MEDIA_STOP			= 0xb2;
static const int VK_MEDIA_PLAY_PAUSE	= 0xb3;
static const int VK_LAUNCH_MAIL			= 0xb4;
static const int VK_LAUNCH_MEDIA_SELECT	= 0xb5;
static const int VK_LAUNCH_APP1			= 0xb6;
static const int VK_LAUNCH_APP2			= 0xb7;
static const int VK_OEM_1				= 0xba;
static const int VK_OEM_PLUS			= 0xbb;
static const int VK_OEM_COMMA			= 0xbc;
static const int VK_OEM_MINUS			= 0xbd;
static const int VK_OEM_PERIOD			= 0xbe;
static const int VK_OEM_2				= 0xbf;
static const int VK_OEM_3				= 0xc0;
static const int VK_OEM_4				= 0xdb;
static const int VK_OEM_5				= 0xdc;
static const int VK_OEM_6				= 0xdd;
static const int VK_OEM_7				= 0xde;
static const int VK_OEM_8				= 0xdf;
static const int VK_OEM_102				= 0xe2;
static const int VK_PROCESSKEY			= 0xe5;
static const int VK_PACKET				= 0xe7;
static const int VK_ATTN				= 0xf6;
static const int VK_CRSEL				= 0xf7;
static const int VK_EXSEL				= 0xf8;
static const int VK_EREOF				= 0xf9;
static const int VK_PLAY				= 0xfa;
static const int VK_ZOOM				= 0xfb;
static const int VK_NONAME				= 0xfc;
static const int VK_PA1					= 0xfd;
static const int VK_OEM_CLEAR			= 0xfe;

}

#endif