//==============================================================================
// jxglib/Terminal.h
//==============================================================================
#ifndef PICO_JXGLIB_TERMINAL_H
#define PICO_JXGLIB_TERMINAL_H
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/LineBuff.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Terminal
//------------------------------------------------------------------------------
class Terminal : public Printable {
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
		const Tokenizer* pTokenizer_;
	public:
		LineEditor(int bytesHistoryBuff);
	public:
		void Initialize();
		void SetTokenizer(const Tokenizer& tokenizer) { pTokenizer_ = &tokenizer; }
		void Begin();
		void Finish();
		bool IsEditing() { return editingFlag_; }
		bool IsEmpty() const { return buff_[0] == '\0'; }
		int GetIByteCursor() const { return iByteCursor_; }
		int GetIByteEnd() const { return ::strlen(buff_); }
		int GetIByteToCompletion() const;
		char* GetPointer(int iByte) { return buff_ + iByte; }
		const char* GetPointer(int iByte) const { return buff_ + iByte; }
		char* GetPointerBegin() { return buff_; }
		char* GetPointerEnd() { return buff_ + GetIByteEnd(); }
		const char* GetPointerAtCursor() const { return buff_ + iByteCursor_; }
		int CountFollowingChars() const { return CountFollowingChars(iByteCursor_); }
		int CountFollowingChars(int iByte) const { return UTF8::CountChars(GetPointer(iByte)); }
		bool Clear();
		bool InsertChar(char ch);
		bool ReplaceWithCompletion(int iByte, const char* str);
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
	class CompletionProvider {
	private:
		int iByte_;
		char strHint_[EditBuffSize];
	public:
		CompletionProvider() : iByte_{-1} {}
	public:
		void Start(const Terminal& terminal, int iByte);
		void End();
		bool IsStarted() const { return iByte_ >= 0; }
		int GetIByte() const { return iByte_; }
		const char* GetHint() const { return strHint_; }
	public:
		virtual void StartCompletion() = 0;
		virtual void EndCompletion() = 0;
		virtual const char* NextCompletion() = 0;
	};
	class ReadableKeyboard : public Readable {
	private:
		Terminal& terminal_;
	public:
		ReadableKeyboard(Terminal& terminal) : terminal_{terminal} {}
	public:
		virtual int Read(void* buff, int bytesBuff) override;
	};
protected:
	LineEditor lineEditor_;
	bool editableFlag_;
	bool breakFlag_;
	Keyboard* pKeyboard_;
	CompletionProvider* pCompletionProvider_;
public:
	Terminal(int bytesHistoryBuff, Keyboard& keyboard);
	Terminal& Initialize();
	Terminal& SetTokenizer(const Tokenizer& tokenizer) { lineEditor_.SetTokenizer(tokenizer); return *this; }
	void SetEditable(bool editableFlag) { editableFlag_ = editableFlag; }
	bool IsEditable() const { return editableFlag_; }
	LineEditor& GetLineEditor() { return lineEditor_; }
	const LineEditor& GetLineEditor() const { return lineEditor_; }
	void SetCompletionProvider(CompletionProvider& completionProvider) {
		pCompletionProvider_ = &completionProvider;
	}
	void EndCompletion() { if (pCompletionProvider_) pCompletionProvider_->End(); }
	char* ReadLine(const char* prompt = "");
	void ReadLine_Begin(const char* prompt);
	char* ReadLine_Process();
	bool ProcessKeyData(const KeyData& keyData);
public:
	Terminal& AttachKeyboard(Keyboard& keyboard) { pKeyboard_ = &keyboard; return *this; }
	Keyboard& GetKeyboard() { return *pKeyboard_; }
	char GetChar() { return GetKeyboard().GetChar(); }
	bool GetKeyDataNB(KeyData* pKeyData) { return GetKeyboard().GetKeyDataNB(pKeyData); }
	int SenseKeyData(KeyData keyDataTbl[], int nKeysMax = 1) {
		return GetKeyboard().SenseKeyData(keyDataTbl, nKeysMax);
	}
	void ClearBreak() { breakFlag_ = false; }
	void SignalBreak() { breakFlag_ = true; }
	bool IsBreak() { bool rtn = breakFlag_; breakFlag_ = false; return rtn; }
public:
	virtual Printable& GetPrintable() = 0;
	virtual Terminal& Edit_Begin() = 0;
	virtual Terminal& Edit_Finish(char chEnd = '\0') = 0;
	virtual Terminal& Edit_InsertChar(int ch) = 0;
	virtual Terminal& Edit_DeleteChar() = 0;
	virtual Terminal& Edit_Back() = 0;
	virtual Terminal& Edit_MoveForward() = 0;
	virtual Terminal& Edit_MoveBackward() = 0;
	virtual Terminal& Edit_MoveHome() = 0;
	virtual Terminal& Edit_MoveEnd() = 0;
	virtual Terminal& Edit_Clear() = 0;
	virtual Terminal& Edit_DeleteToHome() = 0;
	virtual Terminal& Edit_DeleteToEnd() = 0;
	virtual Terminal& Edit_MoveHistoryPrev() = 0;
	virtual Terminal& Edit_MoveHistoryNext() = 0;
	virtual Terminal& Edit_Completion() = 0;
};

//------------------------------------------------------------------------------
// TerminalDumb
//------------------------------------------------------------------------------
class TerminalDumb : public Terminal {
public:
	static TerminalDumb Instance;
public:
	TerminalDumb();
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override { return *this; }
	virtual Printable& RefreshScreen() override { return *this; }
	virtual Printable& Locate(int col, int row) override { return *this; }
	virtual Printable& PutChar(char ch) override { return *this; }
	virtual Printable& PutCharRaw(char ch) override { return *this; }
	virtual Printable& Print(const char* str) override { return *this; }
	virtual Printable& PrintRaw(const char* str) override { return *this; }
	virtual Printable& Println(const char* str = "") override { return *this; }
	virtual Printable& PrintlnRaw(const char* str = "") override { return *this; }
	virtual Printable& VPrintf(const char* format, va_list args) override { return *this; }
	virtual Printable& VPrintfRaw(const char* format, va_list args) override { return *this; }
public:
	// virtual functions of Terminal
	virtual Printable& GetPrintable() override { return *this; }
	virtual Terminal& Edit_Begin() override { return *this; }
	virtual Terminal& Edit_Finish(char chEnd = '\0') override { return *this; }
	virtual Terminal& Edit_InsertChar(int ch) override { return *this; }
	virtual Terminal& Edit_DeleteChar() override { return *this; }
	virtual Terminal& Edit_Back() override { return *this; }
	virtual Terminal& Edit_MoveForward() override { return *this; }
	virtual Terminal& Edit_MoveBackward() override { return *this; }
	virtual Terminal& Edit_MoveHome() override { return *this; }
	virtual Terminal& Edit_MoveEnd() override { return *this; }
	virtual Terminal& Edit_Clear() override { return *this; }
	virtual Terminal& Edit_DeleteToHome() override { return *this; }
	virtual Terminal& Edit_DeleteToEnd() override { return *this; }
	virtual Terminal& Edit_MoveHistoryPrev() override { return *this; }
	virtual Terminal& Edit_MoveHistoryNext() override { return *this; }
	virtual Terminal& Edit_Completion() override { return *this; }
};

}

#endif
