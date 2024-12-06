//==============================================================================
// jxglib/TelePlot.h
//==============================================================================
#ifndef PICO_JXGLIB_TELEPLOT_H
#define PICO_JXGLIB_TELEPLOT_H
#include "pico/stdlib.h"
#include "jxglib/UART.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TelePlot
//------------------------------------------------------------------------------
class TelePlot {
private:
	Printable& printable_;
public:
	TelePlot(Printable& printable) : printable_{printable} {}
};

}

#endif
