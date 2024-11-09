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
	raw.ColumnAddressSet(0, GetScreenWidth());
	raw.RowAddressSet(0, GetScreenHeight());
	raw.DisplayInversionOn();
	::sleep_ms(10);
	raw.NormalDisplayModeOn();
	::sleep_ms(10);
	raw.DisplayOn();
	::sleep_ms(10);
	raw.SetGPIO_BL(true);
}

void ST7789::DrawRGB565(int x, int y, const void* data, int width, int height, int scaleX, int scaleY)
{
	int nDots = width * height;
	const uint16_t* pSrcLeft = reinterpret_cast<const uint16_t*>(data);
	raw.ColumnAddressSet(x, x + width * scaleX - 1);
	raw.RowAddressSet(y, y + height * scaleY - 1);
	raw.MemoryWrite_Begin(16);
	for (int iRow = 0; iRow < height; iRow++) {
		const uint16_t* pSrc;
		for (int iScaleY = 0; iScaleY < scaleY; iScaleY++) {
			pSrc = pSrcLeft;
			for (int iCol = 0; iCol < width; iCol++) {
				uint16_t color = *pSrc++;
				for (int iScaleX = 0; iScaleX < scaleX; iScaleX++) raw.MemoryWrite_Data16(color);
			}
		}
		pSrcLeft = pSrc;
	}
	raw.MemoryWrite_End();
}

void ST7789::WriteBuffer(int x, int y, int width, int height, const uint16_t* buff)
{
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWrite16(buff, width * height);
}

void ST7789::Clear()
{
	raw.ColumnAddressSet(0, GetScreenWidth() - 1);
	raw.RowAddressSet(0, GetScreenHeight() - 1);
	raw.MemoryWriteConst16(colorBg_.RGB565(), GetScreenWidth() * GetScreenHeight());
}

void ST7789::Fill()
{
	raw.ColumnAddressSet(0, GetScreenWidth() - 1);
	raw.RowAddressSet(0, GetScreenHeight() - 1);
	raw.MemoryWriteConst16(colorFg_.RGB565(), GetScreenWidth() * GetScreenHeight());
}

void ST7789::DrawHLine(int x, int y, int width)
{
	if (!AdjustRange(&x, &width, 0, GetScreenWidth())) return;
	if (!CheckRange(y, 0, GetScreenHeight())) return;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y);
	raw.MemoryWriteConst16(colorFg_.RGB565(), width);
}

void ST7789::DrawVLine(int x, int y, int height)
{
	if (!CheckRange(x, 0, GetScreenWidth())) return;
	if (!AdjustRange(&y, &height, 0, GetScreenHeight())) return;
	raw.ColumnAddressSet(x, x);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWriteConst16(colorFg_.RGB565(), height);
}

#if 0
void ST7789::DrawRect(int x, int y, int width, int height)
{
	if (width < 0) {
		x += width + 1;
		width = -width;
	}
	if (height < 0) {
		y += height + 1;
		height = -height;
	}
	DrawRectFill(x, y, width, 1);
	DrawRectFill(x, y + height - 1, width, 1);
	DrawRectFill(x, y, 1, height);
	DrawRectFill(x + width - 1, y, 1, height);
}
#endif

void ST7789::DrawRectFill(int x, int y, int width, int height)
{
	if (!AdjustRange(&x, &width, 0, GetScreenWidth())) return;
	if (!AdjustRange(&y, &height, 0, GetScreenHeight())) return;
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

#if 0
void ST7789::DrawChar(int x, int y, const FontEntry& fontEntry)
{
	DrawBitmap(x, y, fontEntry.data, fontEntry.width, fontEntry.height, context_.fontScaleX, true, context_.fontScaleY);
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

const char* ST7789::DrawStringWrap(int x, int y, int width, int height, const char* str, int htLine)
{
	if (!context_.pFontSet) return str;
	uint32_t code;
	UTF8Decoder decoder;
	int xStart = x;
	int xExceed = (width >= 0)? x + width : GetScreenWidth();
	int yExceed = (height >= 0)? y + height : GetScreenHeight();
	int yAdvance = (htLine >= 0)? htLine : context_.pFontSet->yAdvance * context_.fontScaleY;
	const char* pDone = str;
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		int xAdvance = fontEntry.xAdvance * context_.fontScaleX;
		if (x + fontEntry.width * context_.fontScaleX > xExceed) {
			x = xStart, y += yAdvance;
			if (y + yAdvance > yExceed) break;
		}
		DrawChar(x, y, fontEntry);
		x += xAdvance;
		pDone = p + 1;
	}
	return pDone;
}
#endif

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
