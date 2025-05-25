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
Terminal& Serial::Terminal::Edit_Begin()
{
	GetLineEditor().Begin();
	VT100::SaveCursorPosition(GetPrintable());
	return *this;
}

Terminal& Serial::Terminal::Edit_Finish(char chEnd)
{
	GetPrintable().PutChar(chEnd);
	GetLineEditor().Finish();
	return *this;
}

Terminal& Serial::Terminal::Edit_InsertChar(int ch)
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().InsertChar(ch)) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::CursorBackward(GetPrintable(), GetLineEditor().CountFollowingChars());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_DeleteChar()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteChar()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		VT100::CursorBackward(GetPrintable(), GetLineEditor().CountFollowingChars());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_Back()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().Back()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		VT100::CursorBackward(GetPrintable(), GetLineEditor().CountFollowingChars());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveForward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveForward()) {
		VT100::CursorForward(GetPrintable());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveBackward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveBackward()) {
		VT100::CursorBackward(GetPrintable());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHome()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveEnd()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_Clear()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().Clear()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_DeleteToHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToHome()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_DeleteToEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToEnd()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveHistoryPrev()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryPrev()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		VT100::CursorBackward(GetPrintable(), GetLineEditor().CountFollowingChars());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveHistoryNext()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryNext()) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		VT100::CursorBackward(GetPrintable(), GetLineEditor().CountFollowingChars());
		GetPrintable().RefreshScreen();
		EndComplement();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_Complement()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (!pComplementProvider_) return *this;
	if (!pComplementProvider_->IsStarted()) {
		pComplementProvider_->Start(*this, GetLineEditor().GetIByteToComplement());
	}
	const char* strComplement = pComplementProvider_->NextComplement();
	if (strComplement && GetLineEditor().Replace(strComplement, pComplementProvider_->GetIByte())) {
		VT100::RestoreCursorPosition(GetPrintable());
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		VT100::CursorBackward(GetPrintable(), GetLineEditor().CountFollowingChars());
		GetPrintable().RefreshScreen();
	}
	return *this;
}

void Serial::Terminal::OnTick()
{
	if (!IsEditable() || !GetLineEditor().IsEditing()) return;
	KeyData keyData;
	if (GetKeyDataNB(&keyData)) ProcessKeyData(keyData);
}

}
