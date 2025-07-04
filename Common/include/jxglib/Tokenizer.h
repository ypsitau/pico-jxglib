//==============================================================================
// jxglib/Tokenizer.h
//==============================================================================
#ifndef PICO_JXGLIB_TOKENIZER_H
#define PICO_JXGLIB_TOKENIZER_H
#include <memory.h>
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Tokenizer
//------------------------------------------------------------------------------
class Tokenizer {
private:
	enum class Stat { Head, Quoted, QuotedEscape, NoQuoted, NoQuotedEscape, QuotedInNoQuoted, QuotedInNoQuotedEscape, AfterSpecial };
private:
	const char** specialTokens_;
	int nSpecialTokens_;
public:
	static const Tokenizer Default;
public:
	Tokenizer(const char** specialTokens = nullptr, int nSpecialTokens = 0);
public:
	bool Tokenize(char* str, int bytesStr, char* tokenTbl[], int* pnToken, const char** pErrorMsg = nullptr) const;
	const char* FindLastToken(const char* str) const;
private:
	int FindSpecialToken(const char* p) const;
	static void DeleteChar(char* p);
	static bool InsertChar(char* str, int bytesStr, char* p);
public:
	static bool IsQuoted(const char* str, const char* token) { return (token > str) && token[-1] == '"'; }
};

}

#endif
