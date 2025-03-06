//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"
#include "jxglib/Printable.h"
#include "jxglib/Font.h"
#include "jxglib/LineBuff.h"
#include "jxglib/VT100.h"
#include "jxglib/UART.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
struct Terminal : public Printable {
public:
	static const int EditBuffSize = 128;
public:
	class EventHandler {
	public:
		virtual void OnNewLine(Terminal& terminal) = 0;
	};
	class LineEditor {
	private:
		int iCharCursor_;
		char buff_[EditBuffSize];
		UTF8Decoder decoder_;
		LineBuff historyBuff_;
		const char* pLineStop_History_;
	public:
		LineEditor(int bytesHistoryBuff);
	public:
		bool Initialize();
		bool IsEmpty() const { return buff_[0] == '\0'; }
		int GetICharCursor() const { return iCharCursor_; }
		int GetICharEnd() const { return ::strlen(buff_); }
		char* GetPointer(int iChar) { return buff_ + iChar; }
		char* GetPointerBegin() { return buff_; }
		char* GetPointerEnd() { return buff_ + GetICharEnd(); }
		const char* GetPointer(int iChar) const { return buff_ + iChar; }
		const char* GetPointerAtCursor() const { return buff_ + iCharCursor_; }
		void Clear();
		bool InsertChar(char ch) { return InsertChar(&iCharCursor_, ch); }
		bool InsertChar(int* piChar, char ch);
		bool DeleteChar() { return DeleteChar(iCharCursor_); }
		bool DeleteChar(int iChar);
		bool DeleteLastChar();
		bool MoveForward() { return MoveForward(&iCharCursor_); }
		bool MoveBackward() { return MoveBackward(&iCharCursor_); }
		bool MoveForward(int* pIChar);
		bool MoveBackward(int* pIChar);
		bool MoveHome();
		bool MoveEnd();
		bool DeleteToHome();
		bool DeleteToEnd() { return DeleteToEnd(iCharCursor_); }
		bool DeleteToEnd(int iChar);
	public:
		bool MoveHistoryPrev();
		bool MoveHistoryNext();
		bool EndHistory();
		bool AddHistory(const char* str);
		bool ReplaceWithHistory();
	public:
		LineBuff& GetHistoryBuff() { return historyBuff_; }
		const LineBuff& GetHistoryBuff() const { return historyBuff_; }
	};
	class Input {
	public:
		virtual void OnTick(Terminal& terminal) = 0;
	};
	class InputUART : public Input {
	private:
		UART* pUART_;
		VT100::Decoder decoder_;
	public:
		InputUART() : pUART_{nullptr} {}
	public:
		void SetUART(UART& uart) { pUART_ = &uart; }
	public:
		// virtual function of Input
		virtual void OnTick(Terminal& terminal) override;
	};
public:
	class Tickable_Blink : public Tickable {
	private:
		Terminal& terminal_;
		int msecBlink_;
	public:
		Tickable_Blink(Terminal& terminal, int msecBlink) : Tickable(msecBlink),
				terminal_{terminal}, msecBlink_{msecBlink} {}
	public:
		virtual void OnTick() override { terminal_.BlinkCursor(); }
	};
	class Tickable_Input : public Tickable {
	private:
		Terminal& terminal_;
	public:
		Tickable_Input(Terminal& terminal) : terminal_{terminal} {}
	public:
		virtual void OnTick() override { terminal_.TickInput(); }
	};
public:
	using Dir = Drawable::Dir;
	using Reader = LineBuff::Reader;
private:
	LineEditor lineEditor_;
	Drawable* pDrawable_;
	Rect rectDst_;
	int nLinesWhole_;
	Point ptCurrent_;
	UTF8Decoder decoder_;
	LineBuff lineBuff_;
	Drawable::Context context_;
	EventHandler* pEventHandler_;
	const char* pLineStop_RollBack_;
	bool suppressFlag_;
	bool editingFlag_;
	bool showCursorFlag_;
	bool appearCursorFlag_;
	Color colorTextInEdit_;
	Color colorCursor_;
	int wdCursor_;
	Input* pInput_;
	InputUART inputUART_;
	Tickable_Blink tickable_Blink_;
	Tickable_Input tickable_Input_;
public:
	Terminal(int bytesLineBuff = 4096, int byteshistoryBuff = 512);
public:
	void Initialize() {}
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty, Dir dir = Dir::Normal);
	void AttachInput(Input& input, int msecTick);
	void AttachInput(UART& uart);
	void TickInput() { if (pInput_) pInput_->OnTick(*this); }
