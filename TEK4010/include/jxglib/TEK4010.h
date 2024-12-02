//==============================================================================
// jxglib/TEK4010.h
//==============================================================================
#ifndef PICO_JXGLIB_TEK4010_H
#define PICO_JXGLIB_TEK4010_H
#include "pico/stdlib.h"
#include "jxglib/UART.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TEK4010
//------------------------------------------------------------------------------
struct TEK4010 {
private:
	Printable& printable_;
public:
	TEK4010(Printable& printable) : printable_{printable} {}
public:
	void Test();
};

}

#endif
