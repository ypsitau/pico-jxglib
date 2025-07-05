//==============================================================================
// Display.cpp
//==============================================================================
#include <cctype>
#include "jxglib/Display.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Display
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Display::Terminal
//------------------------------------------------------------------------------
Display::Terminal::Terminal(int bytesLineBuff, int bytesHistoryBuff) :
	jxglib::Terminal(bytesHistoryBuff, KeyboardDumb::Instance), pDrawable_{nullptr}, nLinesWhole_{0}, lineBuff_(bytesLineBuff),
	pEventHandler_{nullptr}, pLineStop_RollBack_{nullptr},
	suppressFlag_{false}, showCursorFlag_{false}, appearCursorFlag_{false}, wdCursor_{2},
	colorTextInEdit_{255, 255, 255}, colorCursor_{255, 255, 255},
	tickable_Blink_(*this, 500), tickable_Keyboard_(*this)
{
}

Display::Terminal& Display::Terminal::AttachKeyboard(Keyboard& keyboard)
{
	jxglib::Terminal::AttachKeyboard(keyboard);
	tickable_Keyboard_.SetTick(50);
	return *this;
}

Display::Terminal& Display::Terminal::AttachDrawable(Drawable& drawable, const Rect& rect, Dir dir)
{
	Initialize();
	if (!GetLineBuff().Initialize()) {
		::panic("failed to allocate memory in Display::Terminal::AttachDrawable()");
		return *this;
	}
	rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : rect;
	ptCurrent_ = Point(rectDst_.x, rectDst_.y);
	pDrawable_ = &drawable;
	return *this;
}

int Display::Terminal::CalcApproxNColsOnDisplay() const
{
	const FontEntry& fontEntry = GetFont().GetFontEntry('M');
	return rectDst_.width / fontEntry.xAdvance;
}

int Display::Terminal::CalcNLinesOnDisplay() const
{
	int yAdvance = context_.CalcAdvanceY();
	return rectDst_.height / yAdvance;
}

Display::Terminal& Display::Terminal::BeginRollBack()
{
	int nLines = CalcNLinesOnDisplay();
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines - 1);
	GetLineBuff().SetLineMark(pLineTop);
	pLineStop_RollBack_ = pLineTop;
	if (showCursorFlag_) EraseCursor(GetLineEditor().GetIByteCursor());
	return *this;
}

Display::Terminal& Display::Terminal::EndRollBack()
{
	DrawLatestTextLines(true);
	GetLineBuff().RemoveLineMark();	// end roll-back
	return *this;
}

Display::Terminal& Display::Terminal::RollUp(int nLines)
{
	if (!GetLineEditor().IsEmpty()) {
		GetLineEditor().Clear();
		DrawLatestTextLines(true);
		return *this;
	}
	if (!IsRollingBack()) BeginRollBack();	
	if (GetLineBuff().MoveLineMarkUp(nLines)) {
		DrawTextLines(0, GetLineBuff().GetLineMark(), CalcNLinesOnDisplay());
		GetDrawable().Refresh();
	}
	return *this;
}

Display::Terminal& Display::Terminal::RollDown(int nLines)
{
	if (!GetLineEditor().IsEmpty()) {
		GetLineEditor().Clear();
		DrawLatestTextLines(true);
		return *this;
	}
	if (GetLineBuff().MoveLineMarkDown(nLines, pLineStop_RollBack_)) {
		if (!IsRollingBack()) BeginRollBack();	
		DrawTextLines(0, GetLineBuff().GetLineMark(), CalcNLinesOnDisplay());
		GetDrawable().Refresh();
		if (GetLineBuff().GetLineMark() == pLineStop_RollBack_) {
			GetLineBuff().RemoveLineMark();	// end roll-back
		}
	}
	return *this;
}

Display::Terminal& Display::Terminal::Suppress(bool suppressFlag)
{
	suppressFlag_ = suppressFlag;
	if (!suppressFlag_) DrawLatestTextLines(true);
	return *this;
}

Display::Terminal& Display::Terminal::ShowCursor(bool showCursorFlag)
{
	showCursorFlag_ = showCursorFlag;
	return *this;
}

