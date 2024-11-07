//==============================================================================
// ST7789.cpp
//==============================================================================
#include "jxglib/ST7789.h"

namespace jxglib {

//int spi_write16_blocking_const(spi_inst_t *spi, uint16_t data, size_t len);

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

void ST7789::Transfer(int x, int y, int width, int height, const uint16_t* buff)
{
	raw.ColumnAddressSet(x, x + width);
	raw.RowAddressSet(y, y + height);
	raw.MemoryWrite16(buff, width * height);
}

void ST7789::Fill()
{
	raw.ColumnAddressSet(0, GetWidth());
	raw.RowAddressSet(0, GetHeight());
	raw.MemoryWriteConst16(context_.colorFg, GetWidth() * GetHeight());
}

void ST7789::DrawHLine(int x, int y, int width)
{
	raw.ColumnAddressSet(x, x + width);
	raw.RowAddressSet(y, y + 1);
	raw.MemoryWriteConst16(context_.colorFg, width);
}

void ST7789::DrawVLine(int x, int y, int height)
{
	raw.ColumnAddressSet(x, x + 1);
	raw.RowAddressSet(y, y + height);
	raw.MemoryWriteConst16(context_.colorFg, height);
}

void ST7789::DrawRectFill(int x, int y, int width, int height)
{
	raw.ColumnAddressSet(x, x + width);
	raw.RowAddressSet(y, y + height);
	raw.MemoryWriteConst16(context_.colorFg, width * height);
}

void ST7789::DrawChar(int x, int y, const FontEntry& fontEntry)
{
	int nDots = fontEntry.width * fontEntry.height;
	int bytes = (fontEntry.width + 7) / 8 * fontEntry.height;
	const uint8_t* pSrcLeft = fontEntry.data;
	raw.ColumnAddressSet(x, x + fontEntry.width * context_.fontScaleX - 1);
	raw.RowAddressSet(y, y + fontEntry.height * context_.fontScaleY - 1);
	raw.MemoryWrite_Begin(16);
	for (int iRow = 0; iRow < fontEntry.height; iRow++) {
		const uint8_t* pSrc;
		for (int scaleY = 0; scaleY < context_.fontScaleY; scaleY++) {
			pSrc = pSrcLeft;
			int iBit = 0;
			uint8_t bits = *pSrc++;
			for (int iCol = 0; iCol < fontEntry.width; iCol++, iBit++, bits <<= 1) {
				if (iBit == 8) {
					iBit = 0;
					bits = *pSrc++;
				}
				uint16_t color = (bits & 0x80)? context_.colorFg : context_.colorBg;
				for (int scaleX = 0; scaleX < context_.fontScaleX; scaleX++) raw.MemoryWrite_Data16(color);
			}
		}
		pSrcLeft = pSrc;
	}
	raw.MemoryWrite_End();
}

void ST7789::DrawChar(int x, int y, uint32_t code)
{
	if (!context_.pFontSet) return;
	const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
	DrawChar(x, y, fontEntry);
}

void ST7789::DrawString(int x, int y, const char* str, const char* strEnd)
{
	if (!context_.pFontSet) return;
	uint32_t code;
	UTF8Decoder decoder;
	for (const char* p = str; *p && p != strEnd; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		DrawChar(x, y, fontEntry);
		x += fontEntry.xAdvance * context_.fontScaleX;
	}
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
