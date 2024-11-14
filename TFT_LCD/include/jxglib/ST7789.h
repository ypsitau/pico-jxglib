//==============================================================================
// jxglib/ST7789.h
//==============================================================================
#ifndef PICO_JXGLIB_ST7789_H
#define PICO_JXGLIB_ST7789_H
#include "jxglib/TFT_LCD.h"

namespace jxglib { namespace ST7789 {

//------------------------------------------------------------------------------
// ST7789::TypeA
//------------------------------------------------------------------------------
class TypeA : public TFT_LCD {
public:
	TypeA(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_CS, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	TypeA(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) {}
public:
	void Initialize() {
		static const ConfigData cfg = {
			.pageAddressOrder		= PageAddressOrder::TopToBottom,
			.columnAddressOrder		= ColumnAddressOrder::LeftToRight,
			.pageColumnOrder		= PageColumnOrder::NormalMode,
			.lineAddressOrder		= LineAddressOrder::TopToBottom,
			.rgbBgrOrder			= RGBBGROrder::RGB,
			.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
			.displayInversionOnFlag	= true,
			.gammaCurve				= 0x01,
		};
		TFT_LCD::Initialize(cfg);
	}
};

} }

#endif
