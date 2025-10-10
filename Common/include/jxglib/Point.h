//==============================================================================
// jxglib/Point.h
//==============================================================================
#ifndef PICO_JXGLIB_POINT_H
#define PICO_JXGLIB_POINT_H
#include "pico/stdlib.h"

namespace jxglib {

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
