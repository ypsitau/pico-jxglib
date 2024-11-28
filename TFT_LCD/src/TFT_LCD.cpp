//==============================================================================
// TFT_LCD.cpp
//==============================================================================
#include "jxglib/TFT_LCD.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TFT_LCD
//------------------------------------------------------------------------------
using PA = TFT_LCD::PageAddressOrder;
using CA = TFT_LCD::ColumnAddressOrder;
using PC = TFT_LCD::PageColumnOrder;

const TFT_LCD::RotateData TFT_LCD::rotateDataTbl_Case1[] =
{
	{ PA::TopToBottom, CA::LeftToRight, PC::NormalMode },	// HorzFromNW (Rotate0)
	{ PA::TopToBottom, CA::RightToLeft, PC::NormalMode },	// HorzFromNE (MirrorHorz)
	{ PA::BottomToTop, CA::LeftToRight, PC::NormalMode },	// HorzFromSW (MirrorVert)
	{ PA::BottomToTop, CA::RightToLeft, PC::NormalMode },	// HorzFromSE (Rotate180)
	{ PA::TopToBottom, CA::LeftToRight, PC::ReverseMode },	// VertFromNW
	{ PA::TopToBottom, CA::RightToLeft, PC::ReverseMode },	// VertFromNE (Rotate270)
	{ PA::BottomToTop, CA::LeftToRight, PC::ReverseMode },	// VertFromSW (Rotate90)
	{ PA::BottomToTop, CA::RightToLeft, PC::ReverseMode },	// VertFromSE
};

const TFT_LCD::RotateData TFT_LCD::rotateDataTbl_Case2[] =
{
	{ PA::BottomToTop, CA::LeftToRight, PC::NormalMode },	// HorzFromNW (Rotate0)
	
	{ PA::TopToBottom, CA::RightToLeft, PC::NormalMode },	// HorzFromNE (MirrorHorz)
	{ PA::BottomToTop, CA::LeftToRight, PC::NormalMode },	// HorzFromSW (MirrorVert)
	{ PA::BottomToTop, CA::RightToLeft, PC::NormalMode },	// HorzFromSE (Rotate180)
	{ PA::TopToBottom, CA::LeftToRight, PC::ReverseMode },	// VertFromNW
	{ PA::TopToBottom, CA::RightToLeft, PC::ReverseMode },	// VertFromNE (Rotate270)
	{ PA::BottomToTop, CA::LeftToRight, PC::ReverseMode },	// VertFromSW (Rotate90)
	{ PA::BottomToTop, CA::RightToLeft, PC::ReverseMode },	// VertFromSE
};

const TFT_LCD::RotateData TFT_LCD::rotateDataTbl_Case3[] =
{
	{ PA::BottomToTop, CA::RightToLeft, PC::NormalMode },	// HorzFromNW (Rotate0)
	
	{ PA::TopToBottom, CA::RightToLeft, PC::NormalMode },	// HorzFromNE (MirrorHorz)
	{ PA::BottomToTop, CA::LeftToRight, PC::NormalMode },	// HorzFromSW (MirrorVert)
	{ PA::BottomToTop, CA::RightToLeft, PC::NormalMode },	// HorzFromSE (Rotate180)
	{ PA::TopToBottom, CA::LeftToRight, PC::ReverseMode },	// VertFromNW
	{ PA::TopToBottom, CA::RightToLeft, PC::ReverseMode },	// VertFromNE (Rotate270)
	{ PA::BottomToTop, CA::LeftToRight, PC::ReverseMode },	// VertFromSW (Rotate90)
	{ PA::BottomToTop, CA::RightToLeft, PC::ReverseMode },	// VertFromSE
};

void TFT_LCD::Initialize(const RotateData& rotateData, const ConfigData& configData)
{
	if (rotateData.pageColumnOrder == PC::NormalMode) {
		width_ = widthSet_, height_ = heightSet_;
	} else {
		width_ = heightSet_, height_ = widthSet_;
	}
	if (widthSet_ < 128) xAdjust_ = (128 - widthSet_) / 2;
	raw.InitGPIO();
	raw.SoftwareReset();
	::sleep_ms(150);
	raw.SleepOut();
	::sleep_ms(50);
	raw.InterfacePixelFormat(5, 5);
		// RGB interface color format     = 65K of RGB interface. No effect on ST7735.
		// Control interface color format = 16bit/pixel
	::sleep_ms(10);
	raw.MemoryDataAccessControl(
		rotateData.pageAddressOrder, rotateData.columnAddressOrder, rotateData.pageColumnOrder,
		configData.lineAddressOrder, configData.rgbBgrOrder, configData.displayDataLatchOrder);
	if (configData.displayInversionOnFlag) raw.DisplayInversionOn();
	::sleep_ms(10);
	raw.NormalDisplayModeOn();
	::sleep_ms(10);
	raw.GammaSet(configData.gammaCurve);
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
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	raw.ColumnAddressSet(xAdjust, xAdjust + display_.GetWidth() - 1);
	raw.RowAddressSet(yAdjust, yAdjust + display_.GetHeight() - 1);
	raw.MemoryWriteConst16(ColorRGB565(color).value, display_.GetWidth() * display_.GetHeight());
}

void TFT_LCD::DispatcherEx::DrawPixel(int x, int y, const Color& color)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x);
	raw.RowAddressSet(y, y);
	raw.MemoryWriteConst16(ColorRGB565(color).value, 1);
}

void TFT_LCD::DispatcherEx::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	if (!AdjustRange(&x, &width, 0, display_.GetWidth())) return;
	if (!AdjustRange(&y, &height, 0, display_.GetHeight())) return;
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWriteConst16(ColorRGB565(color).value, width * height);
}

void TFT_LCD::DispatcherEx::DrawBitmap(int x, int y, const void* data, int width, int height,
					const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	x += xAdjust, y += yAdjust;
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
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	Rect rect = pRectClip? *pRectClip : Rect(0, 0, display_.GetWidth(), display_.GetHeight());
	int xSkip = 0, ySkip = 0;
	int width, height;
	if (Image::IsDirHorz(imageDir)) {
		width = image.GetWidth(), height = image.GetHeight();
	} else {
		width = image.GetHeight(), height = image.GetWidth();
	}
	if (!AdjustRange(&x, &width, rect.x, rect.width, &xSkip)) return;
	if (!AdjustRange(&y, &height, rect.y, rect.height, &ySkip)) return;
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWrite_Begin(16);
	if (image.GetFormat().IsGray()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, ColorGray> >;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, Color> >;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, ColorA> >;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, ColorRGB565> >;
		Image::Reader reader(Reader::Create(image, xSkip, ySkip, width, height, imageDir));
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
