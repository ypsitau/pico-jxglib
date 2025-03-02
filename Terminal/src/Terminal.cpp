//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
Terminal::Terminal(int bytesBuff, int msecBlink) : Tickable(msecBlink),
	pDrawable_{nullptr}, nLinesWhole_{0}, lineBuff_(bytesBuff), pEventHandler_{nullptr}, pLineStop_{nullptr},
	suppressFlag_{false}, showCursorFlag_{false}, blinkFlag_{false}, wdCursor_{2},
	colorEditor_{128, 128, 255}, colorCursor_{128, 128, 255}
{
}

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

void Terminal::AppendChar(char ch, bool suppressFlag)
{
	uint32_t codeUTF32;
	if (!decoder_.FeedChar(ch, &codeUTF32)) return;
	if (!suppressFlag && IsRollingBack()) EndRollBack();
	int yAdvance = context_.CalcAdvanceY();
	const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
	int xAdvance = context_.CalcAdvanceX(fontEntry);
	if (codeUTF32 == '\n') {
		GetLineBuff().PutChar('\n').PutChar('\0');
		GetLineBuff().MoveLineLastHere().PlaceChar('\0');
		ptCurrent_.x = rectDst_.x;
		if (pEventHandler_) pEventHandler_->OnNewLine(*this);
		if (ptCurrent_.y + yAdvance * 2 <= rectDst_.y + rectDst_.height) {
			if (!suppressFlag) GetDrawable().Refresh();
			ptCurrent_.y += yAdvance;
		} else if (!suppressFlag) {
			ScrollUp();
		}
	} else if (codeUTF32 == '\r') {
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
			} else if (!suppressFlag) {
				ScrollUp();
			}
		}
		GetLineBuff().Print(decoder_.GetStringOrg());
		GetLineBuff().PlaceChar('\0');
		if (!suppressFlag) GetDrawable().DrawChar(ptCurrent_, fontEntry, false, &context_);
		ptCurrent_.x += xAdvance;
	}
}

void Terminal::DrawEditorArea()
{
	Point pt = ptCurrent_;;
	int yAdvance = context_.CalcAdvanceY();
	UTF8Decoder decoder;
	Color colorFgSaved = context_.colorFg;
	context_.colorFg = colorEditor_;
	for (const char* p = editor_.GetPointer(0); *p; p++) {
		uint32_t codeUTF32;
		if (!decoder.FeedChar(*p, &codeUTF32)) continue;
		const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
		int xAdvance = context_.CalcAdvanceX(fontEntry);
		if (pt.x + xAdvance > rectDst_.x + rectDst_.width) {
			pt.x = rectDst_.x;
			if (pt.y + yAdvance * 2 <= rectDst_.y + rectDst_.height) {
				pt.y += yAdvance;
			} else {
				// scroll up
			}
		}
		GetDrawable().DrawChar(pt, fontEntry, false, &context_);
		pt.x += xAdvance;
	}
	context_.colorFg = colorFgSaved;
	if (pt.x < rectDst_.x + rectDst_.width) {
		GetDrawable().DrawRectFill(pt.x, pt.y, rectDst_.x + rectDst_.width - pt.x, yAdvance, context_.colorBg);
	}
	pt.y += yAdvance;
	GetDrawable().DrawRectFill(rectDst_.x, pt.y, rectDst_.width, rectDst_.height - pt.y + rectDst_.y, context_.colorBg);
	GetDrawable().Refresh();
}

Point Terminal::CalcCursorPos(int idxCursor)
{
	Point pt = ptCurrent_;
	int yAdvance = context_.CalcAdvanceY();
	UTF8Decoder decoder;
	const char* pEnd = editor_.GetPointer(idxCursor);
	for (const char* p = editor_.GetPointer(0); *p && p < pEnd; p++) {
		uint32_t codeUTF32;
		if (!decoder.FeedChar(*p, &codeUTF32)) continue;
		const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
		int xAdvance = context_.CalcAdvanceX(fontEntry);
		if (pt.x + xAdvance > rectDst_.x + rectDst_.width) {
			pt.x = rectDst_.x;
			pt.y += yAdvance;
		}
		pt.x += xAdvance;
	}
	return pt;
}

void Terminal::DrawCursor()
{
	int yAdvance = context_.CalcAdvanceY();
	GetDrawable()
		.DrawRectFill(CalcCursorPos(), Size(wdCursor_, yAdvance), colorCursor_)
		.Refresh();
}

