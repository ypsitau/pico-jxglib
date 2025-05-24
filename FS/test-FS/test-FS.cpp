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
	} testCases[] = {
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

	for (const auto& testCase : testCases) {
		bool result = FS::DoesMatchWildcard(testCase.pattern, testCase.str);
		::printf("%-16s %-16s expected:%-5s result:%-5s%s\n", testCase.pattern, testCase.str,
			testCase.expected? "true" : "false", result? "true" : "false", (result == testCase.expected) ? "" : " ***");
	}
}

void test_AppendPathName()
{
	struct TestCase {
		const char* base;
		const char* sub;
		const char* expected;
	} testCases[] = {
		{ "/dir",			"file.txt",			"/dir/file.txt",			},
		{ "/dir/",			"file.txt",			"/dir/file.txt",			},
		{ "/dir/sub",		"file.txt",			"/dir/sub/file.txt",		},
		{ "/dir/sub/",		"file.txt",			"/dir/sub/file.txt",		},
		{ "/",				"file.txt",			"/file.txt",				},
		{ "/dir",			"",					"/dir/"						},
		{ "/",				"",					"/",						},
		{ "/dir",			"sub/file.txt",		"/dir/sub/file.txt",		},
		{ "/dir",			"/file.txt",		"/dir/file.txt",			},
		{ "/longdir",		"file.txt",			"/longdir/file.txt",		},
		// ".." cases
		{ "/dir",			"..",				"/",						},
		{ "/dir/sub",		"..",				"/dir/",					},
		{ "/dir",			"../file.txt",		"/file.txt",				},
		{ "/dir/sub",		"../..",			"/",						},
		{ "/",				"..",				"/",						},
		// "." cases
		{ "/dir",			".",				"/dir/",					},
		{ "/dir/sub",		".",				"/dir/sub/",				},
		{ "/dir",			"./file.txt",		"/dir/file.txt",			},
		{ "/dir/sub",		"./..",				"/dir/",					},
		// ".." in the middle of sub
		{ "/dir",			"sub/../file.txt",	"/dir/file.txt",			},
		{ "/dir",			"sub/..",			"/dir/",					},
		{ "/dir",			"sub/../..",		"/",						},
		{ "/dir",			"sub/../other.txt",	"/dir/other.txt",	},
		{ "/dir",			"sub/../sub2/..",	"/dir/",					},
	};
	char buff[64];
	for (const auto& testCase : testCases) {
		::memset(buff, 0, sizeof(buff));
		::strncpy(buff, testCase.base, sizeof(buff) - 1);
		const char* ret = FS::AppendPathName(buff, sizeof(buff), testCase.sub);
		bool ok = (ret != nullptr) && (::strcmp(buff, testCase.expected) == 0);
		::printf("%-14s %-20s -> %-28s%s\n", testCase.base, testCase.sub, buff, ok? "" : " ***");
	}
}

int main()
{
	::stdio_init_all();
	test_DoesMatchWildcard();
	test_AppendPathName();
	for (;;) ::tight_loop_contents();
}
