//==============================================================================
// LVGL.cpp
//==============================================================================
#include "jxglib/LVGL.h"

namespace jxglib::LVGL {

class TickableEx : public Tickable {
public:
	static TickableEx Instance;
public:
	virtual void OnTick() override { ::lv_timer_handler(); }
};

TickableEx TickableEx::Instance;

static uint32_t GetTickMSecCB() { return ::to_ms_since_boot(::get_absolute_time()); }

void Initialize(uint32_t msecTick)
{
	::lv_init();
	::lv_tick_set_cb(GetTickMSecCB);
	TickableEx::Instance.SetTick(msecTick);
	Tickable::AddTickable(TickableEx::Instance);
}

}
