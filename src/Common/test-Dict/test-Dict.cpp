#include <stdio.h>
#include "pico/stdlib.h"

// Dict test
#include "jxglib/Dict.h"
using namespace jxglib;

void test_Dict()
{
	Dict dict;
	// 1. Insert new entries
	dict.SetValue("foo", "bar");
	dict.SetValue("hello", "world");

	// 2. Lookup test
	const char* v1 = dict.Lookup("foo");
	printf("foo = %s\n", v1 ? v1 : "(null)");
	const char* v2 = dict.Lookup("hello");
	printf("hello = %s\n", v2 ? v2 : "(null)");

	// 3. Lookup for non-existent key
	const char* v3 = dict.Lookup("none");
	printf("none = %s\n", v3 ? v3 : "(null)");

	// 4. Overwrite value
	dict.SetValue("foo", "baz");
	const char* v4 = dict.Lookup("foo");
	printf("foo (overwritten) = %s\n", v4 ? v4 : "(null)");

	// 5. Find Entry by key
	Dict::Entry* e = dict.Find("hello");
	printf("Find hello: %s\n", e ? e->GetValue() : "(not found)");
}

int main()
{
	::stdio_init_all();
	test_Dict();
	for (;;) ::tight_loop_contents();
}
