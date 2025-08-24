#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/JSON.h"

void test_JSON()
{
	static const char* testCaseTbl[] = {
	};
	jxglib::JSON json;
	jxglib::JSON::Handler_Debug handler;
	json.SetHandler(handler);
	for (const char* testCase : testCaseTbl) {
		printf("Test case: %s\n", testCase);
		if (json.Parse(testCase)) {
			printf("Parsed successfully.\n");
		} else {
			printf("Parse error: %s\n", json.GetErrorMsg());
		}
	}
}

int main()
{
	::stdio_init_all();
	test_JSON();
	for (;;) ::tight_loop_contents();
}
