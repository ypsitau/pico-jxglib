//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include "pico/stdlib.h"
#include "jxglib/Printable.h"
#include "jxglib/Editable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
class Terminal : public Printable, public Editable {
public:
	Terminal(int bytesHistoryBuff);
};

}

#endif
