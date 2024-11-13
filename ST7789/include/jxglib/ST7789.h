//==============================================================================
// jxglib/ST7789.h
//==============================================================================
#ifndef PICO_JXGLIB_ST7789_H
#define PICO_JXGLIB_ST7789_H
#include "jxglib/ST7735_ST7789.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ST7789
//------------------------------------------------------------------------------
class ST7789 : public ST7735_ST7789 {
public:
	ST7789(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_CS, GPIO gpio_BL) :
			ST7735_ST7789(spi, width, height, gpio_RST, gpio_DC, gpio_CS, gpio_BL, 0, true) {}
	ST7789(spi_inst_t* spi, int width, int height, GPIO gpio_RST, GPIO gpio_DC, GPIO gpio_BL) :
			ST7735_ST7789(spi, width, height, gpio_RST, gpio_DC, gpio_BL, 0, true) {}
};

}

#endif
