//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
bool Terminal::Initialize(int bytes)
{
	return lineBuff_.Allocate(bytes);
}

bool Terminal::AttachOutput(Drawable& drawable, const Rect& rect, AttachDir attachDir)
{
	rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : rect;
	pDrawable_ = &drawable;
	return true;
}

int Terminal::GetColNum() const
{
	const Drawable& drawable = GetDrawable();
	const FontEntry& fontEntry = drawable.GetFont().GetFontEntry('M');
	return rectDst_.width / fontEntry.xAdvance;
}

int Terminal::GetRowNum() const
{
	const Drawable& drawable = GetDrawable();
	const FontSet& fontSet = drawable.GetFont();
	return rectDst_.height / fontSet.yAdvance;
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
	if (decoder_.FeedChar(ch, &code)) {
		const FontEntry& fontEntry = fontSet.GetFontEntry(code);
		int xAdvance = drawable.CalcAdvanceX(fontEntry);
		if (code == '\n') {
			drawable.Refresh();
			lineBuff_.PutChar('\n').PutChar('\0').MarkLineLast().PlaceChar('\0');
			ptCursor_.x = rectDst_.x;
			if (ptCursor_.y + yAdvance * 2 <= rectDst.height) {
				ptCursor_.y += yAdvance;
			} else {
				ScrollVert(DirVert::Up);
			}
		} else if (code == '\r') {
			drawable.Refresh();
			lineBuff_.PutChar('\r').PutChar('\0').MarkLineLast().PlaceChar('\0');
			ptCursor_.x = rectDst_.x;
			drawable.DrawRectFill(rectDst.x, ptCursor_.y, rectDst.width, yAdvance, drawable.GetColorBg());
		} else {
			if (ptCursor_.x + xAdvance > rectDst.width) {
				lineBuff_.PutChar('\0').MarkLineLast().PlaceChar('\0');
				ptCursor_.x = rectDst_.x;
				if (ptCursor_.y + yAdvance * 2 <= rectDst.height) {
					ptCursor_.y += yAdvance;
				} else {
					ScrollVert(DirVert::Up);
				}
			}
			lineBuff_.PutString(decoder_.GetStringOrg()).PlaceChar('\0');
			drawable.DrawChar(ptCursor_, fontEntry);
			ptCursor_.x += xAdvance;
		}
	}
	return *this;
}

void Terminal::DrawTextLines(const char* lineTop, int nLines, int yTop)
{
	Drawable& drawable = GetDrawable();
	int yAdvance = drawable.CalcAdvanceY();
	int y = yTop;
	for (int iLine = 0; iLine < nLines; iLine++) {
		WrappedCharFeeder charFeeder(lineBuff_.CreateCharFeeder(lineTop));
		DrawTextLine(charFeeder, y);
		if (!lineBuff_.NextLine(&lineTop)) break;
		y += yAdvance;
	}
}

void Terminal::DrawTextLine(WrappedCharFeeder& charFeeder, int y)
{
	Drawable& drawable = GetDrawable();
	const FontSet& fontSet = drawable.GetFont();
	int x = rectDst_.x;
	uint32_t code;
	for (;;) {
		char ch = charFeeder.Get();
		if (!ch || ch == '\n' || ch == '\r') break;
		charFeeder.Forward();
		if (decoder_.FeedChar(ch, &code)) {
			const FontEntry& fontEntry = fontSet.GetFontEntry(code);
			int xAdvance = drawable.CalcAdvanceX(fontEntry);
			drawable.DrawChar(x, y, fontEntry);
			x += xAdvance;
		}
	}
	if (x < rectDst_.x + rectDst_.width) {
		int yAdvance = drawable.CalcAdvanceY();
		drawable.DrawRectFill(x, y, rectDst_.x + rectDst_.width - x, yAdvance, drawable.GetColorBg());
	}
}

void Terminal::EraseTextLine(int iLine, int nLines)
{
	Drawable& drawable = GetDrawable();
	int yAdvance = drawable.CalcAdvanceY();
	drawable.DrawRectFill(rectDst_.x, rectDst_.y + yAdvance * iLine, rectDst_.width, yAdvance * nLines, drawable.GetColorBg());
}

void Terminal::ScrollVert(DirVert dirVert)
{
	int nLines = GetRowNum();
	const char* lineTop = lineBuff_.GetLineLast();
	lineBuff_.PrevLine(&lineTop, nLines);
	DrawTextLines(lineTop, nLines - 1, rectDst_.y);
	EraseTextLine(nLines - 1);
}

}
