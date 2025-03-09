#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

void PrintDecode(const char* str)
{
	UTF8::Decoder decoder;
	for (const char* p = str; *p; p++) {
		uint32_t codeUTF32;
		if (decoder.FeedChar(*p, &codeUTF32)) ::printf("%-6x %s\n", codeUTF32, decoder.GetStringOrg());
	}
}

int main()
{
	::stdio_init_all();
	const char* string1 = "Hello, 世界!";
	const char* string2 = "";
	const char* string3 = "éàç";
	const char* string4 = "😀😂🤣";
	::printf("String 1: \"%s\", Character count: %d\n", string1, UTF8::CountChars(string1));
	::printf("String 2: \"%s\", Character count: %d\n", string2, UTF8::CountChars(string2));
	::printf("String 3: \"%s\", Character count: %d\n", string3, UTF8::CountChars(string3));
	::printf("String 4: \"%s\", Character count: %d\n", string4, UTF8::CountChars(string4));
	PrintDecode(string1);
	PrintDecode(string2);
	PrintDecode(string3);
	PrintDecode(string4);
}

