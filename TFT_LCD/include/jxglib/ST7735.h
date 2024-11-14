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
	ST7735(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_CS, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
	ST7735(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) {}
	inline void Initialize();
public:
	using TypeA = ST7735;
	class TypeB : public TFT_LCD {
	public:
		TypeB(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_CS, GPIO gpio_BL) :
				TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) {}
		TypeB(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
				TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) {}
		inline void Initialize();
	};
};

inline void ST7735::Initialize()
{
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

inline void ST7735::TypeB::Initialize()
{
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

}

#endif
