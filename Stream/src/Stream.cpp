//==============================================================================
// Stream.cpp
//==============================================================================
#include <string.h>
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
Stream* Stream::pStdout = &StreamDumb::Instance;
Stream* Stream::pStdin = &StreamDumb::Instance;

Printable& Stream::ClearScreen()
{
	// do nothing
	return *this;
}

Printable& Stream::FlushScreen()
{
	// do nothing
	return *this;
}

Printable& Stream::Locate(int col, int row)
{
	// do nothing
	return *this;
}

Printable& Stream::Print(const char* str)
{
	Write(str, ::strlen(str));
	return *this;
}

//------------------------------------------------------------------------------
// StreamDumb
//------------------------------------------------------------------------------
StreamDumb StreamDumb::Instance;

//------------------------------------------------------------------------------
// Utility Function
//------------------------------------------------------------------------------
Stream& VPrintf(const char* format, va_list args)
{
	Stream::pStdout->VPrintf(format, args);
	return *Stream::pStdout;
}

Stream& Printf(const char* format, ...)
{
	va_list args;
	va_start(args, format);
	Stream::pStdout->VPrintf(format, args);
	va_end(args);
	return *Stream::pStdout;
}

Stream& Print(const char* str)
{
	Stream::pStdout->Print(str);
	return *Stream::pStdout;
}

}
