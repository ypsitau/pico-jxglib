//==============================================================================
// jxglib/Serial.h
//==============================================================================
#ifndef PICO_JXGLIB_SERIAL_H
#define PICO_JXGLIB_SERIAL_H
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/VT100.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Serial
//------------------------------------------------------------------------------
class Serial {
public:
	class Terminal : public jxglib::Terminal, public Tickable {
	private:
		Printable& printable_;
		Keyboard& keyboard_;
	public:
		Terminal(Printable& printable = PrintableStdio::Instance,
						Keyboard& keyboard = KeyboardStdio::Instance, int bytesHistoryBuff = 512) :
			jxglib::Terminal(bytesHistoryBuff), Tickable(0), printable_{printable}, keyboard_{keyboard} {}
	public:
		bool Initialize();
	public:
		// virtual functions of Printable
		virtual Printable& ClearScreen() { return printable_.ClearScreen(); }
		virtual Printable& RefreshScreen() { return printable_.RefreshScreen(); }
		virtual Printable& Locate(int col, int row) { return printable_.Locate(col, row); }
		virtual Printable& PutChar(char ch) { return printable_.PutChar(ch); }
		virtual Printable& PutCharRaw(char ch) { return printable_.PutCharRaw(ch); }
		virtual Printable& Print(const char* str) { return printable_.Print(str); }
		virtual Printable& PrintRaw(const char* str) { return printable_.PrintRaw(str); }
		virtual Printable& Println(const char* str = "") { return printable_.Println(str); }
		virtual Printable& PrintlnRaw(const char* str = "") { return printable_.PrintlnRaw(str); }
		virtual Printable& VPrintf(const char* format, va_list args) { return printable_.VPrintf(format, args); }
		virtual Printable& VPrintfRaw(const char* format, va_list args) { return printable_.VPrintfRaw(format, args); }
	public:
		// virtual functions of Editable
		virtual Printable& GetPrintable() override { return printable_; }
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
public:
	Serial() {}
};

}

#endif
