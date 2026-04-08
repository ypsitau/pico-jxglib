#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/Hash.h"

void test_md5()
{
	printf("Testing MD5 implementation...\n");
	
	// Test case 1: Empty string
	{
		jxglib::Hash::MD5 md5;
		const char* result = md5.Complete();
		printf("MD5(\"\") = %s\n", result);
		printf("Expected:  d41d8cd98f00b204e9800998ecf8427e\n");
	}
	
	// Test case 2: "abc"
	{
		jxglib::Hash::MD5 md5;
		md5.Put("abc", 3);
		const char* result = md5.Complete();
		printf("MD5(\"abc\") = %s\n", result);
		printf("Expected:    900150983cd24fb0d6963f7d28e17f72\n");
	}
	
	// Test case 3: "message digest"
	{
		jxglib::Hash::MD5 md5;
		const char* test_str = "message digest";
		md5.Put(test_str, strlen(test_str));
		const char* result = md5.Complete();
		printf("MD5(\"message digest\") = %s\n", result);
		printf("Expected:                f96b697d7cb7938d525a2f31aaf161d0\n");
	}
	
	// Test case 4: Multiple updates
	{
		jxglib::Hash::MD5 md5;
		md5.Put("a", 1);
		md5.Put("b", 1);
		md5.Put("c", 1);
		const char* result = md5.Complete();
		printf("MD5(\"a\"+\"b\"+\"c\") = %s\n", result);
		printf("Expected:           900150983cd24fb0d6963f7d28e17f72\n");
	}
	
	// Test case 5: "The quick brown fox jumps over the lazy dog"
	{
		jxglib::Hash::MD5 md5;
		const char* test_str = "The quick brown fox jumps over the lazy dog";
		md5.Put(test_str, strlen(test_str));
		const char* result = md5.Complete();
		printf("MD5(dog string) = %s\n", result);
		printf("Expected:         9e107d9d372bb6826bd81d3542a419d6\n");
	}
	
	// Test case 6: Alphabet string
	{
		jxglib::Hash::MD5 md5;
		const char* test_str = "abcdefghijklmnopqrstuvwxyz";
		md5.Put(test_str, strlen(test_str));
		const char* result = md5.Complete();
		printf("MD5(alphabet) = %s\n", result);
		printf("Expected:       c3fcd3d76192e4007dfb496cca67e13b\n");
	}
	
	printf("MD5 tests completed.\n\n");
}

void test_sha1()
{
	printf("Testing SHA1 implementation...\n");
	
	// Test case 1: Empty string
	{
		jxglib::Hash::SHA1 sha1;
		const char* result = sha1.Complete();
		printf("SHA1(\"\") = %s\n", result);
		printf("Expected:  da39a3ee5e6b4b0d3255bfef95601890afd80709\n");
	}
	
	// Test case 2: "abc"
	{
		jxglib::Hash::SHA1 sha1;
		sha1.Put("abc", 3);
		const char* result = sha1.Complete();
		printf("SHA1(\"abc\") = %s\n", result);
		printf("Expected:    a9993e364706816aba3e25717850c26c9cd0d89d\n");
	}
	
	// Test case 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
	{
		jxglib::Hash::SHA1 sha1;
		const char* test_str = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
		sha1.Put(test_str, strlen(test_str));
		const char* result = sha1.Complete();
		printf("SHA1(long string) = %s\n", result);
		printf("Expected:           84983e441c3bd26ebaae4aa1f95129e5e54670f1\n");
	}
	
	// Test case 4: Multiple updates
	{
		jxglib::Hash::SHA1 sha1;
		sha1.Put("a", 1);
		sha1.Put("b", 1);
		sha1.Put("c", 1);
		const char* result = sha1.Complete();
		printf("SHA1(\"a\"+\"b\"+\"c\") = %s\n", result);
		printf("Expected:           a9993e364706816aba3e25717850c26c9cd0d89d\n");
	}

	// Test case 5: "The quick brown fox jumps over the lazy dog"
	{
		jxglib::Hash::SHA1 sha1;
		const char* test_str = "The quick brown fox jumps over the lazy dog";
		sha1.Put(test_str, strlen(test_str));
		const char* result = sha1.Complete();
		printf("SHA1(dog string) = %s\n", result);
		printf("Expected:          2fd4e1c67a2d28fced849ee1bb76e7391b93eb12\n");
	}


	printf("SHA1 tests completed.\n\n");
}

void test_sha256()
{
	printf("Testing SHA256 implementation...\n");
	
	// Test case 1: Empty string
	{
		jxglib::Hash::SHA256 sha256;
		const char* result = sha256.Complete();
		printf("SHA256(\"\") = %s\n", result);
		printf("Expected:    e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855\n");
	}
	
	// Test case 2: "abc"
	{
		jxglib::Hash::SHA256 sha256;
		sha256.Put("abc", 3);
		const char* result = sha256.Complete();
		printf("SHA256(\"abc\") = %s\n", result);
		printf("Expected:      ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad\n");
	}
	
	// Test case 3: "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"
	{
		jxglib::Hash::SHA256 sha256;
		const char* test_str = "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
		sha256.Put(test_str, strlen(test_str));
		const char* result = sha256.Complete();
		printf("SHA256(long string) = %s\n", result);
		printf("Expected:             248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1\n");
	}
	
	// Test case 4: Multiple updates
	{
		jxglib::Hash::SHA256 sha256;
		sha256.Put("a", 1);
		sha256.Put("b", 1);
		sha256.Put("c", 1);
		const char* result = sha256.Complete();
		printf("SHA256(\"a\"+\"b\"+\"c\") = %s\n", result);
		printf("Expected:             ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad\n");
	}
	
	// Test case 5: "The quick brown fox jumps over the lazy dog"
	{
		jxglib::Hash::SHA256 sha256;
		const char* test_str = "The quick brown fox jumps over the lazy dog";
		sha256.Put(test_str, strlen(test_str));
		const char* result = sha256.Complete();
		printf("SHA256(dog string) = %s\n", result);
		printf("Expected:            d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592\n");
	}
	
	printf("SHA256 tests completed.\n\n");
}

int main()
{
	::stdio_init_all();
	::jxglib_labo_init(true);
	
	// Run MD5 tests
	test_md5();
	
	// Run SHA1 tests
	test_sha1();
	
	// Run SHA256 tests
	test_sha256();
	
	while (true) {
		::jxglib_tick();
	}
}
