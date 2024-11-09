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
void SSD1306::Initialize()
{
	AllocBuffer();
	raw.SetDisplayOnOff(0);							// Set Display ON/OFF = 0: OFF
	raw.SetMemoryAddressingMode(0);					// Set Memory Addressing Mode = 0: Horizontal Addressing Mode
	raw.SetDisplayStartLine(0);						// Set Display Start Line = 0
	raw.SetSegmentRemap(1);							// Set Segment Re-map = 1: column address 127 is mapped to SEG0
	raw.SetMultiplexRatio(GetScreenHeight() - 1);	// Set Multiplex Ratio = GetScreenHeight() - 1
	raw.SetCOMOutputScanDirection(1);				// Set COM Output Scan Direction = 1: remapped mode. Scan from COM[N-1] to COM0
	raw.SetDisplayOffset(0);						// Set Display Offset = 0
	raw.SetCOMPinsHardwareConfiguration((GetScreenHeight() == 64)? 1 : 0, 0);
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

void SSD1306::Refresh()
{
	raw.SetColumnAddress(0, GetScreenWidth() - 1);
	raw.SetPageAddress(0, GetNumPages() - 1);
	WriteBuffer();
}

template<class Logic> void SSD1306::DrawHLineT(int x, int y, int width, uint8_t* pDst, uint8_t bits)
{
	for (int i = 0; i < width; i++, pDst++) {
		assert(EnsureSafePointer(pDst));
		*pDst = Logic()(*pDst, bits);
	}
}

template<class Logic> void SSD1306::DrawVLineT(int x, int y, int height, uint8_t* pDst, int page, uint64_t bits)
{
	for ( ; page < GetNumPages() && bits; page++, pDst += GetScreenWidth(), bits >>= 8) {
		assert(EnsureSafePointer(pDst));
		*pDst = Logic()(*pDst, static_cast<uint8_t>(bits & 0b11111111));
	}
}

template<class Logic> void SSD1306::DrawRectFillT(int x, int y, int width, int height, uint8_t* pDst, int page, uint64_t bits)
{
	uint8_t* pDstTop = pDst;
	for ( ; page < GetNumPages() && bits; page++, pDstTop += GetScreenWidth(), bits >>= 8) {
		uint8_t* pDst = pDstTop;
		for (int i = 0; i < width; i++, pDst++) {
			assert(EnsureSafePointer(pDst));
			*pDst = Logic()(*pDst, static_cast<uint8_t>(bits & 0b11111111));
		}
	}
}

template<class Logic> void SSD1306::DrawBitmapT(int x, int y, const void* data, int width, int height, int scaleX, int scaleY)
{
	int bytesPerLine = (width + 7) / 8;
	int wdScaled = width * scaleX;
	int htScaled = height * scaleY;
	if (!AdjustRange(&x, &wdScaled, 0, GetScreenWidth())) return;
	if (!AdjustRange(&y, &htScaled, 0, GetScreenHeight())) return;
	const uint8_t* pSrcBottom = reinterpret_cast<const uint8_t*>(data) + bytesPerLine * height;
	int pageTop;
	uint8_t* pDstTop = GetPointer(x, y, &pageTop);
	int bitOffset = y - pageTop * 8;
	int xCur = x;
	uint32_t bitsDot = (1 << scaleY) - 1;
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
			for (int page = pageTop; page < GetNumPages() && bits; page++, pDstLeft += GetScreenWidth(), bits >>= 8) {
				uint8_t* pDst = pDstLeft;
				uint8_t data = static_cast<uint8_t>(bits & 0b11111111);
				for (int j = 0; j < scaleX && xCur + j < GetScreenWidth(); j++, pDst++) {
					assert(EnsureSafePointer(pDst));
					*pDst = Logic()(*pDst, data);
				}
			}
		}
	}
}

void SSD1306::DrawPixel(int x, int y)
{
	uint8_t* pDst = GetPointer(x, y);
	int bits = 1 << (y & 0b111);
	switch (drawMode_) {
	case DrawMode::Set:		*pDst = Logic_Set()(*pDst, bits); break;
	case DrawMode::Clear:	*pDst = Logic_Clear()(*pDst, bits); break;
	case DrawMode::Invert:	*pDst = Logic_Invert()(*pDst, bits); break;
	default: break;
	}
}

void SSD1306::DrawHLine(int x, int y, int width)
{
	if (!AdjustRange(&x, &width, 0, GetScreenWidth())) return;
	if (!CheckRange(y, 0, GetScreenHeight())) return;
	uint8_t* pDst = GetPointer(x, y);
	uint8_t bits = 0b00000001 << (y & 0b111);
	switch (drawMode_) {
	case DrawMode::Set:		DrawHLineT<Logic_Set>(x, y, width, pDst, bits); break;
	case DrawMode::Clear:	DrawHLineT<Logic_Clear>(x, y, width, pDst, bits); break;
	case DrawMode::Invert:	DrawHLineT<Logic_Invert>(x, y, width, pDst, bits); break;
	default: break;
	}
}

void SSD1306::DrawVLine(int x, int y, int height)
{
	if (!CheckRange(x, 0, GetScreenWidth())) return;
	if (!AdjustRange(&y, &height, 0, GetScreenHeight())) return;
	uint64_t bits = (-1LL << y) & ~(-1LL << (y + height));
	int page;
	uint8_t* pDst = GetPointer(x, y, &page);
	bits >>= page * 8;
	switch (drawMode_) {
	case DrawMode::Set:		DrawVLineT<Logic_Set>(x, y, height, pDst, page, bits); break;
	case DrawMode::Clear:	DrawVLineT<Logic_Clear>(x, y, height, pDst, page, bits); break;
	case DrawMode::Invert:	DrawVLineT<Logic_Invert>(x, y, height, pDst, page, bits); break;
	default: break;
	}
}

void SSD1306::DrawRectFill(int x, int y, int width, int height, DrawMode drawMode)
{
	if (!AdjustRange(&x, &width, 0, GetScreenWidth())) return;
	if (!AdjustRange(&y, &height, 0, GetScreenHeight())) return;
	uint64_t bits = (-1LL << y) & ~(-1LL << (y + height));
	int page;
	uint8_t* pDst = GetPointer(x, y, &page);
	bits >>= page * 8;
	switch (drawMode) {
	case DrawMode::Set:		DrawRectFillT<Logic_Set>(x, y, width, height, pDst, page, bits); break;
	case DrawMode::Clear:	DrawRectFillT<Logic_Clear>(x, y, width, height, pDst, page, bits); break;
	case DrawMode::Invert:	DrawRectFillT<Logic_Invert>(x, y, width, height, pDst, page, bits); break;
	default: break;
	}
}

void SSD1306::DrawBitmap(int x, int y, const void* data, int width, int height, int scaleX, int scaleY)
{
	DrawRectFill(x, y, width, height, DrawMode::Clear);
	switch (drawMode_) {
	case DrawMode::Set:		DrawBitmapT<Logic_Set>(x, y, data, width, height, scaleX, scaleY); break;
	case DrawMode::Clear:	DrawBitmapT<Logic_Clear>(x, y, data, width, height, scaleX, scaleY); break;
	case DrawMode::Invert:	DrawBitmapT<Logic_Invert>(x, y, data, width, height, scaleX, scaleY); break;
	default: break;
	}
}

}
