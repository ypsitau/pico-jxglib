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
	raw.InitGPIO();
	raw.SoftwareReset();
	::sleep_ms(150);
	raw.SleepOut();
	::sleep_ms(50);
	raw.InterfacePixelFormat(5, 5);
			// RGB interface color format     = 65K of RGB interface
			// Control interface color format = 16bit/pixel
	::sleep_ms(10);
	raw.MemoryDataAccessControl(0, 0, 0, 0, 0, 0);
			// Page Address Order            = Top to Bottom
			// Column Address Order          = Left to Right
			// Page/Column Order             = Normal Mode
			// Line Address Order            = LCD Refresh Top to Bottom
			// RGB/BGR Order                 = RGB
			// Display Data Latch Data Order = LCD Refresh Left to Right
	raw.ColumnAddressSet(0, GetWidth());
	raw.RowAddressSet(0, GetHeight());
	raw.DisplayInversionOn();
	::sleep_ms(10);
	raw.NormalDisplayModeOn();
	::sleep_ms(10);
	raw.DisplayOn();
	::sleep_ms(10);
	raw.SetGPIO_BL(true);
}

//------------------------------------------------------------------------------
// ST7789::Raw
//------------------------------------------------------------------------------
void ST7789::Raw::InitGPIO()
{
	::gpio_init(gpio_DC_);
	::gpio_init(gpio_RST_);
	::gpio_init(gpio_BL_);
	::gpio_set_dir(gpio_DC_, GPIO_OUT);
	::gpio_set_dir(gpio_RST_, GPIO_OUT);
	::gpio_set_dir(gpio_BL_, GPIO_OUT);
	::gpio_put(gpio_DC_, 1);
	::gpio_put(gpio_RST_, 1);
	::gpio_put(gpio_BL_, 0);
	if (UsesCS()) {
		::gpio_init(gpio_CS_);
		::gpio_set_dir(gpio_CS_, GPIO_OUT);
		::gpio_put(gpio_CS_, 1);
	}
	::sleep_ms(100);
}

void ST7789::Raw::SendCmd(uint8_t cmd, const uint8_t* data, int len)
{
	if (UsesCS()) {
		::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		::spi_set_format(spi_, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}
	::sleep_us(1);
	if (UsesCS()) ::gpio_put(gpio_CS_, 0);
	::gpio_put(gpio_DC_, 0);
	::sleep_us(1);
	::spi_write_blocking(spi_, &cmd, sizeof(cmd));
	if (len) {
		::sleep_us(1);
		::gpio_put(gpio_DC_, 1);
		::sleep_us(1);
		::spi_write_blocking(spi_, data, len);
	}
	::sleep_us(1);
	if (UsesCS()) ::gpio_put(gpio_CS_, 1);
	::gpio_put(gpio_DC_, 1);
	::sleep_us(1);
}

void ST7789::Raw::SendCmdBy16Bit(uint8_t cmd, const uint16_t* data, int len)
{
	if (UsesCS()) {
		::spi_set_format(spi_, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);	// Mode 0
	} else {
		::spi_set_format(spi_, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);	// Mode 3
	}
	::sleep_us(1);
	if (UsesCS()) ::gpio_put(gpio_CS_, 0);
	::gpio_put(gpio_DC_, 0);
	::sleep_us(1);
	::spi_write_blocking(spi_, &cmd, sizeof(cmd));
	if (len) {
		::sleep_us(1);
		::gpio_put(gpio_DC_, 1);
		::sleep_us(1);
		::spi_write16_blocking(spi_, data, len);
	}
	::sleep_us(1);
	if (UsesCS()) ::gpio_put(gpio_CS_, 1);
	::gpio_put(gpio_DC_, 1);
	::sleep_us(1);
}

}
