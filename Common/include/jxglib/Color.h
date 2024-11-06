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
	static const Color zero;
	static const Color black;
	static const Color silver;
	static const Color gray;
	static const Color white;
	static const Color maroon;
	static const Color red;
	static const Color purple;
	static const Color fuchsia;
	static const Color green;
	static const Color lime;
	static const Color olive;
	static const Color yellow;
	static const Color navy;
	static const Color blue;
	static const Color teal;
	static const Color aqua;
public:
	constexpr static uint16_t RGB565(uint8_t r, uint8_t g, uint8_t b) { return
		((static_cast<uint16_t>(r) & 0xf8) << 8) |
		((static_cast<uint16_t>(g) & 0xfc) << 3) |
		((static_cast<uint16_t>(b) >> 3)); }
	uint16_t RGB565() const { return RGB565(r, g, b); }
};

}

#endif