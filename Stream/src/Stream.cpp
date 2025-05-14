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
	while ((bytesRead = streamFrom.Read(buff, sizeof(buff))) > 0) {
		if (Write(buff, bytesRead) != bytesRead) return false;
	}
	return true;
}

bool Stream::PrintFrom(Stream&& streamFrom)
{
	int bytesRead;
	char buff[1024];
	while ((bytesRead = streamFrom.Read(buff, sizeof(buff) - 1)) > 0) {
		buff[bytesRead] = '\0';
		Print(buff);
	}
	return true;
}

bool Stream::WriteTo(FILE* fp)
{
	int bytesRead;
	char buff[1024];
	while ((bytesRead = Read(buff, sizeof(buff))) > 0) {
		if (::fwrite(buff, sizeof(char), bytesRead, fp) < bytesRead) return false;
	}
	return true;
}

}
