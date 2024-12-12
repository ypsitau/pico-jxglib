//==============================================================================
// jxglib/ILI9341.h
//==============================================================================
#ifndef PICO_JXGLIB_ILI9341_H
#define PICO_JXGLIB_ILI9341_H
#include "jxglib/TFT_LCD.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ILI9341
//------------------------------------------------------------------------------
class ILI9341 : public TFT_LCD {
public:
	ILI9341(spi_inst_t* spi, int width, int height, const GPIO& gpio_DC, const GPIO& gpio_RST, const GPIO& gpio_CS, const GPIO& gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	inline void Initialize(DisplayDir displayDir = DisplayDir::Normal);
public:
	using TypeA = ILI9341;
};

inline void ILI9341::Initialize(DisplayDir displayDir)
{
	//const RotateData& rotateData = rotateDataTbl_Case2[static_cast<int>(displayDir)];
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::BGR,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(displayDir, configData);
}

}

#endif
