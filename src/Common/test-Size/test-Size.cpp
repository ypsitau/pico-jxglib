#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/Size.h"

using namespace jxglib;

void test_Size()
{
	printf("Testing Size::Parse()...\n");
	
	// Test case 1: Valid format with 'x'
	{
		Size size;
		bool result = size.Parse("800x600");
		printf("Test 1 - \"800x600\": %s, width=%d, height=%d\n", 
			   result ? "PASS" : "FAIL", size.width, size.height);
		if (!result || size.width != 800 || size.height != 600) {
			printf("  ERROR: Expected width=800, height=600\n");
		}
	}
	
	// Test case 2: Valid format with 'X' (uppercase)
	{
		Size size;
		bool result = size.Parse("1920X1080");
		printf("Test 2 - \"1920X1080\": %s, width=%d, height=%d\n", 
			   result ? "PASS" : "FAIL", size.width, size.height);
		if (!result || size.width != 1920 || size.height != 1080) {
			printf("  ERROR: Expected width=1920, height=1080\n");
		}
	}
	
	// Test case 3: Valid format with comma
	{
		Size size;
		bool result = size.Parse("640,480");
		printf("Test 3 - \"640,480\": %s, width=%d, height=%d\n", 
			   result ? "PASS" : "FAIL", size.width, size.height);
		if (!result || size.width != 640 || size.height != 480) {
			printf("  ERROR: Expected width=640, height=480\n");
		}
	}
	
	// Test case 4: Valid format with spaces
	{
		Size size;
		bool result = size.Parse("1024  x  768");
		printf("Test 4 - \"1024  x  768\": %s, width=%d, height=%d\n", 
			   result ? "PASS" : "FAIL", size.width, size.height);
		if (!result || size.width != 1024 || size.height != 768) {
			printf("  ERROR: Expected width=1024, height=768\n");
		}
	}
	
	// Test case 5: Zero values
	{
		Size size;
		bool result = size.Parse("0x0");
		printf("Test 5 - \"0x0\": %s, width=%d, height=%d\n", 
			   result ? "PASS" : "FAIL", size.width, size.height);
		if (!result || size.width != 0 || size.height != 0) {
			printf("  ERROR: Expected width=0, height=0\n");
		}
	}
	
	// Test case 6: Invalid format - missing separator
	{
		Size size;
		bool result = size.Parse("800600");
		printf("Test 6 - \"800600\" (invalid): %s\n", 
			   result ? "FAIL (should be false)" : "PASS");
		if (result) {
			printf("  ERROR: Should return false for invalid format\n");
		}
	}
	
	// Test case 7: Invalid format - negative width
	{
		Size size;
		bool result = size.Parse("-800x600");
		printf("Test 7 - \"-800x600\" (invalid): %s\n", 
			   result ? "FAIL (should be false)" : "PASS");
		if (result) {
			printf("  ERROR: Should return false for negative width\n");
		}
	}
	
	// Test case 8: Invalid format - negative height
	{
		Size size;
		bool result = size.Parse("800x-600");
		printf("Test 8 - \"800x-600\" (invalid): %s\n", 
			   result ? "FAIL (should be false)" : "PASS");
		if (result) {
			printf("  ERROR: Should return false for negative height\n");
		}
	}
	
	// Test case 9: Empty string
	{
		Size size;
		bool result = size.Parse("");
		printf("Test 9 - \"\" (empty): %s\n", 
			   result ? "FAIL (should be false)" : "PASS");
		if (result) {
			printf("  ERROR: Should return false for empty string\n");
		}
	}
	
	// Test case 10: NULL pointer
	{
		Size size;
		bool result = size.Parse(nullptr);
		printf("Test 10 - nullptr: %s\n", 
			   result ? "FAIL (should be false)" : "PASS");
		if (result) {
			printf("  ERROR: Should return false for null pointer\n");
		}
	}
	
	// Test case 11: Invalid format - extra characters
	{
		Size size;
		bool result = size.Parse("800x600extra");
		printf("Test 11 - \"800x600extra\" (invalid): %s\n", 
			   result ? "FAIL (should be false)" : "PASS");
		if (result) {
			printf("  ERROR: Should return false for extra characters\n");
		}
	}
	
	// Test case 12: Invalid format - non-numeric values
	{
		Size size;
		bool result = size.Parse("abcxdef");
		printf("Test 12 - \"abcxdef\" (invalid): %s\n", 
			   result ? "FAIL (should be false)" : "PASS");
		if (result) {
			printf("  ERROR: Should return false for non-numeric values\n");
		}
	}
	
	printf("Size::Parse() tests completed.\n\n");
}

int main()
{
	stdio_init_all();
	test_Size();
	for (;;) ::tight_loop_contents();
}
