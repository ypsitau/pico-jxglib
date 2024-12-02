//==============================================================================
// TEK4010.cpp
//==============================================================================
#include "jxglib/TEK4010.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TEK4010
//------------------------------------------------------------------------------
TEK4010& TEK4010::Open()
{
	printable_.PrintfRaw("\x1b[?38h");
	return *this;
}

TEK4010& TEK4010::Close()
{
	printable_.PrintfRaw("\x1b[?38l");
	return *this;
}

TEK4010& TEK4010::SetColor(const Color& color)
{
	printable_.Printf("\x1b[%dm", 30 +
		(color.b? (1 << 2) : 0) | (color.g? (1 << 1) : 0) | (color.r? (1 << 0) : 0));
	return *this;
}

TEK4010& TEK4010::SetColorBg(const Color& color)
{
	printable_.Printf("\x1b[%dm", 40 +
		(color.b? (1 << 2) : 0) | (color.g? (1 << 1) : 0) | (color.r? (1 << 0) : 0));
	return *this;
}

TEK4010& TEK4010::TextMode()
{
	printable_.PutCharRaw(0x1f); //US
	return *this;
}

TEK4010& TEK4010::PutCoordX(uint16_t x)
{
	printable_.PutCharRaw(' ' | static_cast<char>((x >> 5) & 0x1f));
	printable_.PutCharRaw('@' | static_cast<char>(x & 0x1f));
	return *this;
}

TEK4010& TEK4010::PutCoordY(uint16_t y)
{
	printable_.PutCharRaw(' ' | static_cast<char>((y >> 5) & 0x1f));
	printable_.PutCharRaw('`' | static_cast<char>(y & 0x1f));
	return *this;
}

TEK4010& TEK4010::ClearScreen()
{
	printable_.PrintfRaw("\x1b\x0c\r\n");
	return *this;
}

TEK4010& TEK4010::MoveTo(uint16_t x, uint16_t y)
{
	printable_.PutCharRaw(0x1d); //GS
	PutCoordY(y);
	PutCoordX(x);
	return *this;
}

TEK4010& TEK4010::DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int lineStyle)
{
	printable_.PrintRaw("\x1d\x1b");
	printable_.PutCharRaw('`' | lineStyle);
	PutCoordY(y1);
	PutCoordX(x1);
	PutCoordY(y2);
	PutCoordX(x2);
	return *this;
}

TEK4010& TEK4010::Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	VPrintf(format, args);
	va_end(args);
	return *this;
}

void TEK4010::Test()
{
}

}
