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
				Tickable(0), pPrintable_{&Stdio::Instance} {}
	public:
		Terminal& Initialize() { jxglib::Terminal::Initialize(); return *this; }
	public:
		Terminal& AttachKeyboard(Keyboard& keyboard) { jxglib::Terminal::AttachKeyboard(keyboard); return *this; }
		Terminal& AttachPrintable(Printable& printable) { pPrintable_ = &printable; return *this; }
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
		// virtual functions of jxglib::Terminal
		virtual Printable& GetPrintable() override { return *pPrintable_; }
		virtual jxglib::Terminal& Edit_Begin();
		virtual jxglib::Terminal& Edit_Finish(char chEnd = '\0');
		virtual jxglib::Terminal& Edit_InsertChar(int ch);
		virtual jxglib::Terminal& Edit_DeleteChar();
		virtual jxglib::Terminal& Edit_Back();
		virtual jxglib::Terminal& Edit_MoveForward();
		virtual jxglib::Terminal& Edit_MoveBackward();
		virtual jxglib::Terminal& Edit_MoveHome();
		virtual jxglib::Terminal& Edit_MoveEnd();
		virtual jxglib::Terminal& Edit_Clear();
		virtual jxglib::Terminal& Edit_DeleteToHome();
		virtual jxglib::Terminal& Edit_DeleteToEnd();
		virtual jxglib::Terminal& Edit_MoveHistoryPrev();
		virtual jxglib::Terminal& Edit_MoveHistoryNext();
	public:
		// virtual functions of Tickable
		virtual const char* GetTickableName() const override { return "Serial::Terminal"; }
		virtual void OnTick() override;
	};
public:
	Serial() {}
};

}

#endif
