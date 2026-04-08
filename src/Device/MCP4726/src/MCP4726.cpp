//==============================================================================
// MCP4726.cpp
//==============================================================================
#include "jxglib/Device/MCP4726.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// Device::MCP4726
//------------------------------------------------------------------------------
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

}
