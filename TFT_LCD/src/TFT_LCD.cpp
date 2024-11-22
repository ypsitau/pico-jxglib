//==============================================================================
// TFT_LCD.cpp
//==============================================================================
#include "jxglib/TFT_LCD.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TFT_LCD
//------------------------------------------------------------------------------
void TFT_LCD::Initialize(const ConfigData& cfg)
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
	raw.MemoryDataAccessControl(cfg.pageAddressOrder, cfg.columnAddressOrder,
			cfg.pageColumnOrder, cfg.lineAddressOrder,
			cfg.rgbBgrOrder, cfg.displayDataLatchOrder);
	if (cfg.displayInversionOnFlag) raw.DisplayInversionOn();
	::sleep_ms(10);
	raw.NormalDisplayModeOn();
	::sleep_ms(10);
	raw.GammaSet(cfg.gammaCurve);
	::sleep_ms(10);
	raw.DisplayOn();
	Clear();
	::sleep_ms(10);
	raw.SetGPIO_BL(true);
}

//------------------------------------------------------------------------------
// TFT_LCD::DispatcherEx
//------------------------------------------------------------------------------
void TFT_LCD::DispatcherEx::Initialize()
{
	// do nothing
}

void TFT_LCD::DispatcherEx::Refresh()
{
	// do nothing
}

void TFT_LCD::DispatcherEx::Fill(const Color& color)
{
	Raw& raw = display_.raw;
	raw.ColumnAddressSet(0, display_.GetWidth() - 1);
	raw.RowAddressSet(0, display_.GetHeight() - 1);
	raw.MemoryWriteConst16(ColorRGB565(color).value, display_.GetWidth() * display_.GetHeight());
}

void TFT_LCD::DispatcherEx::DrawPixel(int x, int y, const Color& color)
{
	Raw& raw = display_.raw;
	raw.ColumnAddressSet(x, x);
	raw.RowAddressSet(y, y);
	raw.MemoryWriteConst16(ColorRGB565(color).value, 1);
}

void TFT_LCD::DispatcherEx::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
	Raw& raw = display_.raw;
	if (!AdjustRange(&x, &width, 0, display_.GetWidth())) return;
	if (!AdjustRange(&y, &height, 0, display_.GetHeight())) return;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWriteConst16(ColorRGB565(color).value, width * height);
}

void TFT_LCD::DispatcherEx::DrawBitmap(int x, int y, const void* data, int width, int height,
					const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
	Raw& raw = display_.raw;
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	raw.ColumnAddressSet(x, x + width * scaleX - 1);
	raw.RowAddressSet(y, y + height * scaleY - 1);
	raw.MemoryWrite_Begin(16);
	uint16_t colorFg = ColorRGB565(color).value;
	uint16_t colorBg = pColorBg? ColorRGB565(*pColorBg).value : 0;
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

void TFT_LCD::DispatcherEx::DrawImage(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir)
{
	Raw& raw = display_.raw;
	Rect rect = pRectClip? *pRectClip : Rect(0, 0, display_.GetWidth(), display_.GetHeight());
	int xSkip = 0, ySkip = 0;
#if 0
	int wdSrc = image.GetWidth();
	int htSrc = image.GetHeight();
	int wdDst, htDst;
	if (Image::IsDirHorz(imageDir)) {
		wdDst = wdSrc, htDst = htSrc;
	} else {
		wdDst = htSrc, htDst = wdSrc;
	}
	if (!AdjustRange(&x, &wdDst, rect.x, rect.width, &xSkip)) return;
	if (!AdjustRange(&y, &htDst, rect.y, rect.height, &ySkip)) return;
	raw.ColumnAddressSet(x, x + wdDst - 1);
	raw.RowAddressSet(y, y + htDst - 1);
#else
	int wdSrc = image.GetWidth(), htSrc = image.GetHeight();
	int* pwdDst;
	int* phtDst;
	if (Image::IsDirHorz(imageDir)) {
		pwdDst = &wdSrc, phtDst = &htSrc;
	} else {
		pwdDst = &htSrc, phtDst = &wdSrc;
	}
	if (!AdjustRange(&x, pwdDst, rect.x, rect.width, &xSkip)) return;
	if (!AdjustRange(&y, phtDst, rect.y, rect.height, &ySkip)) return;
	raw.ColumnAddressSet(x, x + *pwdDst - 1);
	raw.RowAddressSet(y, y + *phtDst - 1);
#endif
	raw.MemoryWrite_Begin(16);
	if (image.GetFormat().IsGray()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcGray>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, wdSrc, htSrc, imageDir));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGB>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, wdSrc, htSrc, imageDir));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGBA>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, wdSrc, htSrc, imageDir));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::GetColorRGB565_SrcRGB565>;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, wdSrc, htSrc, imageDir));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	}
	raw.MemoryWrite_End();
}

void TFT_LCD::DispatcherEx::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect* pRect)
{
	// do nothing
}

void TFT_LCD::DispatcherEx::ScrollVert(DirVert dirVert, int htScroll, const Rect* pRect)
{
	// do nothing
}

//------------------------------------------------------------------------------
// TFT_LCD::Raw
//------------------------------------------------------------------------------
void TFT_LCD::Raw::InitGPIO()
{
	::gpio_init(gpio_DC_);
	::gpio_init(gpio_RST_);
	::gpio_set_dir(gpio_DC_, GPIO_OUT);
	::gpio_set_dir(gpio_RST_, GPIO_OUT);
	::gpio_put(gpio_DC_, 1);
	::gpio_put(gpio_RST_, 1);
	if (gpio_BL_.IsValid()) {
		::gpio_init(gpio_BL_);
		::gpio_set_dir(gpio_BL_, GPIO_OUT);
		::gpio_put(gpio_BL_, 0);
	}
	if (UsesCS()) {
		::gpio_init(gpio_CS_);
		::gpio_set_dir(gpio_CS_, GPIO_OUT);
		::gpio_put(gpio_CS_, 1);
	}
	::sleep_ms(100);
	SetSPIDataBits(8);
}

void TFT_LCD::Raw::WriteCmd(uint8_t cmd)
{
	::gpio_put(gpio_DC_, 0);
	::sleep_us(1);
	SetSPIDataBits(8);
	::spi_write_blocking(spi_, &cmd, sizeof(cmd));
	::gpio_put(gpio_DC_, 1);
}

void TFT_LCD::Raw::SendCmd(uint8_t cmd)
{
	EnableCS();
	WriteCmd(cmd);
	DisableCS();
}

void TFT_LCD::Raw::SendCmdAndData8(uint8_t cmd, const uint8_t* data, int len)
{
	EnableCS();
	WriteCmd(cmd);
	::sleep_us(1);
	::spi_write_blocking(spi_, data, len);
	DisableCS();
}

void TFT_LCD::Raw::SendCmdAndData16(uint8_t cmd, const uint16_t* data, int len)
{
	EnableCS();
	WriteCmd(cmd);
	::sleep_us(1);
	SetSPIDataBits(16);
	::spi_write16_blocking(spi_, data, len);
	DisableCS();
}

}
