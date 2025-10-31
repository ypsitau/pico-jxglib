//==============================================================================
// Point.cpp
//==============================================================================
#include <ctype.h>
#include <stdlib.h>
#include "jxglib/Point.h"
#include "jxglib/Size.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Point
//------------------------------------------------------------------------------
const Point Point::Zero { 0, 0 };

bool Point::Parse(const char* str)
{
	if (!str || *str == '\0') return false;
	char* endPtr;
	int xTmp = static_cast<int>(::strtol(str, &endPtr, 0));
	if (endPtr == str) return false;
	while (::isspace(*endPtr)) ++endPtr;
	if (*endPtr != ',') return false;
	++endPtr;
	while (::isspace(*endPtr)) ++endPtr;
	int yTmp = static_cast<int>(::strtol(endPtr, &endPtr, 0));
	if (endPtr == str) return false;
	if (*endPtr != '\0') return false;
	x = xTmp;
	y = yTmp;
	return true;
}

Point& Point::operator+=(const Size& sz) { x += sz.width; y += sz.height; return *this; }
Point& Point::operator-=(const Size& sz) { x -= sz.width; y -= sz.height; return *this; }
Point Point::operator+(const Size& sz) const { return Point(x + sz.width, y + sz.height); }
Point Point::operator-(const Size& sz) const { return Point(x - sz.width, y - sz.height); }

//------------------------------------------------------------------------------
// PointFloat
//------------------------------------------------------------------------------
const PointFloat PointFloat::Zero { 0, 0 };

bool PointFloat::Parse(const char* str)
{
	if (!str || *str == '\0') return false;
	char* endPtr;
	float xTmp = static_cast<float>(::strtof(str, &endPtr));
	if (endPtr == str) return false;
	while (::isspace(*endPtr)) ++endPtr;
	if (*endPtr != ',') return false;
	++endPtr;
	while (::isspace(*endPtr)) ++endPtr;
	float yTmp = static_cast<float>(::strtof(endPtr, &endPtr));
	if (endPtr == str) return false;
	if (*endPtr != '\0') return false;
	x = xTmp;
	y = yTmp;
	return true;
}

//------------------------------------------------------------------------------
// PointDouble
//------------------------------------------------------------------------------
const PointDouble PointDouble::Zero { 0, 0 };

bool PointDouble::Parse(const char* str)
{
	if (!str || *str == '\0') return false;
	char* endPtr;
	double xTmp = ::strtod(str, &endPtr);
	if (endPtr == str) return false;
	while (::isspace(*endPtr)) ++endPtr;
	if (*endPtr != ',') return false;
	++endPtr;
	while (::isspace(*endPtr)) ++endPtr;
	double yTmp = ::strtod(endPtr, &endPtr);
	if (endPtr == str) return false;
	if (*endPtr != '\0') return false;
	x = xTmp;
	y = yTmp;
	return true;
}

}
