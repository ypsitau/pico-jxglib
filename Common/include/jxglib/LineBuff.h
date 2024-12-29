//==============================================================================
// jxglib/LineBuff.h
//==============================================================================
#ifndef PICO_JXGLIB_LINEBUFF_H
#define PICO_JXGLIB_LINEBUFF_H
#include <stdio.h>
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LineBuff
//------------------------------------------------------------------------------
class LineBuff {
private:
	char* buffTop_;
	char* buffBottom_;
	char* pWrite_;
	char* pRead_;
	char* pLineTop_;
	char* pLineCur_;
public:
	LineBuff();
public:
	bool Allocate(int bytes);
	const char* PrevLine(const char* p) const;
	const char* NextLine(const char* p) const;
};

}

#endif
