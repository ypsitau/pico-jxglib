#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"
#include "jxglib/VT100.h"

using namespace jxglib;


bool VT100::Terminal::Initialize()
{
	if (!Editable::Initialize()) return false;
	Tickable::AddTickable(*this);
	return true;
}

Editable& VT100::Terminal::Edit_Begin()
{
	SaveCursorPosition(printable_);
	return *this;
}

Editable& VT100::Terminal::Edit_Finish(char chEnd)
{
	printable_.PutChar(chEnd);
	return *this;
}

Editable& VT100::Terminal::Edit_InsertChar(int ch)
{
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
	if (GetLineEditor().MoveForward()) {
		CursorForward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveBackward()
{
	if (GetLineEditor().MoveBackward()) {
		CursorBackward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveHome()
{
	if (GetLineEditor().MoveHome()) {
		RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_MoveEnd()
{
	if (GetLineEditor().MoveEnd()) {
		RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& VT100::Terminal::Edit_DeleteToHome()
{
	return *this;
}

Editable& VT100::Terminal::Edit_DeleteToEnd()
{
	return *this;
}

Editable& VT100::Terminal::Edit_MoveHistoryPrev()
{
	return *this;
}

Editable& VT100::Terminal::Edit_MoveHistoryNext()
{
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
		//case VK_PRIOR:	RollUp();					break;
		//case VK_NEXT:		RollDown();				break;
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

int main()
{
	VT100::Terminal terminal(UART::Default);
	terminal.Initialize();
	::stdio_init_all();
	for (;;) Tickable::Tick();
}