void Display::Terminal::AppendChar(char ch, bool drawFlag)
{
	Point& pt = ptCurrent_;
	uint32_t codeUTF32;
	if (!decoder_.FeedChar(ch, &codeUTF32)) return;
	if (drawFlag && IsRollingBack()) EndRollBack();
	const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
	int xAdvance = context_.CalcAdvanceX(fontEntry);
	int yAdvance = context_.CalcAdvanceY();
	if (codeUTF32 == '\n') {
		GetLineBuff().PutChar('\n').PutChar('\0');
		GetLineBuff().MoveLineLastHere().PlaceChar('\0');
		pt.x = rectDst_.x;
		if (pEventHandler_) pEventHandler_->OnNewLine(*this);
		if (pt.y + yAdvance * 2 <= rectDst_.GetBottomExceed()) {
			if (drawFlag) GetDrawable().Refresh();
			pt.y += yAdvance;
		} else if (drawFlag) {
			ScrollUp(1, true);
		}
	} else if (codeUTF32 == '\r') {
		GetLineBuff().PutChar('\r').PutChar('\0');
		GetLineBuff().MoveLineLastHere().PlaceChar('\0');
		pt.x = rectDst_.x;
	} else {
		if (pt.x + xAdvance > rectDst_.GetRightExceed()) {
			GetLineBuff().PutChar('\0');
			GetLineBuff().MoveLineLastHere();
			pt.x = rectDst_.x;
			if (pEventHandler_) pEventHandler_->OnNewLine(*this);
			if (pt.y + yAdvance * 2 <= rectDst_.GetBottomExceed()) {
				pt.y += yAdvance;
			} else if (drawFlag) {
				ScrollUp(1, true);
			}
		}
		GetLineBuff().Print(decoder_.GetStringOrg());
		GetLineBuff().PlaceChar('\0');
		if (drawFlag) GetDrawable().DrawChar(pt, fontEntry, false, &context_);
		pt.x += xAdvance;
	}
}

void Display::Terminal::AppendString(const char* str, bool drawFlag)
{
	for (const char* p = str; *p; p++) AppendChar(*p, drawFlag);
}

void Display::Terminal::DrawEditorArea()
{
	int yAdvance = context_.CalcAdvanceY();
	Point ptEnd = CalcDrawPos(ptCurrent_, GetLineEditor().GetIByteEnd(), wdCursor_);
	int htExceed = ptEnd.y + yAdvance - (rectDst_.GetBottomExceed() / yAdvance * yAdvance);
	if (htExceed > 0) {
		if (ptCurrent_.y - htExceed >= rectDst_.y) ptCurrent_.y -= htExceed;
		DrawLatestTextLines(false);
	}
	Color colorFgSaved = context_.colorFg;
	context_.colorFg = colorTextInEdit_;
	Point pt = ptCurrent_;
	UTF8::Decoder decoder;
	for (const char* p = GetLineEditor().GetPointerBegin(); *p; p++) {
		uint32_t codeUTF32;
		if (!decoder.FeedChar(*p, &codeUTF32)) continue;
		const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
		int xAdvance = context_.CalcAdvanceX(fontEntry);
		if (pt.x + xAdvance > rectDst_.GetRightExceed()) {
			pt.x = rectDst_.x;
			pt.y += yAdvance;
		}
		if (pt.y + yAdvance <= rectDst_.GetBottomExceed()) {
			GetDrawable().DrawChar(pt, fontEntry, false, &context_);
		}
		pt.x += xAdvance;
	}
	context_.colorFg = colorFgSaved;
	if (pt.x < rectDst_.GetRightExceed() && pt.y + yAdvance <= rectDst_.GetBottomExceed()) {
		GetDrawable().DrawRectFill(pt.x, pt.y, rectDst_.GetRightExceed() - pt.x, yAdvance, context_.colorBg);
	}
	pt.y += yAdvance;
	if (pt.y < rectDst_.GetBottomExceed()) {
		GetDrawable().DrawRectFill(rectDst_.x, pt.y, rectDst_.width, rectDst_.GetBottomExceed() - pt.y, context_.colorBg);
	}
	GetDrawable().Refresh();
}

Point Display::Terminal::CalcDrawPos(const Point& ptBase, int iByte, int wdAdvance)
{
	Point pt = ptBase;
	int yAdvance = context_.CalcAdvanceY();
	UTF8::Decoder decoder;
	const char* pEnd = GetLineEditor().GetPointer(iByte);
	for (const char* p = GetLineEditor().GetPointerBegin(); *p && p < pEnd; p++) {
		uint32_t codeUTF32;
		if (!decoder.FeedChar(*p, &codeUTF32)) continue;
		const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
		int xAdvance = context_.CalcAdvanceX(fontEntry);
		if (pt.x + xAdvance > rectDst_.GetRightExceed()) {
			pt.x = rectDst_.x;
			pt.y += yAdvance;
		}
		pt.x += xAdvance;
	}
	if (wdAdvance > 0) {
		if (pt.x + wdAdvance > rectDst_.GetRightExceed()) {
			pt.x = rectDst_.x;
			pt.y += yAdvance;
		}
		pt.x += wdAdvance;
	}
	return pt;
}

