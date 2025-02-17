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

bool Stream::WriteTo(FILE* fp)
{
	int bytesRead;
	char buff[1024];
	while (Read(buff, sizeof(buff), &bytesRead)) {
		if (::fwrite(buff, sizeof(char), bytesRead, fp) < bytesRead) return false;
	}
	return true;
}

}
