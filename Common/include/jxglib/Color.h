//==============================================================================
// jxglib/Color.h
//==============================================================================
#ifndef PICO_JXGLIB_COLOR_H
#define PICO_JXGLIB_COLOR_H
#include "pico/stdlib.h"

namespace jxglib {

class ColorRGB565;
class ColorBGR565;
class ColorRGB555;
class ColorBGR555;

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
	Color() : r(0), g(0), b(0) {}
	Color(const Color& color) : r{color.r}, g{color.g}, b{color.b} {}
	Color(uint8_t r, uint8_t g, uint8_t b) : r{r}, g{g}, b{b} {}
	Color(const ColorRGB565& colorRGB565);
	Color(const ColorBGR565& colorBGR565);
	Color(const ColorRGB555& colorRGB555);
	Color(const ColorBGR555& colorBGR555);
public:
	Color& operator=(const Color& color) { r = color.r; g = color.g; b = color.b; return *this; }
public:
	uint8_t GetR() const { return r; }
	uint8_t GetG() const { return g; }
	uint8_t GetB() const { return b; }
	bool IsBlack() const { return r == 0 && g == 0 && b == 0; }
	bool IsWhite() const { return r == 255 && g == 255 && b == 255; }
};

//------------------------------------------------------------------------------
// ColorA
//------------------------------------------------------------------------------
struct ColorA : public Color {
	uint8_t a;
public:
	static const ColorA zero;
	static const ColorA black;
	static const ColorA silver;
	static const ColorA gray;
	static const ColorA white;
	static const ColorA maroon;
	static const ColorA red;
	static const ColorA purple;
	static const ColorA fuchsia;
	static const ColorA green;
	static const ColorA lime;
	static const ColorA olive;
	static const ColorA yellow;
	static const ColorA navy;
	static const ColorA blue;
	static const ColorA teal;
	static const ColorA aqua;
public:
	ColorA() : a{0} {}
	ColorA(const Color& color) : Color(color), a{255} {}
	ColorA(const ColorA& colorA) : Color(colorA), a{colorA.a} {}
	ColorA(uint8_t r, uint8_t g, uint8_t b) : Color(r, g, b), a{255} {}
	ColorA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : Color(r, g, b), a{a} {}
public:
	ColorA& operator=(const ColorA& colorA) { r = colorA.r; g = colorA.g; b = colorA.b; a = colorA.a; return *this; }
public:
	uint8_t GetA() const { return a; }
};