void Display::Terminal::DrawCursor(int iByteCursor)
{
	int yAdvance = context_.CalcAdvanceY();
	Point pt = CalcDrawPos(ptCurrent_, iByteCursor, wdCursor_);
	if (pt.y + yAdvance <= rectDst_.GetBottomExceed()) {
		pt.x -= wdCursor_;
		GetDrawable().DrawRectFill(pt, Size(wdCursor_, yAdvance), colorCursor_).Refresh();
	}
}

void Display::Terminal::EraseCursor(int iByteCursor)
{
	const char* p = GetLineEditor().GetPointer(iByteCursor);
	uint32_t codeUTF32 = UTF8::ToUTF32(p);
	Point pt = CalcDrawPos(ptCurrent_, iByteCursor, wdCursor_);
	pt.x -= wdCursor_;
	int yAdvance = context_.CalcAdvanceY();
	if (pt.y + yAdvance <= rectDst_.GetBottomExceed()) {
		GetDrawable().DrawRectFill(pt, Size(wdCursor_, yAdvance), GetColorBg());
		if (codeUTF32) {
			const FontEntry& fontEntry = GetFont().GetFontEntry(codeUTF32);
			int xAdvance = context_.CalcAdvanceX(fontEntry);
			if (pt.x + xAdvance <= rectDst_.GetRightExceed()) {
				Color colorFgSaved = context_.colorFg;
				context_.colorFg = colorTextInEdit_;
				GetDrawable().DrawChar(pt, fontEntry, false, &context_);
				context_.colorFg = colorFgSaved;
			}
		}
		GetDrawable().Refresh();
	}
}

void Display::Terminal::BlinkCursor()
{
	if (showCursorFlag_ && !IsRollingBack()) {
		appearCursorFlag_ = !appearCursorFlag_;
		int iByteCursor = GetLineEditor().GetIByteCursor();
		if (appearCursorFlag_) DrawCursor(iByteCursor); else EraseCursor(iByteCursor);
	}
}

void Display::Terminal::DrawLatestTextLines(bool refreshFlag)
{
	int yAdvance = context_.CalcAdvanceY();
	int nLines = (ptCurrent_.y - rectDst_.y) / yAdvance + 1;
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines - 1);
	DrawTextLines(0, pLineTop, nLines);
	int htLines = yAdvance * nLines;
	GetDrawable().DrawRectFill(rectDst_.x, rectDst_.y + htLines, rectDst_.width, rectDst_.height - htLines, context_.colorBg);
	GetDrawable().Refresh();
}

void Display::Terminal::DrawTextLines(int iLine, const char* pLineTop, int nLines)
{
	if (!pLineTop) return;
	for (int i = 0; i < nLines; i++, iLine++) {
		DrawTextLine(iLine, pLineTop);
		if (!GetLineBuff().NextLine(&pLineTop)) break;
	}
}

void Display::Terminal::DrawTextLine(int iLine, const char* pLineTop)
{
	const FontSet& fontSet = GetFont();
	int yAdvance = context_.CalcAdvanceY();
	int x = rectDst_.x;
	int y = rectDst_.y + yAdvance * iLine;
	uint32_t codeUTF32;
	WrappedCharFeeder charFeeder(GetLineBuff().CreateCharFeeder(pLineTop));
	UTF8::Decoder decoder;
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
	if (x < rectDst_.GetRightExceed()) {
		GetDrawable().DrawRectFill(x, y, rectDst_.GetRightExceed() - x, yAdvance, context_.colorBg);
	}
}

void Display::Terminal::ScrollUp(int nLinesToScroll, bool refreshFlag)
{
	int yAdvance = context_.CalcAdvanceY();
	int nLines = CalcNLinesOnDisplay();
	if (nLinesToScroll > nLines) return;
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines - nLinesToScroll);
	DrawTextLines(0, pLineTop, nLines - nLinesToScroll);
	int htLines = yAdvance * (nLines - nLinesToScroll);
	GetDrawable().DrawRectFill(rectDst_.x, rectDst_.y + htLines, rectDst_.width, rectDst_.height - htLines, context_.colorBg);
	if (refreshFlag) GetDrawable().Refresh();
}

// Virtual functions of Printable
Printable& Display::Terminal::ClearScreen()
{
	GetDrawable().DrawRectFill(rectDst_, GetColorBg()).Refresh();
	GetLineBuff().Clear();
	Locate(0, 0);
	return *this;
}

