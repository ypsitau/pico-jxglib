#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/DMA.h"

using namespace jxglib;

void test_MemoryToMemory()
{
	const char src[] = "Hello, world! (from DMA)";
	char dst[count_of(src)];
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig config;
	config.set_transfer_data_size(DMA_SIZE_8).set_read_increment(true).set_write_increment(true)
		.set_dreq(DREQ_FORCE).set_chain_to(channel).set_ring(false, 0).set_bswap(false)
		.set_irq_quiet(false).set_enable(true).set_sniff_enable(false).set_high_priority(false);
	channel.set_config(config).set_read_addr(src).set_write_addr(dst).set_trans_count_trig(count_of(src));
	channel.wait_for_finish_blocking();
	channel.unclaim();
	::printf("DMA#%d dst: %s\n", static_cast<uint>(channel), dst);
}

int main()
{
	::stdio_init_all();
	test_MemoryToMemory();
}
