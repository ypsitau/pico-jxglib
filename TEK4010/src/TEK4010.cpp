//==============================================================================
// TEK4010.cpp
//==============================================================================
#include "jxglib/TEK4010.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TEK4010
//------------------------------------------------------------------------------
TEK4010& TEK4010::OpenWindow()
{
	printable_.PrintfRaw("\x1b[?38h");
	return *this;
}

TEK4010& TEK4010::CloseWindow()
{
	printable_.PrintfRaw("\x1b[?38l");
	return *this;
}

TEK4010& TEK4010::Beep()
{
	printable_.PutCharRaw(0x07);
	return *this;
}

TEK4010& TEK4010::SetColor(const Color& color)
{
	printable_.Printf("\x1b[%dm", 30 + ColorBGR111(color).value);
	return *this;
}

TEK4010& TEK4010::SetColorBg(const Color& color)
{
	printable_.Printf("\x1b[%dm", 40 + ColorBGR111(color).value);
	return *this;
}

TEK4010& TEK4010::ClearScreen()
{
	SendESC();
	SendCtrl('L');
	return *this;
}

TEK4010& TEK4010::MoveTo(uint16_t x, uint16_t y)
{
	SendChar(',');
	SendCoord(x, y);
	return *this;
}

TEK4010& TEK4010::LineTo(uint16_t x, uint16_t y, LineStyle lineStyle)
{
	SendESC();
	SendChar('`' + static_cast<int>(lineStyle));
	SendCoord(x, y);
	return *this;
}

TEK4010& TEK4010::Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, LineStyle lineStyle)
{
	SendESC();
	SendChar('`' + static_cast<int>(lineStyle));
	SendCoord(x1, y1);
	SendCoord(x2, y2);
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

void TEK4010::SendCoord(uint16_t x, uint16_t y)
{
	SendChar(' ' + static_cast<char>((y >> 5) & 0x1f));
	SendChar('`' + static_cast<char>(y & 0x1f));
	SendChar(' ' + static_cast<char>((x >> 5) & 0x1f));
	SendChar('@' + static_cast<char>(x & 0x1f));
}

}
