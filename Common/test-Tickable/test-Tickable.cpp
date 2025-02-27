#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Tickable.h"

using namespace jxglib;

class Blink : public Tickable {
private:
	const GPIO& gpio_;
	bool value_;
public:
	Blink(const GPIO& gpio, uint32_t msec) : Tickable(msec), gpio_{gpio}, value_{false} {}
	void Initialize() { Tickable::AddTickable(this); gpio_.init().set_dir_OUT(); }
	void OnTick() { gpio_.put(value_); value_ = !value_; }
};

int main()
{
	::stdio_init_all();
	Blink blinks[] = {{GPIO10, 200}, {GPIO11, 400}, {GPIO12, 800}, {GPIO13, 1600}};
	for (int i = 0; i < count_of(blinks); i++) blinks[i].Initialize();
	for (;;) Tickable::Tick();
}
