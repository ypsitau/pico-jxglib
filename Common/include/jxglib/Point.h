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
	Point() : x{0}, y{0} {}
	Point(int x, int y) : x{x}, y{y} {}
};

//------------------------------------------------------------------------------
// PointFloat
//------------------------------------------------------------------------------
struct PointFloat {
	float x, y;
public:
	static const PointFloat Zero;
public:
	PointFloat() : x{0.}, y{0.} {}
	PointFloat(float x, float y) : x{x}, y{y} {}
};

//------------------------------------------------------------------------------
// PointDouble
//------------------------------------------------------------------------------
struct PointDouble {
	double x, y;
public:
	static const PointDouble Zero;
public:
	PointDouble() : x{0.}, y{0.} {}
	PointDouble(double x, double y) : x{x}, y{y} {}
};

}

#endif
