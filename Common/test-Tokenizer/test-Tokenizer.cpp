#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/Tokenizer.h"

using namespace jxglib;

void test_Tokenizer()
{
	Tokenizer tokenizer;
	const char* test_cases[20] = {
		"abc def ghi",
		"abc  def   ghi",
		"  abc def",
		"abc def  ",
		"abc",
		"   ",
		"abc \"def ghi\" jkl",
		"abc \"def \\\"ghi\\\"\" jkl",
		"abc\\ def ghi",
		"abc\tdef\nghi",
		"abc \"def\nghi\" jkl",
		"abc \"def\\n\\tghi\" jkl",
		"abc \"def",
		"abc \\n def",
		"abc \"def\\rghi\" jkl",
		"abc \"def\\tghi\" jkl",
		"abc \"def\\\\ghi\" jkl",
		"abc \"\" jkl",
		"\"abc def\""
	};

	for (int i = 0; i < 20; ++i) {
		char buf[256];
		strncpy(buf, test_cases[i], sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		char* tokenTbl[10];
		int nToken = 10;
		const char* errMsg = nullptr;

		printf("Test case %2d: \"%s\"\n", i + 1, test_cases[i]);
		bool result = tokenizer.Tokenize(buf, &nToken, tokenTbl, &errMsg);

		if (!result) {
			printf("  Error: %s\n\n", errMsg ? errMsg : "unknown");
			continue;
		}

		printf("  Token count: %d\n", nToken);
		for (int j = 0; j < nToken; ++j) {
			printf("    Token[%d]: '%s'\n", j, tokenTbl[j]);
		}
		printf("\n");
	}
}

int main()
{
	stdio_init_all();
	test_Tokenizer();
	return 0;
}
