//==============================================================================
// Stream.cpp
//==============================================================================
#include <string.h>
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
bool Stream::WriteFrom(Stream&& streamFrom)
{
	int bytesRead;
	char buff[1024];
	while (streamFrom.Read(buff, sizeof(buff), &bytesRead)) {
		if (!Write(buff, bytesRead)) return false;
	}
	return true;
}

bool Stream::PrintFrom(Stream&& streamFrom)
{
	int bytesRead;
	char buff[1024];
	while (streamFrom.Read(buff, sizeof(buff) - 1, &bytesRead)) {
		buff[bytesRead] = '\0';
		Print(buff);
	}
	return true;
}

}
