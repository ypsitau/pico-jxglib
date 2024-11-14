//==============================================================================
// ST7735_ST7789.cpp
//==============================================================================
#include "jxglib/ST7735_ST7789.h"

namespace jxglib {

//------------------------------------------------------------------------------
// ST7735_ST7789
//------------------------------------------------------------------------------
void ST7735_ST7789::Initialize()
{
	raw.InitGPIO();
	raw.SoftwareReset();
	::sleep_ms(150);
	raw.SleepOut();
	::sleep_ms(50);
	raw.InterfacePixelFormat(5, 5);
			// RGB interface color format     = 65K of RGB interface. No effect on ST7735.
			// Control interface color format = 16bit/pixel
	::sleep_ms(10);
	raw.MemoryDataAccessControl(configData.pageAddressOrder, configData.columnAddressOrder,
			configData.pageColumnOrder, configData.lineAddressOrder,
			configData.rgbBgrOrder, configData.displayDataLatchOrder);
	if (configData.displayInversionOnFlag) raw.DisplayInversionOn();
	::sleep_ms(10);
	raw.NormalDisplayModeOn();
	::sleep_ms(10);
	raw.DisplayOn();
	Clear();
	::sleep_ms(10);
	raw.SetGPIO_BL(true);
}

void ST7735_ST7789::Clear()
{
	raw.ColumnAddressSet(0, GetWidth() - 1);
	raw.RowAddressSet(0, GetHeight() - 1);
	raw.MemoryWriteConst16(context_.colorBg.RGB565(), GetWidth() * GetHeight());
}

void ST7735_ST7789::Fill()
{
	raw.ColumnAddressSet(0, GetWidth() - 1);
	raw.RowAddressSet(0, GetHeight() - 1);
	raw.MemoryWriteConst16(context_.colorFg.RGB565(), GetWidth() * GetHeight());
}

void ST7735_ST7789::DrawPixel_(int x, int y, const Color& color)
{
	raw.ColumnAddressSet(x, x);
	raw.RowAddressSet(y, y);
	raw.MemoryWriteConst16(color.RGB565(), 1);
}

void ST7735_ST7789::DrawRectFill_(int x, int y, int width, int height, const Color& color)
{
	if (!AdjustRange(&x, &width, 0, GetWidth())) return;
	if (!AdjustRange(&y, &height, 0, GetHeight())) return;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWriteConst16(color.RGB565(), width * height);
}

void ST7735_ST7789::DrawBitmap_(int x, int y, const void* data, int width, int height,
					const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	raw.ColumnAddressSet(x, x + width * scaleX - 1);
	raw.RowAddressSet(y, y + height * scaleY - 1);
	raw.MemoryWrite_Begin(16);
	uint16_t colorFg = color.RGB565();
	uint16_t colorBg = pColorBg? pColorBg->RGB565() : 0;
	for (int iRow = 0; iRow < height; iRow++) {
		const uint8_t* pSrc;
		for (int iScaleY = 0; iScaleY < scaleY; iScaleY++) {
			pSrc = pSrcLeft;
			int iBit = 0;
			uint8_t bits = *pSrc++;
			for (int iCol = 0; iCol < width; iCol++, iBit++, bits <<= 1) {
				if (iBit == 8) {
					iBit = 0;
					bits = *pSrc++;
				}
				uint16_t color = (bits & 0x80)? colorFg : colorBg;
				for (int iScaleX = 0; iScaleX < scaleX; iScaleX++) raw.MemoryWrite_Data16(color);
			}
		}
		pSrcLeft = pSrc;
	}
	raw.MemoryWrite_End();
}

void ST7735_ST7789::DrawImage_(int x, int y, const Image& image)
{
	int xSkip = 0, ySkip = 0;
	int width = image.GetWidth(), height = image.GetHeight();
	if (!AdjustRange(&x, &width, 0, GetWidth(), &xSkip)) return;
	if (!AdjustRange(&y, &height, 0, GetHeight(), &ySkip)) return;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWrite_Begin(16);
	if (image.IsFormatRGB565()) {
		using Reader = Image::Reader<Image::ReadRGB565_SrcRGB565>;
		Image::Reader reader(Reader::HorzFromNW(image, xSkip, ySkip, width, height));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	}
	raw.MemoryWrite_End();
}

//------------------------------------------------------------------------------
// ST7735_ST7789::Raw
//------------------------------------------------------------------------------
void ST7735_ST7789::Raw::InitGPIO()
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
	SetSPIDataBits(8);
}

void ST7735_ST7789::Raw::WriteCmd(uint8_t cmd)
{
	::gpio_put(gpio_DC_, 0);
	::sleep_us(1);
	SetSPIDataBits(8);
	::spi_write_blocking(spi_, &cmd, sizeof(cmd));
	::gpio_put(gpio_DC_, 1);
}

void ST7735_ST7789::Raw::SendCmd(uint8_t cmd)
{
	EnableCS();
	WriteCmd(cmd);
	DisableCS();
}

void ST7735_ST7789::Raw::SendCmdAndData8(uint8_t cmd, const uint8_t* data, int len)
{
	EnableCS();
	WriteCmd(cmd);
	::sleep_us(1);
	::spi_write_blocking(spi_, data, len);
	DisableCS();
}

void ST7735_ST7789::Raw::SendCmdAndData16(uint8_t cmd, const uint16_t* data, int len)
{
	EnableCS();
	WriteCmd(cmd);
	::sleep_us(1);
	SetSPIDataBits(16);
	::spi_write16_blocking(spi_, data, len);
	DisableCS();
}

}
