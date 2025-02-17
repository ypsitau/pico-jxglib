//==============================================================================
// jxglib/Stream.h
//==============================================================================
#ifndef PICO_JXGLIB_STREAM_H
#define PICO_JXGLIB_STREAM_H
#include <utility>
#include "pico/stdlib.h"
#include "jxglib/Printable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
class Stream : public Printable {
public:
	virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) = 0;
	virtual bool Write(const void* buff, int bytesBuff) = 0;
public:
	bool WriteFrom(Stream&& streamFrom);
	bool WriteFrom(Stream& streamFrom) { return WriteFrom(std::move(streamFrom)); }
	bool PrintFrom(Stream&& streamFrom);
	bool PrintFrom(Stream& streamFrom) { return PrintFrom(std::move(streamFrom)); }
	bool WriteTo(FILE* fp);
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override { /* do nothing */ return *this; }
	virtual Printable& RefreshScreen() override { /* do nothing */ return *this; }
	virtual Printable& Locate(int col, int row) override { /* do nothing */ return *this; }
	virtual Printable& PutChar(char ch) override { Write(&ch, sizeof(char)); return *this; }
};

}

#endif
