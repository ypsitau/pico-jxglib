//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Font.h"
#include "jxglib/Canvas.h"
#include "jxglib/Printable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
struct Terminal : public Printable {
public:
	using AttachDir = Canvas::AttachDir;
private:
	Drawable* pDrawable_;
	Canvas canvas_;
	Point ptCursor_;
public:
	Terminal() : pDrawable_{nullptr} {}
public:
	bool AttachOutput(Drawable& drawable, const Rect* pRect = nullptr, AttachDir attachDir = AttachDir::Normal);
	bool AttachOutput(Drawable& drawable, const Rect& rect, AttachDir attachDir = AttachDir::Normal) {
		return AttachOutput(drawable, &rect, attachDir);
	}
	bool AttachOutput(Drawable& drawable, AttachDir attachDir) {
		return AttachOutput(drawable, nullptr, attachDir);
	}
public:
	Drawable& GetDrawable() { return *pDrawable_; }
	const Drawable& GetDrawable() const { return *pDrawable_; }
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
public:
	// Virtual functions of Printable
	virtual Printable& Clear() override;
	virtual Printable& Locate(int col, int row) override;
	virtual Printable& Flush() override;
	virtual Printable& Print(const char* str) override;
};

}

#endif
