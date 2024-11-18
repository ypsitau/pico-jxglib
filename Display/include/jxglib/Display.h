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
	Display(const Format& format, int width, int height) : Drawable(format, width, height) {}
};

}

#endif
