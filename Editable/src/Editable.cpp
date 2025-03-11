//==============================================================================
// Editable.cpp
//==============================================================================
#include <cctype>
#include <memory.h>
#include "jxglib/Editable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Editable
//------------------------------------------------------------------------------
Editable::Editable(int bytesHistoryBuff) : lineEditor_(bytesHistoryBuff)
{
}

bool Editable::Initialize()
{
	return GetLineEditor().Initialize();
}

char* Editable::ReadLine(const char* prompt)
{
	GetPrintable().Print(prompt).RefreshScreen();
	Edit_Begin();
	for (;;) {
		Tickable::Tick();
		if (!GetLineEditor().IsEditing()) break;
	}
	return GetLineEditor().GetPointerBegin();
}

Editable& Editable::AcceptKey(int keyData, bool vkFlag)
{
	if (vkFlag) {
		switch (keyData) {
		case VK_RETURN:	Edit_Finish('\n');		break;
		case VK_DELETE:	Edit_DeleteChar();		break;
		case VK_BACK:	Edit_Back();			break;
		case VK_LEFT:	Edit_MoveBackward();	break;
		case VK_RIGHT:	Edit_MoveForward();		break;
		case VK_UP:		Edit_MoveHistoryPrev();	break;
		case VK_DOWN:	Edit_MoveHistoryNext();	break;
		default: break;
		}
	} else if (keyData < 0x20) {
		switch (keyData + '@') {
		case 'A':		Edit_MoveHome();		break;
		case 'B':		Edit_MoveBackward();	break;
		case 'D':		Edit_DeleteChar();		break;
		case 'E':		Edit_MoveEnd();			break;
		case 'F':		Edit_MoveForward();		break;
		case 'J':		Edit_Finish('\n');		break;
		case 'K':		Edit_DeleteToEnd();		break;
		case 'N':		Edit_MoveHistoryNext();	break;
		case 'P':		Edit_MoveHistoryPrev();	break;
		case 'U':		Edit_DeleteToHome();	break;
		default: break;
		}
	} else {
		Edit_InsertChar(keyData);
	}
	return *this;
}

//------------------------------------------------------------------------------
// Editable::LineEditor
//------------------------------------------------------------------------------
Editable::LineEditor::LineEditor(int bytesHistoryBuff) :
	editingFlag_{false}, iByteCursor_{0}, historyBuff_(bytesHistoryBuff), pLineStop_History_{nullptr}
{
	buff_[0] = '\0';
}

bool Editable::LineEditor::Initialize()
{
	return GetHistoryBuff().Initialize();
}

void Editable::LineEditor::Begin()
{
	Clear();
	editingFlag_ = true;
}

void Editable::LineEditor::Finish()
{
	AddHistory(GetPointerBegin());
	editingFlag_ = false;
}

void Editable::LineEditor::Clear()
{
	iByteCursor_ = 0;
	buff_[0] = '\0';
}

bool Editable::LineEditor::InsertChar(char ch)
{
	if (::strlen(buff_) + 1 < sizeof(buff_)) {
		char *p = GetPointer(iByteCursor_);
		::memmove(p + 1, p, ::strlen(p) + 1);
		*p = ch;
		iByteCursor_++;
		uint32_t codeUTF32;
		if (decoder_.FeedChar(ch, &codeUTF32)) {
			EndHistory();
			return true;
		}
	}
	return false;
}

bool Editable::LineEditor::DeleteChar()
{
	char *p = GetPointer(iByteCursor_);
	if (*p) {
		int bytes;
		UTF8::ToUTF32(p, &bytes);
		::memmove(p, p + bytes, ::strlen(p + bytes) + 1);
		EndHistory();
		return true;
	}
	return false;
}

bool Editable::LineEditor::DeleteLastChar()
{
	int iByte = GetIByteEnd();
	if (MoveBackward(&iByte)) {
		DeleteToEnd(iByte);
		if (iByteCursor_ > iByte) iByteCursor_ = iByte;
		return true;
	}
	return false;
}

bool Editable::LineEditor::MoveForward(int* piByte)
{
	char *pStart = GetPointer(*piByte);
	if (*pStart) {
		char* p = pStart + 1;
		for ( ; (*p & 0xc0) == 0x80; p++) ;
		*piByte += p - pStart;
		return true;
	}
	return false;
}

bool Editable::LineEditor::MoveBackward(int* piByte)
{
	if (*piByte > 0) {
		char* pStart = GetPointer(*piByte);
		char* p = pStart - 1;
		for ( ; p != buff_ && (*p & 0xc0) == 0x80; p--) ;
		*piByte -= pStart - p;
		return true;
	}
	return false;
}

bool Editable::LineEditor::MoveHome()
{
	iByteCursor_ = 0;
	return true;
}

bool Editable::LineEditor::MoveEnd()
{
	iByteCursor_ = ::strlen(buff_);
	return true;
}

bool Editable::LineEditor::DeleteToHome()
{
	if (iByteCursor_ > 0) {
		char* p = GetPointer(iByteCursor_);
		int bytes = ::strlen(p) + 1;
		::memmove(buff_, p, bytes);
		iByteCursor_ = 0;
		EndHistory();
		return true;
	}
	return false;
}

bool Editable::LineEditor::DeleteToEnd(int iByte)
{
	char* p = GetPointer(iByte);
	if (*p) {
		*p = '\0';
		EndHistory();
		return true;
	}
	return false;
}

bool Editable::LineEditor::AddHistory(const char* str)
{
	const char*p = str;
	for ( ; std::isspace(*p); p++) ;
	if (!*p) return false;
	char* pLine = GetHistoryBuff().GetLineLast();
	if (GetHistoryBuff().PrevLine(&pLine)) {
		WrappedCharFeeder charFeeder(GetHistoryBuff().CreateCharFeeder(pLine));
		if (charFeeder.Compare(str) == 0) return false;
	}
	GetHistoryBuff().Print(str).PutChar('\0');
	GetHistoryBuff().MoveLineLastHere().PlaceChar('\0');
	return true;
}

bool Editable::LineEditor::MoveHistoryPrev()
{
	bool updateFlag = false;
	if (GetHistoryBuff().GetLineMark()) {
		updateFlag = GetHistoryBuff().MoveLineMarkUp();
	} else {
		GetHistoryBuff().SetLineMark(GetHistoryBuff().GetLineLast());
		updateFlag = GetHistoryBuff().MoveLineMarkUp();
		if (updateFlag) {
			pLineStop_History_ = GetHistoryBuff().GetLineMark();
		}
	}
	if (updateFlag) ReplaceWithHistory();
	return updateFlag;
}

bool Editable::LineEditor::MoveHistoryNext()
{
	if (GetHistoryBuff().GetLineMark() && GetHistoryBuff().MoveLineMarkDown(pLineStop_History_)) {
		ReplaceWithHistory();
		return true;
	}
	return false;
}

void Editable::LineEditor::EndHistory()
{
	GetHistoryBuff().RemoveLineMark();
}

void Editable::LineEditor::ReplaceWithHistory()
{
	const char* pLineMark = GetHistoryBuff().GetLineMark();
	WrappedCharFeeder charFeeder(GetHistoryBuff().CreateCharFeeder(pLineMark));
	char* buffp = buff_;
	for ( ; ; buffp++, charFeeder.Forward()) {
		char ch = charFeeder.Get();
		*buffp = ch;
		if (!ch) break;
	}
	iByteCursor_ = buffp - buff_;
}

}
