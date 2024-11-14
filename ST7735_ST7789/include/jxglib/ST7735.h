//==============================================================================
// jxglib/ST7735.h
//==============================================================================
#ifndef PICO_JXGLIB_ST7735_H
#define PICO_JXGLIB_ST7735_H
#include "jxglib/ST7735_ST7789.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ST7735
//------------------------------------------------------------------------------
class ST7735 : public ST7735_ST7789 {
public:
	ST7735(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_CS, GPIO gpio_BL) :
			ST7735_ST7789(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) { SetConfigData(); }
	ST7735(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
			ST7735_ST7789(spi, width, height, gpio_RST, gpio_DC, gpio_BL) { SetConfigData(); }
private:
	void SetConfigData() {
		configData.pageAddressOrder			= PageAddressOrder::TopToBottom;
		configData.columnAddressOrder		= ColumnAddressOrder::LeftToRight;
		configData.pageColumnOrder			= PageColumnOrder::NormalMode;
		configData.lineAddressOrder			= LineAddressOrder::TopToBottom;
		configData.rgbBgrOrder				= RGBBGROrder::BGR;
		configData.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight;
		configData.displayInversionOnFlag	= false;
	}
};

}

#endif
