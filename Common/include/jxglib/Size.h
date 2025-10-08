//==============================================================================
// jxglib/Size.h
//==============================================================================
#ifndef PICO_JXGLIB_SIZE_H
#define PICO_JXGLIB_SIZE_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Size
//------------------------------------------------------------------------------
struct Size {
	int width, height;
public:
	static const Size Zero;
public:
	constexpr Size() : width{0}, height{0} {}
	constexpr Size(int width, int height) : width{width}, height{height} {}
public:
	bool IsZero() const { return width == 0 && height == 0; }
public:
	bool operator==(const Size& sz) const { return width == sz.width && height == sz.height; }
	bool operator!=(const Size& sz) const { return !(*this == sz); }
public:
	Size& operator+=(const Size& sz) { width += sz.width; height += sz.height; return *this; }
	Size& operator-=(const Size& sz) { width -= sz.width; height -= sz.height; return *this; }
public:
	Size operator+(const Size& sz) const { return Size(width + sz.width, height + sz.height); }
	Size operator-(const Size& sz) const { return Size(width - sz.width, height - sz.height); }
	Size operator*(float scale) const { return Size(static_cast<int>(width * scale), static_cast<int>(height * scale)); }
	Size operator/(float scale) const { return Size(static_cast<int>(width / scale), static_cast<int>(height / scale)); }
public:
	Size& Set(int width, int height) { this->width = width; this->height = height; return *this; }
public:
	bool Parse(const char* str);
};

}

#endif
