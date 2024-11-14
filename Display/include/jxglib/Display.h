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
	Display(int width, int height) : Drawable(width, height) {}
};

}

#endif
