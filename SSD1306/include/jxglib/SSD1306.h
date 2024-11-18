//==============================================================================
// jxglib/SSD1306.h
// 128x64 Dot Matrix OLED/PLED Segment/Common Driver with Controller
// Specification: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
//==============================================================================
#ifndef PICO_JXGLIB_SSD1306_H
#define PICO_JXGLIB_SSD1306_H
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "jxglib/Common.h"
#include "jxglib/Font.h"
#include "jxglib/Display.h"

namespace jxglib {

//------------------------------------------------------------------------------
// SSD1306
//------------------------------------------------------------------------------
class SSD1306 : public Display {
public:
	class Logic_Set {
	public:
		uint8_t operator()(uint8_t v1, uint8_t v2) { return v1 | v2; }
	};
	class Logic_Clear {
	public:
		uint8_t operator()(uint8_t v1, uint8_t v2) { return v1 & ~v2; }
	};
	class Raw {
	private:
	private:
		i2c_inst_t* i2c_;
		uint8_t addr_;
	public:
		Raw(i2c_inst_t* i2c, uint8_t addr) : i2c_(i2c), addr_(addr) {}
	public:
		uint8_t GetAddr() const { return addr_; }
	public:
		void SendCtrl(uint8_t ctrl) const {
			uint8_t buff[2];
			buff[0] =
				(0b1 << 7) |	// Co = 1
				(0b0 << 6);		// D/C# = 0
			buff[1] = ctrl;
			::i2c_write_blocking(i2c_, addr_, buff, sizeof(buff), false);
		}
		void SendBuffer(const uint8_t* buff, int len) const {
			::i2c_write_blocking(i2c_, addr_, buff, len, false);
		}
	public:
		// 10.1 Fundamental Command
		// 10.1.1 Set Lower Column Start Address for Page Addressing Mode (00h-0Fh)
		// 10.1.2 Set Higher Column Start Address for Page Addressing Mode (10h-1Fh)
		void SetColumnStartAddressForPageAddressingMode(uint8_t columnStartAddr) const {
			SendCtrl(0x00 | (columnStartAddr & 0x0f));
			SendCtrl(0x10 | (columnStartAddr >> 4));
		}
		// 10.1.3 Set Memory Addressing Mode (20h)
		void SetMemoryAddressingMode(uint8_t mode) const {
			SendCtrl(0x20);
			SendCtrl(mode);
		}
		// 10.1.4 Set Column Address (21h)
		void SetColumnAddress(uint8_t columnStartAddr, uint8_t columnEndAddr) const {
			SendCtrl(0x21);
			SendCtrl(columnStartAddr);
			SendCtrl(columnEndAddr);
		}
		// 10.1.5 Set Page Address (22h)
		void SetPageAddress(uint8_t pageStartAddr, uint8_t pageEndAddr) const {
			SendCtrl(0x22);
			SendCtrl(pageStartAddr);
			SendCtrl(pageEndAddr);
		}
		// 10.1.6 Set Display Start Line (40h-7Fh)
		void SetDisplayStartLine(uint8_t startLine) const {
			SendCtrl(0x40 | startLine);
		}
		// 10.1.7 Set Contrast Control for BANK0 (81h)
		void SetContrastControl(uint8_t contrast) const {
			SendCtrl(0x81);
			SendCtrl(contrast);
		}
		// 10.1.8 SetSegmentRe-map (A0h/A1h)
		void SetSegmentRemap(uint8_t flag) const {
			SendCtrl(0xa0 | flag);
		}
		// 10.1.9 Entire Display ON (A4h/A5h)
		void EntireDisplayOn(uint8_t allOnFlag) const {
			SendCtrl(0xa4 | allOnFlag);
		}
		// 10.1.10 Set Normal/Inverse Display (A6h/A7h)
		void SetNormalInverseDisplay(uint8_t inverse) const {
			SendCtrl(0xa6 | inverse);
		}
		// 10.1.11 Set Multiplex Ratio (A8h)
		void SetMultiplexRatio(uint8_t ratio) const {
			SendCtrl(0xa8);
			SendCtrl(ratio);
		}
		// 10.1.12 Set Display ON/OFF (AEh/AFh)
		void SetDisplayOnOff(uint8_t on) const {
			SendCtrl(0xae | on);
		}
		// 10.1.13 Set Page Start Address for Page Addressing Mode (B0h-B7h)
		void SetPageStartAddressForPageAddressingMode(uint8_t pageStartAddrGDDRAM) const {
			SendCtrl(0xb0 | pageStartAddrGDDRAM);
		}
		// 10.1.14 Set COM Output Scan Direction (C0h/C8h)
		void SetCOMOutputScanDirection(uint8_t scanDir) const {
			SendCtrl(0xc0 | (scanDir << 3));
		}
		// 10.1.15 Set Display Offset (D3h)
		void SetDisplayOffset(uint8_t offset) const {
			SendCtrl(0xd3);
			SendCtrl(offset);
		}
		// 10.1.16 Set Display Clock Divide Ratio / Oscillator Frequency (D5h)
		void SetDisplayClockDivideRatioOscillatorFrequency(uint8_t divideRatio, uint8_t oscillatorFreq) const {
			SendCtrl(0xd5);
			SendCtrl((oscillatorFreq << 4) | divideRatio);
		}
		// 10.1.17 Set Pre-charge Period (D9h)
		void SetPrechargePeriod(uint8_t clocksPhase1, uint8_t clocksPhase2) const {
			SendCtrl(0xd9);
			SendCtrl((clocksPhase2 << 4) | clocksPhase1);
		}
		// 10.1.18 Set COM Pins Hardware Configuration (DAh)
		void SetCOMPinsHardwareConfiguration(uint8_t pinConfig, uint8_t enableRemap) const {
			SendCtrl(0xda);
			SendCtrl(0x02 | (enableRemap << 5) | (pinConfig << 4));
		}
		// 10.1.19 Set Vcomh Detect Level (DBh)
		void SetVcomhDeselectLevel(uint8_t level) const {
			SendCtrl(0xdb);
			SendCtrl(level << 4);
		}
		// 10.1.20 NOP (E3h)
		void NOP() const {
			SendCtrl(0xe3);
		}
		// 10.2 Graphic Acceleration Command
		// 10.2.1 Horizontal Scroll Setup (26h/27h)
		void HorizontalScrollSetup(uint8_t left, uint8_t startPageAddr, uint8_t endPageAddr, uint8_t framesInterval) const {
			SendCtrl(0x26 | left);
			SendCtrl(0x00);				// Dummy byte
			SendCtrl(startPageAddr);
			SendCtrl(framesInterval);
			SendCtrl(endPageAddr);
			SendCtrl(0x00);				// Dummy byte
			SendCtrl(0xff);				// Dummy byte
		}
		// 10.2.2 Continuous Vertical and Horizontal Scroll Setup (29h/2Ah)
		void ContinuousVerticalAndHorizontalScrollSetup(uint8_t left, uint8_t startPageAddr, uint8_t endPageAddr, uint8_t framesInterval, uint8_t verticalScrollingOffset) const {
			SendCtrl(0x29 + left);
			SendCtrl(0x00);				// Dummy byte
			SendCtrl(startPageAddr);
			SendCtrl(framesInterval);
			SendCtrl(endPageAddr);
			SendCtrl(verticalScrollingOffset);
		}
		// 10.2.3 Deactivate Scroll (2Eh)
		void DeactivateScroll() const {
			SendCtrl(0x2e);
		}
		// 10.2.4 Activate Scroll (2Fh)
		void ActivateScroll() const {
			SendCtrl(0x2f);
		}
		// 10.2.5 Set Vertical Scroll Area (A3h)
		void SetVerticalScrollArea(uint8_t rowsFixedArea, uint8_t rowsScrollArea) const {
			SendCtrl(0xa3);
			SendCtrl(rowsFixedArea);
			SendCtrl(rowsScrollArea);
		}
		// Charge Pump Regulator
		void ChargePumpSetting(uint8_t enableChargePump) const {
			SendCtrl(0x8d);
			SendCtrl(0x10 | (enableChargePump << 2));
		}
	};
public:
	static const uint8_t DefaultAddr = 0x3c;
	Raw raw;
private:
	static const int heightPerPage_ = 8;
	int numPages_;
	int bufferLen_;
	uint8_t* buffWhole_;
	uint8_t* buff_;
private:
	Context context_;
public:
	SSD1306(i2c_inst_t* i2c, uint8_t addr = DefaultAddr, bool highResoFlag = true) :
			Display(Format::Bitmap, 128, highResoFlag? 64 : 32), raw(i2c, addr),
			buffWhole_(nullptr), buff_(nullptr) {
		numPages_ = height_ / heightPerPage_;
		bufferLen_ = numPages_ * width_;
	}
	~SSD1306() {
		::free(buffWhole_);
	}
public:
	uint8_t GetAddr() const { return raw.GetAddr(); }
	int GetHeightPerPage() const { return heightPerPage_; }
	int GetNumPages() const { return numPages_; }
	int GetBufferLen() const { return bufferLen_; }
public:
	void AllocBuffer() {
		buffWhole_ = reinterpret_cast<uint8_t*>(::malloc(bufferLen_ + 1));
		buffWhole_[0] = 
			(0b0 << 7) |	// Co = 0
			(0b1 << 6);		// D/C# = 1
		buff_ = buffWhole_ + 1;
		FillBuffer(0x00);
	}
	uint8_t* GetPointer() { return buff_; }
	uint8_t* GetPointer(int x) { return buff_ + x; }
	uint8_t* GetPointer(int x, int y) { return buff_ + (y / 8) * width_ + x; }
	uint8_t* GetPointer(int x, int y, int* pPage) { *pPage = y / 8; return buff_ + *pPage * width_ + x; }
	bool EnsureSafePointer(const uint8_t* p) { return buff_ <= p && p < buff_ + bufferLen_; }
	void FillBuffer(uint8_t data) { ::memset(buff_, data, bufferLen_); }
	void SendBuffer() const { raw.SendBuffer(buffWhole_, bufferLen_ + 1); }
public:
	void Initialize();
	void Flash(bool flashFlag) { raw.EntireDisplayOn(static_cast<uint8_t>(flashFlag)); }
private:
	template<class Logic> void DrawRectFillT(int x, int y, int width, int height, uint8_t* pDst, int page, uint64_t bits);
	template<class Logic> void DrawBitmapT(int x, int y, const void* data, int width, int height, int scaleX, int scaleY);
public:
	virtual void Refresh_() override;
	virtual void Fill_(const Color& color) override;
	virtual void DrawPixel_(int x, int y, const Color& color) override;
	virtual void DrawRectFill_(int x, int y, int width, int height, const Color& color) override;
	virtual void DrawBitmap_(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX = 1, int scaleY = 1) override;
	virtual void DrawImage_(int x, int y, const Image& image, const Rect* pRectClip, ImageDir imageDir) override;
};

}

#endif
