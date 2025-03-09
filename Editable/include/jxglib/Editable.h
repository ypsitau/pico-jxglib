//==============================================================================
// jxglib/Editable.h
//==============================================================================
#ifndef PICO_JXGLIB_EDITABLE_H
#define PICO_JXGLIB_EDITABLE_H
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/LineBuff.h"

namespace jxglib {

class Printable;

//------------------------------------------------------------------------------
// Editable
//------------------------------------------------------------------------------
class Editable {
public:
	static const int EditBuffSize = 128;
public:
	class LineEditor {
	private:
		bool editingFlag_;
		int iByteCursor_;
		char buff_[EditBuffSize];
		UTF8::Decoder decoder_;
		LineBuff historyBuff_;
		const char* pLineStop_History_;
	public:
		LineEditor(int bytesHistoryBuff);
	public:
		bool Initialize();
		void Begin();
		void Finish();
		bool IsEditing() { return editingFlag_; }
		bool IsEmpty() const { return buff_[0] == '\0'; }
		int GetIByteCursor() const { return iByteCursor_; }
		int GetIByteEnd() const { return ::strlen(buff_); }
		char* GetPointer(int iByte) { return buff_ + iByte; }
		char* GetPointerBegin() { return buff_; }
		char* GetPointerEnd() { return buff_ + GetIByteEnd(); }
		const char* GetPointer(int iByte) const { return buff_ + iByte; }
		const char* GetPointerAtCursor() const { return buff_ + iByteCursor_; }
		int CountFollowingChars() const { return CountFollowingChars(iByteCursor_); }
		int CountFollowingChars(int iByte) const { return UTF8::CountChars(GetPointer(iByte)); }
		void Clear();
		bool InsertChar(char ch);
		bool DeleteChar();
		bool Back() { return MoveBackward() && DeleteChar(); }
		bool DeleteLastChar();
		bool MoveForward() { return MoveForward(&iByteCursor_); }
		bool MoveBackward() { return MoveBackward(&iByteCursor_); }
		bool MoveForward(int* pIByte);
		bool MoveBackward(int* pIByte);
		bool MoveHome();
		bool MoveEnd();
		bool DeleteToHome();
		bool DeleteToEnd() { return DeleteToEnd(iByteCursor_); }
		bool DeleteToEnd(int iByte);
	public:
		bool AddHistory(const char* str);
		bool MoveHistoryPrev();
		bool MoveHistoryNext();
		void EndHistory();
	private:
		void ReplaceWithHistory();
	public:
		LineBuff& GetHistoryBuff() { return historyBuff_; }
		const LineBuff& GetHistoryBuff() const { return historyBuff_; }
	};
private:
	LineEditor lineEditor_;
public:
	Editable(int byteshistoryBuff = 512);
public:
	bool Initialize();
	LineEditor& GetLineEditor() { return lineEditor_; }
	char* ReadLine(const char* prompt);
public:
	virtual Printable& GetPrintable() = 0;
	virtual Editable& Edit_Begin() = 0;
	virtual Editable& Edit_Finish(char chEnd = '\0') = 0;
	virtual Editable& Edit_InsertChar(int ch) = 0;
	virtual Editable& Edit_DeleteChar() = 0;
	virtual Editable& Edit_Back() = 0;
	virtual Editable& Edit_MoveForward() = 0;
	virtual Editable& Edit_MoveBackward() = 0;
	virtual Editable& Edit_MoveHome() = 0;
	virtual Editable& Edit_MoveEnd() = 0;
	virtual Editable& Edit_DeleteToHome() = 0;
	virtual Editable& Edit_DeleteToEnd() = 0;
	virtual Editable& Edit_MoveHistoryPrev() = 0;
	virtual Editable& Edit_MoveHistoryNext() = 0;
};
		
}

#endif
