#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Hash.h"

void test_sha1()
{
	printf("Testing SHA1 implementation...\n");
	
	// Test case 1: Empty string
	{
		jxglib::Hash::SHA1 sha1;
		const char* result = sha1.Finish();
		printf("SHA1(\"\") = %s\n", result);
		printf("Expected:  da39a3ee5e6b4b0d3255bfef95601890afd80709\n");
	}
	
	// Test case 2: "abc"
	{
		jxglib::Hash::SHA1 sha1;
		sha1.Update("abc", 3);
		const char* result = sha1.Finish();
		printf("SHA1(\"abc\") = %s\n", result);
		printf("Expected:    a9993e364706816aba3e25717850c26c9cd0d89d\n");
	}
	
	// Test case 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
	{
		jxglib::Hash::SHA1 sha1;
		const char* test_str = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
		sha1.Update(test_str, strlen(test_str));
		const char* result = sha1.Finish();
		printf("SHA1(long string) = %s\n", result);
		printf("Expected:           84983e441c3bd26ebaae4aa1f95129e5e54670f1\n");
	}
	
	// Test case 4: Multiple updates
	{
		jxglib::Hash::SHA1 sha1;
		sha1.Update("a", 1);
		sha1.Update("b", 1);
		sha1.Update("c", 1);
		const char* result = sha1.Finish();
		printf("SHA1(\"a\"+\"b\"+\"c\") = %s\n", result);
		printf("Expected:           a9993e364706816aba3e25717850c26c9cd0d89d\n");
	}

	// Test case 5: "The quick brown fox jumps over the lazy dog"
	{
		jxglib::Hash::SHA1 sha1;
		const char* test_str = "The quick brown fox jumps over the lazy dog";
		sha1.Update(test_str, strlen(test_str));
		const char* result = sha1.Finish();
		printf("SHA1(long string) = %s\n", result);
		printf("Expected:           2fd4e1c67a2d28fced849ee1bb76e7391b93eb12\n");
	}


	printf("SHA1 tests completed.\n\n");
}

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(true);
	
	// Run SHA1 tests
	test_sha1();
	
	while (true) {
		::jxglib_tick();
	}
}
