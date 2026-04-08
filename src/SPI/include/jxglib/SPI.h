//==============================================================================
// jxglib/SPI.h
//==============================================================================
#ifndef PICO_JXGLIB_SPI_H
#define PICO_JXGLIB_SPI_H
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// SPI
//------------------------------------------------------------------------------
class SPI {
public:
	struct Config {
		uint SCK;
		uint MOSI;
		uint MISO;
		uint CS;
		uint baudrate;
		spi_cpol_t cpol;
		spi_cpha_t cpha;
		spi_order_t order;
		uint8_t byteDummy;
	};
private:
	spi_inst_t* spi_;
public:
	Config config;
public:
	static SPI None;
public:
	SPI(spi_inst_t* spi);
	SPI(const SPI&) = delete;
public:
	operator spi_inst_t*() { return spi_; }
public:
	bool IsValid() const { return !!spi_; }
public:
	uint init(uint baudrate = 1000000) { return ::spi_init(spi_, baudrate); }
	void deinit() { ::spi_deinit(spi_); }
	uint set_baudrate(uint baudrate) { return ::spi_set_baudrate(spi_, baudrate); }
	uint get_index() const { return ::spi_get_index(spi_); }
	spi_hw_t *get_hw() const { return ::spi_get_hw(spi_); }
	static SPI& get_instance(uint num);
	uint get_dreq(bool is_tx) const { return ::spi_get_dreq(spi_, is_tx); }
	uint get_dreq_rx() const { return ::spi_get_dreq(spi_, false); }
	uint get_dreq_tx() const { return ::spi_get_dreq(spi_, true); }
	bool is_readable() const { return ::spi_is_readable(spi_); }
	bool is_writable() const { return ::spi_is_writable(spi_); }
	int read_blocking(uint8_t repeated_tx_data, uint8_t* dst, size_t len) {
		return ::spi_read_blocking(spi_, repeated_tx_data, dst, len);
	}
	int write_blocking(const uint8_t* src, size_t len) {
		return ::spi_write_blocking(spi_, src, len);
	}
	int write_read_blocking(const uint8_t* src, uint8_t* dst, size_t len) {
		return ::spi_write_read_blocking(spi_, src, dst, len);
	}
	void set_format(uint data_bits, spi_cpol_t cpol, spi_cpha_t cpha, spi_order_t order) {
		::spi_set_format(spi_, data_bits, cpol, cpha, order);
	}
	void set_slave(bool slave) { ::spi_set_slave(spi_, slave); }
	size_t get_const_hw_size() const { return sizeof(spi_hw_t); }
	uint8_t read_byte() { 
		uint8_t data;
		::spi_read_blocking(spi_, 0x00, &data, 1);
		return data;
	}
	void write_byte(uint8_t data) {
		::spi_write_blocking(spi_, &data, 1);
	}
	uint8_t write_read_byte(uint8_t data) {
		uint8_t rx_data;
		::spi_write_read_blocking(spi_, &data, &rx_data, 1);
		return rx_data;
	}
public:
	int Read(uint8_t* dst, size_t len, uint8_t repeated_tx_data = 0x00);
	int Write(const uint8_t* src, size_t len);
	int WriteRead(const uint8_t* src, uint8_t* dst, size_t len);
};

extern SPI SPI0;
extern SPI SPI1;

}

#endif
