//==============================================================================
// jxglib/Device/MCP4726.h
//==============================================================================
#ifndef PICO_JXGLIB_DEVICE_MCP4726_H
#define PICO_JXGLIB_DEVICE_MCP4726_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/Common.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// Device::MCP4726
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

}

#endif
