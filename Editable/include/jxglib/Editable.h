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
		int iCharCursor_;
		char buff_[EditBuffSize];
		UTF8Decoder decoder_;
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
		int GetICharCursor() const { return iCharCursor_; }
		int GetICharEnd() const { return ::strlen(buff_); }
		char* GetPointer(int iChar) { return buff_ + iChar; }
		char* GetPointerBegin() { return buff_; }
		char* GetPointerEnd() { return buff_ + GetICharEnd(); }
		const char* GetPointer(int iChar) const { return buff_ + iChar; }
		const char* GetPointerAtCursor() const { return buff_ + iCharCursor_; }
		void Clear();
		bool InsertChar(char ch);
		bool DeleteChar();
		bool DeleteLastChar();
		bool MoveForward() { return MoveForward(&iCharCursor_); }
		bool MoveBackward() { return MoveBackward(&iCharCursor_); }
		bool MoveForward(int* pIChar);
		bool MoveBackward(int* pIChar);
		bool MoveHome();
		bool MoveEnd();
		bool DeleteToHome();
		bool DeleteToEnd() { return DeleteToEnd(iCharCursor_); }
		bool DeleteToEnd(int iChar);
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
public:
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
