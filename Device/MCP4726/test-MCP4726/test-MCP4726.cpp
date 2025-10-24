#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Device/MCP4726.h"

using namespace jxglib;

Device::MCP4726 dac(i2c1);

ShellCmd(mcp4726, "command to control D/A converter MCP4726")
{
#if 0
	MCP4726::Config cfg;
	dac.ReadConfig(cfg);
	cfg.Print(tout);
	if (argc >= 2) {
		char* endp;
		uint16_t value = strtol(argv[1], &endp, 0);
		if (value > 4095) {
			Printf("エラー: 値は0-4095の範囲で指定してください (指定値: %d)\n", value);
			return Result::Error;
		}
		Printf("MCP4726にDAC値 %d を設定中...", value);
		if (dac.Write(value)) {
			Printf("成功\n");
			float voltage = (float)value / 4095.0f * 3.3f;
			Printf("出力電圧: 約%.2fV\n", voltage);
		} else {
			Printf("失敗\n");
		}
	}
#endif
	Device::MCP4726::Config cfg;
	dac.ReadConfig(cfg);
	for (;;) {
		for (int i = 0; i < 256; ++i) {
			dac.Write(i * 16);
		}
		if (Tickable::TickSub()) break;
	}
	return Result::Success;
}

int main()
{
	GPIO2.set_function_I2C1_SDA().pull_up();
	GPIO3.set_function_I2C1_SCL().pull_up();
	::i2c_init(i2c1, 400'000);
	::stdio_init_all();
	LABOPlatform::Instance.AttachStdio().Initialize();
	for (;;) Tickable::Tick();
}