public:
	Drawable& GetDrawable() { return *pDrawable_; }
	const Drawable& GetDrawable() const { return *pDrawable_; }
	const Rect& GetRectDst() const { return rectDst_; }
	LineEditor& GetLineEditor() { return lineEditor_; }
public:
	Terminal& SetColor(const Color& color) { context_.SetColor(color); return *this; }
	const Color& GetColor() const { return context_.GetColor(); }
	Terminal& SetColorInEdit(const Color& color) { colorTextInEdit_ = color; return *this; }
	Terminal& SetColorCursor(const Color& color) { colorCursor_ = color; return *this; }
	Terminal& SetColorBg(const Color& color) { context_.SetColorBg(color); return *this; }
	const Color& GetColorBg() const { return context_.GetColorBg(); }
	Terminal& SetFont(const FontSet& fontSet, int fontScale = 1) { context_.SetFont(fontSet, fontScale); return *this; }
	const FontSet& GetFont() const { return context_.GetFont(); }
	Terminal& SetFont(const FontSet& fontSet, int fontScaleWidth, int fontScaleHeight) {
		context_.SetFont(fontSet, fontScaleWidth, fontScaleHeight); return *this;
	}
	Terminal& SetFontScale(int fontScale) { context_.SetFontScale(fontScale); return *this; }
	Terminal& SetFontScale(int fontScaleWidth, int fontScaleHeight) {
		context_.SetFontScale(fontScaleWidth, fontScaleHeight); return *this;
	}
	Terminal& SetSpacingRatio(float charWidthRatio, float lineHeightRatio) {
		context_.SetSpacingRatio(charWidthRatio, lineHeightRatio); return *this;
	}
public:
	int GetLineIndex() const { return nLinesWhole_; }
	int CalcApproxNColsOnDisplay() const;
	int CalcNLinesOnDisplay() const;
	LineBuff& GetLineBuff() { return lineBuff_; }
	const LineBuff& GetLineBuff() const { return lineBuff_; }
	Reader CreateReader() const { return GetLineBuff().CreateReader(); }
public:
	bool IsRollingBack() const { return !!GetLineBuff().GetLineMark(); }
	Terminal& BeginRollBack();
	Terminal& EndRollBack();
	Terminal& RollUp();
	Terminal& RollDown();
	Terminal& Suppress(bool suppressFlag = true);
public:
	Terminal& ShowCursor(bool showCursorFlag = true);
public:
	// Virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& RefreshScreen() override;
	virtual Printable& Locate(int col, int row) override;
	virtual Printable& PutChar(char ch) override {
		AppendChar(ptCurrent_, ch, !suppressFlag_); return *this; 
	};
public:
	char* ReadLine(const char* prompt);
public:
	void AppendChar(Point& pt, char ch, bool drawFlag);
	void AppendString(Point& pt, const char* str, bool drawFlag);
	void DrawEditorArea();
	Point CalcDrawPos(const Point& ptBase, int iChar, int wdAdvance);
public:
	void SetCursorBlinkSpeed(int msecBlink) { tickable_Blink_.SetTick(msecBlink); }
	void DrawCursor();
	void EraseCursor() { EraseCursor(lineEditor_.GetICharCursor()); };
	void EraseCursor(int posCursor);
	void BlinkCursor();
private:
	void DrawLatestTextLines(bool refreshFlag);
	void DrawTextLines(int iLine, const char* pLineTop, int nLines);
	void DrawTextLine(int iLine, const char* pLineTop);
	void ScrollUp(int nLinesToScroll, bool refreshFlag);
public:
	Terminal& Edit_Begin(bool showCursorFlag = true);
	Terminal& Edit_Finish(char chEnd = '\0');
	Terminal& Edit_InsertChar(int ch);
	Terminal& Edit_Delete();
	Terminal& Edit_Back();
	Terminal& Edit_MoveForward();
	Terminal& Edit_MoveBackward();
	Terminal& Edit_MoveHome();
	Terminal& Edit_MoveEnd();
	Terminal& Edit_DeleteToHome();
	Terminal& Edit_DeleteToEnd();
	Terminal& Edit_HistoryPrev();
	Terminal& Edit_HistoryNext();
};

}

#endif
