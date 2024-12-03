//==============================================================================
// jxglib/ST7735.h
//==============================================================================
#ifndef PICO_JXGLIB_ST7735_H
#define PICO_JXGLIB_ST7735_H
#include "jxglib/TFT_LCD.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ST7735
//------------------------------------------------------------------------------
class ST7735 : public TFT_LCD {
public:
	ST7735(spi_inst_t* spi, int width, int height, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_CS, const GPIO& gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	ST7735(spi_inst_t* spi, int width, int height, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) {}
	inline void Initialize(DisplayDir displayDir = DisplayDir::Normal);
public:
	using TypeA = ST7735;
	class TypeB : public TFT_LCD {
	public:
		TypeB(spi_inst_t* spi, int width, int height, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_CS, const GPIO& gpio_BL) :
				TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
		TypeB(spi_inst_t* spi, int width, int height, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_BL) :
				TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) {}
		inline void Initialize(DisplayDir displayDir = DisplayDir::Normal);
	};
};

inline void ST7735::Initialize(DisplayDir displayDir)
{
	const RotateData& rotateData = rotateDataTbl_Case1[static_cast<int>(displayDir)];
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::BGR,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(rotateData, configData);
}

inline void ST7735::TypeB::Initialize(DisplayDir displayDir)
{
	const RotateData& rotateData = rotateDataTbl_Case3[static_cast<int>(displayDir)];
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::RGB,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x02,
	};
	TFT_LCD::Initialize(rotateData, configData);
}

}

#endif
