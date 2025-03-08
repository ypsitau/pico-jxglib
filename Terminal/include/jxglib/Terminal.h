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

class Editable {
public:
	static const int EditBuffSize = 128;
public:
	class LineEditor {
	private:
		bool editingFlag_;
		int iCharCursor_;
		char buff_[EditBuffSize];
		UTF8Decoder decoder_;
		LineBuff historyBuff_;
		const char* pLineStop_History_;
	public:
		LineEditor(int bytesHistoryBuff);
	public:
		bool Initialize();
		void Begin();
		void Finish();
		bool IsEditing() { return editingFlag_; }
		bool IsEmpty() const { return buff_[0] == '\0'; }
		int GetICharCursor() const { return iCharCursor_; }
		int GetICharEnd() const { return ::strlen(buff_); }
		char* GetPointer(int iChar) { return buff_ + iChar; }
		char* GetPointerBegin() { return buff_; }
		char* GetPointerEnd() { return buff_ + GetICharEnd(); }
		const char* GetPointer(int iChar) const { return buff_ + iChar; }
		const char* GetPointerAtCursor() const { return buff_ + iCharCursor_; }
		void Clear();
		bool InsertChar(char ch);
		bool DeleteChar();
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
		bool AddHistory(const char* str);
		bool MoveHistoryPrev();
		bool MoveHistoryNext();
		void EndHistory();
	private:
		void ReplaceWithHistory();
	public:
		LineBuff& GetHistoryBuff() { return historyBuff_; }
		const LineBuff& GetHistoryBuff() const { return historyBuff_; }
	};
private:
	LineEditor lineEditor_;
public:
	Editable(int byteshistoryBuff = 512);
public:
	bool Initialize();
	LineEditor& GetLineEditor() { return lineEditor_; }
public:
	virtual Editable& Edit_Begin() = 0;
	virtual Editable& Edit_Finish(char chEnd = '\0') = 0;
	virtual Editable& Edit_InsertChar(int ch) = 0;
	virtual Editable& Edit_DeleteChar() = 0;
	virtual Editable& Edit_Back() = 0;
	virtual Editable& Edit_MoveForward() = 0;
	virtual Editable& Edit_MoveBackward() = 0;
	virtual Editable& Edit_MoveHome() = 0;
	virtual Editable& Edit_MoveEnd() = 0;
	virtual Editable& Edit_DeleteToHome() = 0;
	virtual Editable& Edit_DeleteToEnd() = 0;
	virtual Editable& Edit_MoveHistoryPrev() = 0;
	virtual Editable& Edit_MoveHistoryNext() = 0;
};

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
class Terminal : public Printable, public Editable {
public:
	class EventHandler {
	public:
		virtual void OnNewLine(Terminal& terminal) = 0;
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
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty, Dir dir = Dir::Normal);
	void AttachInput(Input& input, int msecTick);
	void AttachInput(UART& uart);
	void TickInput() { if (pInput_) pInput_->OnTick(*this); }
public:
	Drawable& GetDrawable() { return *pDrawable_; }
	const Drawable& GetDrawable() const { return *pDrawable_; }
	const Rect& GetRectDst() const { return rectDst_; }
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
	virtual Printable& PutChar(char ch) override { AppendChar(ch, !suppressFlag_); return *this; };
public:
	char* ReadLine(const char* prompt);
public:
	void AppendChar(char ch, bool drawFlag);
	void AppendString(const char* str, bool drawFlag);
	void DrawEditorArea();
	Point CalcDrawPos(const Point& ptBase, int iChar, int wdAdvance);
public:
	void SetCursorBlinkSpeed(int msecBlink) { tickable_Blink_.SetTick(msecBlink); }
	void DrawCursor(int iCharCursor);
	void EraseCursor(int iCharCursor);
	void BlinkCursor();
private:
	void DrawLatestTextLines(bool refreshFlag);
	void DrawTextLines(int iLine, const char* pLineTop, int nLines);
	void DrawTextLine(int iLine, const char* pLineTop);
	void ScrollUp(int nLinesToScroll, bool refreshFlag);
public:
	Editable& Edit_Begin() override;
	Editable& Edit_Finish(char chEnd = '\0') override;
	Editable& Edit_InsertChar(int ch) override;
	Editable& Edit_DeleteChar() override;
	Editable& Edit_Back() override;
	Editable& Edit_MoveForward() override;
	Editable& Edit_MoveBackward() override;
	Editable& Edit_MoveHome() override;
	Editable& Edit_MoveEnd() override;
	Editable& Edit_DeleteToHome() override;
	Editable& Edit_DeleteToEnd() override;
	Editable& Edit_MoveHistoryPrev() override;
	Editable& Edit_MoveHistoryNext() override;
};

}

#endif
