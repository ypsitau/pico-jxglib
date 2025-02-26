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
#include "jxglib/Tickable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
struct Terminal : public Printable, public Tickable {
public:
	class EventHandler {
	public:
		virtual void OnNewLine(Terminal& terminal) = 0;
	};
public:
	using Dir = Drawable::Dir;
	using Reader = LineBuff::Reader;
private:
	Drawable* pDrawable_;
	Rect rectDst_;
	int nLinesWhole_;
	Point ptCursor_;
	UTF8Decoder decoder_;
	LineBuff lineBuff_;
	Drawable::Context context_;
	EventHandler* pEventHandler_;
	const char* pLineStop_;
	bool suppressFlag_;
public:
	Terminal(int bytesBuff = 4096, int msecBlink = 500) : Tickable(msecBlink), pDrawable_{nullptr},
		nLinesWhole_{0}, lineBuff_(bytesBuff), pEventHandler_{nullptr}, pLineStop_{nullptr}, suppressFlag_{false} {}
public:
	void Initialize() {}
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty, Dir dir = Dir::Normal);
public:
	Drawable& GetDrawable() { return *pDrawable_; }
	const Drawable& GetDrawable() const { return *pDrawable_; }
	const Rect& GetRectDst() const { return rectDst_; }
public:
	Terminal& SetColor(const Color& color) { context_.SetColor(color); return *this; }
	const Color& GetColor() const { return context_.GetColor(); }
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
	int GetColNum() const;
	int GetRowNum() const;
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
	// Virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& RefreshScreen() override;
	virtual Printable& Locate(int col, int row) override;
	virtual Printable& PutChar(char ch) override;
public:
	// Virtual functions of Tickable
	virtual void OnTick() override;
private:
	void DrawLatestTextLines();
	void DrawTextLines(int iLine, const char* pLineTop, int nLines);
	void DrawTextLine(int iLine, const char* pLineTop);
	void EraseTextLines(int iLine, int nLines);
	void EraseToEndOfLine();
	void ScrollUp();
};

}

#endif
