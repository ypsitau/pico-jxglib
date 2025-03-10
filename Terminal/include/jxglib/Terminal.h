//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"
#include "jxglib/Printable.h"
#include "jxglib/Editable.h"
#include "jxglib/Font.h"
#include "jxglib/LineBuff.h"
#include "jxglib/VT100.h"

namespace jxglib {

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
	class InputStdio : public Input {
	private:
		VT100::Decoder decoder_;
	public:
		InputStdio() {}
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
	UTF8::Decoder decoder_;
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
	InputStdio inputStdio_;
	Tickable_Blink tickable_Blink_;
	Tickable_Input tickable_Input_;
public:
	Terminal(int bytesLineBuff = 4096, int byteshistoryBuff = 512);
public:
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty, Dir dir = Dir::Normal);
	void AttachInput(Input& input, int msecTick);
	void AttachInputStdio();
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
	void AppendChar(char ch, bool drawFlag);
	void AppendString(const char* str, bool drawFlag);
	void DrawEditorArea();
	Point CalcDrawPos(const Point& ptBase, int iByte, int wdAdvance);
public:
	void SetCursorBlinkSpeed(int msecBlink) { tickable_Blink_.SetTick(msecBlink); }
	void DrawCursor(int iByteCursor);
	void EraseCursor(int iByteCursor);
	void BlinkCursor();
private:
	void DrawLatestTextLines(bool refreshFlag);
	void DrawTextLines(int iLine, const char* pLineTop, int nLines);
	void DrawTextLine(int iLine, const char* pLineTop);
	void ScrollUp(int nLinesToScroll, bool refreshFlag);
public:
	// Virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& RefreshScreen() override;
	virtual Printable& Locate(int col, int row) override;
	virtual Printable& PutChar(char ch) override;
public:
	// virtual functions of Editable
	virtual Printable& GetPrintable() override { return *this; }
	virtual Editable& Edit_Begin() override;
	virtual Editable& Edit_Finish(char chEnd = '\0') override;
	virtual Editable& Edit_InsertChar(int ch) override;
	virtual Editable& Edit_DeleteChar() override;
	virtual Editable& Edit_Back() override;
	virtual Editable& Edit_MoveForward() override;
	virtual Editable& Edit_MoveBackward() override;
	virtual Editable& Edit_MoveHome() override;
	virtual Editable& Edit_MoveEnd() override;
	virtual Editable& Edit_DeleteToHome() override;
	virtual Editable& Edit_DeleteToEnd() override;
	virtual Editable& Edit_MoveHistoryPrev() override;
	virtual Editable& Edit_MoveHistoryNext() override;
};

}

#endif
