//==============================================================================
// LVGL.cpp
//==============================================================================
#include "jxglib/LVGL.h"

namespace jxglib::LVGL {

static uint32_t GetTickCB();

void Initialize()
{
	::lv_init();
    ::lv_tick_set_cb(GetTickCB);
}

static uint32_t GetTickCB()
{
    return ::to_ms_since_boot(::get_absolute_time());
}

}
