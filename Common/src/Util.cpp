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

char ParseEscape(const char** pp, bool* pValidFlag)
{
	*pValidFlag = true;
	const char*& p = *pp;
	if (*p == '\0') return '\0';
	char ch = *p++;
	if (ch != '\\') return ch;
	if (*p == '\0') {
		*pValidFlag = false; // invalid escape sequence
		return '\0';
	}
	ch = *p++;
	if (ch == 'n') {
		return '\n';
	} else if (ch == 't') {
		return '\t';
	} else if (ch == 'r') {
		return '\r';
	} else if (ch == '"') {
		return '"';
	} else if (ch >= '0' && ch <= '7') {
		int rtn = ch - '0';
		for (int i = 0; i < 3 && *p >= '0' && *p <= '7'; ++p) {
			rtn = (rtn << 3) + (*p - '0');
		}
		if (rtn > 255) {
			*pValidFlag = false; // invalid escape sequence
			return '\0';
		}
		return static_cast<uint8_t>(rtn);
	} else if (ch == 'x') {
		int rtn = 0;
		int i = 0;
		for ( ; i < 2 && ::isxdigit(*p); ++p, ++i) {
			char ch = ::toupper(*p);
			rtn = (rtn << 4) + (::isdigit(ch)? (ch - '0') : (ch - 'A' + 10));
		}
		if (i == 0) {
			*pValidFlag = false; // invalid escape sequence
			return '\0';
		}
		return static_cast<uint8_t>(rtn);
	}
	return ch;
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

bool DoesContainSpace(const char* str)
{
	for (const char* p = str; *p; p++) {
		if (::isspace(*p)) return true;
	}
	return false;
}

const char* GetPlatformName()
{
#if defined(PICO_RP2040)
	return "RP2040";
#elif defined(PICO_RP2350)
	return "RP2350";
#else
	return "Unknown";
#endif
}

const char* GetCPUArchName()
{
#if defined(PICO_RP2040)
	return "ARM Cortex-M0+";
#elif defined(PICO_RP2350)
#if defined(__riscv)
	return "Hazard3 RISC-V";
#else
	return "ARM Cortex-M33";
#endif
#else
	return "Unknown";
#endif
}

}
