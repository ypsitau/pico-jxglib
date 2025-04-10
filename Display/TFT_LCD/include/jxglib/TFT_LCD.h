//==============================================================================
// jxglib/TFT_LCD.h
//==============================================================================
#ifndef PICO_JXGLIB_TFT_LCD_H
#define PICO_JXGLIB_TFT_LCD_H
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "pico/time.h"
#include "hardware/spi.h"
#include "jxglib/Display.h"
#include "jxglib/Image.h"
#include "jxglib/DMA.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TFT_LCD
//------------------------------------------------------------------------------
class TFT_LCD : public Display {
public:
	struct PinAssign {
		const GPIO& RST;
		const GPIO& DC;
		const GPIO& CS;
		const GPIO& BL;
	};
	struct PinAssignNoCS {
		const GPIO& RST;
		const GPIO& DC;
		const GPIO& BL;
	};
	enum class PageAddressOrder {
		TopToBottom = 0,
		BottomToTop = 1,
	};
	enum class ColumnAddressOrder {
		LeftToRight = 0,
		RightToLeft = 1,
	};
	enum class PageColumnOrder {
		NormalMode = 0,
		ReverseMode = 1,
	};
	enum class RGBInterfaceFormat {
		BPP16	= 5,
		BPP18	= 6,
		BPP24	= 7,
	};
	enum class MCUInterfaceFormat {
		BPP3	= 1,
		BPP16	= 5,
		BPP18	= 6,
		BPP24	= 7,
	};
	enum class LineAddressOrder {
		TopToBottom = 0,
		BottomToTop = 1,
	};
	enum class RGBBGROrder {
		RGB = 0,
		BGR = 1,
	};
	enum class DisplayDataLatchOrder {
		LeftToRight = 0,
		RightToLeft = 1,
	};

