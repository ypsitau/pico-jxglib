//==============================================================================
// jxglib/Color.h
//==============================================================================
#ifndef PICO_JXGLIB_COLOR_H
#define PICO_JXGLIB_COLOR_H
#include "pico/stdlib.h"
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------
struct Color {
	uint8_t r, g, b;
public:
	static const Color Zero;
};

}

#endif