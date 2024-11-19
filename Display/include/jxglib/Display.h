//==============================================================================
// jxglib/Display.h
//==============================================================================
#ifndef PICO_JXGLIB_DISPLAY_H
#define PICO_JXGLIB_DISPLAY_H
#include "pico/stdlib.h"
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Display
//------------------------------------------------------------------------------
struct Display : public Drawable {
public:
	Display(uint32_t capabilities, const Format& format, int width, int height) :
			Drawable(capabilities, format, width, height) {}
};

}

#endif
