//==============================================================================
// Stream.cpp
//==============================================================================
#include <string.h>
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
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

}
