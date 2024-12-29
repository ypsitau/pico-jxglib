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
	ST7789(spi_inst_t* spi, int width, int height, const PinAssign& pinAssign) :
			TFT_LCD(spi, 240, 320, width, height, pinAssign) {}
	ST7789(spi_inst_t* spi, int width, int height, const PinAssignNoCS& pinAssign) :
			TFT_LCD(spi, 240, 320, width, height, pinAssign) {}
	inline void Initialize(Dir displayDir = Dir::Normal);
public:
	using TypeA = ST7789;
};

inline void ST7789::Initialize(Dir displayDir)
{
	static const ConfigData configData = {
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::RGB,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.invertHorzFlag			= false,
		.invertVertFlag			= false,
		.displayInversionOnFlag	= true,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(displayDir, configData);
}

}

#endif
