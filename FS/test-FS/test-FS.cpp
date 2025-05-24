#include <cassert>
#include <cstdio>
#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/FS.h"

using namespace jxglib;

void test_DoesMatchWildcard()
{
	struct TestCase {
		const char* pattern;
		const char* str;
		bool expected;
	} cases[] = {
		{ "file.txt",	"file.txt",							true	},
		{ "file.*",		"file.txt",							true	},
		{ "*.txt",		"file.txt",							true	},
		{ "f?le.txt",	"file.txt",							true	},
		{ "f*e.txt",	"file.txt",							true	},
		{ "*.jpg",		"file.txt",							false	},
		{ "*",			"file.txt",							true	},
		{ "",			"",									true	},
		{ "",			"abc",								false	},
		{ "*",			"",									true	},
		{ "file.???",	"file.txt",							true	},
		{ "file.???",	"file.tx",							false	},
		{ "*.*",		"file.txt",							true	},
		{ "*.*",		"file",								false	},
		{ "*le.tx?",	"file.txt",							true	},
		{ "*le.tx?",	"file.tx",							false	},
		{ "f*.*",		"file.txt",							true	},
		{ "f*.*",		"f.txt",							true	},
		{ "f*.*",		"f",								false	},
		{ "*.*",		".hidden",							false	},
		{ "*",			".hidden",							true	},
		{ ".*",			".hidden",							true	},
		{ ".h*",		".hidden",							true	},
		{ ".g*",		".hidden",							false	},
		{ "*.*",		"normal.file",						true	},
		{ "*.*",		"noext",							false	},
		{ "*",			"noext",							true	},
		{ "a*b*c",		"abc",								true	},
		{ "a*b*c",		"axybzc",							true	},
		{ "a*b*c",		"ac",								false	},
		{ "a*b*c",		"abdc",								true	},
		{ "a*b*c",		"abdcx",							false	},
		{ "a*b*c",		"abdcxzy",							false	},
		{ "a*b*c",		"abdcxyz",							false	},
		{ "a*b*c",		"abdcxyzabc",						false	},
		{ "a*b*c",		"abdcxyzabcde",						false	},
		{ "a*b*c",		"abdcxyzabcde1234567890",			false	},
		{ "a*b*c",		"abdcxyzabcde1234567890abcdefg",	false	},
		{ "a*b*c",		"abdc",								true	},
		{ "a*b*c",		"ab",								false	},
		{ "a?c",		"abc",								true	},
		{ "a?c",		"ac",								false	},
	};

	for (const auto& tc : cases) {
		bool result = FS::DoesMatchWildcard(tc.pattern, tc.str);
		::printf("%-16s %-16s expected:%-5s result:%-5s%s\n", tc.pattern, tc.str,
			tc.expected? "true" : "false", result? "true" : "false", (result == tc.expected) ? "" : " ***");
	}
}
int main()
{
	::stdio_init_all();
	test_DoesMatchWildcard();
	for (;;) ::tight_loop_contents();
}
