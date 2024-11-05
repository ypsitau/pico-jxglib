//==============================================================================
// ST7789.cpp
//==============================================================================
#include "jxglib/ST7789.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ST7789
//------------------------------------------------------------------------------
void ST7789::Initialize()
{

}

//------------------------------------------------------------------------------
// ST7789::Raw
//------------------------------------------------------------------------------
void ST7789::Raw::SendCmd(uint8_t cmd, const void* data, int len)
{
	if (UsesCS()) {
		spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		spi_set_format(spi_, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}
	st7789_data_mode = false;

	sleep_us(1);
	if (UsesCS()) {
		gpio_put(gpio_CS_, 0);
	}
	gpio_put(gpio_DC_, 0);
	sleep_us(1);
	
	spi_write_blocking(spi_, &cmd, sizeof(cmd));
	
	if (len) {
		sleep_us(1);
		gpio_put(gpio_DC_, 1);
		sleep_us(1);
		
		spi_write_blocking(spi_, reinterpret_cast<const uint8_t*>(data), len);
	}

	sleep_us(1);
	if (UsesCS()) {
		gpio_put(gpio_CS_, 1);
	}
	gpio_put(gpio_DC_, 1);
	sleep_us(1);
}

}
