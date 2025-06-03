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
	enum class Stat { Head, Quoted, QuotedEscape, NoQuoted, NoQuotedEscape };
private:
	const char* errorMsg_;
	const char** specialTokens_;
	int nSpecialTokens_;
public:
	Tokenizer(const char** specialTokens = nullptr, int nSpecialTokens = 0);
public:
	bool Tokenize(char* str, int bytesStr, char* tokenTbl[], int* pnToken);
	const char* GetErrorMsg() const { return errorMsg_; }
	const char* FindLastToken(const char* str);
private:
	int FindSpecialToken(const char* p) const;
	static void DeleteChar(char* p);
	static bool InsertChar(char* str, int bytesStr, char* p);

};

}

#endif
