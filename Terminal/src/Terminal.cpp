//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
bool Terminal::AttachOutput(Drawable& drawable, const Rect& rect, AttachDir attachDir)
{
	rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : rect;
	if (drawable.CanScrollVert()) {
		pDrawable_ = &drawable;
	} else {
		if (!canvas_.AttachOutput(drawable, rect, attachDir)) return false;
		pDrawable_ = &canvas_;
	}
	return true;
}

int Terminal::GetColNum() const
{
	const Drawable& drawable = GetDrawable();
	const FontEntry& fontEntry = drawable.GetFont().GetFontEntry('M');
	return drawable.GetWidth() / fontEntry.xAdvance;
}

int Terminal::GetRowNum() const
{
	const Drawable& drawable = GetDrawable();
	const FontSet& fontSet = drawable.GetFont();
	return drawable.GetHeight() / fontSet.yAdvance;
}

Printable& Terminal::ClearScreen()
{
	GetDrawable().Clear();
	return *this;
}

Printable& Terminal::FlushScreen()
{
	GetDrawable().Refresh();
	return *this;
}

Printable& Terminal::Locate(int col, int row)
{
	Drawable& drawable = GetDrawable();
	const FontEntry& fontEntry = drawable.GetFont().GetFontEntry('M');
	ptCursor_ = Point(drawable.CalcAdvanceX(fontEntry) * col, drawable.CalcAdvanceY() * row);
	return *this;
}

Printable& Terminal::PutChar(char ch)
{
	Drawable& drawable = GetDrawable();
	const Rect& rectDst = GetRectDst();
	int yAdvance = drawable.CalcAdvanceY();
	const FontSet& fontSet = drawable.GetFont();
	uint32_t code;
	if (!decoder_.FeedChar(ch, &code)) return *this;
	const FontEntry& fontEntry = fontSet.GetFontEntry(code);
	int xAdvance = drawable.CalcAdvanceX(fontEntry);
	if (code == '\n') {
		drawable.Refresh();
		ptCursor_.x = 0;
		if (ptCursor_.y + yAdvance * 2 <= rectDst.height) {
			ptCursor_.y += yAdvance;
		} else {
			drawable.ScrollVert(DirVert::Up, yAdvance);
		}
	} else if (code == '\r') {
		drawable.Refresh();
		ptCursor_.x = 0;
		drawable.DrawRectFill(0, ptCursor_.y, rectDst.width, yAdvance, drawable.GetColorBg());
	} else {
		if (ptCursor_.x + xAdvance > rectDst.width) {
			ptCursor_.x = 0;
			if (ptCursor_.y + yAdvance * 2 <= rectDst.height) {
				ptCursor_.y += yAdvance;
			} else {
				drawable.ScrollVert(DirVert::Up, yAdvance);
			}
		}
		drawable.DrawChar(ptCursor_, fontEntry);
		ptCursor_.x += xAdvance;
	}
	return *this;
}

}
