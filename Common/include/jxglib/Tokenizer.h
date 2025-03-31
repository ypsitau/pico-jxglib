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
	Tokenizer();
public:
	bool Tokenize(char* str, int *pnToken, char* tokenTbl[], const char** pErrMsg);
public:
	static void DeleteChar(char* p) { if (*p) ::memmove(p, p + 1, ::strlen(p + 1) + 1); }
};

}

#endif
