//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Font.h"
#include "jxglib/Canvas.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
struct Terminal {
private:
	Drawable* pDrawable_;
	Canvas canvas_;
	Point pt_;
public:
	Terminal() : pDrawable_{nullptr} {}
public:
	void AttachOutput(Drawable& drawable);
public:
	Terminal& SetFont(const FontSet& fontSet, int fontScale = 1) {
		pDrawable_->SetFont(fontSet, fontScale);
		return *this;
	}
	Terminal& SetFont(const FontSet& fontSet, int fontScaleWidth, int fontScaleHeight) {
		pDrawable_->SetFont(fontSet, fontScaleWidth, fontScaleHeight);
		return *this;
	}
	void SetFontScale(int fontScale) { pDrawable_->SetFontScale(fontScale); }
	void SetFontScale(int fontScaleWidth, int fontScaleHeight) {
		pDrawable_->SetFontScale(fontScaleWidth, fontScaleHeight);
	}
public:
	Terminal& flush();
	Terminal& puts(const char* str);
	Terminal& vprintf(const char* format, va_list args);
	Terminal& printf(const char* format, ...);
};

}

#endif
