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
		uint8_t GetAddr() const { return addr_; }
	public:
		void WriteByte(uint8_t value) const {
			::i2c_write_blocking(i2c_, addr_, &value, 1, false);
		}
		void SendNibble(uint8_t value) {
			const uint64_t usecDelay = 600;
			WriteByte(value);
			::sleep_us(usecDelay);
			WriteByte(value | 0x04);
			::sleep_us(usecDelay);
			WriteByte(value);
			::sleep_us(usecDelay);
		}
		// bit3: backlight
		// bit0: character/command
		void SendByte(uint8_t value, uint8_t mode) {
			SendNibble((value & 0xf0) | mode);
			SendNibble((value << 4) | mode);
		}
		
	};
public:
	static const uint8_t DefaultAddr = 0x27;
	Raw raw;
public:
	LCD1602(i2c_inst_t* i2c, uint8_t addr = DefaultAddr) : raw(i2c, addr) {}
};

}

#endif
