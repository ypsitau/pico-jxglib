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
	Terminal& puts(const char* str);
	Terminal& vprintf(const char* format, va_list args);
	Terminal& printf(const char* format, ...);
};

}

#endif
