//==============================================================================
// jxglib/Rect.h
//==============================================================================
#ifndef PICO_JXGLIB_RECT_H
#define PICO_JXGLIB_RECT_H
#include "pico/stdlib.h"
#include "jxglib/Point.h"
#include "jxglib/Size.h"

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
	Rect() : x{0}, y{0}, width{0}, height{0} {}
	Rect(int x, int y, int width, int height) : x{x}, y{y}, width{width}, height{height} {}
	Rect(const Point& pt, const Size& size) : x{pt.x}, y{pt.y}, width{size.width}, height{size.height} {}
	int GetLeft() const { return x; }
	int GetRight() const { return x + width; }
	int GetTop() const { return y; }
	int GetBottom() const { return y + height; }
};

}

#endif
