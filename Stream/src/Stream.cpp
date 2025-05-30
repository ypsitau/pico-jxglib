//==============================================================================
// Stream.cpp
//==============================================================================
#include <string.h>
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
bool Stream::WriteTo(Stream&& streamTo)
{
	int bytesRead;
	char buff[512];
	while ((bytesRead = Read(buff, sizeof(buff))) > 0) {
		if (streamTo.Write(buff, bytesRead) != bytesRead) return false;
	}
	return true;
}

bool Stream::PrintTo(Printable&& printable)
{
	int bytesRead;
	char buff[512];
	while ((bytesRead = Read(buff, sizeof(buff) - 1)) > 0) {
		buff[bytesRead] = '\0';
		printable.Print(buff);
	}
	return true;
}

bool Stream::WriteTo(FILE* fp)
{
	int bytesRead;
	char buff[512];
	while ((bytesRead = Read(buff, sizeof(buff))) > 0) {
		if (::fwrite(buff, sizeof(char), bytesRead, fp) < bytesRead) return false;
	}
	return true;
}

}
