#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/DMA.h"

using namespace jxglib;

void test_MemoryToMemory()
{
	const char src[] = "DMA: Memory to Memory";
	char dst[count_of(src)];
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig config;
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(true)
		.set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
		.set_chain_to(channel)
		.set_ring_to_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	channel.set_config(config)
		.set_read_addr(src)
		.set_write_addr(dst)
		.set_trans_count_trig(count_of(src));
	channel.wait_for_finish_blocking();
	channel.unclaim();
	::printf("Transferred by DMA#%d: %s\n", static_cast<uint>(channel), dst);
}

void test_MemoryToPeripheral()
{
	const char src[] = "Memory to Peripheral (UART)\r\n";
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig config;
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(false)
		.set_dreq(::uart_get_dreq(uart_default, true)) // set DREQ of uart_default's tx
		.set_chain_to(channel)
		.set_ring_to_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	channel.set_config(config)
		.set_read_addr(src)
		.set_write_addr(&::uart_get_hw(uart_default)->dr)
		.set_trans_count_trig(count_of(src));
	channel.wait_for_finish_blocking();
	channel.unclaim();
}

void test_MemoryToPeripheral_Chain()
{
	const char word0[] = "Transferring ";
	const char word1[] = "one ";
	const char word2[] = "word ";
	const char word3[] = "at ";
	const char word4[] = "a ";
	const char word5[] = "time.\r\n";
	const struct {
		uint32_t len;
		const char* data;
	} controlBlocks[] = {
		{ count_of(word0) - 1, word0 },
		{ count_of(word1) - 1, word1 },
		{ count_of(word2) - 1, word2 },
		{ count_of(word3) - 1, word3 },
		{ count_of(word4) - 1, word4 },
		{ count_of(word5) - 1, word5 },
		{ 0, NULL }
	};
	DMA::Channel channelCtrl(DMA::claim_unused_channel(true));
	DMA::Channel channelData(DMA::claim_unused_channel(true));
	do {
		DMA::ChannelConfig config;
		config.set_enable(true)
			.set_transfer_data_size(DMA_SIZE_32)
			.set_read_increment(true)
			.set_write_increment(true)
			.set_dreq(DREQ_FORCE)
			.set_chain_to(channelCtrl)
			.set_ring_to_write(3) // Wrap round by 8 (= 1 << 3) bytes
			.set_bswap(false)
			.set_irq_quiet(false)
			.set_sniff_enable(false)
			.set_high_priority(false);
		channelCtrl.set_config(config)
			.set_read_addr(&controlBlocks[0])
			.set_write_addr(&dma_hw->ch[channelData].al3_transfer_count)
			.set_trans_count(2);
	} while (0);
	do {
		DMA::ChannelConfig config;
		config.set_enable(true)
			.set_transfer_data_size(DMA_SIZE_8)
			.set_read_increment(true)
			.set_write_increment(false)
			.set_dreq(::uart_get_dreq(uart_default, true))
			.set_chain_to(channelCtrl)
			.set_ring_to_read(0)
			.set_bswap(false)
			// In QUIET mode, the channel does not generate IRQs at the end of every transfer block.
			// Instead, an IRQ is raised when NULL is written to a trigger register, indicating the
			// end of a control block chain.
			.set_irq_quiet(true)
			.set_sniff_enable(false)
			.set_high_priority(false);
		channelData.set_config(config)
			.set_write_addr(&uart_get_hw(uart_default)->dr);
	} while (0);
	channelCtrl.start();
	while (!(dma_hw->intr & (1u << channelData))) tight_loop_contents();
	channelData.acknowledge_irq0();
	channelCtrl.unclaim();
	channelData.unclaim();
}

int main()
{
	::stdio_init_all();
	//test_MemoryToMemory();
	//test_MemoryToPeripheral();
	test_MemoryToPeripheral_Chain();
}
