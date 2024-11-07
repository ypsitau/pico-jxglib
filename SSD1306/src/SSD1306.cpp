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
	raw.AllocBuffer();
	raw.SetDisplayOnOff(0);							// Set Display ON/OFF = 0: OFF
	raw.SetMemoryAddressingMode(0);					// Set Memory Addressing Mode = 0: Horizontal Addressing Mode
	raw.SetDisplayStartLine(0);						// Set Display Start Line = 0
	raw.SetSegmentRemap(1);							// Set Segment Re-map = 1: column address 127 is mapped to SEG0
	raw.SetMultiplexRatio(GetHeight() - 1);	// Set Multiplex Ratio = GetHeight() - 1
	raw.SetCOMOutputScanDirection(1);				// Set COM Output Scan Direction = 1: remapped mode. Scan from COM[N-1] to COM0
	raw.SetDisplayOffset(0);						// Set Display Offset = 0
	raw.SetCOMPinsHardwareConfiguration((GetHeight() == 64)? 1 : 0, 0);
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
	raw.SetColumnAddress(0, GetWidth() - 1);
	raw.SetPageAddress(0, GetNumPages() - 1);
	raw.WriteBuffer();
}

template<class Logic> void SSD1306::DrawHLineT_NoAdj(int x, int y, int width)
{
	uint8_t data = 0b00000001 << (y & 0b111);
	uint8_t* pDst = raw.GetPointer(x, y);
	for (int i = 0; i < width; i++, pDst++) {
		assert(raw.EnsureSafePointer(pDst));
		*pDst = Logic()(*pDst, data);
	}
}

template<class Logic> void SSD1306::DrawVLineT_NoAdj(int x, int y, int height)
{
	uint64_t bits = (-1LL << y) & ~(-1LL << (y + height));
	int page;
	uint8_t* pDstTop = raw.GetPointer(x, y, &page);
	bits >>= page * 8;
	for (uint8_t* pDst = pDstTop; page < GetNumPages() && bits; page++, pDst += GetWidth(), bits >>= 8) {
		assert(raw.EnsureSafePointer(pDst));
		*pDst = Logic()(*pDst, static_cast<uint8_t>(bits & 0b11111111));
	}
}

template<class Logic> void SSD1306::DrawHLineT(int x, int y, int width)
{
	if (!CheckCoord(y, GetHeight()) || !AdjustCoord(&x, &width, GetWidth())) return;
	DrawHLineT_NoAdj<Logic>(x, y, width);
}

template<class Logic> void SSD1306::DrawVLineT(int x, int y, int height)
{
	if (!CheckCoord(x, GetWidth()) || !AdjustCoord(&y, &height, GetHeight())) return;
	DrawVLineT_NoAdj<Logic>(x, y, height);
}

template<class Logic> void SSD1306::DrawLineT(int x0, int y0, int x1, int y1, int dx, int dy, int sx, int sy)
{
	int err = dx + dy;
	int x = x0, y = y0;
	for (;;) {
		DrawPixelT<Logic>(x, y);
		if (x == x1 && y == y1) break;
		int err2 = 2 * err;
		if (err2 >= dy) {
			err += dy;
			x += sx;
		}
		if (err2 <= dx) {
			err += dx;
			y += sy;
		}
	}
}

template<class Logic> void SSD1306::DrawRectFillT(int x, int y, int width, int height)
{
	if (!AdjustCoord(&x, &width, GetWidth()) || !AdjustCoord(&y, &height, GetHeight())) return;
	uint64_t bits = (-1LL << y) & ~(-1LL << (y + height));
	int page;
	uint8_t* pDstTop = raw.GetPointer(x, y, &page);
	bits >>= page * 8;
	for ( ; page < GetNumPages() && bits; page++, pDstTop += GetWidth(), bits >>= 8) {
		uint8_t* pDst = pDstTop;
		for (int i = 0; i < width; i++, pDst++) {
			assert(raw.EnsureSafePointer(pDst));
			*pDst = Logic()(*pDst, static_cast<uint8_t>(bits & 0b11111111));
		}
	}
}

