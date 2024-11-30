//==============================================================================
// jxglib/Stream.h
//==============================================================================
#ifndef PICO_JXGLIB_STREAM_H
#define PICO_JXGLIB_STREAM_H
#include "pico/stdlib.h"
#include "jxglib/Printable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
class Stream : public Printable {
public:
	static Stream* pStdout;
	static Stream* pStdin;
public:
	virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) = 0;
	virtual bool Write(const void* buff, int bytesBuff) = 0;
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& FlushScreen() override;
	virtual Printable& Locate(int col, int row) override;
	virtual Printable& Print(const char* str) override;
};

//------------------------------------------------------------------------------
// StreamDumb
//------------------------------------------------------------------------------
class StreamDumb : public Stream {
public:
	static StreamDumb Instance;
public:
	virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) { *pBytesRead = 0; return true; }
	virtual bool Write(const void* buff, int bytesBuff) { return true; }
};

//------------------------------------------------------------------------------
// Utility Function
//------------------------------------------------------------------------------
Stream& Print(const char* str);
Stream& VPrintf(const char* format, va_list args);
Stream& Printf(const char* format, ...);

}

#endif
