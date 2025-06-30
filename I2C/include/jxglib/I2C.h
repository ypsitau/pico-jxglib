//==============================================================================
// jxglib/I2C.h
//==============================================================================
#ifndef PICO_JXGLIB_I2C_H
#define PICO_JXGLIB_I2C_H
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// I2C
//------------------------------------------------------------------------------
class I2C {
private:
	i2c_inst_t* i2c_;
public:
	I2C(i2c_inst_t* i2c);
public:
	operator i2c_inst_t*() { return i2c_; }
public:
	uint init(uint baudrate = 100'000) { return ::i2c_init(i2c_, baudrate); }
	void deinit() { ::i2c_deinit(i2c_); }
	uint set_baudrate(uint baudrate) { return ::i2c_set_baudrate(i2c_, baudrate); }
	void set_slave_mode(uint8_t addr, bool slave = true) { ::i2c_set_slave_mode(i2c_, slave, addr); }
	void set_master_mode() { ::i2c_set_slave_mode(i2c_, false, 0x00); }
	uint i2c_get_index() const { return ::i2c_get_index(i2c_); }
	i2c_hw_t *get_hw() const { return ::i2c_get_hw(i2c_); }
	static I2C& get_instance(uint num);
	uint get_dreq(bool is_tx) const { return ::i2c_get_dreq(i2c_, is_tx); }
	uint get_dreq_rx() const { return ::i2c_get_dreq(i2c_, false); }
	uint get_dreq_tx() const { return ::i2c_get_dreq(i2c_, true); }
	bool get_read_available() const { return ::i2c_get_read_available(i2c_); }
	int read_blocking(uint8_t addr, uint8_t* dst, size_t len, bool nostop) {
		return ::i2c_read_blocking(i2c_, addr, dst, len, nostop);
	}
	int read_blocking_until(uint8_t addr, uint8_t* dst, size_t len, bool nostop, absolute_time_t until) {
		return ::i2c_read_blocking_until(i2c_, addr, dst, len, nostop, until);
	}
	uint8_t read_byte_raw() { return ::i2c_read_byte_raw(i2c_); }
	void read_raw_blocking(uint8_t* dst, size_t len) { ::i2c_read_raw_blocking(i2c_, dst, len); }
	int read_timeout_us(uint8_t addr, uint8_t* dst, size_t len, bool nostop, uint timeout_us) {
		return ::i2c_read_timeout_us(i2c_, addr, dst, len, nostop, timeout_us);
	}
	bool get_write_available() const { return ::i2c_get_write_available(i2c_); }
	int write_blocking(uint8_t addr, const uint8_t* src, size_t len, bool nostop) { return ::i2c_write_blocking(i2c_, addr, src, len, nostop); }
	int write_blocking_until(uint8_t addr, const uint8_t* src, size_t len, bool nostop, absolute_time_t until) {
		return ::i2c_write_blocking_until(i2c_, addr, src, len, nostop, until);
	}
	void write_byte_raw(uint8_t value) { ::i2c_write_byte_raw(i2c_, value); }
	void write_raw_blocking(const uint8_t* src, size_t len) { ::i2c_write_raw_blocking(i2c_, src, len); }
	int write_timeout_us(uint8_t addr, const uint8_t* src, size_t len, bool nostop, uint timeout_us) {
		return ::i2c_write_timeout_us(i2c_, addr, src, len, nostop, timeout_us);
	}
public:
	int Read(uint8_t addr, void* dst, size_t len, bool nostop, uint usecTimeout);
	int Write(uint8_t addr, const void* src, size_t len, bool nostop);
};

extern I2C I2C0;
extern I2C I2C1;

}

#endif
