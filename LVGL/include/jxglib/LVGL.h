//==============================================================================
// jxglib/LVGL.h
//==============================================================================
#ifndef PICO_JXGLIB_LVGL_H
#define PICO_JXGLIB_LVGL_H
#include <lvgl/lvgl.h>
#include "jxglib/Tickable.h"
#include "jxglib/LVGL/Adapter.h"

namespace jxglib::LVGL {

void Initialize(uint32_t msecTick = 5);

}

#endif
