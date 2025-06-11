#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/DS323x.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

int main()
{
	stdio_init_all();
	::i2c_init(i2c0, 400'000);
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	DS323x rtc(i2c0);
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
