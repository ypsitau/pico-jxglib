//==============================================================================
// jxglib/Rect.h
//==============================================================================
#ifndef PICO_JXGLIB_RECT_H
#define PICO_JXGLIB_RECT_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Rect
//------------------------------------------------------------------------------
struct Rect {
	int x, y;
	int width, height;
public:
	static const Rect Zero;
public:
	int GetLeft() const { return x; }
	int GetRight() const { return x + width; }
	int GetTop() const { return y; }
	int GetBottom() const { return y + height; }
};

}

#endif
