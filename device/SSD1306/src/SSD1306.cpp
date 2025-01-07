//==============================================================================
// 128x64 Dot Matrix OLED/PLED Segment/Common Driver with Controller
// Specification: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf
//==============================================================================
#include <stdio.h>
#include "jxglib/SSD1306.h"

namespace jxglib {

//------------------------------------------------------------------------------
// SSD1306
//------------------------------------------------------------------------------
void SSD1306::DispatcherEx::Initialize()
{
	AllocBuffer();
	raw.SetDisplayOnOff(0);							// Set Display ON/OFF = 0: OFF
	raw.SetMemoryAddressingMode(0);					// Set Memory Addressing Mode = 0: Horizontal Addressing Mode
	raw.SetDisplayStartLine(0);						// Set Display Start Line = 0
	raw.SetSegmentRemap(1);							// Set Segment Re-map = 1: column address 127 is mapped to SEG0
	raw.SetMultiplexRatio(display_.GetHeight() - 1);// Set Multiplex Ratio = GetHeight() - 1
	raw.SetCOMOutputScanDirection(1);				// Set COM Output Scan Direction = 1: remapped mode. Scan from COM[N-1] to COM0
	raw.SetDisplayOffset(0);						// Set Display Offset = 0
	raw.SetCOMPinsHardwareConfiguration((display_.GetHeight() == 64)? 1 : 0, 0);
													// Set COM Pins Hardware Configuration
													//   A[4] = 0: Sequential COM pin configuration
													//          1: Alternative COM pin configuration
													//   A[5] = 0: Disable COM Left/Right remap
	raw.SetDisplayClockDivideRatioOscillatorFrequency(0, 8);
													// Set Display Clock Divide Ratio/Oscillator Frequency
													//   A[3:0] = 0: divide ratio is 1
													//   A[7:4] = 8: oscilator frequency
	raw.SetPrechargePeriod(1, 15);					// Set Pre-charge Period
													//   A[3:0] = 1: Phase 1 period to 1
													//   A[7:4] = 15: Phase 2 period to 15
	raw.SetVcomhDeselectLevel(0x3);					// Set Vcomh Deselect Level = 3: up to 0.83 x Vcc
	raw.SetContrastControl(255);					// Set Contrast Conrol = 255
	raw.EntireDisplayOn(0);							// Entire Display ON = 0: Resume to RAM content display
	raw.SetNormalInverseDisplay(0);					// Set Normal/Inverse Display = 0: Normal display
	raw.ChargePumpSetting(1);						// Charge Pump Settig = 1: Enable charge pump during display on
	raw.DeactivateScroll();							// Deactivate Scroll
	raw.SetDisplayOnOff(1);							// Set Display ON/OFF = 1: ON
}

void SSD1306::DispatcherEx::Refresh()
{
	raw.SetColumnAddress(0, display_.GetWidth() - 1);
	raw.SetPageAddress(0, GetNumPages() - 1);
	SendBuffer();
}

void SSD1306::DispatcherEx::Fill(const Color& color)
{
	FillBuffer(color.IsBlack()? 0x00 : 0xff);
}

void SSD1306::DispatcherEx::DrawPixel(int x, int y, const Color& color)
{
	uint8_t* pDst = GetPointer(x, y);
	int bits = 1 << (y & 0b111);
	*pDst = color.IsBlack()? Logic_Clear()(*pDst, bits) : Logic_Set()(*pDst, bits);
}

void SSD1306::DispatcherEx::DrawRectFill(int x, int y, int width, int height, const Color& color)
{
	if (!AdjustRange(&x, &width, 0, display_.GetWidth())) return;
	if (!AdjustRange(&y, &height, 0, display_.GetHeight())) return;
	uint64_t bits = (-1LL << y) & ~(-1LL << (y + height));
	int page;
	uint8_t* pDst = GetPointer(x, y, &page);
	bits >>= page * 8;
	uint8_t* pDstTop = pDst;
	bool putFlag = !color.IsBlack();
	if (putFlag) {
		for ( ; page < GetNumPages() && bits; page++, pDstTop += display_.GetWidth(), bits >>= 8) {
			uint8_t* pDst = pDstTop;
			for (int i = 0; i < width; i++, pDst++) {
				assert(EnsureSafePointer(pDst));
				*pDst |= static_cast<uint8_t>(bits & 0b11111111);
			}
		}
	} else {
		for ( ; page < GetNumPages() && bits; page++, pDstTop += display_.GetWidth(), bits >>= 8) {
			uint8_t* pDst = pDstTop;
			for (int i = 0; i < width; i++, pDst++) {
				assert(EnsureSafePointer(pDst));
				*pDst &= ~static_cast<uint8_t>(bits & 0b11111111);
			}
		}
	}
}

void SSD1306::DispatcherEx::DrawBitmap(int x, int y, const void* data, int width, int height,
			const Color& color, const Color* pColorBg, int scaleX, int scaleY)
{
	if (pColorBg) DrawRectFill(x, y, width, height, *pColorBg);
	int bytesPerLine = (width + 7) / 8;
	int wdScaled = width * scaleX;
	int htScaled = height * scaleY;
	if (!AdjustRange(&x, &wdScaled, 0, display_.GetWidth())) return;
	if (!AdjustRange(&y, &htScaled, 0, display_.GetHeight())) return;
	const uint8_t* pSrcBottom = reinterpret_cast<const uint8_t*>(data) + bytesPerLine * height;
	int pageTop;
	uint8_t* pDstTop = GetPointer(x, y, &pageTop);
	int bitOffset = y - pageTop * 8;
	int xCur = x;
	uint32_t bitsDot = (1 << scaleY) - 1;
	bool putFlag = !color.IsBlack();
	for (int i = 0; i < bytesPerLine; i++, pSrcBottom++) {
		for (uint8_t bitMask = 0x80; bitMask; bitMask >>= 1, pDstTop += scaleX, xCur += scaleX) {
			uint64_t bits = 0;
			const uint8_t* pSrc = pSrcBottom;
			for (int j = 0; j < height; j++) {
				pSrc -= bytesPerLine;
				bits = (bits << scaleY) | ((*pSrc & bitMask)? bitsDot : 0);
			}
			bits <<= bitOffset;
			uint8_t* pDstLeft = pDstTop;
			if (putFlag) {
				for (int page = pageTop; page < GetNumPages() && bits; page++, pDstLeft += display_.GetWidth(), bits >>= 8) {
					uint8_t* pDst = pDstLeft;
					uint8_t data = static_cast<uint8_t>(bits & 0b11111111);
					for (int j = 0; j < scaleX && xCur + j < display_.GetWidth(); j++, pDst++) {
						assert(EnsureSafePointer(pDst));
						*pDst |= data;
					}
				}
			} else {
				for (int page = pageTop; page < GetNumPages() && bits; page++, pDstLeft += display_.GetWidth(), bits >>= 8) {
					uint8_t* pDst = pDstLeft;
					uint8_t data = static_cast<uint8_t>(bits & 0b11111111);
					for (int j = 0; j < scaleX && xCur + j < display_.GetWidth(); j++, pDst++) {
						assert(EnsureSafePointer(pDst));
						*pDst &= ~data;
					}
				}
			}
		}
	}
}

void SSD1306::DispatcherEx::DrawImage(int x, int y, const Image& image, const Rect& rectClip, DrawDir drawDir)
{
	// do nothing
}

void SSD1306::DispatcherEx::DrawImageFast(int x, int y, const Image& image)
{
	// do nothing
}

void SSD1306::DispatcherEx::ScrollHorz(DirHorz dirHorz, int wdScroll, const Rect& rectClip)
{
	Rect rect = rectClip.IsEmpty()? Rect(0, 0, display_.GetWidth(), display_.GetHeight()) : rectClip;
	if (rect.width <= wdScroll) return;
	int bytesPerPage = display_.GetWidth();
	uint8_t* pSrcTop;
	uint8_t* pDstTop;
	int advance;
	if (dirHorz == DirHorz::Left) {
		pDstTop = buff_ + rect.x;
		pSrcTop = buff_ + rect.x + wdScroll;
		advance = 1;
	} else if (dirHorz == DirHorz::Right) {
		pDstTop = buff_ + rect.x + rect.width - 1;
		pSrcTop = buff_ + rect.x + rect.width - 1 - wdScroll;
		advance = -1;
	} else {
		return;
	}
	for (int nPixels = rect.width - 1; nPixels > 0; nPixels--, pDstTop += advance, pSrcTop += advance) {
		uint8_t* pDst = pDstTop;
		uint8_t* pSrc = pSrcTop;
		for (int iPage = 0; iPage < numPages_; iPage++) {
			*pDst = *pSrc;
			pDst += bytesPerPage;
			pSrc += bytesPerPage;
		}
	}
}

void SSD1306::DispatcherEx::ScrollVert(DirVert dirVert, int htScroll, const Rect& rectClip)
{
	Rect rect = rectClip.IsEmpty()? Rect(0, 0, display_.GetWidth(), display_.GetHeight()) : rectClip;
	if (rect.height <= htScroll) return;
	int bytesPerPage = display_.GetWidth();
	uint64_t bits = 0;
	uint64_t bitsMask = (-1LL << rect.y) & ~(-1LL << (rect.y + rect.height));
	uint64_t bitsMaskNot = ~bitsMask;
	uint8_t* pBottom = buff_ + numPages_ * bytesPerPage + rect.x;
	for (int i = 0; i < rect.width; i++, pBottom++) {
		uint8_t* p = pBottom;
		for (int iPage = 0; iPage < numPages_; iPage++) {
			p -= bytesPerPage;
			bits = (bits << 8) | *p;
		}
		if (dirVert == DirVert::Up) {
			bits = (bits & bitsMaskNot) | ((bits >> htScroll) & bitsMask);
		} else if (dirVert == DirVert::Down) {
			bits = (bits & bitsMaskNot) | ((bits << htScroll) & bitsMask);
		}
		for (int iPage = 0; iPage < numPages_; iPage++) {
			*p = static_cast<uint8_t>(bits & 0xff);
			p += bytesPerPage;
			bits >>= 8;
		}
	}
}

}
