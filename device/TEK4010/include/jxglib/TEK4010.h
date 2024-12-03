//==============================================================================
// jxglib/TEK4010.h
//==============================================================================
#ifndef PICO_JXGLIB_TEK4010_H
#define PICO_JXGLIB_TEK4010_H
#include "pico/stdlib.h"
#include "jxglib/Color.h"
#include "jxglib/Point.h"
#include "jxglib/Size.h"
#include "jxglib/UART.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TEK4010
//------------------------------------------------------------------------------
struct TEK4010 {
public:
	enum LineStyle {
		Solid	= 0,
		Dotted	= 1,
		DotDashed	= 2,
		ShortDashed	= 3,
		LongDashed	= 4,
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
	TEK4010& SetLineStyle(LineStyle lineStyle);
	TEK4010& MoveTo(int x, int y);
	TEK4010& MoveTo(const Point& pt) { return MoveTo(pt.x, pt.y); }
	TEK4010& DrawLineTo(int x, int y);
	TEK4010& DrawLineTo(const Point& pt) { return DrawLineTo(pt.x, pt.y); }
	TEK4010& DrawLine(int x1, int y1, int x2, int y2);
	TEK4010& DrawLine(const Point& pt1, const Point& pt2) { return DrawLine(pt1.x, pt1.y, pt2.x, pt2.y); }
	TEK4010& DrawRect(int x, int y, int width, int height);
	TEK4010& DrawRect(const Point& pt, const Size& size) { return DrawRect(pt.x, pt.y, size.width, size.height); }
public:
	TEK4010& PrintRaw(const char* str) { printable_.PrintRaw(str); return *this; }
	TEK4010& PutChar(char ch) { SendCtrlShift('O'); printable_.PutChar(ch); return *this; }
	TEK4010& Print(const char* str) { SendCtrlShift('O'); printable_.Print(str); return *this; }
	TEK4010& Println(const char* str) { SendCtrlShift('O'); printable_.Println(str); return *this; }
	TEK4010& VPrintf(const char* format, va_list args) { SendCtrlShift('O'); printable_.VPrintf(format, args); return *this; }
	TEK4010& Printf(const char* format, ...);
public:
	void SendESC() { printable_.PutCharRaw(0x1b); }
	void SendCtrl(char ch) { printable_.PutCharRaw(ch - 0x40); }		// superscript C
	void SendCtrlShift(char ch) { printable_.PutCharRaw(ch - 0x30); }	// superscript C and S
	void SendChar(char ch) { printable_.PutCharRaw(ch); }
	void SendString(const char* str) { printable_.PrintRaw(str); }
	void SendCoord(int x, int y);
};

}

#endif
