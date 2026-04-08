#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/JSON.h"

using namespace jxglib;

void test_JSON()
{
	static const char* testCaseTbl[] = {
		// 1-10: Basic types
		"{}",
		"[]",
		"{\"a\":1}",
		"{\"b\":true}",
		"{\"c\":null}",
		"{\"d\":\"hello\"}",
		"[1,2,3]",
		"[true,false,null]",
		"{\"arr\":[1,2,3]}",
		"{\"obj\":{\"x\":10}}",
		// 11-20: Nesting and empty elements
		"{\"nested\":{\"a\":[1,{\"b\":2}]}}",
		"{\"emptyArr\":[]}",
		"{\"emptyObj\":{}}",
		"[[],{}]",
		"{\"mix\":[1,\"a\",true,null,{\"b\":2}]}" ,
		"{\"num\":-123}",
		"{\"num\":0.001}",
		"{\"num\":1e10}",
		"{\"num\":-2.5e-3}",
		// 21-30: Strings and special characters
		"{\"s\":\"\"}",
		"{\"s\":\" \\t\\n\\r\"}",
		"{\"s\":\"\\\"quoted\\\"\"}",
		"{\"s\":\"\\u3042\"}",
		"{\"s\":\"\\\\\\/\\b\\f\"}",
		"{\"emoji\":\"\\uD83D\\uDE00\"}",
		"{\"long\":\"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"}",
		"{\"escape\":\"\\\\\\\"\\n\"}",
		"{\"multi\":\"line\ntext\"}",
		// 31-40: Arrays, numbers, boundaries
		"[0]",
		"[0.0]",
		"[1e-10]",
		"[null]",
		"[true]",
		"[false]",
		"[\"a\",\"b\",\"c\"]",
		"[{},[]]",
		"[1,{\"a\":2},[3]]",
		"[1,2,3,4,5,6,7,8,9,10]",
		// 41-50: Edge cases, large numbers, complex
		"{\"big\":1234567890123456789}",
		"{\"neg\":-0}",
		"{\"exp\":1.7976931348623157e+308}",
		"{\"min\":2.2250738585072014e-308}",
		"{\"bools\":[true,false,true,false]}",
		"{\"nulls\":[null,null]}",
		"{\"deep\":{\"a\":{\"b\":{\"c\":{\"d\":1}}}}}",
		"{\"arr\":[[[]]]}",
		"{\"mix\":[1,\"a\",true,null,{\"b\":2},[3,4]]}",
		"{\"unicode\":\"こんにちは\"}"
		// 51-70: Mix case (complex, nested, various types)
		,"{\"a\":1,\"b\":[2,3],\"c\":{\"d\":4}}"
		,"[1,{\"a\":2},[3,{\"b\":4}],5]"
		,"{\"x\":true,\"y\":false,\"z\":null,\"arr\":[\"str\",123]}"
		,"{\"obj\":{\"arr\":[{\"k\":\"v\"},{}]}}"
		,"[null,{\"nested\":[1,2,{\"deep\":\"yes\"}]}]"
		,"{\"a\":{\"b\":{\"c\":{\"d\":{\"e\":5}}}}}"
		,"{\"mix\":[1.1,\"two\",false,null,{\"x\":[]},[{}]]}"
		,"[\"a\",1,true,null,{},{\"b\":[2,3]}]"
		,"{\"empty\":[],\"full\":[0,1,2,3]}"
		,"{\"bool\":false,\"num\":0,\"str\":\"\"}"
		,"[1e2,2e-2,3.1415,-0.001]"
		,"{\"unicode\":\"\u00A9\u20AC\"}"
		,"{\"escape\":\"\\\\\\\"\\n\\t\"}"
		,"{\"nestedArr\":[[[[1]]]]}"
		,"{\"multi\":{\"a\":1,\"b\":[2,3],\"c\":{\"d\":4}}}"
		,"[\"mix\",123,true,null,{\"a\":{\"b\":2}}]"
		,"{\"arr\":[1,{\"b\":[2,{\"c\":[3]}]}]}"
		,"{\"complex\":[{\"a\":1},[2,3],\"str\",null,true]}"
		,"[[],[{}],{\"a\":[]},[null]]"
	};
	JSON_Debug json;
	for (const char* testCase : testCaseTbl) {
		printf("Test case: %s\n", testCase);
		json.Reset();
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
