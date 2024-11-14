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
			TFT_LCD(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL) { SetConfigData(); }
private:
	void SetConfigData() {
		configData.pageAddressOrder			= PageAddressOrder::BottomToTop;
		configData.columnAddressOrder		= ColumnAddressOrder::LeftToRight;
		configData.pageColumnOrder			= PageColumnOrder::NormalMode;
		configData.lineAddressOrder			= LineAddressOrder::TopToBottom;
		configData.rgbBgrOrder				= RGBBGROrder::BGR;
		configData.displayDataLatchOrder	= DisplayDataLatchOrder::LeftToRight;
		configData.displayInversionOnFlag	= false;
		configData.gammaCurve				= 0x01;
	}
};

} }

#endif
