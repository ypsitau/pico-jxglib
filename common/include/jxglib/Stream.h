//==============================================================================
// jxglib/Stream.h
//==============================================================================
#ifndef PICO_JXGLIB_STREAM_H
#define PICO_JXGLIB_STREAM_H
#include <stdio.h>
#include <utility>
#include "pico/stdlib.h"
#include "jxglib/Tickable.h"
#include "jxglib/Printable.h"
#include "jxglib/Readable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
class Stream : public Printable, public Readable {
public:
	//virtual int Read(void* buff, int bytesBuff) = 0;
	virtual int Write(const void* buff, int bytesBuff) = 0;
public:
	bool WriteTo(Stream&& streamTo);
	bool WriteTo(Stream& streamTo) { return WriteTo(std::move(streamTo)); }
	bool PrintTo(Printable&& printable);
	bool PrintTo(Printable& printable) { return PrintTo(std::move(printable)); }
	bool WriteTo(FILE* fp);
	bool PrintTo(FILE* fp) { return WriteTo(fp); }
	bool WriteFrom(Stream&& streamFrom) { return streamFrom.WriteTo(*this); }
	bool WriteFrom(Stream& streamFrom) { return streamFrom.WriteTo(*this); }
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override { /* do nothing */ return *this; }
	virtual bool Flush() override { /* do nothing */ return true; }
	virtual Printable& Locate(int col, int row) override { /* do nothing */ return *this; }
	virtual Printable& PutCharRaw(char ch) override { Write(&ch, sizeof(char)); return *this; }
};

}

#endif
