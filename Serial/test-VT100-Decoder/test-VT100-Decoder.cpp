#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/VT100.h"

using namespace jxglib;

int main()
{
	VT100::Decoder decoder;
	::stdio_init_all();
	::printf("Press any keys...\n");
	for (;;) {
		int ch;
		while ((ch = ::stdio_getchar_timeout_us(0)) > 0) decoder.FeedChar(ch);
		int keyData;
		bool vkFlag;
		if (decoder.GetKeyData(&keyData, &vkFlag)) {
			::printf("%-8s %02x\n", vkFlag? "VKey:" : "Ascii:", keyData);
		}
	}
}
