//==============================================================================
// Terminal.cpp
//==============================================================================
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
Terminal::Terminal(int bytesBuff, int msecBlink) :
	pDrawable_{nullptr}, nLinesWhole_{0}, lineBuff_(bytesBuff), pEventHandler_{nullptr}, pLineStop_{nullptr},
	suppressFlag_{false}, editingFlag_{false}, showCursorFlag_{false}, appearCursorFlag_{false}, wdCursor_{2},
	colorTextInEdit_{128, 128, 255}, colorCursor_{128, 128, 255}, pInput_{nullptr},
	tickable_Blink_(*this, msecBlink), tickable_Input_(*this)
{
}

bool Terminal::AttachOutput(Drawable& drawable, const Rect& rect, Dir dir)
{
	if (!GetLineBuff().Initialize()) return false;
	Tickable::AddTickable(tickable_Blink_);
	rectDst_ = rect.IsEmpty()? Rect(0, 0, drawable.GetWidth(), drawable.GetHeight()) : rect;
	ptCurrent_ = Point(rectDst_.x, rectDst_.y);
	pDrawable_ = &drawable;
	return true;
}

void Terminal::AttachInput(Input& input, int msecTick)
{
	if (!pInput_) {
		pInput_ = &input;
		tickable_Input_.SetTick(msecTick);
		Tickable::AddTickable(tickable_Input_);
	}
}

void Terminal::AttachInput(UART& uart)
{
	inputUART_.SetUART(uart);
	AttachInput(inputUART_, 50);
}

int Terminal::CalcApproxNColsOnDisplay() const
{
	const FontEntry& fontEntry = GetFont().GetFontEntry('M');
	return rectDst_.width / fontEntry.xAdvance;
}

int Terminal::CalcNLinesOnDisplay() const
{
	int yAdvance = context_.CalcAdvanceY();
	return rectDst_.height / yAdvance;
}

Terminal& Terminal::BeginRollBack()
{
	int nLines = CalcNLinesOnDisplay();
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines - 1);
	GetLineBuff().SetLineMark(pLineTop);
	pLineStop_ = pLineTop;
	if (showCursorFlag_) EraseCursor();
	return *this;
}

Terminal& Terminal::EndRollBack()
{
	DrawLatestTextLines(true);
	GetLineBuff().RemoveLineMark();	// end roll-back
	return *this;
}

Terminal& Terminal::RollUp()
{
	if (!GetEditor().IsEmpty()) {
		GetEditor().Clear();
		DrawLatestTextLines(true);
		return *this;
	}
	if (!IsRollingBack()) BeginRollBack();	
	if (GetLineBuff().MoveLineMarkUp()) {
		DrawTextLines(0, GetLineBuff().GetLineMark(), CalcNLinesOnDisplay());
		GetDrawable().Refresh();
	}
	return *this;
}

Terminal& Terminal::RollDown()
{
	if (!GetEditor().IsEmpty()) {
		GetEditor().Clear();
		DrawLatestTextLines(true);
		return *this;
	}
	if (GetLineBuff().MoveLineMarkDown(pLineStop_)) {
		if (!IsRollingBack()) BeginRollBack();	
		DrawTextLines(0, GetLineBuff().GetLineMark(), CalcNLinesOnDisplay());
		GetDrawable().Refresh();
		if (GetLineBuff().GetLineMark() == pLineStop_) {
			GetLineBuff().RemoveLineMark();	// end roll-back
		}
	}
	return *this;
}

