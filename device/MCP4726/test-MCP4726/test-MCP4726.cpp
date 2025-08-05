#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

//------------------------------------------------------------------------------
// MCP4726
//------------------------------------------------------------------------------
class MCP4726 {
public:
	enum class VRef { VDD = 0, VDD_1 = 1, Unbuffered = 2, Buffered = 3};
	enum class PowerState { Normal = 0, PowerDown1K = 1, PowerDown100K = 2, PowerDown500K = 3 };
	enum class Gain { x1 = 0, x2 = 1 };
	class Config {
	private:
		uint8_t data_;
	public:
		Config(uint8_t data = 0x00) : data_(data) {}
		operator uint8_t() const { return data_; }
		VRef GetVRef() const { return static_cast<VRef>((data_ >> 3) & 0x03); }
		PowerState GetPowerState() const { return static_cast<PowerState>((data_ >> 1) & 0x03); }
		Gain GetGain() const { return static_cast<Gain>(data_ & 0x01); }
	public:
		void Print(Printable& tout) const;
	};
private:
	i2c_inst_t* i2c_;
	uint32_t msecTimeout_;
	uint8_t addr_;
public:
	MCP4726(i2c_inst_t* i2c, uint32_t msecTimeout = 300, uint8_t addr = 0x60);
public:
	bool WriteConfig(const Config& cfg);
	bool Write(uint16_t value);
	bool WriteAllMemory(const Config& cfg, uint16_t value);
	bool ReadConfig_(Config& cfg, bool volatileFlag) const;
	bool ReadConfig(Config& cfg) const { return ReadConfig_(cfg, true); }
	bool ReadConfig_NV(Config& cfg) const { return ReadConfig_(cfg, false); }
};

MCP4726::MCP4726(i2c_inst_t* i2c, uint32_t msecTimeout, uint8_t addr) :
	i2c_{i2c}, msecTimeout_{msecTimeout}, addr_{addr}
{}

bool MCP4726::WriteConfig(const Config& cfg)
{
	uint8_t data[1];
	data[0] = 0x80 | cfg;
	return ::i2c_write_blocking_until(i2c_, addr_, data, sizeof(data),
					false, ::make_timeout_time_ms(msecTimeout_)) == sizeof(data);
}

bool MCP4726::Write(uint16_t value)
{
	uint8_t data[2];
	data[0] = 0x00 | ((value >> 8) & 0x0f);
	data[1] = value & 0xff;
	return ::i2c_write_blocking_until(i2c_, addr_, data, sizeof(data),
					false, ::make_timeout_time_ms(msecTimeout_)) == sizeof(data);
}

bool MCP4726::WriteAllMemory(const Config& cfg, uint16_t value)
{
	uint8_t data[3];
	data[0] = 0x60 | cfg;
	data[1] = (value >> (8 + 4)) & 0x0f;
	data[2] = (value >> 4) & 0xff;
	return ::i2c_write_blocking_until(i2c_, addr_, data, sizeof(data),
					false, ::make_timeout_time_ms(msecTimeout_)) == sizeof(data);
}

bool MCP4726::ReadConfig_(Config& cfg, bool volatileFlag) const
{
	uint8_t data[6];
	if (::i2c_read_blocking_until(i2c_, addr_, data, sizeof(data),
					false, ::make_timeout_time_ms(msecTimeout_)) != sizeof(data)) return false;
	cfg = Config(data[volatileFlag? 0 : 3]);
	return true;
}

void MCP4726::Config::Print(Printable& tout) const
{
	VRef vref = GetVRef();
	PowerState powerState = GetPowerState();
	Gain gain = GetGain();
	tout.Printf("VRef:%s PowerState:%s Gain:%s\n",
		vref == VRef::VDD ? "VDD" :
		vref == VRef::VDD_1 ? "VDD" :
		vref == VRef::Unbuffered ? "Unbuffered" : "Buffered",
		powerState == PowerState::Normal ? "Normal" :
		powerState == PowerState::PowerDown1K ? "PowerDown1K"
		: powerState == PowerState::PowerDown100K ? "PowerDown100K" : "PowerDown500K",
		gain == Gain::x1 ? "x1" : "x2");
}


MCP4726 dac(i2c1);

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
	MCP4726::Config cfg;
	dac.ReadConfig(cfg);
	for (;;) {
		for (int i = 0; i < 256; ++i) {
			dac.Write(i *  16);
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
