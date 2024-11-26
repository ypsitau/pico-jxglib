#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/DMA.h"

using namespace jxglib;

uint32_t CalcCRC32R(const void* buff, uint32_t length, uint32_t crc = static_cast<uint32_t>(-1))
{
	const uint8_t* buffp = reinterpret_cast<const uint8_t*>(buff); 
	for ( ; length; length--, buffp++) {
		uint32_t byte32 = static_cast<uint32_t>(*buffp);
		for (uint8_t bit = 8; bit; bit--, byte32 >>= 1) {
			crc = (crc >> 1) ^ (((crc ^ byte32) & 1ul)? 0xEDB88320ul : 0ul);
		}
	}
	return crc;
}

uint32_t CalcSum(const void* buff, uint32_t length, uint32_t sum = 0)
{
	const uint8_t* buffp = reinterpret_cast<const uint8_t*>(buff); 
	for ( ; length; length--, buffp++) sum += *buffp;
	return sum;
}

void test_MemoryToMemory()
{
	::printf("\ntest_MemoryToMemory\n");
	const char src[] = "The quick brown fox jumps over the lazy dog.";
	char dst[count_of(src)];
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig config;
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(true)
		.set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
		.set_chain_to(channel) // disable by setting chain_to to itself
		.set_ring_to_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	channel.set_config(config)
		.set_read_addr(src)
		.set_write_addr(dst)
		.set_trans_count_trig(count_of(src))
		.wait_for_finish_blocking();
	::printf("Transferred by DMA#%d: %s\n", static_cast<uint>(channel), dst);
	channel.unclaim();
}

void test_MemoryToPeripheral()
{
	::printf("\ntest_MemoryToPeripheral\n");
	const char src[] = "The quick brown fox jumps over the lazy dog.";
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig config;
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(false)
		.set_dreq(::uart_get_dreq(uart_default, true)) // set DREQ of uart_default's tx
		.set_chain_to(channel) // disable by setting chain_to to itself
		.set_ring_to_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(false)
		.set_high_priority(false);
	::printf("Transferred by DMA#%d: ", static_cast<uint>(channel));
	channel.set_config(config)
		.set_read_addr(src)
		.set_write_addr(&::uart_get_hw(uart_default)->dr)
		.set_trans_count_trig(count_of(src))
		.wait_for_finish_blocking();
	::printf("\n");
	channel.unclaim();
}

void test_MemoryToPeripheral_Chain()
{
	::printf("\ntest_MemoryToPeripheral_Chain\n");
	const char src[] = "The quick brown fox jumps over the lazy dog.";
	const char word0[] = "The ";
	const char word1[] = "quick ";
	const char word2[] = "brown ";
	const char word3[] = "fox ";
	const char word4[] = "jumps ";
	const char word5[] = "over ";
	const char word6[] = "the ";
	const char word7[] = "lazy ";
	const char word8[] = "dog.";
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
		{ count_of(word6) - 1, word6 },
		{ count_of(word7) - 1, word7 },
		{ count_of(word8) - 1, word8 },
		{ 0, nullptr }
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
			.set_chain_to(channelCtrl) // disable by setting chain_to to itself
			.set_ring_to_write(3) // Wrap round by 8 (= 1 << 3) bytes
			.set_bswap(false)
			.set_irq_quiet(false)
			.set_sniff_enable(false)
			.set_high_priority(false);
		// DMA Registers
		//        +0x0        +0x4                +0x8                +0xc
		// 0x000  read_addr   write_addr          transfer_count      ctrl_trig
		// 0x010  al1_ctrl    al1_read_addr       al1_write_addr      al1_transfer_count_trig
		// 0x020  al2_ctrl    al2_transfer_count  al2_read_addr       al2_write_addr_trig
		// 0x030  al3_ctrl    al3_write_addr      al3_transfer_count  al3_read_addr_trig
		channelCtrl.set_config(config)
			.set_read_addr(&controlBlocks[0])
			.set_write_addr(&channelData.get_hw()->al3_transfer_count)
			.set_trans_count(2); // transfer to al3_transfer_count and al3_read_addr_trig
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
	::printf("Transferred by DMA#%d and DMA#%d: ", static_cast<uint>(channelCtrl), static_cast<uint>(channelData));
	channelCtrl.start();
	while (!channelData.get_raw_interrupt_status()) tight_loop_contents();
	::printf("\n");
	channelData.acknowledge_irq0();
	channelCtrl.unclaim();
	channelData.unclaim();
}

