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
		while ((ch = ::stdio_getchar_timeout_us(0)) > 0 && !decoder.FeedChar(ch)) ;
		KeyData keyData;
		if (!decoder.GetKeyData(&keyData)) {
			// nothing to do
		} else if (keyData.IsKeyCode()) {
			::printf("KeyCode VK_%s\n", keyData.GetKeyCodeName());
		} else {
			::printf("Char    0x%02x\n", keyData.GetChar());
		}
	}
}