template<class Logic> void SSD1306::DrawBitmapT(int x, int y, const void* src, int width, int height, int scaleX, int scaleY)
{
	int bytesPerLine = (width + 7) / 8;
	int wdScaled = width * scaleX;
	int htScaled = height * scaleY;
	if (!AdjustCoord(&x, &wdScaled, GetWidth()) || !AdjustCoord(&y, &htScaled, GetHeight())) return;
	const uint8_t* pSrcBottom = reinterpret_cast<const uint8_t*>(src) + bytesPerLine * height;
	int pageTop;
	uint8_t* pDstTop = raw.GetPointer(x, y, &pageTop);
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
			for (int page = pageTop; page < GetNumPages() && bits; page++, pDstLeft += GetWidth(), bits >>= 8) {
				uint8_t* pDst = pDstLeft;
				uint8_t data = static_cast<uint8_t>(bits & 0b11111111);
				for (int j = 0; j < scaleX && xCur + j < GetWidth(); j++, pDst++) {
					assert(raw.EnsureSafePointer(pDst));
					*pDst = Logic()(*pDst, data);
				}
			}
		}
	}
}

template<class Logic> void SSD1306::DrawCharT(int x, int y, const FontEntry& fontEntry)
{
	DrawBitmapT<Logic>(x, y, fontEntry.data, fontEntry.width, fontEntry.height, context_.fontScaleX, context_.fontScaleY);
}

void SSD1306::DrawPixel(int x, int y)
{
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawPixelT<Logic_Set>(x, y); break;
	case DrawMode::Clear:	DrawPixelT<Logic_Clear>(x, y); break;
	case DrawMode::Invert:	DrawPixelT<Logic_Invert>(x, y); break;
	default: break;
	}
}

void SSD1306::DrawHLine_NoAdj(int x, int y, int width)
{
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawHLineT_NoAdj<Logic_Set>(x, y, width); break;
	case DrawMode::Clear:	DrawHLineT_NoAdj<Logic_Clear>(x, y, width); break;
	case DrawMode::Invert:	DrawHLineT_NoAdj<Logic_Invert>(x, y, width); break;
	default: break;
	}
}

void SSD1306::DrawVLine_NoAdj(int x, int y, int width)
{
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawVLineT_NoAdj<Logic_Set>(x, y, width); break;
	case DrawMode::Clear:	DrawVLineT_NoAdj<Logic_Clear>(x, y, width); break;
	case DrawMode::Invert:	DrawVLineT_NoAdj<Logic_Invert>(x, y, width); break;
	default: break;
	}
}

void SSD1306::DrawHLine(int x, int y, int width)
{
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawHLineT<Logic_Set>(x, y, width); break;
	case DrawMode::Clear:	DrawHLineT<Logic_Clear>(x, y, width); break;
	case DrawMode::Invert:	DrawHLineT<Logic_Invert>(x, y, width); break;
	default: break;
	}
}

void SSD1306::DrawVLine(int x, int y, int width)
{
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawVLineT<Logic_Set>(x, y, width); break;
	case DrawMode::Clear:	DrawVLineT<Logic_Clear>(x, y, width); break;
	case DrawMode::Invert:	DrawVLineT<Logic_Invert>(x, y, width); break;
	default: break;
	}
}

void SSD1306::DrawLine(int x0, int y0, int x1, int y1)
{
	if (x0 == x1) {
		DrawVLine(x0, y0, y1 - y0);
		return;
	} else if (y0 == y1) {
		DrawHLine(x0, y0, x1 - x0);
		return;
	}
	int dx, dy, sx, sy;
	if (x0 < x1) {
		dx = x1 - x0;
		sx = +1;
	} else {
		dx = x0 - x1;
		sx = -1;
	}
	if (y0 < y1) {
		dy = y0 - y1;
		sy = +1;
	} else {
		dy = y1 - y0;
		sy = -1;
	}
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawLineT<Logic_Set>(x0, y0, x1, y1, dx, dy, sx, sy); break;
	case DrawMode::Clear:	DrawLineT<Logic_Clear>(x0, y0, x1, y1, dx, dy, sx, sy); break;
	case DrawMode::Invert:	DrawLineT<Logic_Invert>(x0, y0, x1, y1, dx, dy, sx, sy); break;
	default: break;
	}
}

