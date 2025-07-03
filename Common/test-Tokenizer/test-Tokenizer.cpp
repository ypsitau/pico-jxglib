#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/Tokenizer.h"

using namespace jxglib;

void test_Tokenizer()
{
	static const char* specialTokens[] = {
		">>", "<<", "||", ">", "<", "|",
	};
	Tokenizer tokenizer(specialTokens, count_of(specialTokens));
	const char* test_cases[] = {
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
		"\"abc def\"",
		"abc > def",
		"abc >> def",
		"abc>def",
		"abc>>def",
		"abc> def",
		"abc>> def",
		"abc >def",
		"abc >>def",
		"abc >",
		"abc >>",
		"abc>",
		"abc>>",
		"abc \">\"",
	};

	for (int i = 0; i < count_of(test_cases); ++i) {
		char buf[256];
		strncpy(buf, test_cases[i], sizeof(buf));
		buf[sizeof(buf) - 1] = '\0';

		char* tokenTbl[10];
		int nToken = 10;

		printf("Test case %2d: \"%s\"\n", i + 1, test_cases[i]);
		const char* errorMsg;
		bool result = tokenizer.Tokenize(buf, sizeof(buf), tokenTbl, &nToken, &errorMsg);

		if (!result) {
			printf("  Error: %s\n\n", errorMsg ? errorMsg : "unknown");
			continue;
		}

		printf("  Token count: %d\n", nToken);
		for (int j = 0; j < nToken; ++j) {
			printf("    Token[%d]: '%s'%s\n", j, tokenTbl[j], Tokenizer::IsQuoted(buf, tokenTbl[j]) ? " (quoted)" : "");
		}
		printf("\n");
		const char* tokenLast = tokenizer.FindLastToken(test_cases[i]);
		printf("  Last token: '%s'\n", tokenLast);
		printf("\n");
	}
}

int main()
{
	stdio_init_all();
	test_Tokenizer();
	for (;;) ::tight_loop_contents();
}
