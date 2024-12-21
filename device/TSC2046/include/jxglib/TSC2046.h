//==============================================================================
// jxglib/TSC2046.h
//==============================================================================
#ifndef PICO_JXGLIB_TSC2046_H
#define PICO_JXGLIB_TSC2046_H
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/spi.h"
#include "jxglib/GPIO.h"
namespace jxglib {

//------------------------------------------------------------------------------
// TSC2046
//------------------------------------------------------------------------------
class TSC2046 {
private:
	spi_inst_t* spi_;
	const GPIO& gpio_CS_;
	const GPIO& gpio_IRQ_;
public:
	TSC2046(spi_inst_t* spi, const GPIO& gpio_CS, const GPIO& gpio_IRQ);
public:
	void Initialize();
	bool IsTouched();
	void SendCmd(uint8_t cmd);
	uint8_t ReadADC8Bit(uint8_t adc);
	uint16_t ReadADC12Bit(uint8_t adc);
	bool ReadPosition(uint16_t* px, uint16_t* py, uint8_t* pz1 = nullptr, uint8_t* pz2 = nullptr);
private:
	void SPISetFormat() {
		::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	}
	static uint8_t ComposeCmd(uint8_t adc, uint8_t mode, uint8_t ref, uint8_t powerDownMode) {
		return static_cast<uint8_t>((0b1  << 7) | (adc  << 4) | (mode  << 3) | (ref  << 2) | (powerDownMode << 0));
	}
};

}

#endif
