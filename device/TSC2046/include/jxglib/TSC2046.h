//==============================================================================
// jxglib/TSC2046.h
//==============================================================================
#ifndef PICO_JXGLIB_TSC2046_H
#define PICO_JXGLIB_TSC2046_H
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/spi.h"
#include "jxglib/GPIO.h"
#include "jxglib/Drawable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TSC2046
//------------------------------------------------------------------------------
class TSC2046 {
public:
	class Adjuster {
	private:
		int valueMax_;
		float slope_;
		int intercept_;
	public:
		Adjuster(int valueMax = 1, float slope = 1., int intercept = 0) : valueMax_{valueMax}, slope_{slope}, intercept_{intercept} {}
	public:
		int Adjust(int value) const;
		int GetValueMax() const { return valueMax_; }
		float GetSlope() const { return slope_; }
		int GetIntercept() const { return intercept_; }
	};
private:
	spi_inst_t* spi_;
	const GPIO& gpio_CS_;
	const GPIO& gpio_IRQ_;
	Adjuster adjusterX_;
	Adjuster adjusterY_;
	bool hvFlippedFlag_;
private:
	static const int z1Threshold = 3;
public:
	TSC2046(spi_inst_t* spi, const GPIO& gpio_CS, const GPIO& gpio_IRQ);
public:
	void Initialize(bool hvFlippedFlag);
	bool Calibrate(Drawable& drawable);
	bool ReadPositionRaw(int* px, int* py, int* pz1 = nullptr, int* pz2 = nullptr);
	bool ReadPosition(int* px, int* py);
	bool IsTouched();
	void SetAdjusterX(const Adjuster& adjuster) { adjusterX_ = adjuster; }
	void SetAdjusterY(const Adjuster& adjuster) { adjusterY_ = adjuster; }
	const Adjuster& GetAdjusterX() const { return adjusterX_; }
	const Adjuster& GetAdjusterY() const { return adjusterY_; }
	void PrintCalibration() const;
private:
	void SendCmd(uint8_t cmd);
	uint8_t ReadADC8Bit(uint8_t adc);
	uint16_t ReadADC12Bit(uint8_t adc);
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
