#include <stdio.h>
#include <malloc.h>
#include "pico/stdlib.h"
#include "jxglib/Common.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	::printf("%d\n", SignExtend<>(0b111, 3));
	::printf("%d\n", SignExtend<int8_t> (0b01111111, 8));
	::printf("%d\n", SignExtend<int16_t>(0b01111111, 8));
	::printf("%d\n", SignExtend<int32_t>(0b01111111, 8));
	::printf("%d\n", SignExtend<int8_t> (0b01111110, 8));
	::printf("%d\n", SignExtend<int16_t>(0b01111110, 8));
	::printf("%d\n", SignExtend<int32_t>(0b01111110, 8));
	::printf("%d\n", SignExtend<int8_t> (0b11111111, 8));
	::printf("%d\n", SignExtend<int16_t>(0b11111111, 8));
	::printf("%d\n", SignExtend<int32_t>(0b11111111, 8));
	::printf("%d\n", SignExtend<int8_t> (0b11111110, 8));
	::printf("%d\n", SignExtend<int16_t>(0b11111110, 8));
	::printf("%d\n", SignExtend<int32_t>(0b11111110, 8));
	::printf("%d\n", SignExtend<int8_t> (0b11111111111111, 14));
	::printf("%d\n", SignExtend<int16_t>(0b11111111111111, 14));
	::printf("%d\n", SignExtend<int32_t>(0b11111111111111, 14));
	::printf("%d\n", SignExtend<int8_t> (0b11111111111110, 14));
	::printf("%d\n", SignExtend<int16_t>(0b11111111111110, 14));
	::printf("%d\n", SignExtend<int32_t>(0b11111111111110, 14));
	for (;;) tight_loop_contents();
}
