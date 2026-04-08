//==============================================================================
// jxglib/VT100.h
//==============================================================================
#ifndef PICO_JXGLIB_VT100_H
#define PICO_JXGLIB_VT100_H
#include "pico/stdlib.h"
#include "jxglib/Readable.h"
#include "jxglib/Keyboard.h"
#include "jxglib/FIFOBuff.h"

namespace jxglib {

class Printable;

//------------------------------------------------------------------------------
// VT100
//------------------------------------------------------------------------------
class VT100 {
public:
	class Decoder {
	public:
		enum class Stat {
			First, AfterCR, Escape,
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
		bool GetKeyData(KeyData* pKeyData);
	public:
		bool FeedChar(char ch);
	};
	class Keyboard : public jxglib::Keyboard {
	private:
		Readable& readable_;
		Decoder decoder_;
	public:
		Keyboard(Readable& readable) : readable_{readable} {}
	public:
		virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1, bool includeModifiers = false) override;
		virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) override;
		virtual bool GetKeyCodeNB(uint8_t* pKeyCode, uint8_t* pModifier = nullptr) override;
		virtual bool GetKeyDataNB(KeyData* pKeyData) override;
	};
public:
	// Cursor movement
	static void CursorUp(Printable& printable, int nRows = 1);
	static void CursorDown(Printable& printable, int nRows = 1);
	static void CursorForward(Printable& printable, int nCols = 1);
	static void CursorBackward(Printable& printable, int nCols = 1);
	static void CursorNextLine(Printable& printable, int nRows = 1);
	static void CursorPreviousLine(Printable& printable, int nRows = 1);
	static void CursorHorizontalAbsolute(Printable& printable, int n);
	static void CursorPosition(Printable& printable, int row, int column);
	// Line erasure
	static void EraseToEndOfLine(Printable& printable);
	static void EraseToBeginningOfLine(Printable& printable);
	static void EraseCurrentLine(Printable& printable);
	// Screen erasure
	static void EraseFromCursorToEndOfScreen(Printable& printable);
	static void EraseFromBeginningOfScreenToCursor(Printable& printable);
	static void EraseScreen(Printable& printable);
	static void EraseScreenAndScrollbackBuffer(Printable& printable);
	// Character erasure
	static void DeleteCharacters(Printable& printable, int nChars);
	static void EraseCharacters(Printable& printable, int nChars);
	// Scrolling
	static void ScrollUp(Printable& printable, int nRows = 1);
	static void ScrollDown(Printable& printable, int nRows = 1);
	// Other
	static void SaveCursorPosition(Printable& printable);
	static void RestoreCursorPosition(Printable& printable);
};

}

#endif
