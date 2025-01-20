//==============================================================================
// jxglib/LVGL.h
//==============================================================================
#ifndef PICO_JXGLIB_LVGL_H
#define PICO_JXGLIB_LVGL_H
#include <lvgl/lvgl.h>
#include "jxglib/LVGL/Adapter.h"

namespace jxglib::LVGL {

/**
 * @brief Initialize LVGL by calling lv_init() and lv_tick_set_cb() internally.
 */
void Initialize();

}

#endif
