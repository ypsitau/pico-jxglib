#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Editable.h"
#include "jxglib/UART.h"
#include "jxglib/VT100.h"

using namespace jxglib;

class EditableEx : public Editable {
public:
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
};

Editable& EditableEx::Edit_Begin()
{
	::printf("\033[s");	// save position
	return *this;
}

Editable& EditableEx::Edit_Finish(char chEnd)
{
	::printf("%c", chEnd);
	return *this;
}

Editable& EditableEx::Edit_InsertChar(int ch)
{
	if (GetLineEditor().InsertChar(ch)) {
		::printf("\033[u"); // return to saved position
		::printf("%s", GetLineEditor().GetPointerBegin());
		::fflush(stdout);
	}
	return *this;
}

Editable& EditableEx::Edit_DeleteChar()
{
	return *this;
}

Editable& EditableEx::Edit_Back()
{
	return *this;
}

Editable& EditableEx::Edit_MoveForward()
{
	if (GetLineEditor().MoveForward()) {
		::printf("\033[C");
		::fflush(stdout);
	}
	return *this;
}

Editable& EditableEx::Edit_MoveBackward()
{
	if (GetLineEditor().MoveBackward()) {
		::printf("\033[D");
		::fflush(stdout);
	}
	return *this;
}

Editable& EditableEx::Edit_MoveHome()
{
	if (GetLineEditor().MoveHome()) {
		::printf("\033[u"); // return to saved position
		::fflush(stdout);
	}
	return *this;
}

Editable& EditableEx::Edit_MoveEnd()
{
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

void OnTick(UART& uart, VT100::Decoder& decoder_, Editable& editable)
{
	//UART& uart = *pUART_;
	int keyData;
	while (uart.raw.is_readable()) decoder_.FeedChar(uart.raw.getc());
	if (!decoder_.HasKeyData()) {
		// nothing to do
	} else if (decoder_.GetKeyData(&keyData)) {
		switch (keyData) {
		case VK_RETURN:	editable.Edit_Finish('\n');			break;
		case VK_DELETE:	editable.Edit_DeleteChar();			break;
		case VK_BACK:	editable.Edit_Back();				break;
		case VK_LEFT:	editable.Edit_MoveBackward();		break;
		case VK_RIGHT:	editable.Edit_MoveForward();		break;
		case VK_UP:		editable.Edit_MoveHistoryPrev();	break;
		case VK_DOWN:	editable.Edit_MoveHistoryNext();	break;
		//case VK_PRIOR:	editable.RollUp();					break;
		//case VK_NEXT:	editable.RollDown();				break;
		default: break;
		}
	} else if (keyData < 0x20) {
		switch (keyData + '@') {
		case 'A':		editable.Edit_MoveHome();			break;
		case 'B':		editable.Edit_MoveBackward();		break;
		case 'D':		editable.Edit_DeleteChar();			break;
		case 'E':		editable.Edit_MoveEnd();			break;
		case 'F':		editable.Edit_MoveForward();		break;
		case 'J':		editable.Edit_Finish('\n');			break;
		case 'K':		editable.Edit_DeleteToEnd();		break;
		case 'N':		editable.Edit_MoveHistoryNext();	break;
		case 'P':		editable.Edit_MoveHistoryPrev();	break;
		case 'U':		editable.Edit_DeleteToHome();		break;
		default: break;
		}
	} else {
		editable.Edit_InsertChar(keyData);
	}
}

int main()
{
	VT100::Decoder decoder;
	EditableEx editable;
	editable.Initialize();
	::stdio_init_all();
	for (;;) {
		OnTick(UART::Default, decoder, editable);
	}
}
