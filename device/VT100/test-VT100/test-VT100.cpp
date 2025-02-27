#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/UART.h"
#include "jxglib/VT100.h"

using namespace jxglib;

VT100::Decoder decoder;

void UARTHandler(void)
{
	UART& uart = UART::Default;
	while (uart.raw.is_readable()) decoder.FeedChar(uart.raw.getc());
}

int main()
{
	::stdio_init_all();
	UART::Default.irq_set_exclusive_handler(UARTHandler).irq_set_enabled(true);
	UART::Default.raw.set_irq_enables(true, false);
	for (;;) {
		int keyData;
		if (!decoder.HasKeyData()) {
		} else if (decoder.GetKeyData(&keyData)) {
			::printf("Control: %02x\n", keyData);
		} else {
			::printf("Ascii:   %02x\n", keyData);
		}
	}
}
