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
	uint16_t ReadPosition(uint8_t adc);
	uint16_t ReadX() { return ReadPosition(0b101); }
	uint16_t ReadY() { return ReadPosition(0b001); }
	uint16_t ReadZ1() { return ReadPosition(0b011); }
	uint16_t ReadZ2() { return ReadPosition(0b100); }
private:
	static uint8_t ComposeCmd(uint8_t adc, uint8_t mode, uint8_t ref, uint8_t powerDownMode) {
		return static_cast<uint8_t>((0b1  << 7) | (adc  << 4) | (mode  << 3) | (ref  << 2) | (powerDownMode << 0));
	}
};

}

#endif
