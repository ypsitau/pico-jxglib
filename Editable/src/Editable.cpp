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

//------------------------------------------------------------------------------
// Editable::LineEditor
//------------------------------------------------------------------------------
Editable::LineEditor::LineEditor(int bytesHistoryBuff) :
	editingFlag_{false}, iCharCursor_{0}, historyBuff_(bytesHistoryBuff), pLineStop_History_{nullptr}
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

int Editable::LineEditor::GetPosition(int iChar) const
{
	return iChar;
}

void Editable::LineEditor::Clear()
{
	iCharCursor_ = 0;
	buff_[0] = '\0';
}

bool Editable::LineEditor::InsertChar(char ch)
{
	if (::strlen(buff_) + 1 < sizeof(buff_)) {
		char *p = GetPointer(iCharCursor_);
		::memmove(p + 1, p, ::strlen(p) + 1);
		*p = ch;
		iCharCursor_++;
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
	char *p = GetPointer(iCharCursor_);
	if (*p) {
		int bytes;
		UTF8Decoder::ToUTF32(p, &bytes);
		::memmove(p, p + bytes, ::strlen(p + bytes) + 1);
		EndHistory();
		return true;
	}
	return false;
}

bool Editable::LineEditor::DeleteLastChar()
{
	int iChar = GetICharEnd();
	if (MoveBackward(&iChar)) {
		DeleteToEnd(iChar);
		if (iCharCursor_ > iChar) iCharCursor_ = iChar;
		return true;
	}
	return false;
}

bool Editable::LineEditor::MoveForward(int* piChar)
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

bool Editable::LineEditor::MoveBackward(int* piChar)
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

bool Editable::LineEditor::MoveHome()
{
	iCharCursor_ = 0;
	return true;
}

bool Editable::LineEditor::MoveEnd()
{
	iCharCursor_ = ::strlen(buff_);
	return true;
}

bool Editable::LineEditor::DeleteToHome()
{
	if (iCharCursor_ > 0) {
		char* p = GetPointer(iCharCursor_);
		int bytes = ::strlen(p) + 1;
		::memmove(buff_, p, bytes);
		iCharCursor_ = 0;
		EndHistory();
		return true;
	}
	return false;
}

bool Editable::LineEditor::DeleteToEnd(int iChar)
{
	char* p = GetPointer(iChar);
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
	iCharCursor_ = buffp - buff_;
}

}
