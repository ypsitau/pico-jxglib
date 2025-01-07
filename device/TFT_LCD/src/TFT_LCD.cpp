//==============================================================================
// TFT_LCD.cpp
//==============================================================================
#include "jxglib/TFT_LCD.h"
#include "jxglib/DMA.h"

// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80 // Not used unless commandlist invoked

// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0x00
#define TFT_SWRST   0x01

#define TFT_INVOFF  0x20
#define TFT_INVON   0x21

#define TFT_DISPOFF 0x28
#define TFT_DISPON  0x29

#define TFT_CASET   0x2A
#define TFT_PASET   0x2B
#define TFT_RAMWR   0x2C

#define TFT_RAMRD   0x2E
#define TFT_IDXRD   0xDD // ILI9341 only, indexed control register read

#define TFT_MADCTL  0x36
#define TFT_MAD_MY  0x80
#define TFT_MAD_MX  0x40
#define TFT_MAD_MV  0x20
#define TFT_MAD_ML  0x10
#define TFT_MAD_BGR 0x08
#define TFT_MAD_MH  0x04
#define TFT_MAD_RGB 0x00

#ifdef TFT_RGB_ORDER
  #if (TFT_RGB_ORDER == 1)
    #define TFT_MAD_COLOR_ORDER TFT_MAD_RGB
  #else
    #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
  #endif
#else
  #define TFT_MAD_COLOR_ORDER TFT_MAD_BGR
#endif

// All ILI9341 specific commands some are used by init()
#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0A
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_VSCRDEF 0x33
#define ILI9341_MADCTL  0x36
#define ILI9341_VSCRSADD 0x37
#define ILI9341_PIXFMT  0x3A

#define ILI9341_WRDISBV  0x51
#define ILI9341_RDDISBV  0x52
#define ILI9341_WRCTRLD  0x53

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID4   0xD3
#define ILI9341_RDINDEX 0xD9
#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDIDX   0xDD // TBC

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

namespace jxglib {

//------------------------------------------------------------------------------
// TFT_LCD
//------------------------------------------------------------------------------
#if 0
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
	//::sleep_ms(150);

	raw.SendCmd(0xef, 0x03, 0x80, 0x02);
	raw.SendCmd(0xcf, 0x00, 0xc1, 0x30);
	raw.SendCmd(0xed, 0x64, 0x03, 0x12, 0x81);
	raw.SendCmd(0xe8, 0x85, 0x00, 0x78);
	raw.SendCmd(0xcb, 0x39, 0x2c, 0x00, 0x34, 0x02);
	raw.SendCmd(0xf7, 0x20);
	raw.SendCmd(0xea, 0x00, 0x00);
	raw.SendCmd(ILI9341_PWCTR1, 0x23);
	raw.SendCmd(ILI9341_PWCTR2, 0x10);
	raw.SendCmd(ILI9341_VMCTR1, 0x3e, 0x28);
	raw.SendCmd(ILI9341_VMCTR2, 0x86);
	raw.SendCmd(ILI9341_MADCTL, TFT_MAD_MX | TFT_MAD_COLOR_ORDER);
	raw.SendCmd(ILI9341_PIXFMT, 0x55);
	raw.SendCmd(0xb1, 0x00, 0x13);
	raw.SendCmd(ILI9341_DFUNCTR, 0x08, 0x82, 0x27);
	raw.SendCmd(0xf2, 0x00);
	raw.GammaSet(configData.gammaCurve);
	do {
		const uint8_t data[] = {
			0x0F,
			0x31,
			0x2B,
			0x0C,
			0x0E,
			0x08,
			0x4E,
			0xF1,
			0x37,
			0x07,
			0x10,
			0x03,
			0x0E,
			0x09,
			0x00,
		};
		raw.SendCmdAndData8(0xe0, data, count_of(data));	// positive gamma control
	} while (0);
	do {
		const uint8_t data[] = {
			0x0F,
			0x31,
			0x2B,
			0x0C,
			0x0E,
			0x08,
			0x4E,
			0xF1,
			0x37,
			0x07,
			0x10,
			0x03,
			0x0E,
			0x09,
			0x00,
		};
		raw.SendCmdAndData8(0xe1, data, count_of(data));	// negative gamma control
	} while (0);

	::sleep_ms(10);
	raw.SleepOut();
	raw.DisplayOn();
	Clear();
	raw.SetGPIO_BL(1);
}
#endif

#if 1
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
	raw.InterfacePixelFormat(configData.rgbInterfaceFormat, configData.mcuInterfaceFormat);
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
#endif

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
// TFT_LCD::DispatcherRGB565
//------------------------------------------------------------------------------
void TFT_LCD::DispatcherRGB565::Initialize()
{
	// do nothing
}

void TFT_LCD::DispatcherRGB565::Refresh()
{
	// do nothing
}

void TFT_LCD::DispatcherRGB565::Fill(const Color& color)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	raw.ColumnAddressSet(xAdjust, xAdjust + display_.GetWidth() - 1);
	raw.RowAddressSet(yAdjust, yAdjust + display_.GetHeight() - 1);
	raw.MemoryWriteConst16(ColorRGB565(color).value, display_.GetWidth() * display_.GetHeight());
}

void TFT_LCD::DispatcherRGB565::DrawPixel(int x, int y, const Color& color)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x);
	raw.RowAddressSet(y, y);
	raw.MemoryWriteConst16(ColorRGB565(color).value, 1);
}

void TFT_LCD::DispatcherRGB565::DrawRectFill(int x, int y, int width, int height, const Color& color)
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