	struct RotateData {
		PageAddressOrder pageAddressOrder;
		ColumnAddressOrder columnAddressOrder;
		PageColumnOrder pageColumnOrder;
	};
	struct ConfigData {
		RGBInterfaceFormat rgbInterfaceFormat;
		MCUInterfaceFormat mcuInterfaceFormat;
		LineAddressOrder lineAddressOrder;
		RGBBGROrder rgbBgrOrder;
		DisplayDataLatchOrder displayDataLatchOrder;
		bool invertHorzFlag;
		bool invertVertFlag;
		bool displayInversionOnFlag;
		uint8_t gammaCurve;
	};
	class DispatcherRGB565 : public Dispatcher, public DMA::IRQHandler {
	private:
		TFT_LCD& display_;
		DMA::Channel* pDMAChannel_;
		volatile bool finishFlag_;
		DrawImageFastHandler* pDrawImageFastHandler_;
	public:
		DispatcherRGB565(TFT_LCD& display) : display_{display}, pDMAChannel_{nullptr}, finishFlag_{false}, pDrawImageFastHandler_{nullptr} {}
	public:
		// virtual functions of Dispatcher
		virtual bool Initialize() override;
		virtual void Refresh() override;
		virtual void Fill(const Color& color) override;
		virtual void DrawPixel(int x, int y, const Color& color) override;
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) override;
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, const Color* pColorBg,
					int scaleX, int scaleY, const Rect& rectClip, DrawDir drawDir) override;
		virtual void DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir) override;
		virtual void DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler) override;
		virtual void ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip) override;
		virtual void ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip) override;
		// virtual function of DMA::IRQHandler
		virtual void OnDMAInterrupt(DMA::Channel& channel, uint irq_index) override;
	};
	class DispatcherRGB666 : public Dispatcher {
	private:
		TFT_LCD& display_;
	public:
		DispatcherRGB666(TFT_LCD& display) : display_{display} {}
	public:
		virtual bool Initialize() override;
		virtual void Refresh() override;
		virtual void Fill(const Color& color) override;
		virtual void DrawPixel(int x, int y, const Color& color) override;
		virtual void DrawRectFill(int x, int y, int width, int height, const Color& color) override;
		virtual void DrawBitmap(int x, int y, const void* data, int width, int height, const Color& color, const Color* pColorBg,
					int scaleX, int scaleY, const Rect& rectClip, DrawDir drawDir) override;
		virtual void DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir) override;
		virtual void DrawImageFast(int x, int y, const Image& image, bool blockFlag, DrawImageFastHandler* pHandler) override;
		virtual void ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip) override;
		virtual void ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip) override;
	};
	class Raw {
	private:
		spi_inst_t* spi_;
		const GPIO& gpio_RST_;		// Reset
		const GPIO& gpio_DC_;		// Data/Command
		const GPIO& gpio_CS_;		// Chip Select
		const GPIO& gpio_BL_;		// Backlight (maybe Invalid)
		spi_cpol_t cpol_;
		spi_cpha_t cpha_;
	public:
		Raw(spi_inst_t* spi, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_CS, const GPIO& gpio_BL) :
				spi_(spi), gpio_RST_(gpio_RST), gpio_DC_(gpio_DC), gpio_BL_(gpio_BL), gpio_CS_(gpio_CS),
				cpol_(SPI_CPOL_0), cpha_(SPI_CPHA_0) {}
		Raw(spi_inst_t* spi, const GPIO& gpio_RST, const GPIO& gpio_DC, const GPIO& gpio_BL) :
				spi_(spi), gpio_RST_(gpio_RST), gpio_DC_(gpio_DC), gpio_CS_(GPIO::None), gpio_BL_(gpio_BL),
				cpol_(SPI_CPOL_1), cpha_(SPI_CPHA_1) {}
	public:
		spi_inst_t* GetSPI() { return spi_; }
		bool UsesCS() const { return gpio_CS_.IsValid(); }
		void InitGPIO();
		void SetGPIO_BL(bool value) { if (gpio_BL_.IsValid()) gpio_BL_.put(value); }
		void SetSPIDataBits(uint data_bits) { ::spi_set_format(spi_, data_bits, cpol_, cpha_, SPI_MSB_FIRST); }
		void EnableCS() { if (UsesCS()) ::gpio_put(gpio_CS_, 0); }
		void DisableCS() { if (UsesCS()) ::gpio_put(gpio_CS_, 1); }
	public:
		void MemoryWrite_Begin(uint data_bits) {
			EnableCS();
			WriteCmd(0x2c);	// 9.1.22 RAMWR (2Ch): Memory Write
			::sleep_us(1);
			SetSPIDataBits(data_bits);
		}
		void MemoryWrite_Data8(uint8_t data) {
			while (!::spi_is_writable(spi_)) tight_loop_contents();
			spi_get_hw(spi_)->dr = static_cast<uint32_t>(data);
		}
		void MemoryWrite_Data16(uint16_t data) {
			while (!::spi_is_writable(spi_)) tight_loop_contents();
			spi_get_hw(spi_)->dr = static_cast<uint32_t>(data);
		}
		void MemoryWrite_Color(const Color& color) {
			MemoryWrite_Data8(color.r);
			MemoryWrite_Data8(color.g);
			MemoryWrite_Data8(color.b);
		}
		void MemoryWrite_End() {
			while (::spi_is_readable(spi_)) (void)spi_get_hw(spi_)->dr;
			while (spi_get_hw(spi_)->sr & SPI_SSPSR_BSY_BITS) tight_loop_contents();
			while (::spi_is_readable(spi_)) (void)spi_get_hw(spi_)->dr;
			spi_get_hw(spi_)->icr = SPI_SSPICR_RORIC_BITS;
			DisableCS();
		}
	public:
		void WriteCmd(uint8_t cmd);
		void SendCmd(uint8_t cmd);
		void SendCmdAndData8(uint8_t cmd, const uint8_t* data, int len);
		void SendCmdAndData16(uint8_t cmd, const uint16_t* data, int len);
		void SendCmd(uint8_t cmd, uint8_t data) { SendCmdAndData8(cmd, &data, 1); }
		void SendCmd(uint8_t cmd, uint8_t data1, uint8_t data2) {
			uint8_t buff[] = { data1, data2 };
			SendCmdAndData8(cmd, buff, sizeof(buff));
		}
		void SendCmd(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3) {
			uint8_t buff[] = { data1, data2, data3 };
			SendCmdAndData8(cmd, buff, sizeof(buff));
		}
		void SendCmd(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4) {
			uint8_t buff[] = { data1, data2, data3, data4 };
			SendCmdAndData8(cmd, buff, sizeof(buff));
		}
		void SendCmd(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5) {
			uint8_t buff[] = { data1, data2, data3, data4, data5 };
			SendCmdAndData8(cmd, buff, sizeof(buff));
		}
		void SendCmd(uint8_t cmd, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6) {
			uint8_t buff[] = { data1, data2, data3, data4, data5, data6 };
			SendCmdAndData8(cmd, buff, sizeof(buff));
		}
	public:
		// 9.1 System Function Command
		// 9.1.1 NOP (00h): No Operation
		void NoOperation() {
			SendCmd(0x00);
		}
		// 9.1.2 SWRESET (01h): Software Reset
		void SoftwareReset() {
			SendCmd(0x01);
		}
		// 9.1.3 RDDID (04h): Read Display ID
		// 9.1.4 RDDST (09h): Read Display Status
		// 9.1.5 RDDPM (0Ah): Read Display Power Mode
		// 9.1.6 RDDMADCTL (0Bh): Read Display MADCTL
		// 9.1.7 RDDCOLMOD (0Ch): Read Display Pixel Format
		// 9.1.8 RDDIM (0Dh): Read Display Image Mode
		// 9.1.9 RDDSM (0Eh): Read Display Signal Mode
		// 9.1.10 RDDSDR (0Fh): Read Display Self-Diagnostic Result
		// 9.1.11 SLPIN (10h): Sleep In
		void SleepIn() {
			SendCmd(0x10);
		}
		// 9.1.12 SLPOUT (11h): Sleep Out
		void SleepOut() {
			SendCmd(0x11);
		}
		// 9.1.13 PTLON (12h): Partial Display Mode On
		void PartialDisplayModeOn() {
			SendCmd(0x12);
		}
		// 9.1.14 NORON (13h): Normal Display Mode On
		void NormalDisplayModeOn() {
			SendCmd(0x13);
		}
		// 9.1.15 INVOFF (20h): Display Inversion Off
		void DisplayInversionOff() {
			SendCmd(0x20);
		}
		// 9.1.16 INVON (21h): Display Inversion On
		void DisplayInversionOn() {
			SendCmd(0x21);
		}
		// 9.1.17 GAMSET (26h): Gamma Set
		void GammaSet(uint8_t gc) {
			SendCmd(0x26, gc);
		}
		// 9.1.18 DISPOFF (28h): Display Off
		void DisplayOff() {
			SendCmd(0x28);
		}
		// 9.1.19 DISPON (29h): Display On
		void DisplayOn() {
			SendCmd(0x29);
		}
		// 9.1.20 CASET (2Ah): Column Addresss Set
		void ColumnAddressSet(uint16_t xs, uint16_t xe) {
			SendCmd(0x2a, static_cast<uint8_t>(xs >> 8), static_cast<uint8_t>(xs & 0xff),
				static_cast<uint8_t>(xe >> 8), static_cast<uint8_t>(xe & 0xff));
		}
		// 9.1.21 RASET (2Bh): Row Address Set
		void RowAddressSet(uint16_t ys, uint16_t ye) {
			SendCmd(0x2b, static_cast<uint8_t>(ys >> 8), static_cast<uint8_t>(ys & 0xff),
				static_cast<uint8_t>(ye >> 8), static_cast<uint8_t>(ye & 0xff));
		}
		// 9.1.22 RAMWR (2Ch): Memory Write
		void MemoryWrite(const uint8_t* data, int len) {
			SendCmdAndData8(0x2c, data, len);
		}
		void MemoryWrite16(const uint16_t* data, int len) {
			SendCmdAndData16(0x2c, data, len);
		}
		void MemoryWriteConst16(uint16_t data, int len) {
			MemoryWrite_Begin(16);
			while (len-- > 0) MemoryWrite_Data16(data);
			MemoryWrite_End();
		}
		void MemoryWriteConstColor666(const Color& color, int len) {
			MemoryWrite_Begin(8);
			while (len-- > 0) MemoryWrite_Color(color);
			MemoryWrite_End();
		}
		// 9.1.23 RAMRD (2Dh): Memory Read
		// 9.1.24 PTLAR (30h): Partial Area
		void PartialArea(uint16_t psl, uint16_t pel) {
			SendCmd(0x30, static_cast<uint8_t>(psl >> 8), static_cast<uint8_t>(psl & 0xff),
				static_cast<uint8_t>(pel >> 8), static_cast<uint8_t>(pel & 0xff));
		}
		// 9.1.25 VSCRDEF (33h): Vertical Scrolling Definition
		void VerticalScrollingDefinition(uint16_t topFixedArea, uint16_t verticalScrollArea, uint16_t bottomFixedArea) {
			SendCmd(0x33, static_cast<uint8_t>(topFixedArea >> 8), static_cast<uint8_t>(topFixedArea & 0xff),
				static_cast<uint8_t>(verticalScrollArea >> 8), static_cast<uint8_t>(verticalScrollArea & 0xff),
				static_cast<uint8_t>(bottomFixedArea >> 8), static_cast<uint8_t>(bottomFixedArea & 0xff));
		}
		// 9.1.26 TEOFF (34h): Tearing Effect Line Off
		void TearingEffectLineOff() {
			SendCmd(0x34);
		}
		// 9.1.27 TEON (35h): Tearing Effect Line On
		void TearingEffectLineOn() {
			SendCmd(0x35);
		}
		// 9.1.28 MADCTL (36h): Memory Data Access Control
		void MemoryDataAccessControl(PageAddressOrder pageAddressOrder, ColumnAddressOrder columnAddressOrder,
				PageColumnOrder pageColumnOrder, LineAddressOrder lineAddressOrder,
				RGBBGROrder rgbBgrOrder, DisplayDataLatchOrder displayDataLatchOrder) {
			SendCmd(0x36,
				(static_cast<uint8_t>(pageAddressOrder) << 7) |
				(static_cast<uint8_t>(columnAddressOrder) << 6) |
				(static_cast<uint8_t>(pageColumnOrder) << 5) |
				(static_cast<uint8_t>(lineAddressOrder) << 4) |
				(static_cast<uint8_t>(rgbBgrOrder) << 3) |
				(static_cast<uint8_t>(displayDataLatchOrder) << 2));
		}
		void MemoryDataAccessControl(Dir displayDir, const ConfigData& configData) {
			if (configData.invertHorzFlag) displayDir = displayDir.InvertHorz();
			if (configData.invertVertFlag) displayDir = displayDir.InvertVert();
			SendCmd(0x36,
				(displayDir.GetValue() << 5) |
				(static_cast<uint8_t>(configData.lineAddressOrder) << 4) |
				(static_cast<uint8_t>(configData.rgbBgrOrder) << 3) |
				(static_cast<uint8_t>(configData.displayDataLatchOrder) << 2));
		}
		// 9.1.29 VSCSAD (37h): Vertical Scroll Start Address to RAM
		void VerticalScrollStartAddressToRAM(uint16_t vsp) {
			SendCmd(0x37, static_cast<uint8_t>(vsp >> 8), static_cast<uint8_t>(vsp & 0xff));
		}
		// 9.1.30 IDMOFF (38h): Idle Mode Off
		void IdleModeOff() {
			SendCmd(0x38);
		}
		// 9.1.31 IDMON (39h): Idle Mode On
		void IdleModeOn() {
			SendCmd(0x39);
		}
		// 9.1.32 COLMOD (3Ah): Interface Pixel Format
		void InterfacePixelFormat(RGBInterfaceFormat rgbInterfaceFormat, MCUInterfaceFormat mcuInterfaceFormat) {
			SendCmd(0x3a, (static_cast<uint8_t>(rgbInterfaceFormat) << 4) | static_cast<uint8_t>(mcuInterfaceFormat));
		}
		// 9.1.33 WRMEMC (3Ch): Write Memory Continue
		void WriteMemoryContinue(const uint8_t* data, int len) {
			SendCmdAndData8(0x3c, data, len);
		}
		void WriteMemoryContinueBy16Bit(const uint16_t* data, int len) {
			SendCmdAndData16(0x3c, data, len);
		}
		// 9.1.34 RDMEMC (3Eh): Read Memory Continue
		// 9.1.35 STE (44h): Set Tear Scanline
		void SetTearScanline(uint16_t n) {
			SendCmd(0x44, static_cast<uint8_t>(n >> 8), static_cast<uint8_t>(n & 0xff));
		}
		// 9.1.36 GSCAN (45H): Get Scanline
		// 9.1.37 WRDISBV (51h): Write Display Brightness
		void WriteDisplayBrightness(uint8_t dbv) {
			SendCmd(0x51, dbv);
		}
		// 9.1.38 RDDISBV (52h): Read Display Brightness Value
		// 9.1.39 WRCTRLD (53h): Write CTRL Display
		void WriteCTRLDisplay(uint8_t bctrl, uint8_t dd, uint8_t bl) {
			SendCmd(0x53, (bctrl < 5) | (dd < 3) | (bl << 2));
		}	
		// 9.1.40 RDCTRLD (54h): Read CTRL Value Display
		// 9.1.41 WRCASE (55h): Write Content Adaptive Brightness Control and Color Enhancement
		void WriteContentAdaptiveBrightnessControlAndColorEnhancement(uint8_t cectrl, uint8_t ce, uint8_t c) {
			SendCmd(0x55, (cectrl << 7) | (ce << 4) | (c << 0));
		}
		// 9.1.42 RDCABC (56h): Read Content Adaptive Brightness Control
		// 9.1.43 WRCABCMB (5Eh): Write CABC Minimum Brightness
		void WriteCABCMinimumBrightness(uint8_t cmb) {
			SendCmd(0x5e, cmb);
		}
		// 9.1.44 RDCABMB (5Fh): Read CABC Minimum Brightness
		// 9.1.45 RDABCSDR (68h): Read Automatic Brightness Control Self-Diagnostic Result
		// 9.1.46 RDID1 (DAh): Read ID1
		// 9.1.47 RDID2 (DBh): Read ID2
		// 9.1.48 RDID3 (DCh): Read ID3
		// 9.2 System Function Command
	};
