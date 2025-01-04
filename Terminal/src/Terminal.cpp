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

bool Terminal::AttachOutput(Drawable& drawable, const Rect& rect, Dir dir)
{
	rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : rect;
	ptCursor_ = Point(rectDst_.x, rectDst_.y);
	pDrawable_ = &drawable;
	return true;
}

int Terminal::GetColNum() const
{
	const FontEntry& fontEntry = GetFont().GetFontEntry('M');
	return rectDst_.width / fontEntry.xAdvance;
}

int Terminal::GetRowNum() const
{
	return rectDst_.height / GetFont().yAdvance;
}

Printable& Terminal::ClearScreen()
{
	GetDrawable().DrawRectFill(rectDst_, GetColorBg());
	return *this;
}

Printable& Terminal::RefreshScreen()
{
	GetDrawable().Refresh();
	return *this;
}

Printable& Terminal::Locate(int col, int row)
{
	const FontEntry& fontEntry = GetFont().GetFontEntry('M');
	ptCursor_ = Point(
			rectDst_.x + context_.CalcAdvanceX(fontEntry) * col,
			rectDst_.y + context_.CalcAdvanceY() * row);
	return *this;
}

Printable& Terminal::PutChar(char ch)
{
	int yAdvance = context_.CalcAdvanceY();
	const FontSet& fontSet = GetFont();
	uint32_t code;
	if (decoder_.FeedChar(ch, &code)) {
		const FontEntry& fontEntry = fontSet.GetFontEntry(code);
		int xAdvance = context_.CalcAdvanceX(fontEntry);
		if (code == '\n') {
			lineBuff_.PutChar('\n').PutChar('\0').MoveLineLastHere().PlaceChar('\0');
			ptCursor_.x = rectDst_.x;
			if (pEventHandler_) pEventHandler_->OnNewLine(*this);
			if (ptCursor_.y + yAdvance * 2 <= rectDst_.y + rectDst_.height) {
				GetDrawable().Refresh();
				ptCursor_.y += yAdvance;
			} else {
				ScrollVert(DirVert::Up);
			}
		} else if (code == '\r') {
			lineBuff_.PutChar('\r').PutChar('\0').MoveLineLastHere().PlaceChar('\0');
			ptCursor_.x = rectDst_.x;
			GetDrawable().DrawRectFill(rectDst_.x, ptCursor_.y, rectDst_.width, yAdvance, context_.colorBg);
		} else {
			if (ptCursor_.x + xAdvance > rectDst_.x + rectDst_.width) {
				lineBuff_.PutChar('\0').MoveLineLastHere();
				ptCursor_.x = rectDst_.x;
				if (pEventHandler_) pEventHandler_->OnNewLine(*this);
				if (ptCursor_.y + yAdvance * 2 <= rectDst_.y + rectDst_.height) {
					ptCursor_.y += yAdvance;
				} else {
					ScrollVert(DirVert::Up);
				}
			}
			lineBuff_.PutString(decoder_.GetStringOrg()).PlaceChar('\0');
			GetDrawable().DrawChar(ptCursor_, fontEntry, false, &context_);
			ptCursor_.x += xAdvance;
		}
	}
	return *this;
}

void Terminal::DrawTextLines(const char* lineTop, int nLines, int yTop)
{
	int yAdvance = context_.CalcAdvanceY();
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
	const FontSet& fontSet = GetFont();
	int x = rectDst_.x;
	uint32_t code;
	UTF8Decoder decoder;
	for (;;) {
		char ch = charFeeder.Get();
		if (!ch || ch == '\n' || ch == '\r') break;
		charFeeder.Forward();
		if (decoder.FeedChar(ch, &code)) {
			const FontEntry& fontEntry = fontSet.GetFontEntry(code);
			int xAdvance = context_.CalcAdvanceX(fontEntry);
			GetDrawable().DrawChar(x, y, fontEntry, false, &context_);
			x += xAdvance;
		}
	}
	if (x < rectDst_.x + rectDst_.width) {
		int yAdvance = context_.CalcAdvanceY();
		GetDrawable().DrawRectFill(x, y, rectDst_.x + rectDst_.width - x, yAdvance, context_.colorBg);
	}
}

void Terminal::EraseTextLine(int iLine, int nLines)
{
	int yAdvance = context_.CalcAdvanceY();
	GetDrawable().DrawRectFill(rectDst_.x, rectDst_.y + yAdvance * iLine, rectDst_.width, yAdvance * nLines, context_.colorBg);
}

void Terminal::ScrollVert(DirVert dirVert)
{
	int nLines = GetRowNum();
	const char* lineTop = lineBuff_.GetLineLast();
	lineBuff_.PrevLine(&lineTop, nLines - 1);
	DrawTextLines(lineTop, nLines - 1, rectDst_.y);
	EraseTextLine(nLines - 1);
	GetDrawable().Refresh();
}

}
