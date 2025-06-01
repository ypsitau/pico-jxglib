//==============================================================================
// jxglib/Readable.h
//==============================================================================
#ifndef PICO_JXGLIB_READABLE_H
#define PICO_JXGLIB_READABLE_H
#include <string.h>
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Readable
//------------------------------------------------------------------------------
class Readable {
public:
	virtual ~Readable() = default;
public:
	virtual int Read(void* buff, int bytesBuff) = 0;
	virtual int ReadChar();
	virtual int ReadLine(char* buff, int bytesBuff, bool elimEOLFlag = true);
	//virtual int ReadUntil(char* buffer, int size, char terminator) = 0;
};

//------------------------------------------------------------------------------
// ReadableDumb
//------------------------------------------------------------------------------
class ReadableDumb : public Readable {
public:
	static ReadableDumb Instance;
public:
	virtual int Read(void* buff, int bytesBuff) override { return 0; }
};

}

#endif
