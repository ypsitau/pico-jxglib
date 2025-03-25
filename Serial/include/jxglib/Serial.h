//==============================================================================
// jxglib/Serial.h
//==============================================================================
#ifndef PICO_JXGLIB_SERIAL_H
#define PICO_JXGLIB_SERIAL_H
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
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
		Printable* pPrintable_;
	public:
		Terminal(int bytesHistoryBuff = 512) : jxglib::Terminal(bytesHistoryBuff, Stdio::Keyboard::Instance),
				Tickable(0), pPrintable_{&PrintableStdio::Instance} {}
	public:
		bool Initialize();
	public:
		void AttachOutput(Printable& printable) { pPrintable_ = &printable; }
	public:
		// virtual functions of Printable
		virtual Printable& ClearScreen() { return pPrintable_->ClearScreen(); }
		virtual Printable& RefreshScreen() { return pPrintable_->RefreshScreen(); }
		virtual Printable& Locate(int col, int row) { return pPrintable_->Locate(col, row); }
		virtual Printable& PutChar(char ch) { return pPrintable_->PutChar(ch); }
		virtual Printable& PutCharRaw(char ch) { return pPrintable_->PutCharRaw(ch); }
		virtual Printable& Print(const char* str) { return pPrintable_->Print(str); }
		virtual Printable& PrintRaw(const char* str) { return pPrintable_->PrintRaw(str); }
		virtual Printable& Println(const char* str = "") { return pPrintable_->Println(str); }
		virtual Printable& PrintlnRaw(const char* str = "") { return pPrintable_->PrintlnRaw(str); }
		virtual Printable& VPrintf(const char* format, va_list args) { return pPrintable_->VPrintf(format, args); }
		virtual Printable& VPrintfRaw(const char* format, va_list args) { return pPrintable_->VPrintfRaw(format, args); }
	public:
		// virtual functions of Editable
		virtual Printable& GetPrintable() override { return *pPrintable_; }
		virtual Editable& Edit_Begin();
		virtual Editable& Edit_Finish(char chEnd = '\0');
		virtual Editable& Edit_InsertChar(int ch);
		virtual Editable& Edit_DeleteChar();
		virtual Editable& Edit_Back();
		virtual Editable& Edit_MoveForward();
		virtual Editable& Edit_MoveBackward();
		virtual Editable& Edit_MoveHome();
		virtual Editable& Edit_MoveEnd();
		virtual Editable& Edit_Clear();
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
