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
		void SendRawByte(uint8_t value) const {
			::i2c_write_blocking(i2c_, addr_, &value, 1, false);
		}
		void SendNibble(uint8_t value) {
			const uint64_t usecDelay = 600;
			SendRawByte(value | (0 << 2));
			::sleep_us(usecDelay);
			SendRawByte(value | (1 << 2));
			::sleep_us(usecDelay);
			SendRawByte(value | (0 << 2));
			::sleep_us(usecDelay);
		}
		// bit3: Backlight
		// bit2: Clock
		// bit1: R/W (1: Read, 0: Write)
		// bit0: RS (1: Data, 0: Instruction)
		void SendByte(uint8_t value, uint8_t mode) {
			mode |= (1 << 3);	// Backlight
			SendNibble((value & 0xf0) | mode);
			SendNibble((value << 4) | mode);
		}
		void SendInst(uint8_t inst) { SendByte(inst, 0); }
		void SendData(uint8_t data) { SendByte(data, 1); }
	};
public:
	static const uint8_t DefaultAddr = 0x27;
	Raw raw;
public:
	LCD1602(i2c_inst_t* i2c, uint8_t addr = DefaultAddr) : raw(i2c, addr) {}
};

}

#endif
