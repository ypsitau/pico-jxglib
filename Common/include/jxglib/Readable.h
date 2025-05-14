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
	// Readable interface
	virtual ~Readable() = default;
	virtual int Read(char* buffer, int size) = 0;
	virtual int ReadLine(char* buffer, int size) = 0;
	virtual int ReadUntil(char* buffer, int size, char terminator) = 0;
};

#if 0
//------------------------------------------------------------------------------
// ReadableDumb
//------------------------------------------------------------------------------
class ReadableDumb : public Readable {
public:
	static ReadableDumb Instance;
public:
	virtual Printable& ClearScreen() override { return *this; }
	virtual Printable& RefreshScreen() override { return *this; }
	virtual Printable& Locate(int col, int row) override { return *this; }
	virtual Printable& PutChar(char ch) override { return *this; }
};
#endif

}

#endif
