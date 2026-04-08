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
	static const Rect Empty;
public:
	constexpr Rect() : x{0}, y{0}, width{0}, height{0} {}
	constexpr Rect(int x, int y, int width, int height) : x{x}, y{y}, width{width}, height{height} {}
	constexpr Rect(const Point& pt, const Size& size) : x{pt.x}, y{pt.y}, width{size.width}, height{size.height} {}
	bool IsEmpty() const { return width == 0 || height == 0; }
	int GetLeft() const { return x; }
	int GetRight() const { return x + width - 1; }
	int GetRightExceed() const { return x + width; }
	int GetTop() const { return y; }
	int GetBottom() const { return y + height - 1; }
	int GetBottomExceed() const { return y + height; }
	int GetCenterH() const { return x + width / 2; }
	int GetCenterV() const { return y + height / 2; }
	Point GetPointN() const { return Point(x + width / 2, y); }
	Point GetPointE() const { return Point(x + width - 1, y + height / 2); }
	Point GetPointW() const { return Point(x, y + height / 2); }
	Point GetPointS() const { return Point(x + width / 2, y + height - 1); }
	Point GetPointNW() const { return Point(x, y); }
	Point GetPointNE() const { return Point(x + width - 1, y); }
	Point GetPointSW() const { return Point(x, y + height - 1); }
	Point GetPointSE() const { return Point(x + width - 1, y + height - 1); }
	Point GetPointCenter() const { return Point(x + width / 2, y + height / 2); }
	Size GetSize() const { return Size(width, height); }
	Rect Inflate(int amount) const;
	Rect Deflate(int amount) { return Inflate(-amount); }
	bool Adjust(const Rect& rectBound);
	bool Contains(const Point& pt) const { return x < pt.x && pt.x < x + width && y < pt.y && pt.y < y + height; }
public:
	Rect& operator=(const Rect& rect) { x = rect.x, y = rect.y, width = rect.width, height = rect.height; return *this; }
};

}

#endif
