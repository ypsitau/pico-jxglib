//==============================================================================
// LCD1602.cpp
//==============================================================================
#include <memory.h>
#include "jxglib/LCD1602.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LCD1602
//------------------------------------------------------------------------------
void LCD1602::Initialize()
{
	raw.ReturnHome(1);
	raw.ReturnHome(1);
	raw.ReturnHome(1);
	raw.ReturnHome();
	raw.EntryModeSet(1, 0);
	raw.FunctionSet(0, 1, 0);
	raw.DisplayOnOffControl(1, 0, 0);
	raw.ClearDisplay();
	::memset(vram_, ' ', sizeof(vram_));
}

Printable& LCD1602::ClearScreen()
{
	x_ = 0, y_ = 0;
	chPrev_ = '\0';
	raw.ClearDisplay();
	::memset(vram_, ' ', sizeof(vram_));
	return *this;
}

bool LCD1602::Flush()
{
	raw.SetDisplayDataRAMAddress(CalcAddr(0, 0));
	const uint8_t* p = vram_;
	for (int i = 0; i < sizeof(vram_); i++, p++) raw.WriteData(*p);
	return true;
}

Printable& LCD1602::Locate(int x, int y)
{
	x_ = x, y_ = y;
	chPrev_ = '\0';
	raw.SetDisplayDataRAMAddress(CalcAddr(x_, y_));
	return *this;
}

Printable& LCD1602::Print(const char* str)
{
	for (const char* p = str; *p; p++) {
		char ch = *p;
		if (chPrev_ == '\n' || x_ >= nCols) {
			x_ = 0, y_++;
			if (y_ >= nRows) {
				::memcpy(vram_, vram_ + nCols, sizeof(vram_) - nCols);
				::memset(vram_ + sizeof(vram_) - nCols, ' ', nCols);
				Flush();
				y_ = nRows - 1;
			}
			raw.SetDisplayDataRAMAddress(CalcAddr(x_, y_));
		//} else if (chPrev_ == '\r') {
		//	x_ = 0;
		//	uint8_t* p = vram_ + y_ * nCols;
		//	::memset(p, ' ', nCols);
		//	raw.SetDisplayDataRAMAddress(CalcAddr(x_, y_));
		//	for (int i = 0; i < nCols; i++, p++) raw.WriteData(*p);
		//	raw.SetDisplayDataRAMAddress(CalcAddr(x_, y_));
		}
		if (ch != '\n' && ch != '\r') {
			vram_[x_ + y_ * nCols] = static_cast<uint8_t>(ch);
			raw.WriteData(static_cast<uint8_t>(ch));
			x_++;
		}
		chPrev_ = ch;
	}
	return *this;
}

Printable& LCD1602::PutCharRaw(char ch)
{
	raw.WriteData(static_cast<uint8_t>(ch));
}

}
