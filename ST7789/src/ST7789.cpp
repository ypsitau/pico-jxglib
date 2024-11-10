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

void ST7789::WriteBuffer(int x, int y, int width, int height, const uint16_t* buff)
{
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWrite16(buff, width * height);
}

void ST7789::Clear()
{
	raw.ColumnAddressSet(0, GetWidth() - 1);
	raw.RowAddressSet(0, GetHeight() - 1);
	raw.MemoryWriteConst16(colorBg_.RGB565(), GetWidth() * GetHeight());
}

void ST7789::Fill()
{
	raw.ColumnAddressSet(0, GetWidth() - 1);
	raw.RowAddressSet(0, GetHeight() - 1);
	raw.MemoryWriteConst16(colorFg_.RGB565(), GetWidth() * GetHeight());
}

void ST7789::DrawHLine(int x, int y, int width)
{
	if (!AdjustRange(&x, &width, 0, GetWidth())) return;
	if (!CheckRange(y, 0, GetHeight())) return;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y);
	raw.MemoryWriteConst16(colorFg_.RGB565(), width);
}

void ST7789::DrawVLine(int x, int y, int height)
{
	if (!CheckRange(x, 0, GetWidth())) return;
	if (!AdjustRange(&y, &height, 0, GetHeight())) return;
	raw.ColumnAddressSet(x, x);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWriteConst16(colorFg_.RGB565(), height);
}

void ST7789::DrawRectFill(int x, int y, int width, int height)
{
	if (!AdjustRange(&x, &width, 0, GetWidth())) return;
	if (!AdjustRange(&y, &height, 0, GetHeight())) return;
	raw.ColumnAddressSet(x, x + width);
	raw.RowAddressSet(y, y + height);
	raw.MemoryWriteConst16(colorFg_.RGB565(), width * height);
}

void ST7789::DrawBitmap(int x, int y, const void* data, int width, int height, bool transparentBgFlag, int scaleX, int scaleY)
{
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	raw.ColumnAddressSet(x, x + width * scaleX - 1);
	raw.RowAddressSet(y, y + height * scaleY - 1);
	raw.MemoryWrite_Begin(16);
	uint16_t colorFg = colorFg_.RGB565();
	uint16_t colorBg = colorBg_.RGB565();
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

void ST7789::DrawImage(int x, int y, const Image& image)
{
	int xSkip = 0, ySkip = 0;
	int width = image.GetWidth(), height = image.GetHeight();
	if (!AdjustRange(&x, &width, 0, GetWidth(), &xSkip)) return;
	if (!AdjustRange(&y, &height, 0, GetHeight(), &ySkip)) return;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWrite_Begin(16);
	const uint8_t* pSrc = image.GetPointer(xSkip, ySkip);
	if (image.IsFormatRGB565()) {
		Scanner scanner(Scanner<GetterRGB565_SrcRGB565>::HorzNW(pSrc, width, height,
									image.GetBytesPerPixel(), image.GetBytesPerLine()));
		while (!scanner.HasDone()) raw.MemoryWrite_Data16(scanner.ScanForward());
	}
	raw.MemoryWrite_End();
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
	SetSPIDataBits(8);
}

void ST7789::Raw::WriteCmd(uint8_t cmd)
{
	::gpio_put(gpio_DC_, 0);
	::sleep_us(1);
	SetSPIDataBits(8);
	::spi_write_blocking(spi_, &cmd, sizeof(cmd));
	::gpio_put(gpio_DC_, 1);
}

void ST7789::Raw::SendCmd(uint8_t cmd)
{
	EnableCS();
	WriteCmd(cmd);
	DisableCS();
}

void ST7789::Raw::SendCmdAndData8(uint8_t cmd, const uint8_t* data, int len)
{
	EnableCS();
	WriteCmd(cmd);
	::sleep_us(1);
	::spi_write_blocking(spi_, data, len);
	DisableCS();
}

void ST7789::Raw::SendCmdAndData16(uint8_t cmd, const uint16_t* data, int len)
{
	EnableCS();
	WriteCmd(cmd);
	::sleep_us(1);
	SetSPIDataBits(16);
	::spi_write16_blocking(spi_, data, len);
	DisableCS();
}

}