void SSD1306::DrawRect(int x, int y, int width, int height)
{
	if (width < 0) {
		x += width + 1;
		width = -width;
	}
	if (height < 0) {
		y += height + 1;
		height = -height;
	}
	int xLeft = x, xRight = x + width - 1;
	int yTop = y, yBottom = y + height - 1;
	int xLeftAdjust = xLeft, widthAdjust = width;
	int yTopAdjust = yTop, heightAdjust = height;
	if (AdjustCoord(&xLeftAdjust, &widthAdjust, GetWidth())) {
		if (CheckCoord(yTop, GetHeight())) {
			DrawHLine_NoAdj(xLeftAdjust, yTop, widthAdjust);
		}
		if (CheckCoord(yBottom, GetHeight())) {
			DrawHLine_NoAdj(xLeftAdjust, yBottom, widthAdjust);
		}
	}
	if (AdjustCoord(&yTopAdjust, &heightAdjust, GetHeight())) {
		if (CheckCoord(xLeft, GetWidth())) {
			DrawVLine_NoAdj(xLeft, yTopAdjust, heightAdjust);
		}
		if (CheckCoord(xRight, GetWidth())) {
			DrawVLine_NoAdj(xRight, yTopAdjust, heightAdjust);
		}
	}
}

void SSD1306::DrawRectFill(int x, int y, int width, int height)
{
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawRectFillT<Logic_Set>(x, y, width, height); break;
	case DrawMode::Clear:	DrawRectFillT<Logic_Clear>(x, y, width, height); break;
	case DrawMode::Invert:	DrawRectFillT<Logic_Invert>(x, y, width, height); break;
	default: break;
	}
}

void SSD1306::DrawBitmap(int x, int y, const void* src, int width, int height, int scaleX, int scaleY)
{
	DrawRectFillT<Logic_Clear>(x, y, width, height);
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawBitmapT<Logic_Set>(x, y, src, width, height, scaleX, scaleY); break;
	case DrawMode::Clear:	DrawBitmapT<Logic_Clear>(x, y, src, width, height, scaleX, scaleY); break;
	case DrawMode::Invert:	DrawBitmapT<Logic_Invert>(x, y, src, width, height, scaleX, scaleY); break;
	default: break;
	}
}

void SSD1306::DrawChar(int x, int y, const FontEntry& fontEntry)
{
	switch (context_.drawMode) {
	case DrawMode::Set:		DrawCharT<Logic_Set>(x, y, fontEntry); break;
	case DrawMode::Clear:	DrawCharT<Logic_Clear>(x, y, fontEntry); break;
	case DrawMode::Invert:	DrawCharT<Logic_Invert>(x, y, fontEntry); break;
	default: break;
	}
}

void SSD1306::DrawChar(int x, int y, uint32_t code)
{
	if (!context_.pFontSet) return;
	const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
	DrawChar(x, y, fontEntry);
}

void SSD1306::DrawString(int x, int y, const char* str, const char* strEnd)
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

const char* SSD1306::DrawStringBBox(int x, int y, int width, int height, const char* str, int htLine)
{
	if (!context_.pFontSet) return str;
	uint32_t code;
	UTF8Decoder decoder;
	int xStart = x;
	int xEnd = (width >= 0)? x + width : GetWidth();
	int yEnd = (height >= 0)? y + height : GetHeight();
	int yAdvance = (htLine >= 0)? htLine : context_.pFontSet->yAdvance * context_.fontScaleY;
	const char* pDone = str;
	for (const char* p = str; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = context_.pFontSet->GetFontEntry(code);
		int xAdvance = fontEntry.xAdvance * context_.fontScaleX;
		if (x + fontEntry.width * context_.fontScaleX > xEnd) {
			x = xStart, y += yAdvance;
			if (y + yAdvance > yEnd) break;
		}
		DrawChar(x, y, fontEntry);
		x += xAdvance;
		pDone = p + 1;
	}
	return pDone;
}

bool SSD1306::AdjustCoord(int* pV, int* pDist, int vLimit)
{
	int& v = *pV, &dist = *pDist;
	if (dist == 0) return false;
	if (dist < 0) {
		v += dist + 1, dist = -dist;
	}
	if (v < 0) {
		dist += v, v = 0;
	}
	if (v >= vLimit) return false;
	if (v + dist > vLimit) {
		dist = vLimit - v;
	}
	return true;
}

}
