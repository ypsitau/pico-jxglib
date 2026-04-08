//==============================================================================
// Readable.cpp
//==============================================================================
#include "jxglib/Readable.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Readable
//------------------------------------------------------------------------------
int Readable::ReadChar()
{
	char ch;
	return (Read(&ch, sizeof(ch)) > 0)? ch : -1; // Return -1 if no character is read
}

int Readable::ReadLine(char* buff, int bytesBuff, bool elimEOLFlag)
{
	int bytesRead = 0;
	buff[0] = '\0';
	while (bytesRead < bytesBuff - 1) {
		int ch = ReadChar();
		if (ch < 0) { // End of stream or error
			if (bytesRead == 0) return -1; // No data read
			break;
		}
		if (ch == '\n') {
			if (!elimEOLFlag) buff[bytesRead++] = static_cast<char>(ch);
			break; // End of line
		}
		buff[bytesRead++] = static_cast<char>(ch);
	}
	buff[bytesRead] = '\0'; // Null-terminate the string
	return bytesRead;
}

//------------------------------------------------------------------------------
// ReadableDumb
//------------------------------------------------------------------------------
ReadableDumb ReadableDumb::Instance;

}
