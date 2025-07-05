//==============================================================================
// VT100.cpp
//==============================================================================
#include <string.h>
#include "jxglib/VT100.h"
#include "jxglib/Printable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// VT100
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// VT100::Decoder
//------------------------------------------------------------------------------
VT100::Decoder::Decoder() : stat_{Stat::First}
{
}

bool VT100::Decoder::FeedChar(char ch)
{
	bool rtn = false;
	bool contFlag = false;
	//::printf("%02x %c\n", ch, (ch == 0x1b)? 'e' : ch);
	do {
		contFlag = false;
		switch (stat_) {
		case Stat::First: {
			switch (ch) {
			case 0x08: {
				buff_.WriteData(VK_BACK);
				rtn = true;
				break;
			}
			case 0x0d: {
				buff_.WriteData(VK_RETURN);
				rtn = true;
				break;
			}
			case 0x1b: {
				stat_ = Stat::Escape;
				break;
			}
			case 0x7f: {
				buff_.WriteData(VK_DELETE);
				rtn = true;
				break;
			}
			default: {
				buff_.WriteData(OffsetForAscii + ch);
				rtn = true;
				break;
			}
			}
			break;
		}
		case Stat::Escape: {
			switch (ch) {
			case 0x1b: {
				buff_.WriteData(0x1b);
				rtn = true;
				stat_ = Stat::First;
				break;
			}
			case 'N': {
				stat_ = Stat::SS2;
				break;
			}
			case 'O': {
				stat_ = Stat::SS3;
				break;
			}
			case 'P': {
				stat_ = Stat::DCS;
				break;
			}
			case '[': {
				iBuffParameter_ = iBuffIntermediate_ = 0;
				stat_ = Stat::CSI_Parameter;
				break;
			}
			case '\\': {
				stat_ = Stat::ST;
				break;
			}
			case ']': {
				stat_ = Stat::OSC;
				break;
			}
			case 'X': {
				stat_ = Stat::SOS;
				break;
			}
			case '^': {
				stat_ = Stat::PM;
				break;
			}
			case '_': {
				stat_ = Stat::APC;
				break;
			}
			default: {
				stat_ = Stat::First;
				break;
			}
			}
			break;
		}
		case Stat::SS2: {
			stat_ = Stat::First;
			break;
		}
		case Stat::SS3: {
			switch (ch) {
			default: {
				stat_ = Stat::First;
				break;
			}
			}
			break;
		}
		case Stat::DCS: {
			stat_ = Stat::First;
			break;
		}
		case Stat::CSI_Parameter: {
			if (0x30 <= ch && ch <= 0x3f) {
				if (iBuffParameter_ < count_of(buffParameter_) - 1) buffParameter_[iBuffParameter_++] = ch;
			} else {
				buffParameter_[iBuffParameter_] = '\0';
				contFlag = true;
				stat_ = Stat::CSI_Intermediate;
			}
			break;
		}
		case Stat::CSI_Intermediate: {
			if (0x20 <= ch && ch <= 0x2f) {
				if (iBuffIntermediate_ < count_of(buffIntermediate_) - 1) buffIntermediate_[iBuffIntermediate_++] = ch;
			} else {
				buffIntermediate_[iBuffIntermediate_] = '\0';
				contFlag = true;
				stat_ = Stat::CSI_Final;
			}
			break;
		}
		case Stat::CSI_Final: {
			switch (ch) {
			case 'A': {
				buff_.WriteData(VK_UP);
				rtn = true;
				break;
			}
			case 'B': {
				buff_.WriteData(VK_DOWN);
				rtn = true;
				break;
			}
			case 'C': {
				buff_.WriteData(VK_RIGHT);
				rtn = true;
				break;
			}
			case 'D': {
				buff_.WriteData(VK_LEFT);
				rtn = true;
				break;
			}
			case '~': {
				if (::strcmp(buffParameter_, "1") == 0) {
					buff_.WriteData(VK_HOME);
					rtn = true;
				} else if (::strcmp(buffParameter_, "4") == 0) {
					buff_.WriteData(VK_END);
					rtn = true;
				} else if (::strcmp(buffParameter_, "5") == 0) {
					buff_.WriteData(VK_PRIOR);
					rtn = true;
				} else if (::strcmp(buffParameter_, "6") == 0) {
					buff_.WriteData(VK_NEXT);
					rtn = true;
				} else {
				}
				break;
			}
			}
			stat_ = Stat::First;
			break;
		}
		case Stat::ST: {
			stat_ = Stat::First;
			break;
		}
		case Stat::OSC: {
			stat_ = Stat::First;
			break;
		}
		case Stat::SOS: {
			stat_ = Stat::First;
			break;
		}
		case Stat::PM: {
			stat_ = Stat::First;
			break;
		}
		case Stat::APC: {
			stat_ = Stat::First;
			break;
		}
		default: {
			stat_ = Stat::First;
			break;
		}
		}
	} while (contFlag);
	return rtn;
}

