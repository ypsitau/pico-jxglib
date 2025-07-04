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
		virtual jxglib::Terminal& Edit_Begin() override;
		virtual jxglib::Terminal& Edit_Finish(char chEnd = '\0') override;
		virtual jxglib::Terminal& Edit_InsertChar(int ch) override;
		virtual jxglib::Terminal& Edit_DeleteChar() override;
		virtual jxglib::Terminal& Edit_Back() override;
		virtual jxglib::Terminal& Edit_MoveForward() override;
		virtual jxglib::Terminal& Edit_MoveBackward() override;
		virtual jxglib::Terminal& Edit_MoveHome() override;
		virtual jxglib::Terminal& Edit_MoveEnd() override;
		virtual jxglib::Terminal& Edit_Clear() override;
		virtual jxglib::Terminal& Edit_DeleteToHome() override;
		virtual jxglib::Terminal& Edit_DeleteToEnd() override;
		virtual jxglib::Terminal& Edit_MoveHistoryPrev() override;
		virtual jxglib::Terminal& Edit_MoveHistoryNext() override;
		virtual jxglib::Terminal& Edit_Completion() override;
	public:
		void MoveToBegin() { VT100::RestoreCursorPosition(GetPrintable()); }
		void MoveToEnd() { VT100::CursorBackward(GetPrintable(), GetLineEditor().CountFollowingChars()); }
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
