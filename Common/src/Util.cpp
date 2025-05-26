//==============================================================================
// Util
//==============================================================================
#include <string.h>
#include <ctype.h>
#include "jxglib/Util.h"

namespace jxglib {

void WaitMSecSinceBoot(uint32_t msec)
{
	while (::to_ms_since_boot(::get_absolute_time()) < msec) ;
}

bool StartsWith(const char* str, const char* prefix)
{
	for ( ; *prefix; str++, prefix++) {
		if (*str != *prefix) return false;
	}
	return true;
}

bool StartsWithICase(const char* str, const char* prefix)
{
	for ( ; *prefix; str++, prefix++) {
		if (::toupper(*str) != ::toupper(*prefix)) return false;
	}
	return true;
}

bool EndsWith(const char* str, const char* suffix)
{
	const char* pStr = str + ::strlen(str);
	const char* pSuffix = suffix + ::strlen(suffix);
	while (pSuffix > suffix) {
		if (pStr <= str || *--pStr != *--pSuffix) return false;
	}
	return true;
}

bool EndsWithICase(const char* str, const char* suffix)
{
	const char* pStr = str + ::strlen(str);
	const char* pSuffix = suffix + ::strlen(suffix);
	while (pSuffix > suffix) {
		if (pStr <= str || ::toupper(*--pStr) != ::toupper(*--pSuffix)) return false;
	}
	return true;
}

}
