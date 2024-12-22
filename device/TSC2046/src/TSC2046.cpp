//==============================================================================
// TSC2046.cpp
//==============================================================================
#include "jxglib/TSC2046.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TSC2046
//------------------------------------------------------------------------------
TSC2046::TSC2046(spi_inst_t* spi, const GPIO& gpio_CS, const GPIO& gpio_IRQ) :
		spi_{spi}, gpio_CS_{gpio_CS}, gpio_IRQ_{gpio_IRQ} {}

void TSC2046::Initialize()
{
	gpio_CS_.init().set_dir_OUT().put(1);
	gpio_IRQ_.init().set_dir_IN();
}

bool TSC2046::Calibrate(Drawable& drawable)
{
	const int msecDelay = 100;
	const int nSamples = 10;
	int sizeCross = drawable.GetWidth() * 10 / 100; 								// 10% of the drawable's width
	Rect rectSense = drawable.GetRect().Deflate(drawable.GetWidth() * 10 / 100);	// 10% of the drawable's width
	int wdCross = sizeCross * 10 / 100;
	if (wdCross == 0) wdCross = 1;
	Point ptTbl[2] = {rectSense.GetPointNW(), rectSense.GetPointSE()};
	Point adcTbl[2];
	for (int i = 0; i < count_of(ptTbl); i++) {
		drawable.Clear().DrawCross(ptTbl[i].x, ptTbl[i].y, sizeCross, sizeCross, wdCross, wdCross).Refresh();
		int xSum = 0, ySum = 0;
		int x, y;
		while (IsTouched()) ::sleep_ms(msecDelay);
		for (int iSample = 0; iSample < nSamples; iSample++) {
			while (!ReadPositionRaw(&x, &y)) ::sleep_ms(msecDelay);
			xSum += x, ySum += y;
			::sleep_ms(msecDelay);
		}
		adcTbl[i].x = xSum / nSamples, adcTbl[i].y = ySum / nSamples;
	}
	drawable.Clear().Refresh();
	if (adcTbl[0].x == adcTbl[1].x || adcTbl[0].y == adcTbl[1].y) return false;
	if (adcTbl[0].x < adcTbl[1].x) {
		float slope = static_cast<float>(ptTbl[1].x - ptTbl[0].x) / (adcTbl[1].x - adcTbl[0].x);
		adjusterX_ = Adjuster(drawable.GetWidth() - 1, slope, ptTbl[0].x - static_cast<int>(slope * adcTbl[0].x));
	} else {
		float slope = static_cast<float>(ptTbl[0].x - ptTbl[1].x) / (adcTbl[0].x - adcTbl[1].x);
		adjusterX_ = Adjuster(drawable.GetWidth() - 1, slope, ptTbl[1].x - static_cast<int>(slope * adcTbl[1].x));
	}
	if (adcTbl[0].y < adcTbl[1].y) {
		float slope = static_cast<float>(ptTbl[1].y - ptTbl[0].y) / (adcTbl[1].y - adcTbl[0].y);
		adjusterY_ = Adjuster(drawable.GetHeight() - 1, slope, ptTbl[0].y - static_cast<int>(slope * adcTbl[0].y));
	} else {
		float slope = static_cast<float>(ptTbl[0].y - ptTbl[1].y) / (adcTbl[0].y - adcTbl[1].y);
		adjusterY_ = Adjuster(drawable.GetHeight() - 1, slope, ptTbl[1].y - static_cast<int>(slope * adcTbl[1].y));
	}
	//for (int i = 0; i < count_of(ptTbl); i++) {
	//	::printf("%d, %d -> %d, %d\n", adcTbl[i].x, adcTbl[i].y, ptTbl[i].x, ptTbl[i].y);
	//}
	return true;
}

bool TSC2046::ReadPositionRaw(int* px, int* py, int* pz1, int* pz2)
{
	gpio_CS_.put(0);
	SPISetFormat();
	*px = ReadADC12Bit(0b101);
	*py = ReadADC12Bit(0b001);
	int z1 = ReadADC8Bit(0b011);
	int z2 = ReadADC8Bit(0b100);
	gpio_CS_.put(1);
	if (pz1) *pz1 = z1;
	if (pz2) *pz2 = z2;
	return z1 > 0;
}

bool TSC2046::ReadPosition(int* px, int* py)
{
	int x, y;
	bool rtn = ReadPositionRaw(&x, &y);
	*px = adjusterX_.Adjust(x);
	*py = adjusterY_.Adjust(y);
	return rtn;
}

bool TSC2046::IsTouched()
{
	gpio_CS_.put(0);
	SPISetFormat();
	int z1 = ReadADC8Bit(0b011);
	gpio_CS_.put(1);
	return z1 > 0;
}

void TSC2046::SendCmd(uint8_t cmd)
{
	uint8_t buffSend[3] = { cmd, 0x00, 0x00 };
	uint8_t buffRecv[3];
	gpio_CS_.put(0);
	SPISetFormat();
	::spi_write_read_blocking(spi_, buffSend, buffRecv, sizeof(buffSend));
	gpio_CS_.put(1);
}

uint8_t TSC2046::ReadADC8Bit(uint8_t adc)
{
	uint8_t buffSend[2] = {
		ComposeCmd(
			adc,	// ADC .. 0b001:Y, 0b011:Z1, 0b100:Z2, 0b101:X
			0b1,	// Mode .. 8 bit
			0b1,	// Reference .. Single-end
			0b01), 	// Power Down Mode .. Reference Off, ADC On
		0x00, 		// dummy data to squeeze receiving data
	};
	uint8_t buffRecv[2];
	::spi_write_read_blocking(spi_, buffSend, buffRecv, sizeof(buffSend));
	return buffRecv[1];
}

uint16_t TSC2046::ReadADC12Bit(uint8_t adc)
{
	uint8_t buffSend[3] = {
		ComposeCmd(
			adc,	// ADC .. 0b001:Y, 0b011:Z1, 0b100:Z2, 0b101:X
			0b0,	// Mode .. 12 bit
			0b1,	// Reference .. Single-end
			0b01), 	// Power Down Mode .. Reference Off, ADC On
		0x00, 0x00,		// dummy data to squeeze receiving data
	};
	uint8_t buffRecv[3];
	::spi_write_read_blocking(spi_, buffSend, buffRecv, sizeof(buffSend));
	return (static_cast<uint16_t>(buffRecv[1]) << 4) | (buffRecv[2] >> 4);
}

//------------------------------------------------------------------------------
// TSC2046::Adjuster
//------------------------------------------------------------------------------
int TSC2046::Adjuster::Adjust(int value) const
{
	int valueAdj = static_cast<int>(slope_ * value + intercept_);
	return (valueAdj < 0)? 0 : (valueAdj > valueMax_)? valueMax_ : valueAdj;
}

}
