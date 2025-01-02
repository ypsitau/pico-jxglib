//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Font.h"
#include "jxglib/Canvas.h"
#include "jxglib/Printable.h"
#include "jxglib/LineBuff.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
struct Terminal : public Printable {
public:
	using AttachDir = Canvas::AttachDir;
	using Stream = LineBuff::Stream;
private:
	Drawable* pDrawable_;
	Rect rectDst_;
	Canvas canvas_;
	Point ptCursor_;
	UTF8Decoder decoder_;
	LineBuff lineBuff_;
public:
	Terminal() : pDrawable_{nullptr} {}
public:
	bool Initialize(int bytes = 1024);
	bool AttachOutput(Drawable& drawable, const Rect& rect = Rect::Empty, AttachDir attachDir = AttachDir::Normal);
	bool AttachOutput(Drawable& drawable, AttachDir attachDir) {
		return AttachOutput(drawable, Rect::Empty, attachDir);
	}
public:
	Drawable& GetDrawable() { return *pDrawable_; }
	const Drawable& GetDrawable() const { return *pDrawable_; }
	const Rect& GetRectDst() const { return rectDst_; }
public:
	Terminal& SetFont(const FontSet& fontSet, int fontScale = 1) {
		pDrawable_->SetFont(fontSet, fontScale);
		return *this;
	}
	Terminal& SetFont(const FontSet& fontSet, int fontScaleWidth, int fontScaleHeight) {
		pDrawable_->SetFont(fontSet, fontScaleWidth, fontScaleHeight);
		return *this;
	}
	Terminal& SetFontScale(int fontScale) { pDrawable_->SetFontScale(fontScale); return *this; }
	Terminal& SetFontScale(int fontScaleWidth, int fontScaleHeight) {
		pDrawable_->SetFontScale(fontScaleWidth, fontScaleHeight);
		return *this;
	}
	Terminal& SetColor(const Color& color) { pDrawable_->SetColor(color); return *this; }
	Terminal& SetColorBg(const Color& colorBg) { pDrawable_->SetColorBg(colorBg); return *this; }
	int GetColNum() const;
	int GetRowNum() const;
	const LineBuff& GetLineBuff() const { return lineBuff_; }
	Stream CreateStream() const { return GetLineBuff().CreateStream(); }
public:
	// Virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& FlushScreen() override;
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