Terminal& Terminal::Suppress(bool suppressFlag)
{
	suppressFlag_ = suppressFlag;
	if (!suppressFlag_) DrawLatestTextLines(true);
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

bool Terminal::ReadLine(char* buff, int bytes)
{
	Edit_Begin();
	for (;;) {
		Tickable::Tick();
		if (!editingFlag_) break;
	}
	const char* src = GetEditor().GetPointerBegin();
	int bytesToCopy = ChooseMin(static_cast<int>(::strlen(src)), bytes - 1);
	::memcpy(buff, src, bytesToCopy);
	buff[bytesToCopy] = '\0';
	return true;
}

void Terminal::AppendChar(Point& pt, char ch, bool drawFlag)
{
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

void Terminal::AppendString(Point& pt, const char* str, bool drawFlag)
{
	for (const char* p = str; *p; p++) AppendChar(pt, *p, drawFlag);
}

void Terminal::DrawEditorArea()
{
	int yAdvance = context_.CalcAdvanceY();
	Point ptEnd = CalcDrawPos(ptCurrent_, GetEditor().GetICharEnd(), wdCursor_);
	int htExceed = ptEnd.y + yAdvance - (rectDst_.GetBottomExceed() / yAdvance * yAdvance);
	if (htExceed > 0) {
		if (ptCurrent_.y - htExceed >= rectDst_.y) ptCurrent_.y -= htExceed;
		DrawLatestTextLines(false);
	}
	Color colorFgSaved = context_.colorFg;
	context_.colorFg = colorTextInEdit_;
	Point pt = ptCurrent_;
	UTF8Decoder decoder;
	for (const char* p = GetEditor().GetPointerBegin(); *p; p++) {
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

Point Terminal::CalcDrawPos(const Point& ptBase, int iChar, int wdAdvance)
{
	Point pt = ptBase;
	int yAdvance = context_.CalcAdvanceY();
	UTF8Decoder decoder;
	const char* pEnd = GetEditor().GetPointer(iChar);
	for (const char* p = GetEditor().GetPointerBegin(); *p && p < pEnd; p++) {
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

void Terminal::DrawCursor()
{
	int yAdvance = context_.CalcAdvanceY();
	Point pt = CalcDrawPos(ptCurrent_, GetEditor().GetICharCursor(), wdCursor_);
	if (pt.y + yAdvance <= rectDst_.GetBottomExceed()) {
		pt.x -= wdCursor_;
		GetDrawable().DrawRectFill(pt, Size(wdCursor_, yAdvance), colorCursor_).Refresh();
	}
}

void Terminal::EraseCursor(int iChar)
{
	const char* p = GetEditor().GetPointer(iChar);
	uint32_t codeUTF32 = UTF8Decoder::ToUTF32(p);
	Point pt = CalcDrawPos(ptCurrent_, iChar, wdCursor_);
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

void Terminal::BlinkCursor()
{
	if (showCursorFlag_ && !IsRollingBack()) {
		appearCursorFlag_ = !appearCursorFlag_;
		if (appearCursorFlag_) DrawCursor(); else EraseCursor();
	}
}

void Terminal::DrawLatestTextLines(bool refreshFlag)
{
	int yAdvance = context_.CalcAdvanceY();
	int nLines = (ptCurrent_.y - rectDst_.y) / yAdvance;
	const char* pLineTop = GetLineBuff().GetLineLast();
	GetLineBuff().PrevLine(&pLineTop, nLines);
	DrawTextLines(0, pLineTop, nLines);
	int htLines = yAdvance * nLines;
	GetDrawable().DrawRectFill(rectDst_.x, rectDst_.y + htLines, rectDst_.width, rectDst_.height - htLines, context_.colorBg);
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
	if (x < rectDst_.GetRightExceed()) {
		GetDrawable().DrawRectFill(x, y, rectDst_.GetRightExceed() - x, yAdvance, context_.colorBg);
	}
}

void Terminal::ScrollUp(int nLinesToScroll, bool refreshFlag)
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

Terminal& Terminal::Edit_Begin(bool showCursorFlag)
{
	ShowCursor(showCursorFlag);
	editingFlag_ = true;
	GetEditor().Clear();
	return *this;
}

Terminal& Terminal::Edit_Finish(char chEnd)
{
	if (!editingFlag_) return *this;
	AppendString(ptCurrent_, GetEditor().GetPointerBegin(), false);
	if (chEnd) AppendChar(ptCurrent_, chEnd, false);
	DrawLatestTextLines(true);
	ShowCursor(false);
	editingFlag_ = false;
	return *this;
}

Terminal& Terminal::Edit_InsertChar(int ch)
{
	if (!editingFlag_) return *this;
	if (IsRollingBack()) EndRollBack();
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().InsertChar(ch)) {
		int yAdvance = context_.CalcAdvanceY();
		for ( ; !GetEditor().IsEmpty(); GetEditor().DeleteLastChar()) {
			Point ptEnd = CalcDrawPos(rectDst_.GetPointNW(), GetEditor().GetICharEnd(), wdCursor_);
			if (ptEnd.y + yAdvance <= rectDst_.GetBottomExceed()) break;
		}
		EraseCursor(iChar);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_Delete()
{
	if (!editingFlag_) return *this;
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().DeleteChar()) {
		EraseCursor(iChar);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_Back()
{
	if (!editingFlag_) return *this;
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().MoveBackward() && GetEditor().DeleteChar()) {
		EraseCursor(iChar);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveForward()
{
	if (!editingFlag_) return *this;
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().MoveForward()) {
		EraseCursor(iChar);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveBackward()
{
	if (!editingFlag_) return *this;
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().MoveBackward()) {
		EraseCursor(iChar);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveHome()
{
	if (!editingFlag_) return *this;
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().MoveHome()) {
		EraseCursor(iChar);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_MoveEnd()
{
	if (!editingFlag_) return *this;
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().MoveEnd()) {
		EraseCursor(iChar);
		DrawCursor();
	}
	return *this;
}

Terminal& Terminal::Edit_DeleteToEnd()
{
	if (!editingFlag_) return *this;
	int iChar = GetEditor().GetICharCursor();
	if (GetEditor().DeleteToEnd()) {
		EraseCursor(iChar);
		DrawEditorArea();
		DrawCursor();
	}
	return *this;
}

//------------------------------------------------------------------------------
// Terminal::Editor
//------------------------------------------------------------------------------
Terminal::Editor::Editor() : iCharCursor_{0}
{
	buff_[0] = '\0';
}

void Terminal::Editor::Clear()
{
	iCharCursor_ = 0;
	buff_[0] = '\0';
}

bool Terminal::Editor::InsertChar(int* piChar, char ch)
{
	if (::strlen(buff_) + 1 < sizeof(buff_)) {
		char *p = GetPointer(*piChar);
		::memmove(p + 1, p, ::strlen(p) + 1);
		*p = ch;
		(*piChar)++;
		uint32_t codeUTF32;
		return decoder_.FeedChar(ch, &codeUTF32);
	}
	return false;
}

bool Terminal::Editor::DeleteChar(int iChar)
{
	char *p = GetPointer(iChar);
	if (*p) {
		int bytes;
		UTF8Decoder::ToUTF32(p, &bytes);
		::memmove(p, p + bytes, ::strlen(p + bytes) + 1);
		return true;
	}
	return false;
}

bool Terminal::Editor::DeleteLastChar()
{
	int iChar = GetICharEnd();
	if (MoveBackward(&iChar)) {
		DeleteToEnd(iChar);
		if (iCharCursor_ > iChar) iCharCursor_ = iChar;
		return true;
	}
	return false;
}

bool Terminal::Editor::MoveForward(int* piChar)
{
	char *pStart = GetPointer(*piChar);
	if (*pStart) {
		char* p = pStart + 1;
		for ( ; (*p & 0xc0) == 0x80; p++) ;
		*piChar += p - pStart;
		return true;
	}
	return false;
}

bool Terminal::Editor::MoveBackward(int* piChar)
{
	if (*piChar > 0) {
		char* pStart = GetPointer(*piChar);
		char* p = pStart - 1;
		for ( ; p != buff_ && (*p & 0xc0) == 0x80; p--) ;
		*piChar -= pStart - p;
		return true;
	}
	return false;
}

bool Terminal::Editor::MoveHome()
{
	iCharCursor_ = 0;
	return true;
}

bool Terminal::Editor::MoveEnd()
{
	iCharCursor_ = ::strlen(buff_);
	return true;
}

bool Terminal::Editor::DeleteToEnd(int iChar)
{
	char *p = GetPointer(iChar);
	if (*p) {
		*p = '\0';
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
// Terminal::InputUART
//------------------------------------------------------------------------------
void Terminal::InputUART::OnTick(Terminal& terminal)
{
	UART& uart = *pUART_;
	int keyData;
	while (uart.raw.is_readable()) decoder_.FeedChar(uart.raw.getc());
	if (!decoder_.HasKeyData()) {
		// nothing to do
	} else if (decoder_.GetKeyData(&keyData)) {
		switch (keyData) {
		case VK_RETURN:	terminal.Edit_Finish('\n');		break;
		case VK_DELETE:	terminal.Edit_Delete();			break;
		case VK_BACK:	terminal.Edit_Back();			break;
		case VK_LEFT:	terminal.Edit_MoveBackward();	break;
		case VK_RIGHT:	terminal.Edit_MoveForward();	break;
		case VK_PRIOR:	terminal.RollUp();				break;
		case VK_NEXT:	terminal.RollDown();			break;
		default: break;
		}
	} else if (keyData < 0x20) {
		switch (keyData + '@') {
		case 'A': terminal.Edit_MoveHome();		break;
		case 'B': terminal.Edit_MoveBackward();	break;
		case 'C': break;
		case 'D': terminal.Edit_Delete();		break;
		case 'E': terminal.Edit_MoveEnd();		break;
		case 'F': terminal.Edit_MoveForward();	break;
		case 'K': terminal.Edit_DeleteToEnd();	break;
		case 'N': break;
		case 'P': break;
		default: break;
		}
	} else {
		terminal.Edit_InsertChar(keyData);
	}
}

}
