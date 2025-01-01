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
#if 0
	if (drawable.CanScrollVert()) {
		rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : rect;
		pDrawable_ = &drawable;
	} else {
		rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : Rect(0, 0, rect.width, rect.height);
		if (!canvas_.AttachOutput(drawable, rect, attachDir)) return false;
		pDrawable_ = &canvas_;
	}
#endif
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
	if (decoder_.FeedChar(ch, &code)) {
		const FontEntry& fontEntry = fontSet.GetFontEntry(code);
		int xAdvance = drawable.CalcAdvanceX(fontEntry);
		if (code == '\n') {
			drawable.Refresh();
			lineBuff_.PutChar('\0').MarkLineCur().PlaceChar('\0');
			ptCursor_.x = rectDst_.x;
			if (ptCursor_.y + yAdvance * 2 <= rectDst.height) {
				ptCursor_.y += yAdvance;
			} else {
				ScrollVert(DirVert::Up);
			}
		} else if (code == '\r') {
			drawable.Refresh();
			lineBuff_.PutChar('\0').MarkLineCur().PlaceChar('\0');
			ptCursor_.x = rectDst_.x;
			drawable.DrawRectFill(0, ptCursor_.y, rectDst.width, yAdvance, drawable.GetColorBg());
		} else {
			if (ptCursor_.x + xAdvance > rectDst.width) {
				lineBuff_.PutChar('\0').MarkLineCur();
				ptCursor_.x = rectDst_.x;
				if (ptCursor_.y + yAdvance * 2 <= rectDst.height) {
					ptCursor_.y += yAdvance;
				} else {
					ScrollVert(DirVert::Up);
				}
			}
			lineBuff_.PutString(decoder_.GetStringOrg());
			drawable.DrawChar(ptCursor_, fontEntry);
			ptCursor_.x += xAdvance;
		}
	}
	return *this;
}

void Terminal::DrawStrings(int x, int y, const char* lineTop, int nLines)
{
	::printf("DrawStrings %dlines\n", nLines);
	Drawable& drawable = GetDrawable();
	int yAdvance = drawable.CalcAdvanceY();
	for (int iLine = 0; iLine < nLines; iLine++) {
		CharFeederWrapped charFeeder(lineBuff_.MakeCharFeeder(lineTop));
		::printf("%2d %04x\n", iLine, charFeeder.GetOffset());
		DrawString(x, y, charFeeder);
		lineTop = lineBuff_.NextLine(lineTop);
		y += yAdvance;
	}
}

void Terminal::DrawString(int x, int y, CharFeederWrapped& charFeeder)
{
	Drawable& drawable = GetDrawable();
	const FontSet& fontSet = drawable.GetFont();
	uint32_t code;
	for (;;) {
		char ch = charFeeder.Get();
		if (!ch) break;
		charFeeder.Forward();
		if (decoder_.FeedChar(ch, &code)) {
			const FontEntry& fontEntry = fontSet.GetFontEntry(code);
			int xAdvance = drawable.CalcAdvanceX(fontEntry);
			drawable.DrawChar(x, y, fontEntry);
			x += xAdvance;
		}
	}
}

void Terminal::ScrollVert(DirVert dirVert)
{
	int nLines = GetRowNum() - 3;
	const char* lineTop = lineBuff_.PrevLine(lineBuff_.GetLineCur(), nLines);
	//GetDrawable().Clear();
	DrawStrings(rectDst_.x, rectDst_.y, lineTop, nLines);
	//EraseLine(nLines - 1);
	//Drawable& drawable = GetDrawable();
	//int yAdvance = drawable.CalcAdvanceY();
	//drawable.ScrollVert(dirVert, yAdvance, rectDst_);
}

void Terminal::EraseLine(int iLine, int nLines)
{
	Drawable& drawable = GetDrawable();
	int yAdvance = drawable.CalcAdvanceY();
	drawable.DrawRectFill(rectDst_.x, rectDst_.y + yAdvance * iLine, rectDst_.width, yAdvance * nLines, drawable.GetColorBg());
}

}
