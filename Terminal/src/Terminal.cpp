//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
bool Terminal::AttachOutput(Drawable& drawable, const Rect& rect, Dir dir)
{
	if (!GetLineBuff().Initialize()) return false;
	Tickable::AddTickable(this);
	rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : rect;
	ptCurrent_ = Point(rectDst_.x, rectDst_.y);
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
	int yAdvance = context_.CalcAdvanceY();
	return rectDst_.height / yAdvance;
}

Terminal& Terminal::BeginRollBack()
{
	int nLines = GetRowNum();
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines - 1);
	GetLineBuff().SetLineMark(pLineTop);
	pLineStop_ = pLineTop;
	return *this;
}

Terminal& Terminal::EndRollBack()
{
	DrawLatestTextLines();	
	GetLineBuff().RemoveLineMark();
	return *this;
}

Terminal& Terminal::RollUp()
{
	if (!IsRollingBack()) BeginRollBack();	
	if (GetLineBuff().MoveLineMarkUp()) {
		DrawTextLines(0, GetLineBuff().GetLineMark(), GetRowNum());
		GetDrawable().Refresh();
	}
	return *this;
}

Terminal& Terminal::RollDown()
{
	if (!IsRollingBack()) BeginRollBack();	
	if (GetLineBuff().MoveLineMarkDown(pLineStop_)) {
		DrawTextLines(0, GetLineBuff().GetLineMark(), GetRowNum());
		GetDrawable().Refresh();
	}
	return *this;
}

Terminal& Terminal::Suppress(bool suppressFlag)
{
	suppressFlag_ = suppressFlag;
	if (!suppressFlag_) DrawLatestTextLines();
	return *this;
}

Terminal& Terminal::BeginEdit()
{
	buffEdit_[0] = '\0';
	iBuffEdit_ = 0;
	return *this;
}

Terminal& Terminal::EndEdit()
{
	return *this;
}


Terminal& Terminal::ShowCursor(bool showCursorFlag)
{
	showCursorFlag_ = showCursorFlag;
	return *this;
}

Printable& Terminal::ClearScreen()
{
	GetDrawable().DrawRectFill(rectDst_, GetColorBg()).Refresh();
	GetLineBuff().Clear();
	Locate(0, 0);
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
	ptCurrent_ = Point(
		rectDst_.x + context_.CalcAdvanceX(fontEntry) * col,
		rectDst_.y + context_.CalcAdvanceY() * row);
	return *this;
}

Printable& Terminal::PutChar(char ch)
{
	if (!suppressFlag_ && IsRollingBack()) EndRollBack();	
	int yAdvance = context_.CalcAdvanceY();
	const FontSet& fontSet = GetFont();
	uint32_t code;
	if (decoder_.FeedChar(ch, &code)) {
		const FontEntry& fontEntry = fontSet.GetFontEntry(code);
		int xAdvance = context_.CalcAdvanceX(fontEntry);
		if (code == '\n') {
			GetLineBuff().PutChar('\n').PutChar('\0');
			GetLineBuff().MoveLineLastHere().PlaceChar('\0');
			ptCurrent_.x = rectDst_.x;
			if (pEventHandler_) pEventHandler_->OnNewLine(*this);
			if (ptCurrent_.y + yAdvance * 2 <= rectDst_.y + rectDst_.height) {
				if (!suppressFlag_) GetDrawable().Refresh();
				ptCurrent_.y += yAdvance;
			} else if (!suppressFlag_) {
				ScrollUp();
			}
		} else if (code == '\r') {
			GetLineBuff().PutChar('\r').PutChar('\0');
			GetLineBuff().MoveLineLastHere().PlaceChar('\0');
			ptCurrent_.x = rectDst_.x;
		} else {
			if (ptCurrent_.x + xAdvance > rectDst_.x + rectDst_.width) {
				GetLineBuff().PutChar('\0');
				GetLineBuff().MoveLineLastHere();
				ptCurrent_.x = rectDst_.x;
				if (pEventHandler_) pEventHandler_->OnNewLine(*this);
				if (ptCurrent_.y + yAdvance * 2 <= rectDst_.y + rectDst_.height) {
					ptCurrent_.y += yAdvance;
				} else if (!suppressFlag_) {
					ScrollUp();
				}
			}
			GetLineBuff().Print(decoder_.GetStringOrg());
			GetLineBuff().PlaceChar('\0');
			if (!suppressFlag_) GetDrawable().DrawChar(ptCurrent_, fontEntry, false, &context_);
			ptCurrent_.x += xAdvance;
		}
	}
	return *this;
}

void Terminal::DrawCursor()
{
	int yAdvance = context_.CalcAdvanceY();
	GetDrawable()
		.SetColor(Color::white)
		.DrawRectFill(ptCurrent_.x, ptCurrent_.y, wdCursor_, yAdvance)
		.Refresh();
}

void Terminal::EraseCursor()
{
	int yAdvance = context_.CalcAdvanceY();
	GetDrawable()
		.SetColor(Color::black)
		.DrawRectFill(ptCurrent_.x, ptCurrent_.y, wdCursor_, yAdvance)
		.Refresh();
}

void Terminal::OnTick()
{
	if (showCursorFlag_) {
		blinkFlag_ = !blinkFlag_;
		if (blinkFlag_) DrawCursor(); else EraseCursor();
	}
}

void Terminal::DrawLatestTextLines()
{
	int nLines = GetRowNum();
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines - 1);
	DrawTextLines(0, pLineTop, nLines);
	GetDrawable().Refresh();
}

void Terminal::DrawTextLines(int iLine, const char* pLineTop, int nLines)
{
	if (!pLineTop) return;
	for (int i = 0; i < nLines; i++, iLine++) {
		DrawTextLine(iLine, pLineTop);
		if (!GetLineBuff().NextLine(&pLineTop)) break;
	}
}

void Terminal::DrawTextLine(int iLine, const char* pLineTop)
{
	const FontSet& fontSet = GetFont();
	int yAdvance = context_.CalcAdvanceY();
	int x = rectDst_.x;
	int y = rectDst_.y + yAdvance * iLine;
	uint32_t code;
	WrappedCharFeeder charFeeder(GetLineBuff().CreateCharFeeder(pLineTop));
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
		GetDrawable().DrawRectFill(x, y, rectDst_.x + rectDst_.width - x, yAdvance, context_.colorBg);
	}
}

void Terminal::EraseTextLines(int iLine, int nLines)
{
	int yAdvance = context_.CalcAdvanceY();
	GetDrawable().DrawRectFill(rectDst_.x, rectDst_.y + yAdvance * iLine, rectDst_.width, yAdvance * nLines, context_.colorBg);
}

void Terminal::EraseToEndOfLine()
{
	int yAdvance = context_.CalcAdvanceY();
	GetDrawable().DrawRectFill(ptCurrent_.x, ptCurrent_.y,
		rectDst_.x + rectDst_.width - ptCurrent_.x, yAdvance, context_.colorBg).Refresh();
}

void Terminal::ScrollUp()
{
	int nLines = GetRowNum();
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines - 1);
	DrawTextLines(0, pLineTop, nLines - 1);
	EraseTextLines(nLines - 1, 1);
	GetDrawable().Refresh();
}

}