void Terminal::EraseCursor(int idxCursor)
{
	const char* p = editor_.GetPointer(idxCursor);
	uint32_t codeUTF32 = UTF8Decoder::ToUTF32(p);
	Point pt = CalcCursorPos(idxCursor);
	int yAdvance = context_.CalcAdvanceY();
	GetDrawable().DrawRectFill(pt, Size(wdCursor_, yAdvance), GetColorBg());
	if (codeUTF32) {
		const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
		Color colorFgSaved = context_.colorFg;
		context_.colorFg = colorEditor_;
		GetDrawable().DrawChar(pt, fontEntry, false, &context_);
		context_.colorFg = colorFgSaved;
	}
	GetDrawable().Refresh();
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
	uint32_t codeUTF32;
	WrappedCharFeeder charFeeder(GetLineBuff().CreateCharFeeder(pLineTop));
	UTF8Decoder decoder;
	for (;;) {
		char ch = charFeeder.Get();
		if (!ch || ch == '\n' || ch == '\r') break;
		charFeeder.Forward();
		if (decoder.FeedChar(ch, &codeUTF32)) {
			const FontEntry& fontEntry = fontSet.GetFontEntry(codeUTF32);
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

Terminal& Terminal::Edit_Finish(char chEnd)
{
	for (const char* p = GetEditor().GetPointer(0); *p; p++) AppendChar(*p, true);
	if (chEnd) AppendChar(chEnd, true);
	GetEditor().Clear();
	DrawLatestTextLines();
	return *this;
}

Terminal& Terminal::Edit_InsertChar(int ch)
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().InsertChar(ch)) {
		EraseCursor(idxCursor);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_Delete()
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().DeleteChar()) {
		EraseCursor(idxCursor);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_Back()
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().MoveBackward() && GetEditor().DeleteChar()) {
		EraseCursor(idxCursor);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveForward()
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().MoveForward()) {
		EraseCursor(idxCursor);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveBackward()
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().MoveBackward()) {
		EraseCursor(idxCursor);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveHome()
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().MoveHome()) {
		EraseCursor(idxCursor);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveEnd()
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().MoveEnd()) {
		EraseCursor(idxCursor);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_KillLine()
{
	int idxCursor = GetEditor().GetIdxCursor();
	if (GetEditor().KillLine()) {
		EraseCursor(idxCursor);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

//------------------------------------------------------------------------------
// Terminal::Editor
//------------------------------------------------------------------------------
Terminal::Editor::Editor() : idxCursor_{0}
{
	buff_[0] = '\0';
}

void Terminal::Editor::Clear()
{
	idxCursor_ = 0;
	buff_[0] = '\0';
}

bool Terminal::Editor::InsertChar(char ch)
{
	if (::strlen(buff_) + 1 < sizeof(buff_)) {
		char *p = GetPointerAtCursor();
		::memmove(p + 1, p, ::strlen(p) + 1);
		*p = ch;
		idxCursor_++;
		uint32_t codeUTF32;
		return decoder_.FeedChar(ch, &codeUTF32);
	}
	return false;
}

bool Terminal::Editor::DeleteChar()
{
	char *p = GetPointerAtCursor();
	if (*p) {
		int bytes;
		UTF8Decoder::ToUTF32(p, &bytes);
		::memmove(p, p + bytes, ::strlen(p + bytes) + 1);
		return true;
	}
	return false;
}

bool Terminal::Editor::MoveForward()
{
	char *pStart = GetPointerAtCursor();
	if (*pStart) {
		char* p = pStart + 1;
		for ( ; (*p & 0xc0) == 0x80; p++) ;
		idxCursor_ += p - pStart;
		return true;
	}
	return false;
}

bool Terminal::Editor::MoveBackward()
{
	if (idxCursor_ > 0) {
		char* pStart = GetPointerAtCursor();
		char* p = pStart - 1;
		for ( ; p != buff_ && (*p & 0xc0) == 0x80; p--) ;
		idxCursor_ -= pStart - p;
		return true;
	}
	return false;
}

bool Terminal::Editor::MoveHome()
{
	idxCursor_ = 0;
	return true;
}

bool Terminal::Editor::MoveEnd()
{
	idxCursor_ = ::strlen(buff_);
	return true;
}

bool Terminal::Editor::KillLine()
{
	char *p = GetPointerAtCursor();
	if (*p) {
		*p = '\0';
		return true;
	}
	return false;
}

}
