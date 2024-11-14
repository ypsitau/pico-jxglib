//==============================================================================
// jxglib/ILI9341.h
//==============================================================================
#ifndef PICO_JXGLIB_ILI9341_H
#define PICO_JXGLIB_ILI9341_H
#include "jxglib/TFT_LCD.h"

namespace jxglib { namespace ILI9341 {

//------------------------------------------------------------------------------
// ILI9341::TypeA
//------------------------------------------------------------------------------
class TypeA : public TFT_LCD {
public:
	TypeA(spi_inst_t* spi, int width, int height, GPIO gpio_DC, GPIO gpio_RST, GPIO gpio_CS, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
public:
	void Initialize() {
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
};

} }

#endif
