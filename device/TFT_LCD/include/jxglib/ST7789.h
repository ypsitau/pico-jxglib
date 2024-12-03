//==============================================================================
// jxglib/ST7789.h
//==============================================================================
#ifndef PICO_JXGLIB_ST7789_H
#define PICO_JXGLIB_ST7789_H
#include "jxglib/TFT_LCD.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ST7789
//------------------------------------------------------------------------------
class ST7789 : public TFT_LCD {
public:
	ST7789(spi_inst_t* spi, int width, int height, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_CS, const GPIO& gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	ST7789(spi_inst_t* spi, int width, int height, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) {}
	inline void Initialize(DisplayDir displayDir = DisplayDir::Normal);
public:
	using TypeA = ST7789;
};

inline void ST7789::Initialize(DisplayDir displayDir)
{
	const RotateData& rotateData = rotateDataTbl_Case1[static_cast<int>(displayDir)];
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::RGB,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.displayInversionOnFlag	= true,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(rotateData, configData);
}

}

#endif
