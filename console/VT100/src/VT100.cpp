//==============================================================================
// VT100.cpp
//==============================================================================
#include <string.h>
#include "jxglib/VT100.h"

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

void VT100::Decoder::FeedChar(char ch)
{
	bool contFlag = false;
	//::printf("%02x %c\n", ch, (ch == 0x1b)? 'e' : ch);
	do {
		contFlag = false;
		switch (stat_) {
		case Stat::First: {
			switch (ch) {
			case 0x08: {
				buff_.WriteData(VK_BACK);
				break;
			}
			case 0x0d: {
				buff_.WriteData(VK_RETURN);
				break;
			}
			case 0x1b: {
				stat_ = Stat::Escape;
				break;
			}
			case 0x7f: {
				buff_.WriteData(VK_DELETE);
				break;
			}
			default: {
				buff_.WriteData(OffsetForAscii + ch);
				break;
			}
			}
			break;
		}
		case Stat::Escape: {
			switch (ch) {
			case 0x1b: {
				buff_.WriteData(0x1b);
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
				break;
			}
			case 'B': {
				buff_.WriteData(VK_DOWN);
				break;
			}
			case 'C': {
				buff_.WriteData(VK_RIGHT);
				break;
			}
			case 'D': {
				buff_.WriteData(VK_LEFT);
				break;
			}
			case '~': {
				if (::strcmp(buffParameter_, "1") == 0) {
					buff_.WriteData(VK_HOME);
				} else if (::strcmp(buffParameter_, "4") == 0) {
					buff_.WriteData(VK_END);
				} else if (::strcmp(buffParameter_, "5") == 0) {
					buff_.WriteData(VK_PRIOR);
				} else if (::strcmp(buffParameter_, "6") == 0) {
					buff_.WriteData(VK_NEXT);
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
}

bool VT100::Decoder::GetKeyData(int* pKeyData)
{
	*pKeyData = buff_.ReadData();
	if (*pKeyData < OffsetForAscii) return true;
	*pKeyData -= OffsetForAscii;
	return false;
}

//------------------------------------------------------------------------------
// VT100::Terminal
//------------------------------------------------------------------------------
bool VT100::Terminal::Initialize()
{
	if (!Editable::Initialize()) return false;
	Tickable::AddTickable(*this);
	return true;
}

Editable& VT100::Terminal::Edit_Begin()
{
	GetLineEditor().Begin();
	SaveCursorPosition(printable_);
	return *this;
}

Editable& VT100::Terminal::Edit_Finish(char chEnd)
{
	printable_.PutChar(chEnd);
	GetLineEditor().Finish();
	return *this;
}

Editable& VT100::Terminal::Edit_InsertChar(int ch)
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().InsertChar(ch)) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_DeleteChar()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteChar()) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		EraseToEndOfLine(printable_);
		CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_Back()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().Back()) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		EraseToEndOfLine(printable_);
		CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveForward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveForward()) {
		CursorForward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveBackward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveBackward()) {
		CursorBackward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHome()) {
		RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveEnd()) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_DeleteToHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToHome()) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		EraseToEndOfLine(printable_);
		RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_DeleteToEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToEnd()) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		EraseToEndOfLine(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveHistoryPrev()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryPrev()) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		EraseToEndOfLine(printable_);
		CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveHistoryNext()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryNext()) {
		RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		EraseToEndOfLine(printable_);
		CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

void VT100::Terminal::OnTick()
{
	UART& uart = UART::Default;
	int keyData;
	while (uart.raw.is_readable()) decoder_.FeedChar(uart.raw.getc());
	if (!decoder_.HasKeyData()) {
		// nothing to do
	} else if (decoder_.GetKeyData(&keyData)) {
		switch (keyData) {
		case VK_RETURN:	Edit_Finish('\n');		break;
		case VK_DELETE:	Edit_DeleteChar();		break;
		case VK_BACK:	Edit_Back();			break;
		case VK_LEFT:	Edit_MoveBackward();	break;
		case VK_RIGHT:	Edit_MoveForward();		break;
		case VK_UP:		Edit_MoveHistoryPrev();	break;
		case VK_DOWN:	Edit_MoveHistoryNext();	break;
		//case VK_PRIOR:RollUp();				break;
		//case VK_NEXT:	RollDown();				break;
		default: break;
		}
	} else if (keyData < 0x20) {
		switch (keyData + '@') {
		case 'A':		Edit_MoveHome();		break;
		case 'B':		Edit_MoveBackward();	break;
		case 'D':		Edit_DeleteChar();		break;
		case 'E':		Edit_MoveEnd();			break;
		case 'F':		Edit_MoveForward();		break;
		case 'J':		Edit_Finish('\n');		break;
		case 'K':		Edit_DeleteToEnd();		break;
		case 'N':		Edit_MoveHistoryNext();	break;
		case 'P':		Edit_MoveHistoryPrev();	break;
		case 'U':		Edit_DeleteToHome();	break;
		default: break;
		}
	} else {
		Edit_InsertChar(keyData);
	}
}

}
