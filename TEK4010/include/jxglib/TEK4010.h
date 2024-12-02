//==============================================================================
// jxglib/TEK4010.h
//==============================================================================
#ifndef PICO_JXGLIB_TEK4010_H
#define PICO_JXGLIB_TEK4010_H
#include "pico/stdlib.h"
#include "jxglib/Color.h"
#include "jxglib/UART.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TEK4010
//------------------------------------------------------------------------------
struct TEK4010 {
public:
	enum LineStyle {
		Solid	= 0,
	};
private:
	Printable& printable_;
public:
	TEK4010(Printable& printable) : printable_{printable} {}
public:
	TEK4010& OpenWindow();
	TEK4010& CloseWindow();
public:
	TEK4010& Beep();
	TEK4010& SetColor(const Color& color);
	TEK4010& SetColorBg(const Color& color);
	TEK4010& ClearScreen();
	TEK4010& MoveTo(uint16_t x, uint16_t y);
	TEK4010& DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, int lineStyle);
	TEK4010& PutChar(char ch) { EnterAlphaMode(); printable_.PutChar(ch); return *this; }
	TEK4010& Print(const char* str) { EnterAlphaMode(); printable_.Print(str); return *this; }
	TEK4010& Println(const char* str) { EnterAlphaMode(); printable_.Println(str); return *this; }
	TEK4010& VPrintf(const char* format, va_list args) { EnterAlphaMode(); printable_.VPrintf(format, args); return *this; }
	TEK4010& Printf(const char* format, ...);
public:
	void EnterMarkerMode() { printable_.PutCharRaw('L' - 0x30); }
	void EnterGraphMode() { printable_.PutCharRaw('M' - 0x30); }
	void EnterPlotMode() { printable_.PutCharRaw('N' - 0x30); }
	void EnterAlphaMode() { printable_.PutCharRaw('O' - 0x30); }
	void PutCoordX(uint16_t x);
	void PutCoordY(uint16_t y);
};

}

#endif
