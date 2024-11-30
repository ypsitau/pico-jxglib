//==============================================================================
// Stream.cpp
//==============================================================================
#include <string.h>
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
Printable& Stream::Clear()
{
	// do nothing
	return *this;
}

Printable& Stream::Flush()
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

}