void TFT_LCD::DispatcherRGB565::DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, int scaleX, int scaleY)
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
	uint16_t colorRGB565 = ColorRGB565(color).value;
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
				if (bits & 0x80) {
					for (int iScaleX = 0; iScaleX < scaleX; iScaleX++) raw.MemoryWrite_Data16(colorRGB565);
				}
			}
		}
		pSrcLeft = pSrc;
	}
	raw.MemoryWrite_End();
}

void TFT_LCD::DispatcherRGB565::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
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

void TFT_LCD::DispatcherRGB565::DrawImageFast(int x, int y, const Image& image)
{

	Raw& raw = display_.raw;
	spi_inst_t* spi = raw.GetSPI();
	int xAdjust, yAdjust;
	display_.CalcPosAdjust(Dir::Normal, &xAdjust, &yAdjust);
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x + image.GetWidth() - 1);
	raw.RowAddressSet(y, y + image.GetHeight() - 1);
	raw.MemoryWrite_Begin(16);
	DMA::ChannelConfig config;
	DMA::Channel& channel = *DMA::claim_unused_channel(true);
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

void TFT_LCD::DispatcherRGB565::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
	// do nothing
}

void TFT_LCD::DispatcherRGB565::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
	// do nothing
}

//------------------------------------------------------------------------------
// TFT_LCD::DispatcherRGB666
//------------------------------------------------------------------------------
void TFT_LCD::DispatcherRGB666::Initialize()
{
	// do nothing
}

void TFT_LCD::DispatcherRGB666::Refresh()
{
	// do nothing
}

void TFT_LCD::DispatcherRGB666::Fill(const Color& color)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	raw.ColumnAddressSet(xAdjust, xAdjust + display_.GetWidth() - 1);
	raw.RowAddressSet(yAdjust, yAdjust + display_.GetHeight() - 1);
	raw.MemoryWriteConstColor666(color, display_.GetWidth() * display_.GetHeight());
}

void TFT_LCD::DispatcherRGB666::DrawPixel(int x, int y, const Color& color)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x);
	raw.RowAddressSet(y, y);
	raw.MemoryWriteConstColor666(color, 1);
}

void TFT_LCD::DispatcherRGB666::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	if (!AdjustRange(&x, &width, 0, display_.GetWidth())) return;
	if (!AdjustRange(&y, &height, 0, display_.GetHeight())) return;
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x + width - 1);
	raw.RowAddressSet(y, y + height - 1);
	raw.MemoryWriteConstColor666(color, width * height);
}


void TFT_LCD::DispatcherRGB666::DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, int scaleX, int scaleY)
{
	Raw& raw = display_.raw;
	int xAdjust = display_.GetXAdjust(), yAdjust = display_.GetYAdjust();
	int nDots = width * height;
	int bytes = (width + 7) / 8 * height;
	const uint8_t* pSrcLeft = reinterpret_cast<const uint8_t*>(data);
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x + width * scaleX - 1);
	raw.RowAddressSet(y, y + height * scaleY - 1);
	raw.MemoryWrite_Begin(8);
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
				if (bits & 0x80) {
					for (int iScaleX = 0; iScaleX < scaleX; iScaleX++) raw.MemoryWrite_Color666(color);
				}
			}
		}
		pSrcLeft = pSrc;
	}
	raw.MemoryWrite_End();
}

void TFT_LCD::DispatcherRGB666::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
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
	raw.MemoryWrite_Begin(8);
	if (image.GetFormat().IsGray()) {
		using Reader = Image::Reader<Image::Getter_T<Color, ColorGray> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::Getter_T<Color, Color> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::Getter_T<Color, ColorA> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::Getter_T<Color, ColorRGB565> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	}
	raw.MemoryWrite_End();
	raw.MemoryDataAccessControl(saved.displayDir, saved.configData);
}

void TFT_LCD::DispatcherRGB666::DrawImageFast(int x, int y, const Image& image)
{

	Raw& raw = display_.raw;
	spi_inst_t* spi = raw.GetSPI();
	int xAdjust, yAdjust;
	display_.CalcPosAdjust(Dir::Normal, &xAdjust, &yAdjust);
	x += xAdjust, y += yAdjust;
	raw.ColumnAddressSet(x, x + image.GetWidth() - 1);
	raw.RowAddressSet(y, y + image.GetHeight() - 1);
	int xSrc = 0, ySrc = 0;
	int xSkip = 0, ySkip = 0;
	int wdImage = image.GetWidth(), htImage = image.GetHeight();
	raw.MemoryWrite_Begin(8);
	if (image.GetFormat().IsGray()) {
		using Reader = Image::Reader<Image::Getter_T<Color, ColorGray> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	} else if (image.GetFormat().IsRGB()) {
		using Reader = Image::Reader<Image::Getter_T<Color, Color> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	} else if (image.GetFormat().IsRGBA()) {
		using Reader = Image::Reader<Image::Getter_T<Color, ColorA> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	} else if (image.GetFormat().IsRGB565()) {
		using Reader = Image::Reader<Image::Getter_T<Color, ColorRGB565> >;
		Image::Reader reader(Reader::Normal(image, xSrc + xSkip, ySrc + ySkip, wdImage, htImage));
		while (!reader.HasDone()) raw.MemoryWrite_Color666(reader.ReadForward());
	}
	raw.MemoryWrite_End();
}

void TFT_LCD::DispatcherRGB666::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
	// do nothing
}

void TFT_LCD::DispatcherRGB666::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
	// do nothing
}

//------------------------------------------------------------------------------
// TFT_LCD::Raw
//------------------------------------------------------------------------------
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
