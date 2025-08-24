//==============================================================================
// jxglib/JSON.h
// https://www.json.org/json-en.html
//==============================================================================
#ifndef PICO_JXGLIB_JSON_H
#define PICO_JXGLIB_JSON_H
#include <ctype.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// JSON
//------------------------------------------------------------------------------
class JSON {
public:
	class Handler {
	public:
		virtual void OnString(const char* str) = 0;
		virtual void OnStringNamed(const char* objectName, const char* str) = 0;
		virtual void OnNumber(double num) = 0;
		virtual void OnNumberNamed(const char* objectName, double num) = 0;
		virtual void OnSymbol(const char* symbol) = 0;
		virtual void OnSymbolNamed(const char* objectName, const char* symbol) = 0;
		virtual void OnObjectStart() = 0;
		virtual void OnObjectStartNamed(const char* objectName) = 0;
		virtual void OnObjectEnd() = 0;
		virtual void OnArrayStart() = 0;
		virtual void OnArrayStartNamed(const char* objectName) = 0;
		virtual void OnArrayEnd() = 0;
	};
	class Handler_Debug : public Handler {
	private:
		int indentLevel_;
	public:
		Handler_Debug() : indentLevel_{0} {}
	public:
		void Reset() { indentLevel_ = 0; }
	public:
		virtual void OnString(const char* str) override;
		virtual void OnStringNamed(const char* objectName, const char* str) override;
		virtual void OnNumber(double num) override;
		virtual void OnNumberNamed(const char* objectName, double num) override;
		virtual void OnSymbol(const char* symbol) override;
		virtual void OnSymbolNamed(const char* objectName, const char* symbol) override;
		virtual void OnObjectStart() override;
		virtual void OnObjectStartNamed(const char* objectName) override;
		virtual void OnObjectEnd() override;
		virtual void OnArrayStart() override;
		virtual void OnArrayStartNamed(const char* objectName) override;
		virtual void OnArrayEnd() override;
	};
public:
	enum class Stat {
		Value,
		SeekNextValue,
		SeekObjectValue,
		String,
		String_Escape,
		String_Unicode,
		Number,
		Number_IntegerTop,
		Number_Integer,
		Number_AfterInteger,
		Number_Fraction,
		Number_AfterFraction,
		Number_ExponentSign,
		Number_ExponentTop,
		Number_Exponent,
		Symbol,
	};
	enum class Context {
		ArrayValue,
		ObjectName,
		ObjectValue,
	};
protected:
	Stat stat_;
	bool strictFlag_;
	Context context_;
	int iLine_;
	int iBuff_;
	char buff_[256];
	char objectName_[256];
	int iGroupStack_;
	char groupStack_[32];
	int nCodeDigits_;
	uint32_t codeAccum_;
	const char* errorMsg_;
	Handler* pHandler_;
public:
	JSON();
public:
	void Reset();
public:
	const char* GetErrorMsg() const { return errorMsg_; }
	void SetHandler(Handler& handler) { pHandler_ = &handler; }
public:
	bool Parse(const char* str);
	bool Parse(Stream& stream);
	bool FeedChar(char ch);
private:
	void ClearBuff() { iBuff_ = 0; buff_[0] = '\0'; }
	bool AddBuff(char ch);
	const char* TerminateBuff() { buff_[iBuff_] = '\0'; return buff_; }
private:
	void ReportString();
	void ReportNumber();
	void ReportSymbol();
	void ReportObjectStart();
	void ReportObjectEnd();
	void ReportArrayStart();
	void ReportArrayEnd();
public:
	static bool IsSymbolCharFirst(char ch) { return ::isalpha(ch) || ch == '_'; }
	static bool IsSymbolChar(char ch) { return ::isalnum(ch) || ch == '_'; }
	static bool IsDigit(char ch) { return ::isdigit(ch); }
	static bool IsDigit1_9(char ch) { return ch >= '1' && ch <= '9'; }
	static bool IsWhitespace(char ch) { return ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t'; }
};

}

#endif
