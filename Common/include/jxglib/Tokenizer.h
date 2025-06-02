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
public:
	enum class Mode { Normal, Shell };
private:
	enum class Stat { Head, Quoted, QuotedEscape, NoQuoted, NoQuotedEscape };
private:
	Mode mode_;
	const char* errorMsg_;
public:
	Tokenizer(Mode mode = Mode::Normal);
public:
	bool Tokenize(char* str, int bytesStr, char* tokenTbl[], int* pnToken);
	const char* GetErrorMsg() const { return errorMsg_; }
	const char* FindLastToken(const char* str);
public:
	static void DeleteChar(char* p);
	static bool InsertChar(char* str, int bytesStr, char* p);
};

}

#endif
