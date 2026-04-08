//==============================================================================
// jxglib/Point.h
//==============================================================================
#ifndef PICO_JXGLIB_POINT_H
#define PICO_JXGLIB_POINT_H
#include "pico/stdlib.h"

namespace jxglib {

class Size;

//------------------------------------------------------------------------------
// Point
//------------------------------------------------------------------------------
struct Point {
	int x, y;
public:
	static const Point Zero;
public:
	constexpr Point() : x{0}, y{0} {}
	constexpr Point(int x, int y) : x{x}, y{y} {}
public:
	bool Parse(const char* str);
public:
	bool operator==(const Point& pt) const { return x == pt.x && y == pt.y; }
	bool operator!=(const Point& pt) const { return !(*this == pt); }
public:
	Point& operator+=(const Point& pt) { x += pt.x; y += pt.y; return *this; }
	Point& operator+=(const Size& sz);
	Point& operator-=(const Point& pt) { x -= pt.x; y -= pt.y; return *this; }
	Point& operator-=(const Size& sz);
public:
	Point operator+(const Point& pt) const { return Point(x + pt.x, y + pt.y); }
	Point operator+(const Size& sz) const;
	Point operator-(const Point& pt) const { return Point(x - pt.x, y - pt.y); }
	Point operator-(const Size& sz) const;
	Point operator*(float scale) const { return Point(static_cast<int>(x * scale), static_cast<int>(y * scale)); }
	Point operator/(float scale) const { return Point(static_cast<int>(x / scale), static_cast<int>(y / scale)); }
};

//------------------------------------------------------------------------------
// PointFloat
//------------------------------------------------------------------------------
struct PointFloat {
	float x, y;
public:
	static const PointFloat Zero;
public:
	constexpr PointFloat() : x{0.}, y{0.} {}
	constexpr PointFloat(float x, float y) : x{x}, y{y} {}
public:
	bool Parse(const char* str);
};

//------------------------------------------------------------------------------
// PointDouble
//------------------------------------------------------------------------------
struct PointDouble {
	double x, y;
public:
	static const PointDouble Zero;
public:
	constexpr PointDouble() : x{0.}, y{0.} {}
	constexpr PointDouble(double x, double y) : x{x}, y{y} {}
public:
	bool Parse(const char* str);
};

}

#endif
