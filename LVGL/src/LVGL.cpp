//==============================================================================
// LVGL.cpp
//==============================================================================
#include "jxglib/LVGL.h"

namespace jxglib::LVGL {

static uint32_t GetTickMSecCB() { return ::to_ms_since_boot(::get_absolute_time()); }

void Initialize()
{
	::lv_init();
	::lv_tick_set_cb(GetTickMSecCB);
}

}
