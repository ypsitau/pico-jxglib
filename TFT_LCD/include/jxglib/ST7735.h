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
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) { SetConfigData(); }
	TypeA(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) { SetConfigData(); }
private:
	void SetConfigData() {
		configData.pageAddressOrder			= PageAddressOrder::TopToBottom;
		configData.columnAddressOrder		= ColumnAddressOrder::LeftToRight;
		configData.pageColumnOrder			= PageColumnOrder::NormalMode;
		configData.lineAddressOrder			= LineAddressOrder::TopToBottom;
		configData.rgbBgrOrder				= RGBBGROrder::BGR;
		configData.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight;
		configData.displayInversionOnFlag	= false;
		configData.gammaCurve				= 0x01;
	}
};

//------------------------------------------------------------------------------
// ST7735::TypeB
//------------------------------------------------------------------------------
class TypeB : public TFT_LCD {
public:
	TypeB(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_CS, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) { SetConfigData(); }
	TypeB(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_BL) { SetConfigData(); }
private:
	void SetConfigData() {
		configData.pageAddressOrder			= PageAddressOrder::BottomToTop;
		configData.columnAddressOrder		= ColumnAddressOrder::RightToLeft;
		configData.pageColumnOrder			= PageColumnOrder::NormalMode;
		configData.lineAddressOrder			= LineAddressOrder::TopToBottom;
		configData.rgbBgrOrder				= RGBBGROrder::RGB;
		configData.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight;
		configData.displayInversionOnFlag	= false;
		configData.gammaCurve				= 0x02;
	}
};

} }

#endif
