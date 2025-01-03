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

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
struct Terminal : public Printable {
public:
	using AttachDir = Drawable::Dir;
	using Stream = LineBuff::Stream;
private:
	Drawable* pDrawable_;
	Rect rectDst_;
	Point ptCursor_;
	UTF8Decoder decoder_;
	LineBuff lineBuff_;
	Drawable::Context context_;
public:
	Terminal() : pDrawable_{nullptr} {}
public:
	bool Initialize(int bytes = 1024);
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty, AttachDir attachDir = AttachDir::Normal);
public:
	Drawable& GetDrawable() { return *pDrawable_; }
	const Drawable& GetDrawable() const { return *pDrawable_; }
	const Rect& GetRectDst() const { return rectDst_; }
public:
	Terminal& SetColor(const Color& color) { context_.colorFg = color; return *this; }
	const Color& GetColor() const { return context_.colorFg; }
	Terminal& SetColorBg(const Color& color) { context_.colorBg = color; return *this; }
	const Color& GetColorBg() const { return context_.colorBg; }
	Terminal& SetFont(const FontSet& fontSet, int fontScale = 1) {
		context_.pFontSet = &fontSet;
		context_.fontScaleWidth = context_.fontScaleHeight = fontScale;
		return *this;
	}
	const FontSet& GetFont() const { return *context_.pFontSet; }
	Terminal& SetFont(const FontSet& fontSet, int fontScaleWidth, int fontScaleHeight) {
		context_.pFontSet = &fontSet; context_.fontScaleWidth = fontScaleWidth, context_.fontScaleHeight = fontScaleHeight;
		return *this;
	}
	Terminal& SetFontScale(int fontScale) {
		context_.fontScaleWidth = context_.fontScaleHeight = fontScale;
		return *this;
	}
	Terminal& SetFontScale(int fontScaleWidth, int fontScaleHeight) {
		context_.fontScaleWidth = fontScaleWidth, context_.fontScaleHeight = fontScaleHeight;
		return *this;
	}
	Terminal& SetSpacingRatio(float charWidthRatio, float lineHeightRatio) {
		context_.charWidthRatio = charWidthRatio, context_.lineHeightRatio = lineHeightRatio;
		return *this;
	}
public:
	int GetColNum() const;
	int GetRowNum() const;
	const LineBuff& GetLineBuff() const { return lineBuff_; }
	Stream CreateStream() const { return GetLineBuff().CreateStream(); }
public:
	// Virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& RefreshScreen() override;
	virtual Printable& Locate(int col, int row) override;
	virtual Printable& PutChar(char ch) override;
private:
	void DrawTextLines(const char* lineTop, int nLines, int yTop);
	void DrawTextLine(WrappedCharFeeder& charFeeder, int y);
	void EraseTextLine(int iLine, int nLines = 1);
	void ScrollVert(DirVert dirVert);
};

}

#endif
