//==============================================================================
// jxglib/ST7735.h
//==============================================================================
#ifndef PICO_JXGLIB_ST7735_H
#define PICO_JXGLIB_ST7735_H
#include "jxglib/TFT_LCD.h"

namespace jxglib { namespace ST7735 {

//------------------------------------------------------------------------------
// ST7735::TypeA
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
			.rgbBgrOrder			= RGBBGROrder::BGR,
			.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
			.displayInversionOnFlag	= false,
			.gammaCurve				= 0x01,
		};
		TFT_LCD::Initialize(cfg);
	}
};

//------------------------------------------------------------------------------
// ST7735::TypeB
//------------------------------------------------------------------------------
class TypeB : public TFT_LCD {
public:
	TypeB(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_CS, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	TypeB(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) {}
public:
	void Initialize() {
		static const ConfigData cfg = {
			.pageAddressOrder		= PageAddressOrder::BottomToTop,
			.columnAddressOrder		= ColumnAddressOrder::RightToLeft,
			.pageColumnOrder		= PageColumnOrder::NormalMode,
			.lineAddressOrder		= LineAddressOrder::TopToBottom,
			.rgbBgrOrder			= RGBBGROrder::RGB,
			.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight,
			.displayInversionOnFlag	= false,
			.gammaCurve				= 0x02,
		};
		TFT_LCD::Initialize(cfg);
	}
};

} }

#endif
