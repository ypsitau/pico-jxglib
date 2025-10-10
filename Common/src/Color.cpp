//==============================================================================
// Color.cpp
// Color names come from: https://www.w3.org/TR/css-color-3/
//==============================================================================
#include <string.h>
#include <stdlib.h>
#include "jxglib/Color.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Color
//------------------------------------------------------------------------------
const Color Color::zero			{   0,   0,   0 };
const Color Color::black		{   0,   0,   0 };
const Color Color::silver		{ 192, 192, 192 };
const Color Color::gray			{ 128, 128, 128 };
const Color Color::white		{ 255, 255, 255 };
const Color Color::maroon		{ 128,   0,   0 };
const Color Color::red			{ 255,   0,   0 };
const Color Color::purple		{ 128,   0, 128 };
const Color Color::fuchsia		{ 255,   0, 255 };
const Color Color::green		{   0, 128,   0 };
const Color Color::lime			{   0, 255,   0 };
const Color Color::olive		{ 128, 128,   0 };
const Color Color::yellow		{ 255, 255,   0 };
const Color Color::navy			{   0,   0, 128 };
const Color Color::blue			{   0,   0, 255 };
const Color Color::teal			{   0, 128, 128 };
const Color Color::aqua			{   0, 255, 255 };

uint8_t Color::CalcGray() const
{
	// Gray = 0.299*R + 0.587*G + 0.114*B
	return static_cast<uint8_t>((
		static_cast<uint16_t>(r) * 77 +		// .299 * 256
		static_cast<uint16_t>(g) * 150 +	// .587 * 256
		static_cast<uint16_t>(b) * 29)		// .114 * 256
		>> 8);								// divided by 256
}

