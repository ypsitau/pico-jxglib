//==============================================================================
// KeyLayout.cpp
//==============================================================================
#include "jxglib/KeyLayout.h"

namespace jxglib {

//------------------------------------------------------------------------------
// KeyLayout
//------------------------------------------------------------------------------
KeyData KeyLayout::CreateKeyData(uint8_t keyCode, uint8_t modifier) const
{
	bool isCtrlDown = KeyData::IsCtrlDown(modifier);
	char charCode = (keyCode == VK_TAB)? '\0' : (keyCode == VK_RETURN)? '\0' :
			(keyCode == VK_ESCAPE)? '\0' : ConvKeyCodeToCharCode(keyCode, modifier);
	if (charCode == 0) return KeyData(keyCode, true, modifier);
	if (!isCtrlDown) {
		// nothing to do
	} else if ('A' <= charCode && charCode <='Z') {
		charCode = charCode - 'A' + 1;
	} else  if ('a' <= charCode && charCode <='z') {
		charCode = charCode - 'a' + 1;
	}
	return KeyData(charCode, false, modifier);
}

uint8_t KeyLayout::ConvKeyCodeToCharCode(uint8_t keyCode, uint8_t modifier) const
{
	const CharEntry& charEntry = GetCharEntryTbl()[keyCode];
	return (KeyData::IsShiftDown(modifier))? charEntry.charCodeShift : charEntry.charCode;
}

//------------------------------------------------------------------------------
// KeyLayout_101
//------------------------------------------------------------------------------
const KeyLayout_101 KeyLayout_101::Instance;

const KeyLayout::CharEntry* KeyLayout_101::GetCharEntryTbl() const
{
	static const CharEntry charEntryTbl[] = {
		{0,			0		},	// 0x00			
		{0,			0		},	// 0x01: VK_LBUTTON		
		{0,			0		},	// 0x02: VK_RBUTTON		
		{0,			0		},	// 0x03: VK_CANCEL			
		{0,			0		},	// 0x04: VK_MBUTTON		
		{0,			0		},	// 0x05: VK_XBUTTON1		
		{0,			0		},	// 0x06: VK_XBUTTON2		
		{0,			0		},	// 0x07			
		{'\b',		'\b'	},	// 0x08: VK_BACK			
		{'\t',		'\t'	},	// 0x09: VK_TAB			
		{0,			0		},	// 0x0a			
		{0,			0		},	// 0x0b
		{0,			0		},	// 0x0c: VK_CLEAR
		{'\r',		'\r'	},	// 0x0d: VK_RETURN
		{0,			0		},	// 0x0e
		{0,			0		},	// 0x0f
		{0,			0		},	// 0x10: VK_SHIFT
		{0,			0		},	// 0x11: VK_CONTROL
		{0,			0		},	// 0x12: VK_MENU
		{0,			0		},	// 0x13: VK_PAUSE
		{0,			0		},	// 0x14: VK_CAPITAL
		{0,			0		},	// 0x15: VK_KANA, VK_HANGUL
		{0,			0		},	// 0x16: VK_IME_ON
		{0,			0		},	// 0x17: VK_JUNJA
		{0,			0		},	// 0x18: VK_FINAL
		{0,			0		},	// 0x19: VK_KANJI
		{0,			0		},	// 0x1a
		{'\x1b',	'\x1b'	},	// 0x1b: VK_ESCAPE
		{0,			0		},	// 0x1c: VK_CONVERT
		{0,			0		},	// 0x1d: VK_NONCONVERT
		{0,			0		},	// 0x1e: VK_ACCEPT
		{0,			0		},	// 0x1f: VK_MODECHANGE
		{' ',		' '		},	// 0x20: VK_SPACE
		{0,			0		},	// 0x21: VK_PRIOR
		{0,			0		},	// 0x22: VK_NEXT
		{0,			0		},	// 0x23: VK_END
		{0,			0		},	// 0x24: VK_HOME
		{0,			0		},	// 0x25: VK_LEFT
		{0,			0		},	// 0x26: VK_UP
		{0,			0		},	// 0x27: VK_RIGHT
		{0,			0		},	// 0x28: VK_DOWN
		{0,			0		},	// 0x29: VK_SELECT
		{0,			0		},	// 0x2a: VK_PRINT
		{0,			0		},	// 0x2b: VK_EXECUTE
		{0,			0		},	// 0x2c: VK_SNAPSHOT
		{0,			0		},	// 0x2d: VK_INSERT
		{0,			0		},	// 0x2e: VK_DELETE
		{0,			0		},	// 0x2f: VK_HELP
		{'0',		')'	 	},	// 0x30: '0'
		{'1',		'!' 	},	// 0x31: '1'
		{'2',		'@' 	},	// 0x32: '2'
		{'3',		'#' 	},	// 0x33: '3'
		{'4',		'$' 	},	// 0x34: '4'
		{'5',		'%' 	},	// 0x35: '5'
		{'6',		'^' 	},	// 0x36: '6'
		{'7',		'&' 	},	// 0x37: '7'
		{'8',		'*' 	},	// 0x38: '8'
		{'9',		'(' 	},	// 0x39: '9'
		{0,			0		},	// 0x3a
		{0,			0		},	// 0x3b
		{0,			0		},	// 0x3c
		{0,			0		},	// 0x3d
		{0,			0		},	// 0x3e
		{0,			0		},	// 0x3f
		{0,			0		},	// 0x40
		{'a',		'A' 	},	// 0x41: 'A'
		{'b',		'B' 	},	// 0x42: 'B'
		{'c',		'C' 	},	// 0x43: 'C'
		{'d',		'D' 	},	// 0x44: 'D'
		{'e',		'E' 	},	// 0x45: 'E'
		{'f',		'F' 	},	// 0x46: 'F'
		{'g',		'G' 	},	// 0x47: 'G'
		{'h',		'H' 	},	// 0x48: 'H'
		{'i',		'I' 	},	// 0x49: 'I'
		{'j',		'J' 	},	// 0x4a: 'J'
		{'k',		'K' 	},	// 0x4b: 'K'
		{'l',		'L' 	},	// 0x4c: 'L'
		{'m',		'M' 	},	// 0x4d: 'M'
		{'n',		'N' 	},	// 0x4e: 'N'
		{'o',		'O' 	},	// 0x4f: 'O'
		{'p',		'P' 	},	// 0x50: 'P'
		{'q',		'Q' 	},	// 0x51: 'Q'
		{'r',		'R' 	},	// 0x52: 'R'
		{'s',		'S' 	},	// 0x53: 'S'
		{'t',		'T' 	},	// 0x54: 'T'
		{'u',		'U' 	},	// 0x55: 'U'
		{'v',		'V' 	},	// 0x56: 'V'
		{'w',		'W' 	},	// 0x57: 'W'
		{'x',		'X' 	},	// 0x58: 'X'
		{'y',		'Y' 	},	// 0x59: 'Y'
		{'z',		'Z' 	},	// 0x5a: 'Z'
		{0,			0		},	// 0x5b: VK_LWIN
		{0,			0		},	// 0x5c: VK_RWIN
		{0,			0		},	// 0x5d: VK_APPS
		{0,			0		},	// 0x5e
		{0,			0		},	// 0x5f: VK_SLEEP
		{'0',		0		},	// 0x60: VK_NUMPAD0
		{'1',		0		},	// 0x61: VK_NUMPAD1
		{'2',		0		},	// 0x62: VK_NUMPAD2
		{'3',		0		},	// 0x63: VK_NUMPAD3
		{'4',		0		},	// 0x64: VK_NUMPAD4
		{'5',		0		},	// 0x65: VK_NUMPAD5
		{'6',		0		},	// 0x66: VK_NUMPAD6
		{'7',		0		},	// 0x67: VK_NUMPAD7
		{'8',		0		},	// 0x68: VK_NUMPAD8
		{'9',		0		},	// 0x69: VK_NUMPAD9
		{'*',		0		},	// 0x6a: VK_MULTIPLY
		{'+',		0		},	// 0x6b: VK_ADD
		{',',		0		},	// 0x6c: VK_SEPARATOR
		{'-',		0		},	// 0x6d: VK_SUBTRACT
		{'.',		0		},	// 0x6e: VK_DECIMAL
		{'/',		0		},	// 0x6f: VK_DIVIDE
		{0,			0		},	// 0x70: VK_F1
		{0,			0		},	// 0x71: VK_F2
		{0,			0		},	// 0x72: VK_F3
		{0,			0		},	// 0x73: VK_F4
		{0,			0		},	// 0x74: VK_F5
		{0,			0		},	// 0x75: VK_F6
		{0,			0		},	// 0x76: VK_F7
		{0,			0		},	// 0x77: VK_F8
		{0,			0		},	// 0x78: VK_F9
		{0,			0		},	// 0x79: VK_F10
		{0,			0		},	// 0x7a: VK_F11
		{0,			0		},	// 0x7b: VK_F12
		{0,			0		},	// 0x7c: VK_F13
		{0,			0		},	// 0x7d: VK_F14
		{0,			0		},	// 0x7e: VK_F15
		{0,			0		},	// 0x7f: VK_F16
		{0,			0		},	// 0x80: VK_F17
		{0,			0		},	// 0x81: VK_F18
		{0,			0		},	// 0x82: VK_F19
		{0,			0		},	// 0x83: VK_F20
		{0,			0		},	// 0x84: VK_F21
		{0,			0		},	// 0x85: VK_F22
		{0,			0		},	// 0x86: VK_F23
		{0,			0		},	// 0x87: VK_F24
		{0,			0		},	// 0x88
		{0,			0		},	// 0x89
		{0,			0		},	// 0x8a
		{0,			0		},	// 0x8b
		{0,			0		},	// 0x8c
		{0,			0		},	// 0x8d
		{0,			0		},	// 0x8e
		{0,			0		},	// 0x8f
		{0,			0		},	// 0x90: VK_NUMLOCK
		{0,			0		},	// 0x91: VK_SCROLL
		{0,			0		},	// 0x92
		{0,			0		},	// 0x93
		{0,			0		},	// 0x94
		{0,			0		},	// 0x95
		{0,			0		},	// 0x96
		{0,			0		},	// 0x97
		{0,			0		},	// 0x98
		{0,			0		},	// 0x99
		{0,			0		},	// 0x9a
		{0,			0		},	// 0x9b
		{0,			0		},	// 0x9c
		{0,			0		},	// 0x9d
		{0,			0		},	// 0x9e
		{0,			0		},	// 0x9f
		{0,			0		},	// 0xa0: VK_LSHIFT
		{0,			0		},	// 0xa1: VK_RSHIFT
		{0,			0		},	// 0xa2: VK_LCONTROL
		{0,			0		},	// 0xa3: VK_RCONTROL
		{0,			0		},	// 0xa4: VK_LMENU
		{0,			0		},	// 0xa5: VK_RMENU
		{0,			0		},	// 0xa6: VK_BROWSER_BACK
		{0,			0		},	// 0xa7: VK_BROWSER_FORWARD
		{0,			0		},	// 0xa8: VK_BROWSER_REFRESH
		{0,			0		},	// 0xa9: VK_BROWSER_STOP
		{0,			0		},	// 0xaa: VK_BROWSER_SEARCH
		{0,			0		},	// 0xab: VK_BROWSER_FAVORITES
		{0,			0		},	// 0xac: VK_BROWSER_HOME
		{0,			0		},	// 0xad: VK_VOLUME_MUTE
		{0,			0		},	// 0xae: VK_VOLUME_DOWN
		{0,			0		},	// 0xaf: VK_VOLUME_UP
		{0,			0		},	// 0xb0: VK_MEDIA_NEXT_TRACK
		{0,			0		},	// 0xb1: VK_MEDIA_PREV_TRACK
		{0,			0		},	// 0xb2: VK_MEDIA_STOP
		{0,			0		},	// 0xb3: VK_MEDIA_PLAY_PAUSE
		{0,			0		},	// 0xb4: VK_LAUNCH_MAIL
		{0,			0		},	// 0xb5: VK_LAUNCH_MEDIA_SELECT
		{0,			0		},	// 0xb6: VK_LAUNCH_APP1
		{0,			0		},	// 0xb7: VK_LAUNCH_APP2
		{0,			0		},	// 0xb8
		{0,			0		},	// 0xb9
		{';',		':'		},	// 0xba: VK_OEM_1
		{'=',		'+'		},	// 0xbb: VK_OEM_PLUS
		{',',		'<'		},	// 0xbc: VK_OEM_COMMA
		{'-',		'_'		},	// 0xbd: VK_OEM_MINUS
		{'.',		'>'		},	// 0xbe: VK_OEM_PERIOD
		{'/',		'?'		},	// 0xbf: VK_OEM_2
		{'`',		'~'		},	// 0xc0: VK_OEM_3
		{0,			0		},	// 0xc1
		{0,			0		},	// 0xc2
		{0,			0		},	// 0xc3
		{0,			0		},	// 0xc4
		{0,			0		},	// 0xc5
		{0,			0		},	// 0xc6
		{0,			0		},	// 0xc7
		{0,			0		},	// 0xc8
		{0,			0		},	// 0xc9
		{0,			0		},	// 0xca
		{0,			0		},	// 0xcb
		{0,			0		},	// 0xcc
		{0,			0		},	// 0xcd
		{0,			0		},	// 0xce
		{0,			0		},	// 0xcf
		{0,			0		},	// 0xd0
		{0,			0		},	// 0xd1
		{0,			0		},	// 0xd2
		{0,			0		},	// 0xd3
		{0,			0		},	// 0xd4
		{0,			0		},	// 0xd5
		{0,			0		},	// 0xd6
		{0,			0		},	// 0xd7
		{0,			0		},	// 0xd8
		{0,			0		},	// 0xd9
		{0,			0		},	// 0xda
		{'[',		'{'		},	// 0xdb: VK_OEM_4
		{'\\',		'|'		},	// 0xdc: VK_OEM_5
		{']',		'}'		},	// 0xdd: VK_OEM_6
		{'\'',		'"'		},	// 0xde: VK_OEM_7
		{0,			0		},	// 0xdf: VK_OEM_8
		{0,			0		},	// 0xe0
		{0,			0		},	// 0xe1
		{'\\',		'_'		},	// 0xe2: VK_OEM_102
		{0,			0		},	// 0xe3
		{0,			0		},	// 0xe4
		{0,			0		},	// 0xe5: VK_PROCESSKEY
		{0,			0		},	// 0xe6
		{0,			0		},	// 0xe7: VK_PACKET
		{0,			0		},	// 0xe8
		{0,			0		},	// 0xe9
		{0,			0		},	// 0xea
		{0,			0		},	// 0xeb
		{0,			0		},	// 0xec
		{0,			0		},	// 0xed
		{0,			0		},	// 0xee
		{0,			0		},	// 0xef
		{0,			0		},	// 0xf0
		{0,			0		},	// 0xf1
		{0,			0		},	// 0xf2
		{0,			0		},	// 0xf3
		{0,			0		},	// 0xf4
		{0,			0		},	// 0xf5
		{0,			0		},	// 0xf6: VK_ATTN
		{0,			0		},	// 0xf7: VK_CRSEL
		{0,			0		},	// 0xf8: VK_EXSEL
		{0,			0		},	// 0xf9: VK_EREOF
		{0,			0		},	// 0xfa: VK_PLAY
		{0,			0		},	// 0xfb: VK_ZOOM
		{0,			0		},	// 0xfc: VK_NONAME
		{0,			0		},	// 0xfd: VK_PA1
		{0,			0		},	// 0xfe: VK_OEM_CLEAR
		{0,			0		},	// 0xff
	};
	return charEntryTbl;
}

//------------------------------------------------------------------------------
// KeyLayout_106
//------------------------------------------------------------------------------
const KeyLayout_106 KeyLayout_106::Instance;

const KeyLayout::CharEntry* KeyLayout_106::GetCharEntryTbl() const
{
	static const CharEntry charEntryTbl[] = {
		{0,			0		},	// 0x00			
		{0,			0		},	// 0x01: VK_LBUTTON		
		{0,			0		},	// 0x02: VK_RBUTTON		
		{0,			0		},	// 0x03: VK_CANCEL			
		{0,			0		},	// 0x04: VK_MBUTTON		
		{0,			0		},	// 0x05: VK_XBUTTON1		
		{0,			0		},	// 0x06: VK_XBUTTON2		
		{0,			0		},	// 0x07			
		{'\b',		'\b'	},	// 0x08: VK_BACK			
		{'\t',		'\t'	},	// 0x09: VK_TAB			
		{0,			0		},	// 0x0a			
		{0,			0		},	// 0x0b
		{0,			0		},	// 0x0c: VK_CLEAR
		{'\r',		'\r'	},	// 0x0d: VK_RETURN
		{0,			0		},	// 0x0e
		{0,			0		},	// 0x0f
		{0,			0		},	// 0x10: VK_SHIFT
		{0,			0		},	// 0x11: VK_CONTROL
		{0,			0		},	// 0x12: VK_MENU
		{0,			0		},	// 0x13: VK_PAUSE
		{0,			0		},	// 0x14: VK_CAPITAL
		{0,			0		},	// 0x15: VK_KANA, VK_HANGUL
		{0,			0		},	// 0x16: VK_IME_ON
		{0,			0		},	// 0x17: VK_JUNJA
		{0,			0		},	// 0x18: VK_FINAL
		{0,			0		},	// 0x19: VK_KANJI
		{0,			0		},	// 0x1a
		{'\x1b',	'\x1b'	},	// 0x1b: VK_ESCAPE
		{0,			0		},	// 0x1c: VK_CONVERT
		{0,			0		},	// 0x1d: VK_NONCONVERT
		{0,			0		},	// 0x1e: VK_ACCEPT
		{0,			0		},	// 0x1f: VK_MODECHANGE
		{' ',		' '		},	// 0x20: VK_SPACE
		{0,			0		},	// 0x21: VK_PRIOR
		{0,			0		},	// 0x22: VK_NEXT
		{0,			0		},	// 0x23: VK_END
		{0,			0		},	// 0x24: VK_HOME
		{0,			0		},	// 0x25: VK_LEFT
		{0,			0		},	// 0x26: VK_UP
		{0,			0		},	// 0x27: VK_RIGHT
		{0,			0		},	// 0x28: VK_DOWN
		{0,			0		},	// 0x29: VK_SELECT
		{0,			0		},	// 0x2a: VK_PRINT
		{0,			0		},	// 0x2b: VK_EXECUTE
		{0,			0		},	// 0x2c: VK_SNAPSHOT
		{0,			0		},	// 0x2d: VK_INSERT
		{0,			0		},	// 0x2e: VK_DELETE
		{0,			0		},	// 0x2f: VK_HELP
		{'0',		0	 	},	// 0x30: '0'
		{'1',		'!' 	},	// 0x31: '1'
		{'2',		'\"' 	},	// 0x32: '2'
		{'3',		'#' 	},	// 0x33: '3'
		{'4',		'$' 	},	// 0x34: '4'
		{'5',		'%' 	},	// 0x35: '5'
		{'6',		'&' 	},	// 0x36: '6'
		{'7',		'\'' 	},	// 0x37: '7'
		{'8',		'(' 	},	// 0x38: '8'
		{'9',		')' 	},	// 0x39: '9'
		{0,			0		},	// 0x3a
		{0,			0		},	// 0x3b
		{0,			0		},	// 0x3c
		{0,			0		},	// 0x3d
		{0,			0		},	// 0x3e
		{0,			0		},	// 0x3f
		{0,			0		},	// 0x40
		{'a',		'A' 	},	// 0x41: 'A'
		{'b',		'B' 	},	// 0x42: 'B'
		{'c',		'C' 	},	// 0x43: 'C'
		{'d',		'D' 	},	// 0x44: 'D'
		{'e',		'E' 	},	// 0x45: 'E'
		{'f',		'F' 	},	// 0x46: 'F'
		{'g',		'G' 	},	// 0x47: 'G'
		{'h',		'H' 	},	// 0x48: 'H'
		{'i',		'I' 	},	// 0x49: 'I'
		{'j',		'J' 	},	// 0x4a: 'J'
		{'k',		'K' 	},	// 0x4b: 'K'
		{'l',		'L' 	},	// 0x4c: 'L'
		{'m',		'M' 	},	// 0x4d: 'M'
		{'n',		'N' 	},	// 0x4e: 'N'
		{'o',		'O' 	},	// 0x4f: 'O'
		{'p',		'P' 	},	// 0x50: 'P'
		{'q',		'Q' 	},	// 0x51: 'Q'
		{'r',		'R' 	},	// 0x52: 'R'
		{'s',		'S' 	},	// 0x53: 'S'
		{'t',		'T' 	},	// 0x54: 'T'
		{'u',		'U' 	},	// 0x55: 'U'
		{'v',		'V' 	},	// 0x56: 'V'
		{'w',		'W' 	},	// 0x57: 'W'
		{'x',		'X' 	},	// 0x58: 'X'
		{'y',		'Y' 	},	// 0x59: 'Y'
		{'z',		'Z' 	},	// 0x5a: 'Z'
		{0,			0		},	// 0x5b: VK_LWIN
		{0,			0		},	// 0x5c: VK_RWIN
		{0,			0		},	// 0x5d: VK_APPS
		{0,			0		},	// 0x5e
		{0,			0		},	// 0x5f: VK_SLEEP
		{'0',		0		},	// 0x60: VK_NUMPAD0
		{'1',		0		},	// 0x61: VK_NUMPAD1
		{'2',		0		},	// 0x62: VK_NUMPAD2
		{'3',		0		},	// 0x63: VK_NUMPAD3
		{'4',		0		},	// 0x64: VK_NUMPAD4
		{'5',		0		},	// 0x65: VK_NUMPAD5
		{'6',		0		},	// 0x66: VK_NUMPAD6
		{'7',		0		},	// 0x67: VK_NUMPAD7
		{'8',		0		},	// 0x68: VK_NUMPAD8
		{'9',		0		},	// 0x69: VK_NUMPAD9
		{'*',		0		},	// 0x6a: VK_MULTIPLY
		{'+',		0		},	// 0x6b: VK_ADD
		{',',		0		},	// 0x6c: VK_SEPARATOR
		{'-',		0		},	// 0x6d: VK_SUBTRACT
		{'.',		0		},	// 0x6e: VK_DECIMAL
		{'/',		0		},	// 0x6f: VK_DIVIDE
		{0,			0		},	// 0x70: VK_F1
		{0,			0		},	// 0x71: VK_F2
		{0,			0		},	// 0x72: VK_F3
		{0,			0		},	// 0x73: VK_F4
		{0,			0		},	// 0x74: VK_F5
		{0,			0		},	// 0x75: VK_F6
		{0,			0		},	// 0x76: VK_F7
		{0,			0		},	// 0x77: VK_F8
		{0,			0		},	// 0x78: VK_F9
		{0,			0		},	// 0x79: VK_F10
		{0,			0		},	// 0x7a: VK_F11
		{0,			0		},	// 0x7b: VK_F12
		{0,			0		},	// 0x7c: VK_F13
		{0,			0		},	// 0x7d: VK_F14
		{0,			0		},	// 0x7e: VK_F15
		{0,			0		},	// 0x7f: VK_F16
		{0,			0		},	// 0x80: VK_F17
		{0,			0		},	// 0x81: VK_F18
		{0,			0		},	// 0x82: VK_F19
		{0,			0		},	// 0x83: VK_F20
		{0,			0		},	// 0x84: VK_F21
		{0,			0		},	// 0x85: VK_F22
		{0,			0		},	// 0x86: VK_F23
		{0,			0		},	// 0x87: VK_F24
		{0,			0		},	// 0x88
		{0,			0		},	// 0x89
		{0,			0		},	// 0x8a
		{0,			0		},	// 0x8b
		{0,			0		},	// 0x8c
		{0,			0		},	// 0x8d
		{0,			0		},	// 0x8e
		{0,			0		},	// 0x8f
		{0,			0		},	// 0x90: VK_NUMLOCK
		{0,			0		},	// 0x91: VK_SCROLL
		{0,			0		},	// 0x92
		{0,			0		},	// 0x93
		{0,			0		},	// 0x94
		{0,			0		},	// 0x95
		{0,			0		},	// 0x96
		{0,			0		},	// 0x97
		{0,			0		},	// 0x98
		{0,			0		},	// 0x99
		{0,			0		},	// 0x9a
		{0,			0		},	// 0x9b
		{0,			0		},	// 0x9c
		{0,			0		},	// 0x9d
		{0,			0		},	// 0x9e
		{0,			0		},	// 0x9f
		{0,			0		},	// 0xa0: VK_LSHIFT
		{0,			0		},	// 0xa1: VK_RSHIFT
		{0,			0		},	// 0xa2: VK_LCONTROL
		{0,			0		},	// 0xa3: VK_RCONTROL
		{0,			0		},	// 0xa4: VK_LMENU
		{0,			0		},	// 0xa5: VK_RMENU
		{0,			0		},	// 0xa6: VK_BROWSER_BACK
		{0,			0		},	// 0xa7: VK_BROWSER_FORWARD
		{0,			0		},	// 0xa8: VK_BROWSER_REFRESH
		{0,			0		},	// 0xa9: VK_BROWSER_STOP
		{0,			0		},	// 0xaa: VK_BROWSER_SEARCH
		{0,			0		},	// 0xab: VK_BROWSER_FAVORITES
		{0,			0		},	// 0xac: VK_BROWSER_HOME
		{0,			0		},	// 0xad: VK_VOLUME_MUTE
		{0,			0		},	// 0xae: VK_VOLUME_DOWN
		{0,			0		},	// 0xaf: VK_VOLUME_UP
		{0,			0		},	// 0xb0: VK_MEDIA_NEXT_TRACK
		{0,			0		},	// 0xb1: VK_MEDIA_PREV_TRACK
		{0,			0		},	// 0xb2: VK_MEDIA_STOP
		{0,			0		},	// 0xb3: VK_MEDIA_PLAY_PAUSE
		{0,			0		},	// 0xb4: VK_LAUNCH_MAIL
		{0,			0		},	// 0xb5: VK_LAUNCH_MEDIA_SELECT
		{0,			0		},	// 0xb6: VK_LAUNCH_APP1
		{0,			0		},	// 0xb7: VK_LAUNCH_APP2
		{0,			0		},	// 0xb8
		{0,			0		},	// 0xb9
		{':',		'*'		},	// 0xba: VK_OEM_1
		{';',		'+'		},	// 0xbb: VK_OEM_PLUS
		{',',		'<'		},	// 0xbc: VK_OEM_COMMA
		{'-',		'='		},	// 0xbd: VK_OEM_MINUS
		{'.',		'>'		},	// 0xbe: VK_OEM_PERIOD
		{'/',		'?'		},	// 0xbf: VK_OEM_2
		{'@',		'`'		},	// 0xc0: VK_OEM_3
		{0,			0		},	// 0xc1
		{0,			0		},	// 0xc2
		{0,			0		},	// 0xc3
		{0,			0		},	// 0xc4
		{0,			0		},	// 0xc5
		{0,			0		},	// 0xc6
		{0,			0		},	// 0xc7
		{0,			0		},	// 0xc8
		{0,			0		},	// 0xc9
		{0,			0		},	// 0xca
		{0,			0		},	// 0xcb
		{0,			0		},	// 0xcc
		{0,			0		},	// 0xcd
		{0,			0		},	// 0xce
		{0,			0		},	// 0xcf
		{0,			0		},	// 0xd0
		{0,			0		},	// 0xd1
		{0,			0		},	// 0xd2
		{0,			0		},	// 0xd3
		{0,			0		},	// 0xd4
		{0,			0		},	// 0xd5
		{0,			0		},	// 0xd6
		{0,			0		},	// 0xd7
		{0,			0		},	// 0xd8
		{0,			0		},	// 0xd9
		{0,			0		},	// 0xda
		{'[',		'{'		},	// 0xdb: VK_OEM_4
		{'\\',		'|'		},	// 0xdc: VK_OEM_5
		{']',		'}'		},	// 0xdd: VK_OEM_6
		{'^',		'~'		},	// 0xde: VK_OEM_7
		{0,			0		},	// 0xdf: VK_OEM_8
		{0,			0		},	// 0xe0
		{0,			0		},	// 0xe1
		{'\\',		'_'		},	// 0xe2: VK_OEM_102
		{0,			0		},	// 0xe3
		{0,			0		},	// 0xe4
		{0,			0		},	// 0xe5: VK_PROCESSKEY
		{0,			0		},	// 0xe6
		{0,			0		},	// 0xe7: VK_PACKET
		{0,			0		},	// 0xe8
		{0,			0		},	// 0xe9
		{0,			0		},	// 0xea
		{0,			0		},	// 0xeb
		{0,			0		},	// 0xec
		{0,			0		},	// 0xed
		{0,			0		},	// 0xee
		{0,			0		},	// 0xef
		{0,			0		},	// 0xf0
		{0,			0		},	// 0xf1
		{0,			0		},	// 0xf2
		{0,			0		},	// 0xf3
		{0,			0		},	// 0xf4
		{0,			0		},	// 0xf5
		{0,			0		},	// 0xf6: VK_ATTN
		{0,			0		},	// 0xf7: VK_CRSEL
		{0,			0		},	// 0xf8: VK_EXSEL
		{0,			0		},	// 0xf9: VK_EREOF
		{0,			0		},	// 0xfa: VK_PLAY
		{0,			0		},	// 0xfb: VK_ZOOM
		{0,			0		},	// 0xfc: VK_NONAME
		{0,			0		},	// 0xfd: VK_PA1
		{0,			0		},	// 0xfe: VK_OEM_CLEAR
		{0,			0		},	// 0xff
	};
	return charEntryTbl;
}

}
