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
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		MoveToEnd();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_DeleteChar()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteChar()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		MoveToEnd();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_Back()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().Back()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		MoveToEnd();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveForward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveForward()) {
		VT100::CursorForward(GetPrintable());
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveBackward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveBackward()) {
		VT100::CursorBackward(GetPrintable());
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHome()) {
		MoveToBegin();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveEnd()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_Clear()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().Clear()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		MoveToBegin();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_DeleteToHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToHome()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		MoveToBegin();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_DeleteToEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().DeleteToEnd()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveHistoryPrev()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryPrev()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		MoveToEnd();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_MoveHistoryNext()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (GetLineEditor().MoveHistoryNext()) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		MoveToEnd();
		GetPrintable().Flush();
		EndCompletion();
	}
	return *this;
}

Terminal& Serial::Terminal::Edit_Completion()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (!pCompletionProvider_) return *this;
	if (!pCompletionProvider_->IsStarted()) {
		pCompletionProvider_->Start(*this, GetLineEditor().GetIByteToCompletion());
	}
	const char* strCompletion = pCompletionProvider_->NextCompletion();
	if (strCompletion && GetLineEditor().ReplaceWithCompletion(pCompletionProvider_->GetIByte(), strCompletion)) {
		MoveToBegin();
		GetPrintable().Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(GetPrintable());
		MoveToEnd();
		GetPrintable().Flush();
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
