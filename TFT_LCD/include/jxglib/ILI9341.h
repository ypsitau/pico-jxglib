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
	ILI9341(spi_inst_t* spi, int width, int height, GPIO gpio_DC, GPIO gpio_RST, GPIO gpio_CS, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	inline void Initialize();
public:
	using TypeA = ILI9341;
};

inline void ILI9341::Initialize()
{
	static const ConfigData cfg = {
		.pageAddressOrder		= PageAddressOrder::BottomToTop,
		.columnAddressOrder		= ColumnAddressOrder::LeftToRight,
		.pageColumnOrder		= PageColumnOrder::NormalMode,
		.lineAddressOrder		= LineAddressOrder::TopToBottom,
		.rgbBgrOrder			= RGBBGROrder::BGR,
		.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
		.displayInversionOnFlag	= false,
		.gammaCurve				= 0x01,
	};
	TFT_LCD::Initialize(cfg);
}

}

#endif
