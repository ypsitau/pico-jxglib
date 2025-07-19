#include <stdio.h>
#include <malloc.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/DMA.h"

#if defined(NDEBUG)
static const char* compileVariant = "Release";
#else
static const char* compileVariant = "Debug";
#endif

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
	DMA::Channel& channel = *DMA::claim_unused_channel();
	DMA::ChannelConfig config;
	absolute_time_t absTimeStart = ::get_absolute_time();
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
	int64_t timeUSec = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
	::printf("Transferred by DMA#%d %lldusec: %s\n", static_cast<uint>(channel), timeUSec, dst);
	channel.unclaim();
}

void test_MemoryToPeripheral()
{
	::printf("\ntest_MemoryToPeripheral\n");
	const char src[] = "The quick brown fox jumps over the lazy dog.";
	DMA::Channel& channel = *DMA::claim_unused_channel();
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
	DMA::Channel& channelCtrl = *DMA::claim_unused_channel();
	DMA::Channel& channelData = *DMA::claim_unused_channel();
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
	DMA::Channel& channel = *DMA::claim_unused_channel();
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
	DMA::Channel& channel = *DMA::claim_unused_channel();
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

class IRQHandlerDMA : public DMA::IRQHandler {
public:
	virtual void OnDMAInterrupt(DMA::Channel& channel, uint irq_index) {
		::printf("Channel#%d finished\n", channel.GetChannel());
		channel.acknowledge_irqn(irq_index);
	}
};

void test_Interrupt()
{
	::printf("\ntest_Interrupt\n");
	const char src[] = "The quick brown fox jumps over the lazy dog.";
	char dst[count_of(src)];
	IRQHandlerDMA irqHandlerDMA;
	for (int i = 0; i < 4; i++) {
		DMA::Channel& channel = *DMA::claim_unused_channel();
		channel.SetSharedIRQHandler(0, irqHandlerDMA).EnableIRQ(0);
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
		::printf("Channel#%d started\n", channel.GetChannel());
		channel.set_config(config)
			.set_read_addr(src)
			.set_write_addr(dst)
			.set_trans_count_trig(count_of(src));
	}
	for (;;) ;
}

void test_Benchmark()
{
	::printf("\ntest_Benchmark\n");
	uint8_t* srcRAM = reinterpret_cast<uint8_t*>(::malloc(65536));
	const uint8_t* srcFlash = reinterpret_cast<const uint8_t*>(0x10000000);
	uint8_t* dst = reinterpret_cast<uint8_t*>(::malloc(65536));
	int64_t timeUSec_DMA8Bit, timeUSec_DMA8BitPreConfig;
	int64_t timeUSec_DMA16Bit, timeUSec_DMA16BitPreConfig;
	int64_t timeUSec_DMA32Bit, timeUSec_DMA32BitPreConfig;
	int64_t timeUSec_CPU8Bit, timeUSec_CPU16Bit, timeUSec_CPU32Bit;
	int64_t timeUSec_memcpy;
	struct TestCase {
		const char* name;
		const uint8_t* src;
		int bytesToTrans;
	} testCaseTbl[] = {
		{ "RAM to RAM", srcRAM, 64 },
		{ "Flash to RAM", srcFlash, 64 },
		{ "RAM to RAM", srcRAM, 256 },
		{ "Flash to RAM", srcFlash, 256 },
		{ "RAM to RAM", srcRAM, 1024 },
		{ "Flash to RAM", srcFlash, 1024 },
		{ "RAM to RAM", srcRAM, 4096 },
		{ "Flash to RAM", srcFlash, 4096 },
		{ "RAM to RAM", srcRAM, 16384 },
		{ "Flash to RAM", srcFlash, 16384 },
		{ "RAM to RAM", srcRAM, 65536 },
		{ "Flash to RAM", srcFlash, 65536 },
	};
	::memcpy(srcRAM, srcFlash, 65536);
	for (int iTestCase = 0; iTestCase < count_of(testCaseTbl); iTestCase++) {
		const TestCase& testCase = testCaseTbl[iTestCase];
		const uint8_t* src = testCase.src;
		int bytesToTrans = testCase.bytesToTrans;
		do {	
			absolute_time_t absTimeStart = ::get_absolute_time();
			DMA::Channel& channel = *DMA::claim_unused_channel();
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
			channel.set_config(config);
			channel.set_read_addr(src)
				.set_write_addr(dst)
				.set_trans_count_trig(bytesToTrans)
				.wait_for_finish_blocking();
			channel.unclaim();
			timeUSec_DMA8Bit = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		} while (0);
		do {	
			DMA::Channel& channel = *DMA::claim_unused_channel();
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
			channel.set_config(config);
			absolute_time_t absTimeStart = ::get_absolute_time();
			channel.set_read_addr(src)
				.set_write_addr(dst)
				.set_trans_count_trig(bytesToTrans)
				.wait_for_finish_blocking();
			timeUSec_DMA8BitPreConfig = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
			channel.unclaim();
		} while (0);
		do {	
			absolute_time_t absTimeStart = ::get_absolute_time();
			DMA::Channel& channel = *DMA::claim_unused_channel();
			DMA::ChannelConfig config;
			config.set_enable(true)
				.set_transfer_data_size(DMA_SIZE_16)
				.set_read_increment(true)
				.set_write_increment(true)
				.set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
				.set_chain_to(channel) // disable by setting chain_to to itself
				.set_ring_to_read(0)
				.set_bswap(false)
				.set_irq_quiet(false)
				.set_sniff_enable(false)
				.set_high_priority(false);
			channel.set_config(config);
			channel.set_read_addr(src)
				.set_write_addr(dst)
				.set_trans_count_trig(bytesToTrans / sizeof(uint16_t))
				.wait_for_finish_blocking();
			channel.unclaim();
			timeUSec_DMA16Bit = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		} while (0);
		do {	
			DMA::Channel& channel = *DMA::claim_unused_channel();
			DMA::ChannelConfig config;
			config.set_enable(true)
				.set_transfer_data_size(DMA_SIZE_16)
				.set_read_increment(true)
				.set_write_increment(true)
				.set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
				.set_chain_to(channel) // disable by setting chain_to to itself
				.set_ring_to_read(0)
				.set_bswap(false)
				.set_irq_quiet(false)
				.set_sniff_enable(false)
				.set_high_priority(false);
			channel.set_config(config);
			absolute_time_t absTimeStart = ::get_absolute_time();
			channel.set_read_addr(src)
				.set_write_addr(dst)
				.set_trans_count_trig(bytesToTrans / sizeof(uint16_t))
				.wait_for_finish_blocking();
			timeUSec_DMA16BitPreConfig = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
			channel.unclaim();
		} while (0);
		do {	
			absolute_time_t absTimeStart = ::get_absolute_time();
			DMA::Channel& channel = *DMA::claim_unused_channel();
			DMA::ChannelConfig config;
			config.set_enable(true)
				.set_transfer_data_size(DMA_SIZE_32)
				.set_read_increment(true)
				.set_write_increment(true)
				.set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
				.set_chain_to(channel) // disable by setting chain_to to itself
				.set_ring_to_read(0)
				.set_bswap(false)
				.set_irq_quiet(false)
				.set_sniff_enable(false)
				.set_high_priority(false);
			channel.set_config(config);
			channel.set_read_addr(src)
				.set_write_addr(dst)
				.set_trans_count_trig(bytesToTrans / sizeof(uint32_t))
				.wait_for_finish_blocking();
			channel.unclaim();
			timeUSec_DMA32Bit = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		} while (0);
		do {	
			DMA::Channel& channel = *DMA::claim_unused_channel();
			DMA::ChannelConfig config;
			config.set_enable(true)
				.set_transfer_data_size(DMA_SIZE_32)
				.set_read_increment(true)
				.set_write_increment(true)
				.set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
				.set_chain_to(channel) // disable by setting chain_to to itself
				.set_ring_to_read(0)
				.set_bswap(false)
				.set_irq_quiet(false)
				.set_sniff_enable(false)
				.set_high_priority(false);
			channel.set_config(config);
			absolute_time_t absTimeStart = ::get_absolute_time();
			channel.set_read_addr(src)
				.set_write_addr(dst)
				.set_trans_count_trig(bytesToTrans / sizeof(uint32_t))
				.wait_for_finish_blocking();
			timeUSec_DMA32BitPreConfig = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
			channel.unclaim();
		} while (0);
		do {
			absolute_time_t absTimeStart = ::get_absolute_time();
			const uint8_t* pSrc = src;
			uint8_t* pDst = dst;
			for (int i = bytesToTrans; i > 0; i--) *pDst++ = *pSrc++;
			timeUSec_CPU8Bit = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		} while (0);
		do {
			absolute_time_t absTimeStart = ::get_absolute_time();
			const uint16_t* pSrc = reinterpret_cast<const uint16_t*>(src);
			uint16_t* pDst = reinterpret_cast<uint16_t*>(dst);
			for (int i = bytesToTrans / sizeof(uint16_t); i > 0; i--) *pDst++ = *pSrc++;
			timeUSec_CPU16Bit = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		} while (0);
		do {
			absolute_time_t absTimeStart = ::get_absolute_time();
			const uint32_t* pSrc = reinterpret_cast<const uint32_t*>(src);
			uint32_t* pDst = reinterpret_cast<uint32_t*>(dst);
			for (int i = bytesToTrans / sizeof(uint32_t); i > 0; i--) *pDst++ = *pSrc++;
			timeUSec_CPU32Bit = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		} while (0);
		do {
			absolute_time_t absTimeStart = ::get_absolute_time();
			::memcpy(dst, src, bytesToTrans);
			timeUSec_memcpy = ::absolute_time_diff_us(absTimeStart, ::get_absolute_time());
		} while (0);
		::printf("%s %d bytes [%s]\n"
				"  DMA(8bit)              %lld usec\n"
				"  DMA(8bit, pre-config)  %lld usec\n"
				"  DMA(16bit)             %lld usec\n"
				"  DMA(16bit, pre-config) %lld usec\n"
				"  DMA(32bit)             %lld usec\n"
				"  DMA(32bit, pre-config) %lld usec\n"
				"  CPU(8bit)              %lld usec\n"
				"  CPU(16bit)             %lld usec\n"
				"  CPU(32bit)             %lld usec\n"
				"  memcpy                 %lld usec\n",
			testCase.name, bytesToTrans, compileVariant,
			timeUSec_DMA8Bit, timeUSec_DMA8BitPreConfig,
			timeUSec_DMA16Bit, timeUSec_DMA16BitPreConfig,
			timeUSec_DMA32Bit, timeUSec_DMA32BitPreConfig,
			timeUSec_CPU8Bit, timeUSec_CPU16Bit, timeUSec_CPU32Bit, timeUSec_memcpy);
	}
	::free(srcRAM);
	::free(dst);
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
	//test_Interrupt();
	test_Benchmark();
	for (;;) ::tight_loop_contents();
}
