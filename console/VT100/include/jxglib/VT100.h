//==============================================================================
// jxglib/VT100.h
//==============================================================================
#ifndef PICO_JXGLIB_VT100_H
#define PICO_JXGLIB_VT100_H
#include "pico/stdlib.h"
#include "jxglib/Printable.h"
#include "jxglib/Editable.h"
#include "jxglib/KeyCode.h"
#include "jxglib/FIFOBuff.h"

namespace jxglib {

//------------------------------------------------------------------------------
// VT100
//------------------------------------------------------------------------------
class VT100 {
public:
	class Decoder {
	public:
		enum class Stat {
			First, Escape,
			SS2,
			SS3,
			DCS,
			CSI_Parameter, CSI_Intermediate, CSI_Final,
			ST,
			OSC,
			SOS,
			PM,
			APC,
		};
		static const int OffsetForAscii = 0x100;
	private:
		Stat stat_;
		FIFOBuff<int, 32> buff_;
		int iBuffParameter_;
		int iBuffIntermediate_;
		char buffParameter_[32];
		char buffIntermediate_[32];
	public:
		Decoder();
	public:
		bool HasKeyData() const { return buff_.HasData(); }
		bool GetKeyData(int* pKeyData);
	public:
		void FeedChar(char ch);
	};
	class Terminal : public Printable, public Editable, public Tickable {
	private:
		Printable& printable_;
		Decoder decoder_;
	public:
		Terminal(Printable& printable = PrintableStdio::Instance) : Tickable(0), printable_{printable} {}
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
	// Cursor movement
	static void CursorUp(Printable& printable, int nRows = 1) { if (nRows > 0) printable.Printf("\x1b[%dA", nRows); }
	static void CursorDown(Printable& printable, int nRows = 1) { if (nRows > 0) printable.Printf("\x1b[%dB", nRows); }
	static void CursorForward(Printable& printable, int nCols = 1) { if (nCols > 0) printable.Printf("\x1b[%dC", nCols); }
	static void CursorBackward(Printable& printable, int nCols = 1) { if (nCols > 0) printable.Printf("\x1b[%dD", nCols); }
	static void CursorNextLine(Printable& printable, int nRows = 1) { if (nRows > 0) printable.Printf("\x1b[%dE", nRows); }
	static void CursorPreviousLine(Printable& printable, int nRows = 1) { if (nRows > 0) printable.Printf("\x1b[%dF", nRows); }
	static void CursorHorizontalAbsolute(Printable& printable, int n) { printable.Printf("\x1b[%dG", n); }
	static void CursorPosition(Printable& printable, int row, int column) { printable.Printf("\x1b[%d;%dH", row, column); }
	// Line erasure
	static void EraseToEndOfLine(Printable& printable) { printable.Printf("\x1b[0K"); }
	static void EraseToBeginningOfLine(Printable& printable) { printable.Printf("\x1b[1K"); }
	static void EraseCurrentLine(Printable& printable) { printable.Printf("\x1b[2K"); }
	// Screen erasure
	static void EraseFromCursorToEndOfScreen(Printable& printable) { printable.Printf("\x1b[0J"); }
	static void EraseFromBeginningOfScreenToCursor(Printable& printable) { printable.Printf("\x1b[1J"); }
	static void EraseScreen(Printable& printable) { printable.Printf("\x1b[2J"); }
	static void EraseScreenAndScrollbackBuffer(Printable& printable) { printable.Printf("\x1b[3J"); }
	// Character erasure
	static void DeleteCharacters(Printable& printable, int nChars) { if (nChars > 0) printable.Printf("\x1b[%dP", nChars); }
	static void EraseCharacters(Printable& printable, int nChars) { if (nChars > 0) printable.Printf("\x1b[%dX", nChars); }
	// Scrolling
	static void ScrollUp(Printable& printable, int nRows = 1) { if (nRows > 0) printable.Printf("\x1b[%dS", nRows); }
	static void ScrollDown(Printable& printable, int nRows = 1) { if (nRows > 0) printable.Printf("\x1b[%dT", nRows); }
	// Other
	static void SaveCursorPosition(Printable& printable) { printable.Printf("\x1b[s"); }
	static void RestoreCursorPosition(Printable& printable) { printable.Printf("\x1b[u"); }
};

}

#endif
