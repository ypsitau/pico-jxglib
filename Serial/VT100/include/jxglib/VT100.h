//==============================================================================
// jxglib/VT100.h
//==============================================================================
#ifndef PICO_JXGLIB_VT100_H
#define PICO_JXGLIB_VT100_H
#include "pico/stdlib.h"
#include "jxglib/Printable.h"
#include "jxglib/Keyboard.h"
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
		bool GetKeyData(int* pKeyData, bool* pvkFlag);
	public:
		void FeedChar(char ch);
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