const struct SnifferRecord {
	const char* name;
	uint mode;
	uint32_t seed_value;
} snifferRecordTbl[] = {
	{ "CRC32",	DMA_SNIFF_CTRL_CALC_VALUE_CRC32,	0xffffffff },
	{ "CRC32R",	DMA_SNIFF_CTRL_CALC_VALUE_CRC32R,	0xffffffff },
	{ "CRC16",	DMA_SNIFF_CTRL_CALC_VALUE_CRC16,	0xffff },
	{ "CRC16R",	DMA_SNIFF_CTRL_CALC_VALUE_CRC16R,	0xffff },
	{ "EVEN",	DMA_SNIFF_CTRL_CALC_VALUE_EVEN,		0x00000000 },
	{ "SUM",	DMA_SNIFF_CTRL_CALC_VALUE_SUM,		0x00000000 },
};

void test_MemoryToMemory_SnifferCalcCRC()
{
	::printf("\ntest_MemoryToMemory_SnifferCalcCRC\n");
	const char src[] = "The quick brown fox jumps over the lazy dog.";
	char dst[count_of(src)];
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig config;
	DMA::Sniffer::set_output_reverse_enabled(true);
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(true)
		.set_dreq(DREQ_FORCE)		// see RP2040 Datasheet 2.5.3.1 System DREQ Table
		.set_chain_to(channel)		// disable by setting chain_to to itself
		.set_ring_to_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(true)		// enable sniff hardware
		.set_high_priority(false);
	for (int i = 0; i < count_of(snifferRecordTbl); i++) {
		::memset(dst, 0x00, sizeof(dst));
		auto& snifferRecord = snifferRecordTbl[i];
		DMA::Sniffer::set_data_accumulator(snifferRecord.seed_value);
		channel.sniffer_enable(snifferRecord.mode)
			.set_config(config)
			.set_read_addr(src)
			.set_write_addr(dst)
			.set_trans_count_trig(count_of(src))
			.wait_for_finish_blocking();
		::printf("Transferred by DMA#%d: %s\n", static_cast<uint>(channel), dst);
		::printf("%-8s %08x %08x %08x\n", snifferRecord.name, DMA::Sniffer::get_data_accumulator(),
			CalcCRC32R(src, count_of(src)), CalcSum(src, count_of(src)));
	}
	channel.unclaim();
}

void test_MemoryToPeripheral_SnifferCalcCRC()
{
	::printf("\ntest_MemoryToPeripheral_SnifferCalcCRC\n");
	const char src[] = "The quick brown fox jumps over the lazy dog.";
	DMA::Channel channel(DMA::claim_unused_channel(true));
	DMA::ChannelConfig config;
	DMA::Sniffer::set_output_reverse_enabled(true);
	config.set_enable(true)
		.set_transfer_data_size(DMA_SIZE_8)
		.set_read_increment(true)
		.set_write_increment(false)
		.set_dreq(::uart_get_dreq(uart_default, true)) // set DREQ of uart_default's tx
		.set_chain_to(channel) // disable by setting chain_to to itself
		.set_ring_to_read(0)
		.set_bswap(false)
		.set_irq_quiet(false)
		.set_sniff_enable(true) // enable sniff
		.set_high_priority(false);
	for (int i = 0; i < count_of(snifferRecordTbl); i++) {
		auto& snifferRecord = snifferRecordTbl[i];
		DMA::Sniffer::set_data_accumulator(snifferRecord.seed_value);
		::printf("Transferred by DMA#%d: ", static_cast<uint>(channel));
		channel.sniffer_enable(snifferRecord.mode)
			.set_config(config)
			.set_read_addr(src)
			.set_write_addr(&::uart_get_hw(uart_default)->dr)
			.set_trans_count_trig(count_of(src))
			.wait_for_finish_blocking();
		::printf("\n");
		::printf("%-8s %08x %08x %08x\n", snifferRecord.name, DMA::Sniffer::get_data_accumulator(),
			CalcCRC32R(src, count_of(src)), CalcSum(src, count_of(src)));
	}
	channel.unclaim();
}

int main()
{
	::stdio_init_all();
	::printf("----\n");
	test_MemoryToMemory();
	test_MemoryToPeripheral();
	test_MemoryToPeripheral_Chain();
	test_MemoryToMemory_SnifferCalcCRC();
	test_MemoryToPeripheral_SnifferCalcCRC();
}
