//==============================================================================
// Serial.cpp
//==============================================================================
#include "jxglib/Serial.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Serial
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Serial::Terminal
//------------------------------------------------------------------------------
bool Serial::Terminal::Initialize()
{
	return Editable::Initialize();
}

Editable& Serial::Terminal::Edit_Begin()
{
	GetLineEditor().Begin();
	VT100::SaveCursorPosition(printable_);
	return *this;
}

Editable& Serial::Terminal::Edit_Finish(char chEnd)
{
	printable_.PutChar(chEnd);
	GetLineEditor().Finish();
	return *this;
}

Editable& Serial::Terminal::Edit_InsertChar(int ch)
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().InsertChar(ch)) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_DeleteChar()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteChar()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		VT100::CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_Back()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().Back()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		VT100::CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_MoveForward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveForward()) {
		VT100::CursorForward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_MoveBackward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveBackward()) {
		VT100::CursorBackward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_MoveHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHome()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_MoveEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveEnd()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_DeleteToHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToHome()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		VT100::RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_DeleteToEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToEnd()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_MoveHistoryPrev()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryPrev()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		VT100::CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& Serial::Terminal::Edit_MoveHistoryNext()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryNext()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		VT100::CursorBackward(printable_, GetLineEditor().CountFollowingChars());
		printable_.RefreshScreen();
	}
	return *this;
}

void Serial::Terminal::OnTick()
{
	int ch;
	while ((ch = ::stdio_getchar_timeout_us(0)) > 0) decoder_.FeedChar(ch);
	Keyboard::KeyData keyData;
	if (decoder_.GetKeyData(keyData)) FeedKeyData(keyData);
}

}
