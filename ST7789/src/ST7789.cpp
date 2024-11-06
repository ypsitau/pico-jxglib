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
	// SWRESET (01h): Software Reset
	raw.SoftwareReset();
	::sleep_ms(150);
	// SLPOUT (11h): Sleep Out
	raw.SleepOut();
	::sleep_ms(50);
	// COLMOD (3Ah): Interface Pixel Format
	// - RGB interface color format     = 65K of RGB interface
	// - Control interface color format = 16bit/pixel
	raw.InterfacePixelFormat(5, 5);
	::sleep_ms(10);
	// MADCTL (36h): Memory Data Access Control
	// - Page Address Order            = Top to Bottom
	// - Column Address Order          = Left to Right
	// - Page/Column Order             = Normal Mode
	// - Line Address Order            = LCD Refresh Top to Bottom
	// - RGB/BGR Order                 = RGB
	// - Display Data Latch Data Order = LCD Refresh Left to Right
	//SendCmd(0x36, 0x00);
	raw.MemoryDataAccessControl(0, 0, 0, 0, 0, 0);
	raw.ColumnAddressSet(0, GetWidth());
	raw.RowAddressSet(0, GetHeight());
	// INVON (21h): Display Inversion On
	//SendCmd(0x21);
	raw.DisplayInversionOn();
	::sleep_ms(10);
	// NORON (13h): Normal Display Mode On
	//SendCmd(0x13);
	raw.NormalDisplayModeOn();
	::sleep_ms(10);
	// DISPON (29h): Display On
	//SendCmd(0x29);
	raw.DisplayOn();
	::sleep_ms(10);
	raw.SetGPIO_BL(true);
}

//------------------------------------------------------------------------------
// ST7789::Raw
//------------------------------------------------------------------------------
void ST7789::Raw::InitGPIO()
{
	if (UsesCS()) {
		::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		::spi_set_format(spi_, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}
	if (UsesCS()) ::gpio_init(gpio_CS_);
	::gpio_init(gpio_DC_);
	::gpio_init(gpio_RST_);
	::gpio_init(gpio_BL_);
	if (UsesCS()) ::gpio_set_dir(gpio_CS_, GPIO_OUT);
	::gpio_set_dir(gpio_DC_, GPIO_OUT);
	::gpio_set_dir(gpio_RST_, GPIO_OUT);
	::gpio_set_dir(gpio_BL_, GPIO_OUT);
	if (UsesCS()) ::gpio_put(gpio_CS_, 1);
	::gpio_put(gpio_DC_, 1);
	::gpio_put(gpio_RST_, 1);
	::sleep_ms(100);
}

void ST7789::Raw::Initialize()
{
	if (UsesCS()) {
		::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		::spi_set_format(spi_, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}
	if (UsesCS()) ::gpio_init(gpio_CS_);
	::gpio_init(gpio_DC_);
	::gpio_init(gpio_RST_);
	::gpio_init(gpio_BL_);
	if (UsesCS()) ::gpio_set_dir(gpio_CS_, GPIO_OUT);
	::gpio_set_dir(gpio_DC_, GPIO_OUT);
	::gpio_set_dir(gpio_RST_, GPIO_OUT);
	::gpio_set_dir(gpio_BL_, GPIO_OUT);
	if (UsesCS()) ::gpio_put(gpio_CS_, 1);
	::gpio_put(gpio_DC_, 1);
	::gpio_put(gpio_RST_, 1);
	::sleep_ms(100);
	// SWRESET (01h): Software Reset
	SendCmd(0x01);
	::sleep_ms(150);
	// SLPOUT (11h): Sleep Out
	SendCmd(0x11);
	::sleep_ms(50);
	// COLMOD (3Ah): Interface Pixel Format
	// - RGB interface color format     = 65K of RGB interface
	// - Control interface color format = 16bit/pixel
	SendCmd(0x3a, 0x55);
	::sleep_ms(10);
	// MADCTL (36h): Memory Data Access Control
	// - Page Address Order            = Top to Bottom
	// - Column Address Order          = Left to Right
	// - Page/Column Order             = Normal Mode
	// - Line Address Order            = LCD Refresh Top to Bottom
	// - RGB/BGR Order                 = RGB
	// - Display Data Latch Data Order = LCD Refresh Left to Right
	SendCmd(0x36, 0x00);
	ColumnAddressSet(0, width_);
	RowAddressSet(0, height_);
	// INVON (21h): Display Inversion On
	SendCmd(0x21);
	::sleep_ms(10);
	// NORON (13h): Normal Display Mode On
	SendCmd(0x13);
	::sleep_ms(10);
	// DISPON (29h): Display On
	SendCmd(0x29);
	::sleep_ms(10);
	::gpio_put(gpio_BL_, 1);
}

void ST7789::Raw::SendCmd(uint8_t cmd, const uint8_t* data, int len)
{
	if (UsesCS()) {
		::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		::spi_set_format(spi_, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}
	dataModeFlag_ = false;
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
		::spi_set_format(spi_, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	} else {
		::spi_set_format(spi_, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
	}
	dataModeFlag_ = false;
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

void ST7789::Raw::PutPixel(uint16_t pixel)
{
	if (!dataModeFlag_) {
		::sleep_us(1);
		if (UsesCS()) ::gpio_put(gpio_CS_, 0);
		::gpio_put(gpio_DC_, 0);
		::sleep_us(1);
		// RAMWR (2Ch): Memory Write
		uint8_t cmd = 0x2c;
		::spi_write_blocking(spi_, &cmd, sizeof(cmd));
		::sleep_us(1);
		if (UsesCS()) ::gpio_put(gpio_CS_, 0);
		::gpio_put(gpio_DC_, 1);
		::sleep_us(1);
		if (UsesCS()) {
			::spi_set_format(spi_, 16, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
		} else {
			::spi_set_format(spi_, 16, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
		}
		dataModeFlag_ = true;
	}
	spi_write16_blocking(spi_, &pixel, 1);
}

void ST7789::Raw::Fill(uint16_t pixel)
{
	int num_pixels = width_ * height_;
	ColumnAddressSet(0, width_);
	RowAddressSet(0, height_);
	for (int i = 0; i < num_pixels; i++) {
		PutPixel(pixel);
	}
}

}