bool Color::Parse(const char* str)
{
	struct NameMap {
		const char* name;
		Color color;
	};
	static const NameMap nameMapTbl[] = {
		{"zero",		Color::zero},
		{"black",		Color::black},
		{"silver",		Color::silver},
		{"gray",		Color::gray},
		{"white",		Color::white},
		{"maroon",		Color::maroon},
		{"red",			Color::red},
		{"purple",		Color::purple},
		{"fuchsia",		Color::fuchsia},
		{"green",		Color::green},
		{"lime",		Color::lime},
		{"olive",		Color::olive},
		{"yellow",		Color::yellow},
		{"navy",		Color::navy},
		{"blue",		Color::blue},
		{"teal",		Color::teal},
		{"aqua",		Color::aqua},
		{nullptr, Color::zero}
	};
	if (!str) return false;
	if (str[0] == '#') {
		// #RRGGBB
		if (::strlen(str) != 7) return false;
		char* endptr;
		long val = ::strtol(str + 1, &endptr, 16);
		if (*endptr != '\0' || val < 0 || val > 0xffffff) return false;
		r = static_cast<uint8_t>((val >> 16) & 0xff);
		g = static_cast<uint8_t>((val >> 8) & 0xff);
		b = static_cast<uint8_t>(val & 0xff);
		return true;
	} else {
		for (const NameMap* p = nameMapTbl; p->name; p++) {
			if (::strcasecmp(str, p->name) == 0) {
				*this = p->color;
				return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------
// ColorA
//------------------------------------------------------------------------------
const ColorA ColorA::zero		{   0,   0,   0, 255};
const ColorA ColorA::black		{   0,   0,   0, 255};
const ColorA ColorA::silver		{ 192, 192, 192, 255};
const ColorA ColorA::gray		{ 128, 128, 128, 255};
const ColorA ColorA::white		{ 255, 255, 255, 255};
const ColorA ColorA::maroon		{ 128,   0,   0, 255};
const ColorA ColorA::red		{ 255,   0,   0, 255};
const ColorA ColorA::purple		{ 128,   0, 128, 255};
const ColorA ColorA::fuchsia	{ 255,   0, 255, 255};
const ColorA ColorA::green		{   0, 128,   0, 255};
const ColorA ColorA::lime		{   0, 255,   0, 255};
const ColorA ColorA::olive		{ 128, 128,   0, 255};
const ColorA ColorA::yellow		{ 255, 255,   0, 255};
const ColorA ColorA::navy		{   0,   0, 128, 255};
const ColorA ColorA::blue		{   0,   0, 255, 255};
const ColorA ColorA::teal		{   0, 128, 128, 255};
const ColorA ColorA::aqua		{   0, 255, 255, 255};

bool ColorA::Parse(const char* str, uint8_t a)
{
	this->a = a;
	return Color::Parse(str);
}

//------------------------------------------------------------------------------
// ColorGray
//------------------------------------------------------------------------------
bool ColorGray::Parse(const char* str)
{
	Color color;
	if (!color.Parse(str)) return false;
	value = color.CalcGray();
	return true;
}

//------------------------------------------------------------------------------
// ColorRGB565
//------------------------------------------------------------------------------
const ColorRGB565 ColorRGB565::zero			{   0,   0,   0 };
const ColorRGB565 ColorRGB565::black		{   0,   0,   0 };
const ColorRGB565 ColorRGB565::silver		{ 192, 192, 192 };
const ColorRGB565 ColorRGB565::gray			{ 128, 128, 128 };
const ColorRGB565 ColorRGB565::white		{ 255, 255, 255 };
const ColorRGB565 ColorRGB565::maroon		{ 128,   0,   0 };
const ColorRGB565 ColorRGB565::red			{ 255,   0,   0 };
const ColorRGB565 ColorRGB565::purple		{ 128,   0, 128 };
const ColorRGB565 ColorRGB565::fuchsia		{ 255,   0, 255 };
const ColorRGB565 ColorRGB565::green		{   0, 128,   0 };
const ColorRGB565 ColorRGB565::lime			{   0, 255,   0 };
const ColorRGB565 ColorRGB565::olive		{ 128, 128,   0 };
const ColorRGB565 ColorRGB565::yellow		{ 255, 255,   0 };
const ColorRGB565 ColorRGB565::navy			{   0,   0, 128 };
const ColorRGB565 ColorRGB565::blue			{   0,   0, 255 };
const ColorRGB565 ColorRGB565::teal			{   0, 128, 128 };
const ColorRGB565 ColorRGB565::aqua			{   0, 255, 255 };

bool ColorRGB565::Parse(const char* str)
{
	Color color;
	if (!color.Parse(str)) return false;
	*this = ColorRGB565(color);
	return true;
}

//------------------------------------------------------------------------------
// ColorBGR565
//------------------------------------------------------------------------------
const ColorBGR565 ColorBGR565::zero			{   0,   0,   0 };
const ColorBGR565 ColorBGR565::black		{   0,   0,   0 };
const ColorBGR565 ColorBGR565::silver		{ 192, 192, 192 };
const ColorBGR565 ColorBGR565::gray			{ 128, 128, 128 };
const ColorBGR565 ColorBGR565::white		{ 255, 255, 255 };
const ColorBGR565 ColorBGR565::maroon		{ 128,   0,   0 };
const ColorBGR565 ColorBGR565::red			{ 255,   0,   0 };
const ColorBGR565 ColorBGR565::purple		{ 128,   0, 128 };
const ColorBGR565 ColorBGR565::fuchsia		{ 255,   0, 255 };
const ColorBGR565 ColorBGR565::green		{   0, 128,   0 };
const ColorBGR565 ColorBGR565::lime			{   0, 255,   0 };
const ColorBGR565 ColorBGR565::olive		{ 128, 128,   0 };
const ColorBGR565 ColorBGR565::yellow		{ 255, 255,   0 };
const ColorBGR565 ColorBGR565::navy			{   0,   0, 128 };
const ColorBGR565 ColorBGR565::blue			{   0,   0, 255 };
const ColorBGR565 ColorBGR565::teal			{   0, 128, 128 };
const ColorBGR565 ColorBGR565::aqua			{   0, 255, 255 };

bool ColorBGR565::Parse(const char* str)
{
	Color color;
	if (!color.Parse(str)) return false;
	*this = ColorBGR565(color);
	return true;
}

//------------------------------------------------------------------------------
// ColorRGB555
//------------------------------------------------------------------------------
const ColorRGB555 ColorRGB555::zero			{   0,   0,   0 };
const ColorRGB555 ColorRGB555::black		{   0,   0,   0 };
const ColorRGB555 ColorRGB555::silver		{ 192, 192, 192 };
const ColorRGB555 ColorRGB555::gray			{ 128, 128, 128 };
const ColorRGB555 ColorRGB555::white		{ 255, 255, 255 };
const ColorRGB555 ColorRGB555::maroon		{ 128,   0,   0 };
const ColorRGB555 ColorRGB555::red			{ 255,   0,   0 };
const ColorRGB555 ColorRGB555::purple		{ 128,   0, 128 };
const ColorRGB555 ColorRGB555::fuchsia		{ 255,   0, 255 };
const ColorRGB555 ColorRGB555::green		{   0, 128,   0 };
const ColorRGB555 ColorRGB555::lime			{   0, 255,   0 };
const ColorRGB555 ColorRGB555::olive		{ 128, 128,   0 };
const ColorRGB555 ColorRGB555::yellow		{ 255, 255,   0 };
const ColorRGB555 ColorRGB555::navy			{   0,   0, 128 };
const ColorRGB555 ColorRGB555::blue			{   0,   0, 255 };
const ColorRGB555 ColorRGB555::teal			{   0, 128, 128 };
const ColorRGB555 ColorRGB555::aqua			{   0, 255, 255 };

bool ColorRGB555::Parse(const char* str)
{
	Color color;
	if (!color.Parse(str)) return false;
	*this = ColorRGB555(color);
	return true;
}

//------------------------------------------------------------------------------
// ColorBGR555
//------------------------------------------------------------------------------
const ColorBGR555 ColorBGR555::zero			{   0,   0,   0 };
const ColorBGR555 ColorBGR555::black		{   0,   0,   0 };
const ColorBGR555 ColorBGR555::silver		{ 192, 192, 192 };
const ColorBGR555 ColorBGR555::gray			{ 128, 128, 128 };
const ColorBGR555 ColorBGR555::white		{ 255, 255, 255 };
const ColorBGR555 ColorBGR555::maroon		{ 128,   0,   0 };
const ColorBGR555 ColorBGR555::red			{ 255,   0,   0 };
const ColorBGR555 ColorBGR555::purple		{ 128,   0, 128 };
const ColorBGR555 ColorBGR555::fuchsia		{ 255,   0, 255 };
const ColorBGR555 ColorBGR555::green		{   0, 128,   0 };
const ColorBGR555 ColorBGR555::lime			{   0, 255,   0 };
const ColorBGR555 ColorBGR555::olive		{ 128, 128,   0 };
const ColorBGR555 ColorBGR555::yellow		{ 255, 255,   0 };
const ColorBGR555 ColorBGR555::navy			{   0,   0, 128 };
const ColorBGR555 ColorBGR555::blue			{   0,   0, 255 };
const ColorBGR555 ColorBGR555::teal			{   0, 128, 128 };
const ColorBGR555 ColorBGR555::aqua			{   0, 255, 255 };

bool ColorBGR555::Parse(const char* str)
{
	Color color;
	if (!color.Parse(str)) return false;
	*this = ColorBGR555(color);
	return true;
}

//------------------------------------------------------------------------------
// ColorRGB111
//------------------------------------------------------------------------------
const ColorRGB111 ColorRGB111::zero			{   0,   0,   0 };
const ColorRGB111 ColorRGB111::black		{   0,   0,   0 };
const ColorRGB111 ColorRGB111::silver		{ 192, 192, 192 };
const ColorRGB111 ColorRGB111::gray			{ 128, 128, 128 };
const ColorRGB111 ColorRGB111::white		{ 255, 255, 255 };
const ColorRGB111 ColorRGB111::maroon		{ 128,   0,   0 };
const ColorRGB111 ColorRGB111::red			{ 255,   0,   0 };
const ColorRGB111 ColorRGB111::purple		{ 128,   0, 128 };
const ColorRGB111 ColorRGB111::fuchsia		{ 255,   0, 255 };
const ColorRGB111 ColorRGB111::green		{   0, 128,   0 };
const ColorRGB111 ColorRGB111::lime			{   0, 255,   0 };
const ColorRGB111 ColorRGB111::olive		{ 128, 128,   0 };
const ColorRGB111 ColorRGB111::yellow		{ 255, 255,   0 };
const ColorRGB111 ColorRGB111::navy			{   0,   0, 128 };
const ColorRGB111 ColorRGB111::blue			{   0,   0, 255 };
const ColorRGB111 ColorRGB111::teal			{   0, 128, 128 };
const ColorRGB111 ColorRGB111::aqua			{   0, 255, 255 };

bool ColorRGB111::Parse(const char* str)
{
	Color color;
	if (!color.Parse(str)) return false;
	*this = ColorRGB111(color);
	return true;
}

//------------------------------------------------------------------------------
// ColorBGR111
//------------------------------------------------------------------------------
const ColorBGR111 ColorBGR111::zero			{   0,   0,   0 };
const ColorBGR111 ColorBGR111::black		{   0,   0,   0 };
const ColorBGR111 ColorBGR111::silver		{ 192, 192, 192 };
const ColorBGR111 ColorBGR111::gray			{ 128, 128, 128 };
const ColorBGR111 ColorBGR111::white		{ 255, 255, 255 };
const ColorBGR111 ColorBGR111::maroon		{ 128,   0,   0 };
const ColorBGR111 ColorBGR111::red			{ 255,   0,   0 };
const ColorBGR111 ColorBGR111::purple		{ 128,   0, 128 };
const ColorBGR111 ColorBGR111::fuchsia		{ 255,   0, 255 };
const ColorBGR111 ColorBGR111::green		{   0, 128,   0 };
const ColorBGR111 ColorBGR111::lime			{   0, 255,   0 };
const ColorBGR111 ColorBGR111::olive		{ 128, 128,   0 };
const ColorBGR111 ColorBGR111::yellow		{ 255, 255,   0 };
const ColorBGR111 ColorBGR111::navy			{   0,   0, 128 };
const ColorBGR111 ColorBGR111::blue			{   0,   0, 255 };
const ColorBGR111 ColorBGR111::teal			{   0, 128, 128 };
const ColorBGR111 ColorBGR111::aqua			{   0, 255, 255 };

bool ColorBGR111::Parse(const char* str)
{
	Color color;
	if (!color.Parse(str)) return false;
	*this = ColorBGR111(color);
	return true;
}

}