bool VT100::Decoder::GetKeyData(KeyData* pKeyData)
{
	if (HasKeyData()) {
		int code = buff_.ReadData();
		bool keyCodeFlag = (code < OffsetForAscii);
		if (!keyCodeFlag) code -= OffsetForAscii;
		const uint8_t modifier = 0;
		*pKeyData = KeyData(code, keyCodeFlag, modifier);
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
// VT100::Keyboard
//------------------------------------------------------------------------------
int VT100::Keyboard::SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax, bool includeModifiers)
{
	if (nKeysMax == 0) return 0;
	int nKeys = 0;
	KeyData keyData;
	if (GetKeyDataNB(&keyData) && keyData.IsKeyCode()) keyCodeTbl[nKeys++] = keyData.GetKeyCode();
	return nKeys;
}

int VT100::Keyboard::SenseKeyData(KeyData keyDataTbl[], int nKeysMax)
{
	return (nKeysMax > 0 && GetKeyDataNB(&keyDataTbl[0]))? 1 : 0;
}

bool VT100::Keyboard::GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier)
{
	KeyData keyData;
	if (!GetKeyDataNB(&keyData)) {
		// nothing to do
	} else if (keyData.IsKeyCode()) {
		return keyData.GetKeyCode();
	} else {
		char ch = keyData.GetChar();
		if (('0' <= ch && ch <= '9') || ('A' <= ch && ch <= 'Z') ||
				ch == VK_BACK || ch == VK_TAB || ch == VK_RETURN || ch == VK_ESCAPE || ch == VK_SPACE) {
			*pKeyCode = static_cast<uint8_t>(ch);
			return true;
		} else if ('a' <= ch && ch <= 'z') {
			*pKeyCode = static_cast<uint8_t>(ch - 'a' + 'A');
			return true;
		}
	}
	return false;
}

bool VT100::Keyboard::GetKeyDataNB(KeyData* pKeyData)
{
	int ch;
	while ((ch = readable_.ReadChar()) > 0 && !decoder_.FeedChar(ch)) ;
	return decoder_.GetKeyData(pKeyData);
}

//------------------------------------------------------------------------------
// Cursor movement
//------------------------------------------------------------------------------
void VT100::CursorUp(Printable& printable, int nRows) { if (nRows > 0) printable.Printf("\x1b[%dA", nRows); }
void VT100::CursorDown(Printable& printable, int nRows) { if (nRows > 0) printable.Printf("\x1b[%dB", nRows); }
void VT100::CursorForward(Printable& printable, int nCols) { if (nCols > 0) printable.Printf("\x1b[%dC", nCols); }
void VT100::CursorBackward(Printable& printable, int nCols) { if (nCols > 0) printable.Printf("\x1b[%dD", nCols); }
void VT100::CursorNextLine(Printable& printable, int nRows) { if (nRows > 0) printable.Printf("\x1b[%dE", nRows); }
void VT100::CursorPreviousLine(Printable& printable, int nRows) { if (nRows > 0) printable.Printf("\x1b[%dF", nRows); }
void VT100::CursorHorizontalAbsolute(Printable& printable, int n) { printable.Printf("\x1b[%dG", n); }
void VT100::CursorPosition(Printable& printable, int row, int column) { printable.Printf("\x1b[%d;%dH", row, column); }
// Line erasure
void VT100::EraseToEndOfLine(Printable& printable) { printable.Printf("\x1b[0K"); }
void VT100::EraseToBeginningOfLine(Printable& printable) { printable.Printf("\x1b[1K"); }
void VT100::EraseCurrentLine(Printable& printable) { printable.Printf("\x1b[2K"); }
// Screen erasure
void VT100::EraseFromCursorToEndOfScreen(Printable& printable) { printable.Printf("\x1b[0J"); }
void VT100::EraseFromBeginningOfScreenToCursor(Printable& printable) { printable.Printf("\x1b[1J"); }
void VT100::EraseScreen(Printable& printable) { printable.Printf("\x1b[2J"); }
void VT100::EraseScreenAndScrollbackBuffer(Printable& printable) { printable.Printf("\x1b[3J"); }
// Character erasure
void VT100::DeleteCharacters(Printable& printable, int nChars) { if (nChars > 0) printable.Printf("\x1b[%dP", nChars); }
void VT100::EraseCharacters(Printable& printable, int nChars) { if (nChars > 0) printable.Printf("\x1b[%dX", nChars); }
// Scrolling
void VT100::ScrollUp(Printable& printable, int nRows) { if (nRows > 0) printable.Printf("\x1b[%dS", nRows); }
void VT100::ScrollDown(Printable& printable, int nRows) { if (nRows > 0) printable.Printf("\x1b[%dT", nRows); }
// Other
void VT100::SaveCursorPosition(Printable& printable) { printable.Printf("\x1b[s"); }
void VT100::RestoreCursorPosition(Printable& printable) { printable.Printf("\x1b[u"); }

}
