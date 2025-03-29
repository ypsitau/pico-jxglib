//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Printable.h"
#include "jxglib/Editable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
class Terminal : public Printable, public Editable {
private:
	Keyboard* pKeyboard_;
public:
	Terminal(int bytesHistoryBuff, Keyboard& keyboard);
public:
	void AttachInput(Keyboard& keyboard) { pKeyboard_ = &keyboard; }
	Keyboard& GetKeyboard() { return *pKeyboard_; }
	char GetChar() { return GetKeyboard().GetChar(); }
	bool GetKeyDataNB(KeyData* pKeyData) { return GetKeyboard().GetKeyDataNB(pKeyData); }
	int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) {
		return GetKeyboard().SenseKeyData(keyDataTbl, nKeysMax);
	}
};

//------------------------------------------------------------------------------
// TerminalDumb
//------------------------------------------------------------------------------
class TerminalDumb : public Terminal {
public:
	static TerminalDumb Instance;
public:
	TerminalDumb();
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() { return *this; }
	virtual Printable& RefreshScreen() { return *this; }
	virtual Printable& Locate(int col, int row) { return *this; }
	virtual Printable& PutChar(char ch) { return *this; }
	virtual Printable& PutCharRaw(char ch) { return *this; }
	virtual Printable& Print(const char* str) { return *this; }
	virtual Printable& PrintRaw(const char* str) { return *this; }
	virtual Printable& Println(const char* str = "") { return *this; }
	virtual Printable& PrintlnRaw(const char* str = "") { return *this; }
	virtual Printable& VPrintf(const char* format, va_list args) { return *this; }
	virtual Printable& VPrintfRaw(const char* format, va_list args) { return *this; }
public:
	// virtual functions of Editable
	virtual Printable& GetPrintable() override { return *this; }
	virtual Editable& Edit_Begin() override { return *this; }
	virtual Editable& Edit_Finish(char chEnd = '\0') override { return *this; }
	virtual Editable& Edit_InsertChar(int ch) override { return *this; }
	virtual Editable& Edit_DeleteChar() override { return *this; }
	virtual Editable& Edit_Back() override { return *this; }
	virtual Editable& Edit_MoveForward() override { return *this; }
	virtual Editable& Edit_MoveBackward() override { return *this; }
	virtual Editable& Edit_MoveHome() override { return *this; }
	virtual Editable& Edit_MoveEnd() override { return *this; }
	virtual Editable& Edit_Clear() override { return *this; }
	virtual Editable& Edit_DeleteToHome() override { return *this; }
	virtual Editable& Edit_DeleteToEnd() override { return *this; }
	virtual Editable& Edit_MoveHistoryPrev() override { return *this; }
	virtual Editable& Edit_MoveHistoryNext() override { return *this; }
};

}

#endif
