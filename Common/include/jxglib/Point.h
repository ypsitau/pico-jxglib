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
	Point(int x, int y) : x{x}, y{y} {}
};

}

#endif