Printable& Display::Terminal::RefreshScreen()
{
	GetDrawable().Refresh();
	return *this;
}

Printable& Display::Terminal::Locate(int col, int row)
{
	const FontEntry& fontEntry = GetFont().GetFontEntry('M');
	ptCurrent_ = Point(
		rectDst_.x + context_.CalcAdvanceX(fontEntry) * col,
		rectDst_.y + context_.CalcAdvanceY() * row);
	return *this;
}

Printable& Display::Terminal::GetSize(int* pnCols, int* pnRows)
{
	const FontEntry& fontEntry = GetFont().GetFontEntry('M');
	*pnCols = GetDrawable().GetWidth() / fontEntry.width;
	*pnRows = GetDrawable().GetHeight() / context_.CalcAdvanceY();
	return *this;
}

Printable& Display::Terminal::PutCharRaw(char ch)
{
	AppendChar(ch, !suppressFlag_);
	return *this;
};

// virtual functions of jxglib::Terminal
Terminal& Display::Terminal::Edit_Begin()
{
	ShowCursor();
	GetLineEditor().Begin();
	return *this;
}

Terminal& Display::Terminal::Edit_Finish(char chEnd)
{
	if (!GetLineEditor().IsEditing()) return *this;
	GetLineEditor().EndHistory();
	const char* str = GetLineEditor().GetPointerBegin();
	AppendString(str, false);
	if (chEnd) AppendChar(chEnd, false);
	DrawLatestTextLines(true);
	ShowCursor(false);
	GetLineEditor().Finish();
	EndCompletion();
	return *this;
}

Terminal& Display::Terminal::Edit_InsertChar(int ch)
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (IsRollingBack()) EndRollBack();
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().InsertChar(ch)) {
		int yAdvance = context_.CalcAdvanceY();
		for ( ; !GetLineEditor().IsEmpty(); GetLineEditor().DeleteLastChar()) {
			Point ptEnd = CalcDrawPos(rectDst_.GetPointNW(), GetLineEditor().GetIByteEnd(), wdCursor_);
			if (ptEnd.y + yAdvance <= rectDst_.GetBottomExceed()) break;
		}
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_DeleteChar()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().DeleteChar()) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_Back()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().Back()) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_MoveForward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().MoveForward()) {
		EraseCursor(iByteCursorPrev);
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_MoveBackward()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().MoveBackward()) {
		EraseCursor(iByteCursorPrev);
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_MoveHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().MoveHome()) {
		EraseCursor(iByteCursorPrev);
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_MoveEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().MoveEnd()) {
		EraseCursor(iByteCursorPrev);
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_Clear()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().Clear()) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_DeleteToHome()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().DeleteToHome()) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_DeleteToEnd()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().DeleteToEnd()) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_MoveHistoryPrev()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().MoveHistoryPrev()) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_MoveHistoryNext()
{
	if (!GetLineEditor().IsEditing()) return *this;
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (GetLineEditor().MoveHistoryNext()) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
		EndCompletion();
	}
	return *this;
}

Terminal& Display::Terminal::Edit_Completion()
{
	if (!GetLineEditor().IsEditing()) return *this;
	if (!pCompletionProvider_) return *this;
	if (!pCompletionProvider_->IsStarted()) {
		pCompletionProvider_->Start(*this, GetLineEditor().GetIByteToCompletion());
	}
	const char* strCompletion = pCompletionProvider_->NextCompletion();
	int iByteCursorPrev = GetLineEditor().GetIByteCursor();
	if (strCompletion && GetLineEditor().ReplaceWithCompletion(pCompletionProvider_->GetIByte(), strCompletion)) {
		EraseCursor(iByteCursorPrev);
		DrawEditorArea();
		DrawCursor(GetLineEditor().GetIByteCursor());
	}
	return *this;
}

//------------------------------------------------------------------------------
// Display::Terminal::Tickable_Keyboard
//------------------------------------------------------------------------------
void Display::Terminal::Tickable_Keyboard::OnTick()
{
	KeyData keyData;
	if (terminal_.GetKeyDataNB(&keyData)) {
		if (keyData.IsKeyCode()) {
			int nLines = keyData.IsShiftDown()? 10 : 1;
			switch (keyData.GetKeyCode()) {
			case VK_PRIOR: terminal_.RollUp(nLines); return;
			case VK_NEXT: terminal_.RollDown(nLines); return;
			default: break;
			}
		}
		if (terminal_.IsEditable()) terminal_.ProcessKeyData(keyData);
	}
}

}
