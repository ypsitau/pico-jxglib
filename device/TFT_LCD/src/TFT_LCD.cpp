//==============================================================================
// TFT_LCD.cpp
//==============================================================================
#include "jxglib/TFT_LCD.h"
#include "jxglib/DMA.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TFT_LCD
//------------------------------------------------------------------------------
void TFT_LCD::Initialize(Dir displayDir, const ConfigData& configData)
{
	saved_.displayDir = displayDir;
	saved_.configData = configData;
	if (displayDir.IsHorz()) {
		width_ = widthPhysical_, height_ = heightPhysical_;
	} else {
		width_ = heightPhysical_, height_ = widthPhysical_;
	}
	CalcPosAdjust(displayDir, &xAdjust_, &yAdjust_);
	raw.InitGPIO();
	raw.SoftwareReset();
	::sleep_ms(150);
	raw.SleepOut();
	::sleep_ms(50);
	raw.InterfacePixelFormat(5, 5);
		// RGB interface color format     = 65K of RGB interface. No effect on ST7735.
		// Control interface color format = 16bit/pixel
	::sleep_ms(10);
	raw.MemoryDataAccessControl(displayDir, configData);
	if (configData.displayInversionOnFlag) raw.DisplayInversionOn();
	::sleep_ms(10);
	raw.NormalDisplayModeOn();
	::sleep_ms(10);
	raw.GammaSet(configData.gammaCurve);
	::sleep_ms(10);
	raw.DisplayOn();
	Clear();
	::sleep_ms(10);
	raw.SetGPIO_BL(1);
}

void TFT_LCD::CalcPosAdjust(Dir displayDir, int* pxAdjust, int* pyAdjust) const
{
	if (displayDir.IsHorz()) {
		*pxAdjust = (widthTypical_ - widthPhysical_) / 2;
		*pyAdjust = displayDir.IsBottomToTop()? heightTypical_ - heightPhysical_ : 0;
	} else {
		*pxAdjust = displayDir.IsBottomToTop()? heightTypical_ - heightPhysical_ : 0;
		*pyAdjust = (widthTypical_ - widthPhysical_) / 2;
	}
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

void TFT_LCD::DispatcherEx::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
{
	Raw& raw = display_.raw;
	const Saved& saved = display_.GetSaved();
	Dir displayDir = saved.displayDir.Transform(drawDir);
	int xAdjust, yAdjust;
	display_.CalcPosAdjust(displayDir, &xAdjust, &yAdjust);
	int xSrc = rectClip.x, ySrc = rectClip.y;
	int wdImage = rectClip.width, htImage = rectClip.height;
	int xSkip = 0, ySkip = 0;
	if (drawDir.IsHorz()) {
		if (!AdjustRange(&x, &wdImage, 0, display_.GetWidth(), &xSkip)) return;
		if (!AdjustRange(&y, &htImage, 0, display_.GetHeight(), &ySkip)) return;
	} else {
		if (!AdjustRange(&x, &htImage, 0, display_.GetWidth(), &xSkip)) return;
		if (!AdjustRange(&y, &wdImage, 0, display_.GetHeight(), &ySkip)) return;
	}
	x += xAdjust, y += yAdjust;
	raw.MemoryDataAccessControl(displayDir, saved.configData);
	raw.ColumnAddressSet(x, x + wdImage - 1);
	raw.RowAddressSet(y, y + htImage - 1);
	raw.MemoryWrite_Begin(16);
	if (image.GetFormat().IsGray()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, ColorGray> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, Color> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, ColorA> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::Getter_T<ColorRGB565, ColorRGB565> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Data16(reader.ReadForward());
	}
	raw.MemoryWrite_End();
	raw.MemoryDataAccessControl(saved.displayDir, saved.configData);
}

void TFT_LCD::DispatcherEx::DrawImageFast(int x, int y, const Image& image)
{
	Raw& raw = display_.raw;
	spi_inst_t* spi = raw.GetSPI();
	DMA::Channel channel(DMA::claim_unused_channel(true));
	int xAdjust, yAdjust;
	display_.CalcPosAdjust(Dir::Normal, &xAdjust, &yAdjust);
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x + image.GetWidth() - 1);
	raw.RowAddressSet(y, y + image.GetHeight() - 1);
	raw.MemoryWrite_Begin(16);
	DMA::ChannelConfig config;
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_16)
		.set_read_increment(true)
		.set_write_increment(false)
		.set_dreq(::spi_get_dreq(spi, true))	// see RP2040 Datasheet 2.5.3.1 System DREQ Table
		.set_chain_to(channel)					// disable chain_to by setting it to the own channel
		.set_ring_to_read(0)
		.set_bswap(image.GetFormat().IsRGB565BE())
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	channel.set_config(config);
	channel.set_read_addr(image.GetPointer())
		.set_write_addr(&::spi_get_hw(spi)->dr)
		.set_trans_count_trig(image.GetWidth() * image.GetHeight())
		.wait_for_finish_blocking();
	channel.unclaim();
	raw.MemoryWrite_End();
}

void TFT_LCD::DispatcherEx::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
	// do nothing
}

void TFT_LCD::DispatcherEx::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
	// do nothing
}

//------------------------------------------------------------------------------
// TFT_LCD::Raw
//------------------------------------------------------------------------------
#if 0
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
#endif
void TFT_LCD::Raw::InitGPIO()
{
	gpio_DC_.init().set_dir_OUT().put(1);
	gpio_RST_.init().set_dir_OUT().put(1);
	if (gpio_BL_.IsValid()) gpio_BL_.init().set_dir_OUT().put(1);
	if (UsesCS()) gpio_CS_.init().set_dir_OUT().put(1);
	::sleep_ms(100);
	SetSPIDataBits(8);
}

void TFT_LCD::Raw::WriteCmd(uint8_t cmd)
{
	//::gpio_put(gpio_DC_, 0);
	gpio_DC_.put(0);
	::sleep_us(1);
	SetSPIDataBits(8);
	::spi_write_blocking(spi_, &cmd, sizeof(cmd));
	//::gpio_put(gpio_DC_, 1);
	gpio_DC_.put(1);
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
