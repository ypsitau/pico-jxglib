#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Editable.h"
#include "jxglib/UART.h"
#include "jxglib/VT100.h"

using namespace jxglib;

class EditableEx : public Editable, public Tickable {
private:
	Printable& printable_;
	VT100::Decoder decoder_;
public:
	EditableEx(Printable& printable) : Tickable(0), printable_{printable} {}
public:
	bool Initialize();
public:
	// virtual functions of Editable
	virtual Editable& Edit_Begin();
	virtual Editable& Edit_Finish(char chEnd = '\0');
	virtual Editable& Edit_InsertChar(int ch);
	virtual Editable& Edit_DeleteChar();
	virtual Editable& Edit_Back();
	virtual Editable& Edit_MoveForward();
	virtual Editable& Edit_MoveBackward();
	virtual Editable& Edit_MoveHome();
	virtual Editable& Edit_MoveEnd();
	virtual Editable& Edit_DeleteToHome();
	virtual Editable& Edit_DeleteToEnd();
	virtual Editable& Edit_MoveHistoryPrev();
	virtual Editable& Edit_MoveHistoryNext();
public:
	// virtual function of Tickable
	virtual void OnTick() override;
};

bool EditableEx::Initialize()
{
	if (!Editable::Initialize()) return false;
	Tickable::AddTickable(*this);
	return true;
}

Editable& EditableEx::Edit_Begin()
{
	VT100::SaveCursorPosition(printable_);
	return *this;
}

Editable& EditableEx::Edit_Finish(char chEnd)
{
	printable_.PutChar(chEnd);
	return *this;
}

Editable& EditableEx::Edit_InsertChar(int ch)
{
	int nChars = GetLineEditor().CountFollowingChars();
	if (GetLineEditor().InsertChar(ch)) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::CursorBackward(printable_, nChars);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& EditableEx::Edit_DeleteChar()
{
	int nChars = GetLineEditor().CountFollowingChars();
	if (GetLineEditor().DeleteChar()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		VT100::CursorBackward(printable_, nChars - 1);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& EditableEx::Edit_Back()
{
	int nChars = GetLineEditor().CountFollowingChars();
	if (GetLineEditor().Back()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.Print(GetLineEditor().GetPointerBegin());
		VT100::EraseToEndOfLine(printable_);
		VT100::CursorBackward(printable_, nChars);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& EditableEx::Edit_MoveForward()
{
	if (GetLineEditor().MoveForward()) {
		VT100::CursorForward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& EditableEx::Edit_MoveBackward()
{
	if (GetLineEditor().MoveBackward()) {
		VT100::CursorBackward(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& EditableEx::Edit_MoveHome()
{
	if (GetLineEditor().MoveHome()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& EditableEx::Edit_MoveEnd()
{
	if (GetLineEditor().MoveEnd()) {
		VT100::RestoreCursorPosition(printable_);
		printable_.RefreshScreen();
	}
	return *this;
}

Editable& EditableEx::Edit_DeleteToHome()
{
	return *this;
}

Editable& EditableEx::Edit_DeleteToEnd()
{
	return *this;
}

Editable& EditableEx::Edit_MoveHistoryPrev()
{
	return *this;
}

Editable& EditableEx::Edit_MoveHistoryNext()
{
	return *this;
}

void EditableEx::OnTick()
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
	EditableEx editable(UART::Default);
	editable.Initialize();
	::stdio_init_all();
	for (;;) Tickable::Tick();
}