public:
	Raw raw;
	DispatcherRGB565 dispatcherRGB565_;
	DispatcherRGB666 dispatcherRGB666_;
	int widthTypical_, heightTypical_;
	int widthPhysical_, heightPhysical_;
	int xAdjust_, yAdjust_;
	struct Saved {
		Dir displayDir;
		ConfigData configData;
	} saved_;
public:
	TFT_LCD(spi_inst_t* spi, const Format& format, int widthTypical, int heightTypical, int width, int height, const PinAssign& pinAssign) :
			Display(Capability::Device | Capability::DrawImage | Capability::DrawImageFast, format, width, height),
			raw(spi, pinAssign.RST, pinAssign.DC, pinAssign.CS, pinAssign.BL), dispatcherRGB565_(*this), dispatcherRGB666_(*this),
			widthTypical_{widthTypical}, heightTypical_{heightTypical},
			widthPhysical_{width}, heightPhysical_{height}, xAdjust_{0}, yAdjust_{0} {
		if (format.IsRGB()) {
			SetDispatcher(dispatcherRGB666_);
		} else {
			SetDispatcher(dispatcherRGB565_);
		}
	}
	TFT_LCD(spi_inst_t* spi, const Format& format, int widthTypical, int heightTypical, int width, int height, const PinAssignNoCS& pinAssign) :
			Display(Capability::Device | Capability::DrawImage, format, width, height),
			raw(spi, pinAssign.RST, pinAssign.DC, pinAssign.BL), dispatcherRGB565_(*this), dispatcherRGB666_(*this),
			widthTypical_{widthTypical}, heightTypical_{heightTypical},
			widthPhysical_{width}, heightPhysical_{height}, xAdjust_{0}, yAdjust_{0} {
		if (format.IsRGB()) {
			SetDispatcher(dispatcherRGB666_);
		} else {
			SetDispatcher(dispatcherRGB565_);
		}
	}
public:
	TFT_LCD& Initialize(Dir displayDir, const ConfigData& configData);
	const Saved& GetSaved() const { return saved_; }
	virtual Dir GetDirection() const override { return saved_.displayDir; }
	bool UsesCS() { return raw.UsesCS(); }
	int GetBytesPerLine() const { return GetWidth() * 2; }
	int GetWidthPhysical() const { return widthPhysical_; }
	int GetHeightPhysical() const { return heightPhysical_; }
	int GetXAdjust() const { return xAdjust_; }
	int GetYAdjust() const { return yAdjust_; }
	void CalcPosAdjust(Dir displayDir, int* pxAdjust, int* pyAdjust) const;
};

}

#endif
