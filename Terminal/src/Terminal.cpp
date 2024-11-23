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

Printable& Terminal::Clear()
{
	GetDrawable().Clear();
	return *this;
}

Printable& Terminal::Flush()
{
	GetDrawable().Refresh();
	return *this;
}

Printable& Terminal::Locate(int col, int row)
{
	Drawable& drawable = GetDrawable();
	const FontSet& fontSet = drawable.GetFont();
	const FontEntry& fontEntry = fontSet.GetFontEntry('M');
	ptCursor_ = Point(drawable.CalcAdvanceX(fontEntry) * col, drawable.CalcAdvanceY() * row);
	return *this;
}

Printable& Terminal::Puts(const char* str)
{
	Drawable& drawable = GetDrawable();
	const FontSet& fontSet = drawable.GetFont();
	uint32_t code;
	UTF8Decoder decoder;
	const char* p = str;
	int yAdvance = drawable.CalcAdvanceY();
	for ( ; *p; p++) {
		if (!decoder.FeedChar(*p, &code)) continue;
		const FontEntry& fontEntry = fontSet.GetFontEntry(code);
		int xAdvance = drawable.CalcAdvanceX(fontEntry);
		if (code == '\n') {
			drawable.Refresh();
			ptCursor_.x = 0;
			if (ptCursor_.y + yAdvance * 2 <= drawable.GetHeight()) {
				ptCursor_.y += yAdvance;
			} else {
				drawable.ScrollVert(DirVert::Up, yAdvance);
			}
		} else if (code == '\r') {
			drawable.Refresh();
			ptCursor_.x = 0;
			drawable.DrawRectFill(0, ptCursor_.y, drawable.GetWidth(), yAdvance, drawable.GetColorBg());
		} else {
			if (ptCursor_.x + xAdvance > drawable.GetWidth()) {
				ptCursor_.x = 0;
				if (ptCursor_.y + yAdvance * 2 <= drawable.GetHeight()) {
					ptCursor_.y += yAdvance;
				} else {
					drawable.ScrollVert(DirVert::Up, yAdvance);
				}
			}
			drawable.DrawChar(ptCursor_, fontEntry);
			ptCursor_.x += xAdvance;
		}
	}
	return *this;
}

}
