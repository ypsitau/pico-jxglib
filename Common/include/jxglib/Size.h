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
};

}

#endif
