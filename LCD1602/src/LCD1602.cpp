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
	::memset(vram_, 0x00, sizeof(vram_));
}

LCD1602& LCD1602::ReturnHome()
{
	x_ = 0, y_ = 0;
	raw.ReturnHome();
	return *this;
}

LCD1602& LCD1602::ClearDisplay()
{
	x_ = 0, y_ = 0;
	raw.ClearDisplay();
	return *this;
}

LCD1602& LCD1602::SetPosition(uint8_t x, uint8_t y)
{
	x_ = x, y_ = y;
	raw.SetDisplayDataRAMAddress(CalcAddr(x_, y_));
	return *this;
}

LCD1602& LCD1602::PutChar(char ch)
{
	if (y_ < 2) {
		vram_[x_ + y_ * 16] = static_cast<uint8_t>(ch);
		raw.WriteData(static_cast<uint8_t>(ch));
		x_++;
	}
	if (x_ >= 16) {
		x_ = 0, y_++;
		if (y_ < 2) raw.SetDisplayDataRAMAddress(CalcAddr(x_, y_));
	}
	return *this;
}

LCD1602& LCD1602::Print(const char* str, const char* strEnd)
{
	for (const char* p = str; *p && p != strEnd; p++) PutChar(*p);
	return *this;
}

LCD1602& LCD1602::FlushVRAM()
{
	const uint8_t* p = vram_;
	for (int i = 0; i < sizeof(vram_); i++, p++) raw.WriteData(*p);
	return *this;
}

}
