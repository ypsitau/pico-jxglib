//==============================================================================
// jxglib/LCD1602.h
//==============================================================================
#ifndef PICO_JXGLIB_LCD1602_H
#define PICO_JXGLIB_LCD1602_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LCD1602
//------------------------------------------------------------------------------
struct LCD1602 {
public:
	class Raw {
	private:
		i2c_inst_t* i2c_;
		uint8_t addr_;
	public:
		Raw(i2c_inst_t* i2c, uint8_t addr) : i2c_{i2c}, addr_{addr} {}
	public:
		
	};
public:
	static const uint8_t DefaultAddr = 0x27;
	Raw raw;
public:
	LCD1602(i2c_inst_t* i2c, uint8_t addr = DefaultAddr) : raw(i2c, addr) {}
};

}

#endif
