#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "jxglib/SDCard.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	SDCard sdCard(spi0, 10 * 1000 * 1000, {CS: GPIO5});	// 10MHz
	sdCard.Initialize();
	uint8_t buf[512];
	sdCard.ReadBlock(0, buf, 1);
	SDCard::PrintMBR(buf);
	for (int i = 0; i < 512; i++) buf[i] = static_cast<uint8_t>(i);
	//::memset(buf, 0x00, sizeof(buf));
	sdCard.WriteBlock(1, buf, 1);
	sdCard.WriteBlock(2, buf, 1);
	::memset(buf, 0x00, sizeof(buf));
	for (int i = 0; i < 4; i++) {
		::printf("sector#%d\n", i);
		sdCard.ReadBlock(i, buf, 1);
		Dump(buf, 512);
	}
}
