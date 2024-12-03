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
		Dotted	= 1,
		DotDashed	= 2,
		ShortDashed	= 3,
		LongDashed	= 4,
	};
private:
	Printable& printable_;
	LineStyle lineStyleCur_;
public:
	TEK4010(Printable& printable) : printable_{printable}, lineStyleCur_{LineStyle::Solid} {}
public:
	TEK4010& OpenWindow();
	TEK4010& CloseWindow();
public:
	TEK4010& SetLineStyle(LineStyle lineStyle) { lineStyleCur_ = lineStyle; return *this; }
	TEK4010& Beep();
	TEK4010& SetColor(const Color& color);
	TEK4010& SetColorBg(const Color& color);
	TEK4010& ClearScreen();
	TEK4010& MoveTo(uint16_t x, uint16_t y);
	TEK4010& LineTo(uint16_t x, uint16_t y, LineStyle lineStyle);
	TEK4010& LineTo(uint16_t x, uint16_t y) { return LineTo(x, y, lineStyleCur_); }
	TEK4010& Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, LineStyle lineStyle);
	TEK4010& Line(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
		return Line(x1, y1, x2, y2, lineStyleCur_);
	}
	TEK4010& PrintRaw(const char* str) { printable_.PrintRaw(str); return *this; }
	TEK4010& PutChar(char ch) { EnterAlphaMode(); printable_.PutChar(ch); return *this; }
	TEK4010& Print(const char* str) { EnterAlphaMode(); printable_.Print(str); return *this; }
	TEK4010& Println(const char* str) { EnterAlphaMode(); printable_.Println(str); return *this; }
	TEK4010& VPrintf(const char* format, va_list args) { EnterAlphaMode(); printable_.VPrintf(format, args); return *this; }
	TEK4010& Printf(const char* format, ...);
public:
	void SendESC() { printable_.PutCharRaw(0x1b); }
	void SendCtrl(char ch) { printable_.PutCharRaw(ch - 0x40); }		// superscript C
	void SendCtrlShift(char ch) { printable_.PutCharRaw(ch - 0x30); }	// superscript C and S
	void SendChar(char ch) { printable_.PutCharRaw(ch); }
	void SendString(const char* str) { printable_.PrintRaw(str); }
	void SendCoord(uint16_t x, uint16_t y);

	void EnterMarkerMode()	{ SendCtrlShift('L'); }	// Control: FS
	void EnterGraphMode()	{ SendCtrlShift('M'); }	// Control: GS
	void EnterPlotMode()	{ SendCtrlShift('N'); }	// Control: RS
	void EnterAlphaMode()	{ SendCtrlShift('O'); }	// Control: US
};

}

#endif
