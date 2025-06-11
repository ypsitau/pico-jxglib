#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/DS323x.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::i2c_init(i2c0, 100'000);	// Initialize I2C0 at 100 kHz
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	DS323x rtc(i2c0);
	LFS::Flash lfs1("A", 0x1010'0000, 0x0004'0000);	// XIP address and size
	LFS::Flash lfs2("B", 0x1014'0000, 0x0004'0000);	// XIP address and size
	LFS::Flash lfs3("C", 0x0004'0000);				// XIP address and size
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
