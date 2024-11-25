#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/DMA.h"

using namespace jxglib;

void test_MemoryToMemory()
{
	const char src[] = "DMA: Memory to Memory";
	char dst[count_of(src)];
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig channelConfig;
	channelConfig.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(true)
		.set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
		.set_chain_to(channel)
		.set_ring(false, 0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	channel.set_config(channelConfig)
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
	DMA::ChannelConfig channelConfig;
	channelConfig.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(false)
		.set_dreq(::uart_get_dreq(uart_default, true)) // set DREQ of uart_default's tx
		.set_chain_to(channel)
		.set_ring(false, 0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	channel.set_config(channelConfig)
		.set_read_addr(src)
		.set_write_addr(&::uart_get_hw(uart_default)->dr)
		.set_trans_count_trig(count_of(src));
	channel.wait_for_finish_blocking();
	channel.unclaim();
}

void test_MemoryToMemory_Chain()
{
	const char word0[] = "Transferring ";
	const char word1[] = "one ";
	const char word2[] = "word ";
	const char word3[] = "at ";
	const char word4[] = "a ";
	const char word5[] = "time.\n";
	const struct {
		uint32_t len;
		const char *data;
	} control_blocks[] = {
		{ count_of(word0) - 1, word0 },
		{ count_of(word1) - 1, word1 },
		{ count_of(word2) - 1, word2 },
		{ count_of(word3) - 1, word3 },
		{ count_of(word4) - 1, word4 },
		{ count_of(word5) - 1, word5 },
		{ 0, NULL}
	};
	DMA::Channel ctrl_chan(DMA::claim_unused_channel(true));
	DMA::Channel data_chan(DMA::claim_unused_channel(true));


	dma_channel_config c = dma_channel_get_default_config(ctrl_chan);
	channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
	channel_config_set_read_increment(&c, true);
	channel_config_set_write_increment(&c, true);
	channel_config_set_ring(&c, true, 3); // 1 << 3 byte boundary on write ptr

	dma_channel_configure(
		ctrl_chan,
		&c,
		&dma_hw->ch[data_chan].al3_transfer_count, // Initial write address
		&control_blocks[0],                        // Initial read address
		2,                                         // Halt after each control block
		false                                      // Don't start yet
	);


	c = dma_channel_get_default_config(data_chan);
	channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
	channel_config_set_dreq(&c, uart_get_dreq(uart_default, true));
	channel_config_set_chain_to(&c, ctrl_chan);
	// Raise the IRQ flag when 0 is written to a trigger register (end of chain):
	channel_config_set_irq_quiet(&c, true);

	dma_channel_configure(
		data_chan,
		&c,
		&uart_get_hw(uart_default)->dr,
		NULL,           // Initial read address and transfer count are unimportant;
		0,              // the control channel will reprogram them each time.
		false           // Don't start yet.
	);

	// Everything is ready to go. Tell the control channel to load the first
	// control block. Everything is automatic from here.
	//dma_start_channel_mask(1u << ctrl_chan);
	ctrl_chan.start();

	// The data channel will assert its IRQ flag when it gets a null trigger,
	// indicating the end of the control block list. We're just going to wait
	// for the IRQ flag instead of setting up an interrupt handler.
	while (!(dma_hw->intr & (1u << data_chan))) tight_loop_contents();
	//dma_hw->ints0 = 1u << data_chan;
	data_chan.acknowledge_irq0();
}

int main()
{
	::stdio_init_all();
	//test_MemoryToMemory();
	//test_MemoryToPeripheral();
	test_MemoryToMemory_Chain();
}
