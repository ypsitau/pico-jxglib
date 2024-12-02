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
	virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) = 0;
	virtual bool Write(const void* buff, int bytesBuff) = 0;
public:
	// virtual functions of Printable
	virtual Printable& ClearScreen() override;
	virtual Printable& FlushScreen() override;
	virtual Printable& Locate(int col, int row) override;
};

}

#endif
