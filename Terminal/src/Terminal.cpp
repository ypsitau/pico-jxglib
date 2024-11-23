//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
bool Terminal::AttachOutput(Drawable& drawable, const Rect* pRect, AttachDir attachDir)
{
	if (drawable.CanScrollVert()) {
		pDrawable_ = &drawable;
	} else {
		if (!canvas_.AttachOutput(drawable, pRect, attachDir)) return false;
		pDrawable_ = &canvas_;
	}
	return true;
}

void Terminal::Clear()
{
	GetDrawable().Clear();
}

void Terminal::Flush()
{
	GetDrawable().Refresh();
}

void Terminal::Puts(const char* str)
{
	Drawable& drawable = GetDrawable();
	uint32_t code;
	UTF8Decoder decoder;
	const char* p = str;
	const FontSet& fontSet = drawable.GetFont();
	int yAdvance = drawable.CalcAdvanceY();
	for ( ; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = fontSet.GetFontEntry(code);
		int xAdvance = drawable.CalcAdvanceX(fontEntry);
		if (code == '\n') {
			drawable.Refresh();
			pt_.x = 0;
			if (pt_.y + yAdvance * 2 <= drawable.GetHeight()) {
				pt_.y += yAdvance;
			} else {
				drawable.ScrollVert(DirVert::Up, yAdvance);
			}
		} else if (code == '\r') {
			drawable.Refresh();
			pt_.x = 0;
		} else {
			if (pt_.x + xAdvance > drawable.GetWidth()) {
				pt_.x = 0;
				if (pt_.y + yAdvance * 2 <= drawable.GetHeight()) {
					pt_.y += yAdvance;
				} else {
					drawable.ScrollVert(DirVert::Up, yAdvance);
				}
			}
			drawable.DrawChar(pt_, fontEntry);
			pt_.x += xAdvance;
		}
	}
}

}
