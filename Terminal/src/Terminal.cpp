//==============================================================================
// Terminal.cpp
//==============================================================================
#include <cctype>
#include <memory.h>
#include "jxglib/Terminal.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
Terminal::Terminal(int bytesHistoryBuff, Keyboard& keyboard) :
	lineEditor_{bytesHistoryBuff}, editableFlag_{true}, pKeyboard_{&keyboard}, pCompletionProvider_{nullptr}
{
}

Terminal& Terminal::Initialize()
{
	GetLineEditor().Initialize();
	return *this;
}

char* Terminal::ReadLine(const char* prompt)
{
	GetPrintable().Print(prompt).Flush();
	ClearBreak();
	Edit_Begin();
	for (;;) {
		Tickable::Tick();
		if (!GetLineEditor().IsEditing()) break;
	}
	return GetLineEditor().GetPointerBegin();
}

void Terminal::ReadLine_Begin(const char* prompt)
{
	GetPrintable().Print(prompt).Flush();
	ClearBreak();
	Edit_Begin();
}

char* Terminal::ReadLine_Process()
{
	return GetLineEditor().IsEditing()? nullptr : GetLineEditor().GetPointerBegin();
}

bool Terminal::ProcessKeyData(const KeyData& keyData)
{
	if (keyData.IsKeyCode()) {
		switch (keyData.GetKeyCode()) {
		case VK_BACK:		Edit_Back();			return true;
		case VK_RETURN:		Edit_Finish('\n');		return true;
		//case VK_ESCAPE:	Edit_Clear();			return true;
		case VK_END:		Edit_MoveEnd();			return true;
		case VK_HOME:		Edit_MoveHome();		return true;
		case VK_LEFT:		Edit_MoveBackward();	return true;
		case VK_UP:			Edit_MoveHistoryPrev();	return true;
		case VK_RIGHT:		Edit_MoveForward();		return true;
		case VK_DOWN:		Edit_MoveHistoryNext();	return true;
		case VK_DELETE:		Edit_DeleteChar();		return true;
		case VK_TAB:		Edit_Completion();		return true;
		default: break;
		}
	} else if (keyData.GetChar() < 0x20) {
		switch (keyData.GetChar() + '@') {
		case 'A':			Edit_MoveHome();		return true;
		case 'B':			Edit_MoveBackward();	return true;
		case 'C':			SignalBreak();			return true;
		case 'D':			Edit_DeleteChar();		return true;
		case 'E':			Edit_MoveEnd();			return true;
		case 'F':			Edit_MoveForward();		return true;
		case 'G':			break;
		case 'H':			Edit_Back();			return true;
		case 'I':			Edit_Completion();		return true;
		case 'J':			Edit_Finish('\n');		return true;
		case 'K':			Edit_DeleteToEnd();		return true;
		case 'L':			break;
		case 'M':			Edit_Finish('\n');		return true;
		case 'N':			Edit_MoveHistoryNext();	return true;
		case 'O':			break;
		case 'P':			Edit_MoveHistoryPrev();	return true;
		case 'Q':			break;
		case 'R':			break;
		case 'S':			break;
		case 'T':			break;
		case 'U':			Edit_DeleteToHome();	return true;
		case 'V':			break;
		case 'W':			break;
		case 'X':			break;
		case 'Y':			break;
		case 'Z':			break;
		default: break;
		}
	} else {
		Edit_InsertChar(keyData.GetChar());
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------
// TerminalDumb
//------------------------------------------------------------------------------
TerminalDumb TerminalDumb::Instance;

TerminalDumb::TerminalDumb() : Terminal(0, KeyboardDumb::Instance)
{
}

//------------------------------------------------------------------------------
// Terminal::LineEditor
//------------------------------------------------------------------------------
Terminal::LineEditor::LineEditor(int bytesHistoryBuff) : editingFlag_{false}, enableHistoryFlag_{true},
	iByteCursor_{0}, historyBuff_(bytesHistoryBuff), pLineStop_History_{nullptr}, pTokenizer_{&Tokenizer::Default}
{
	buff_[0] = '\0';
}

void Terminal::LineEditor::Initialize()
{
	if (!GetHistoryBuff().Initialize()) {
		::panic("failed to initialize the history buffer in Terminal::LineEditor::Initialize()\n");
	}
}

void Terminal::LineEditor::Begin()
{
	Clear();
	editingFlag_ = true;
}

void Terminal::LineEditor::Finish()
{
	AddHistory(GetPointerBegin());
	EndHistory();
	editingFlag_ = false;
}

int Terminal::LineEditor::GetIByteToCompletion() const
{
	int iByte = pTokenizer_->FindLastToken(buff_) - buff_;
	return iByte;
}

bool Terminal::LineEditor::Clear()
{
	if (buff_[0]) {
		iByteCursor_ = 0;
		buff_[0] = '\0';
		return true;
	}
	return false;
}

bool Terminal::LineEditor::InsertChar(char ch)
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

bool Terminal::LineEditor::ReplaceWithCompletion(int iByte, const char* str)
{
	int bytes = ::strlen(str);
	char* p = GetPointer(iByte);
	if (DoesContainSpace(str)) {
		if (iByte + bytes + 1 + 2 >= sizeof(buff_)) return false;
		*p++ = '"';
		::memcpy(p, str, bytes); p += bytes;
		*p++ = '"';
		*p++ = '\0';
		iByteCursor_ = iByte + bytes + 2;
	} else {
		if (iByte + bytes + 1 >= sizeof(buff_)) return false;
		::memcpy(p, str, bytes + 1);
		iByteCursor_ = iByte + bytes;
	}
	EndHistory();
	return true;
}

bool Terminal::LineEditor::DeleteChar()
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

bool Terminal::LineEditor::DeleteLastChar()
{
	int iByte = GetIByteEnd();
	if (MoveBackward(&iByte)) {
		DeleteToEnd(iByte);
		if (iByteCursor_ > iByte) iByteCursor_ = iByte;
		return true;
	}
	return false;
}

bool Terminal::LineEditor::MoveForward(int* piByte)
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

bool Terminal::LineEditor::MoveBackward(int* piByte)
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

bool Terminal::LineEditor::MoveHome()
{
	iByteCursor_ = 0;
	return true;
}

bool Terminal::LineEditor::MoveEnd()
{
	iByteCursor_ = ::strlen(buff_);
	return true;
}

bool Terminal::LineEditor::DeleteToHome()
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

bool Terminal::LineEditor::DeleteToEnd(int iByte)
{
	char* p = GetPointer(iByte);
	if (*p) {
		*p = '\0';
		EndHistory();
		return true;
	}
	return false;
}

bool Terminal::LineEditor::AddHistory(const char* str)
{
	if (!enableHistoryFlag_) return false;
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

bool Terminal::LineEditor::MoveHistoryPrev()
{
	if (!enableHistoryFlag_) return false;
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

bool Terminal::LineEditor::MoveHistoryNext()
{
	if (!enableHistoryFlag_) return false;
	if (GetHistoryBuff().GetLineMark() && GetHistoryBuff().MoveLineMarkDown(1, pLineStop_History_)) {
		ReplaceWithHistory();
		return true;
	}
	return false;
}

void Terminal::LineEditor::EndHistory()
{
	if (!enableHistoryFlag_) return;
	GetHistoryBuff().RemoveLineMark();
}

void Terminal::LineEditor::ReplaceWithHistory()
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

//------------------------------------------------------------------------------
// Terminal::CompletionProvider
//------------------------------------------------------------------------------
void Terminal::CompletionProvider::Start(const Terminal& terminal, int iByte)
{
	iByte_ = iByte;
	::strcpy(strHint_, terminal.GetLineEditor().GetPointer(iByte));
	StartCompletion();
}

void Terminal::CompletionProvider::End()
{
	EndCompletion();
	iByte_ = -1;
}

//------------------------------------------------------------------------------
// Terminal::ReadableKeyboard
//------------------------------------------------------------------------------
int Terminal::ReadableKeyboard::Read(void* buff, int bytesBuff)
{
	terminal_.ClearBreak();
	terminal_.Edit_Begin();
	for (;;) {
		Tickable::Tick();
		if (!terminal_.GetLineEditor().IsEditing()) break;
		if (terminal_.IsBreak()) {
			terminal_.Print("^C\n");
			return -1;
		}
	}
	char* str = terminal_.GetLineEditor().GetPointerBegin();
	int bytes = ::strlen(str);
	::memcpy(buff, str, bytes);
	char* buffp = static_cast<char*>(buff) + bytes;
	*buffp++ = '\n'; bytes++;
	return bytes;
}

}