//------------------------------------------------------------------------------
// ColorRGB565
//------------------------------------------------------------------------------
struct ColorRGB565 {
	uint16_t value;
public:
	static const ColorRGB565 zero;
	static const ColorRGB565 black;
	static const ColorRGB565 silver;
	static const ColorRGB565 gray;
	static const ColorRGB565 white;
	static const ColorRGB565 maroon;
	static const ColorRGB565 red;
	static const ColorRGB565 purple;
	static const ColorRGB565 fuchsia;
	static const ColorRGB565 green;
	static const ColorRGB565 lime;
	static const ColorRGB565 olive;
	static const ColorRGB565 yellow;
	static const ColorRGB565 navy;
	static const ColorRGB565 blue;
	static const ColorRGB565 teal;
	static const ColorRGB565 aqua;
public:
	ColorRGB565(const ColorRGB565& colorRGB565) : value{colorRGB565.value} {}
	constexpr ColorRGB565(uint8_t r, uint8_t g, uint8_t b) : value{static_cast<uint16_t>(
			(static_cast<uint32_t>(r & 0xf8) << (6 + 5 - 3)) |
			(static_cast<uint32_t>(g & 0xfc) << (5 - 2)) |
			((static_cast<uint32_t>(b) >> 3)))} {}
	ColorRGB565(const Color& color) : ColorRGB565(color.r, color.g, color.b) {}
public:
	ColorRGB565& operator=(const ColorRGB565& colorRGB565) { value = colorRGB565.value; return *this; }
public:
	operator uint16_t () { return value; }
	uint8_t GetR() const {
		uint8_t elem = static_cast<uint8_t>((value >> (6 + 5 - 3)) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	uint8_t GetG() const {
		uint8_t elem = static_cast<uint8_t>((value >> (5 - 2)) & 0xfc);
		return elem? (elem | 0x03) : elem;
	}
	uint8_t GetB() const {
		uint8_t elem = static_cast<uint8_t>((value << 3) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	bool IsBlack() const { return value == 0x0000; }
	bool IsWhite() const { return value == 0xffff; }
};

//------------------------------------------------------------------------------
// ColorBGR565
//------------------------------------------------------------------------------
struct ColorBGR565 {
	uint16_t value;
public:
	static const ColorBGR565 zero;
	static const ColorBGR565 black;
	static const ColorBGR565 silver;
	static const ColorBGR565 gray;
	static const ColorBGR565 white;
	static const ColorBGR565 maroon;
	static const ColorBGR565 red;
	static const ColorBGR565 purple;
	static const ColorBGR565 fuchsia;
	static const ColorBGR565 green;
	static const ColorBGR565 lime;
	static const ColorBGR565 olive;
	static const ColorBGR565 yellow;
	static const ColorBGR565 navy;
	static const ColorBGR565 blue;
	static const ColorBGR565 teal;
	static const ColorBGR565 aqua;
public:
	ColorBGR565(const ColorBGR565& colorBGR565) : value{colorBGR565.value} {}
	constexpr ColorBGR565(uint8_t r, uint8_t g, uint8_t b) : value{static_cast<uint16_t>(
			(static_cast<uint32_t>(b & 0xf8) << (6 + 5 - 3)) |
			(static_cast<uint32_t>(g & 0xfc) << (5 - 2)) |
			((static_cast<uint32_t>(r) >> 3)))} {}
	ColorBGR565(const Color& color) : ColorBGR565(color.r, color.g, color.b) {}
public:
	ColorBGR565& operator=(const ColorBGR565& colorBGR565) { value = colorBGR565.value; return *this; }
public:
	operator uint16_t () { return value; }
	uint8_t GetR() const {
		uint8_t elem = static_cast<uint8_t>((value << 3) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	uint8_t GetG() const {
		uint8_t elem = static_cast<uint8_t>((value >> (5 - 2)) & 0xfc);
		return elem? (elem | 0x03) : elem;
	}
	uint8_t GetB() const {
		uint8_t elem = static_cast<uint8_t>((value >> (6 + 5 - 3)) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	bool IsBlack() const { return value == 0x0000; }
	bool IsWhite() const { return value == 0xffff; }
};

//------------------------------------------------------------------------------
// ColorRGB555
//------------------------------------------------------------------------------
struct ColorRGB555 {
	uint16_t value;
public:
	static const ColorRGB555 zero;
	static const ColorRGB555 black;
	static const ColorRGB555 silver;
	static const ColorRGB555 gray;
	static const ColorRGB555 white;
	static const ColorRGB555 maroon;
	static const ColorRGB555 red;
	static const ColorRGB555 purple;
	static const ColorRGB555 fuchsia;
	static const ColorRGB555 green;
	static const ColorRGB555 lime;
	static const ColorRGB555 olive;
	static const ColorRGB555 yellow;
	static const ColorRGB555 navy;
	static const ColorRGB555 blue;
	static const ColorRGB555 teal;
	static const ColorRGB555 aqua;
public:
	ColorRGB555(const ColorRGB555& colorRGB555) : value{colorRGB555.value} {}
	constexpr ColorRGB555(uint8_t r, uint8_t g, uint8_t b) : value{static_cast<uint16_t>(
			(static_cast<uint32_t>(r & 0xf8) << (5 + 5 - 3)) |
			(static_cast<uint32_t>(g & 0xfc) << (5 - 2)) |
			((static_cast<uint32_t>(b) >> 3)))} {}
	ColorRGB555(const Color& color) : ColorRGB555(color.r, color.g, color.b) {}
public:
	ColorRGB555& operator=(const ColorRGB555& colorRGB555) { value = colorRGB555.value; return *this; }
public:
	operator uint16_t () { return value; }
	uint8_t GetR() const {
		uint8_t elem = static_cast<uint8_t>((value >> (5 + 5 - 3)) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	uint8_t GetG() const {
		uint8_t elem = static_cast<uint8_t>((value >> (5 - 2)) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	uint8_t GetB() const {
		uint8_t elem = static_cast<uint8_t>((value << 3) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	bool IsBlack() const { return value == 0x0000; }
	bool IsWhite() const { return value == 0xffff; }
};

//------------------------------------------------------------------------------
// ColorBGR555
//------------------------------------------------------------------------------
struct ColorBGR555 {
	uint16_t value;
public:
	static const ColorBGR555 zero;
	static const ColorBGR555 black;
	static const ColorBGR555 silver;
	static const ColorBGR555 gray;
	static const ColorBGR555 white;
	static const ColorBGR555 maroon;
	static const ColorBGR555 red;
	static const ColorBGR555 purple;
	static const ColorBGR555 fuchsia;
	static const ColorBGR555 green;
	static const ColorBGR555 lime;
	static const ColorBGR555 olive;
	static const ColorBGR555 yellow;
	static const ColorBGR555 navy;
	static const ColorBGR555 blue;
	static const ColorBGR555 teal;
	static const ColorBGR555 aqua;
public:
	ColorBGR555(const ColorBGR555& colorBGR555) : value{colorBGR555.value} {}
	constexpr ColorBGR555(uint8_t r, uint8_t g, uint8_t b) : value{static_cast<uint16_t>(
			(static_cast<uint32_t>(b & 0xf8) << (5 + 5 - 3)) |
			(static_cast<uint32_t>(g & 0xfc) << (5 - 2)) |
			((static_cast<uint32_t>(r) >> 3)))} {}
	ColorBGR555(const Color& color) : ColorBGR555(color.r, color.g, color.b) {}
public:
	ColorBGR555& operator=(const ColorBGR555& colorBGR555) { value = colorBGR555.value; return *this; }
public:
	operator uint16_t () { return value; }
	uint8_t GetR() const {
		uint8_t elem = static_cast<uint8_t>((value << 3) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	uint8_t GetG() const {
		uint8_t elem = static_cast<uint8_t>((value >> (5 - 2)) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	uint8_t GetB() const {
		uint8_t elem = static_cast<uint8_t>((value >> (5 + 5 - 3)) & 0xf8);
		return elem? (elem | 0x07) : elem;
	}
	bool IsBlack() const { return value == 0x0000; }
	bool IsWhite() const { return value == 0xffff; }
};

inline Color::Color(const ColorRGB565& colorRGB565) :
		r{colorRGB565.GetR()}, g{colorRGB565.GetG()}, b{colorRGB565.GetB()} {}

inline Color::Color(const ColorBGR565& colorBGR565) :
		r{colorBGR565.GetR()}, g{colorBGR565.GetG()}, b{colorBGR565.GetB()} {}

inline Color::Color(const ColorRGB555& colorRGB555) :
		r{colorRGB555.GetR()}, g{colorRGB555.GetG()}, b{colorRGB555.GetB()} {}

inline Color::Color(const ColorBGR555& colorBGR555) :
		r{colorBGR555.GetR()}, g{colorBGR555.GetG()}, b{colorBGR555.GetB()} {}

}

#endif
