#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/PWM.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

void test_CalcClkdivAndWrap()
{
	printf("=== PWM::CalcClkdivAndWrap Test Cases ===\n");
	
	struct TestCase {
		uint32_t target_freq;
		const char* description;
	};
	
	TestCase test_cases[] = {
		{1000, "1kHz (typical servo PWM)"},
		{50, "50Hz (standard servo frequency)"},
		{20000, "20kHz (ultrasonic frequency)"},
		{100000, "100kHz (high frequency PWM)"},
		{1, "1Hz (very low frequency)"},
		{1000000, "1MHz (very high frequency)"},
		{440, "440Hz (musical note A4)"},
		{60, "60Hz (power line frequency)"},
		{0, "0Hz (edge case)"}
	};
	
	for (size_t i = 0; i < sizeof(test_cases) / sizeof(test_cases[0]); i++) {
		float clkdiv;
		uint16_t wrap;
		
		printf("\nTest %zu: %s\n", i + 1, test_cases[i].description);
		printf("Target frequency: %u Hz\n", test_cases[i].target_freq);
		
		// Call the function under test
		PWM::CalcClkdivAndWrap(test_cases[i].target_freq, &clkdiv, &wrap);
		
		printf("Calculated clkdiv: %.4f\n", clkdiv);
		printf("Calculated wrap: %u\n", wrap);
		
		if (test_cases[i].target_freq > 0) {
			// Calculate actual frequency
			// Assuming 125MHz system clock for calculation
			uint32_t actual_freq = PWM::CalcFreq(clkdiv, wrap);
			
			printf("Actual frequency: %u Hz\n", actual_freq);
			
			// Calculate error
			int32_t error = static_cast<int32_t>(actual_freq) - static_cast<int32_t>(test_cases[i].target_freq);
			float error_percent = (float)error / test_cases[i].target_freq * 100.0f;
			
			printf("Error: %d Hz (%.3f%%)\n", error, error_percent);
			
			// Validate ranges
			bool clkdiv_valid = (clkdiv >= 1.0f && clkdiv <= 256.0f);
			bool wrap_valid = (wrap <= 65535);
			
			printf("Validation: clkdiv %s, wrap %s\n", 
				clkdiv_valid ? "OK" : "FAIL", 
				wrap_valid ? "OK" : "FAIL");
		} else {
			printf("Zero frequency - no calculation performed\n");
		}
		
		printf("----------------------------------------\n");
	}
	
	printf("\n=== Test Summary ===\n");
	printf("All test cases completed.\n");
	printf("Review the error percentages to verify accuracy.\n\n");
}

int main()
{
	::stdio_init_all();
	test_CalcClkdivAndWrap();
	for (;;) Tickable::Tick();
}
