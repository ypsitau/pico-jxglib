#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/dma.h"

namespace jxglib {

class DMA {
public:
	class ChannelConfig {
	private:
		dma_channel_config config_;
	public:
		ChannelConfig(const dma_channel_config& config) : config_{config} {}
	public:
		dma_channel_config& GetEntity() { return config_; }
		const dma_channel_config& GetEntity() const { return config_; }
		dma_channel_config* GetEntityPtr() { return &config_; }
		const dma_channel_config* GetEntityPtr() const { return &config_; }
	public:
		void set_read_increment(bool incr) { ::channel_config_set_read_increment(&config_, incr); }
		void set_write_increment(bool incr) { ::channel_config_set_write_increment(&config_, incr); }
		void set_dreq(uint dreq) { ::channel_config_set_dreq(&config_, dreq); }
		void set_chain_to(uint chain_to) { ::channel_config_set_chain_to(&config_, chain_to); }
		void set_transfer_data_size(enum dma_channel_transfer_size size) { ::channel_config_set_transfer_data_size(&config_, size); }
		void set_ring(bool write, uint size_bits) { ::channel_config_set_ring(&config_, write, size_bits); }
		void set_bswap(bool bswap) { ::channel_config_set_bswap(&config_, bswap); }
		void set_irq_quiet(bool irq_quiet) { ::channel_config_set_irq_quiet(&config_, irq_quiet); }
		void set_high_priority(bool high_priority) { ::channel_config_set_high_priority(&config_, high_priority); }
		void set_enable(bool enable) { ::channel_config_set_enable(&config_, enable); }
		void set_sniff_enable(bool sniff_enable) { ::channel_config_set_sniff_enable(&config_, sniff_enable); }
		uint32_t get_ctrl_value() const { return ::channel_config_get_ctrl_value(&config_); }
	};
	class Channel {
	private:
		uint channel_;
	public:
		void claim() { ::dma_channel_claim(channel_); }
		void unclaim() { ::dma_channel_unclaim(channel_); }
		bool is_claimed() { return ::dma_channel_is_claimed(channel_); }
		void set_config(const ChannelConfig& channelConfig, bool trigger) {
			::dma_channel_set_config(channel_, channelConfig.GetEntityPtr(), trigger);
		}
		void set_read_addr(const volatile void *read_addr, bool trigger) {
			::dma_channel_set_read_addr(channel_, read_addr, trigger);
		}
		void set_write_addr(volatile void *write_addr, bool trigger) {
			::dma_channel_set_write_addr(channel_, write_addr, trigger);
		}
		void set_trans_count(uint32_t trans_count, bool trigger) {
			::dma_channel_set_trans_count(channel_, trans_count, trigger);
		}
		void configure(const ChannelConfig& channelConfig, volatile void *write_addr, const volatile void *read_addr, uint transfer_count, bool trigger) {
			::dma_channel_configure(channel_, channelConfig.GetEntityPtr(), write_addr, read_addr, transfer_count, trigger);
		}
		void transfer_from_buffer_now(const volatile void *read_addr, uint32_t transfer_count) {
			::dma_channel_transfer_from_buffer_now(channel_, read_addr, transfer_count);
		}
		void transfer_to_buffer_now(volatile void *write_addr, uint32_t transfer_count) {
			::dma_channel_transfer_to_buffer_now(channel_, write_addr, transfer_count);
		}
		void start() { ::dma_channel_start(channel_); }
		void abort() { ::dma_channel_abort(channel_); }
		void set_irq0_enabled(bool enabled) { ::dma_channel_set_irq0_enabled(channel_, enabled); }
		void set_irq1_enabled(bool enabled) { ::dma_channel_set_irq1_enabled(channel_, enabled); }
		bool get_irq0_status() { return ::dma_channel_get_irq0_status(channel_); }
		bool get_irq1_status() { return ::dma_channel_get_irq1_status(channel_); }
		void acknowledge_irq0() { ::dma_channel_acknowledge_irq0(channel_); }
		void acknowledge_irq1() { ::dma_channel_acknowledge_irq1(channel_); }
		bool is_busy() { return ::dma_channel_is_busy(channel_); }
		void wait_for_finish_blocking() { ::dma_channel_wait_for_finish_blocking(channel_); }
		void cleanup() { ::dma_channel_cleanup(channel_); }
		ChannelConfig get_default_config() { return ChannelConfig(::dma_channel_get_default_config(channel_)); }
		ChannelConfig get_config() { return ChannelConfig(::dma_get_channel_config(channel_)); }
	};

#if 0
void dma_irqn_set_channel_enabled(uint irq_index, uint channel, bool enabled)
void dma_irqn_set_channel_mask_enabled(uint irq_index, uint32_t channel_mask, bool enabled)
bool dma_irqn_get_channel_status(uint irq_index, uint channel)
void dma_irqn_acknowledge_channel(uint irq_index, uint channel)

void dma_timer_claim(uint timer)
void dma_timer_unclaim(uint timer)
bool dma_timer_is_claimed(uint timer)
void dma_timer_set_fraction(uint timer, uint16_t numerator, uint16_t denominator)
#endif

	static void claim_mask(uint32_t channel_mask) { ::dma_claim_mask(channel_mask); }
	static void unclaim_mask(uint32_t channel_mask) { ::dma_unclaim_mask(channel_mask); }
	static int claim_unused_channel(bool required) { return ::dma_claim_unused_channel(required); }
	static void start_channel_mask(uint32_t chan_mask) { ::dma_start_channel_mask(chan_mask); }
	static void set_irq0_channel_mask_enabled(uint32_t channel_mask, bool enabled) { ::dma_set_irq0_channel_mask_enabled(channel_mask, enabled); }
	static void set_irq1_channel_mask_enabled(uint32_t channel_mask, bool enabled) { ::dma_set_irq1_channel_mask_enabled(channel_mask, enabled); }
	static void sniffer_enable(uint channel, uint mode, bool force_channel_enable) { ::dma_sniffer_enable(channel, mode, force_channel_enable); }
	static void sniffer_set_byte_swap_enabled(bool swap) { ::dma_sniffer_set_byte_swap_enabled(swap); }
	static void sniffer_set_output_invert_enabled(bool invert) { ::dma_sniffer_set_output_invert_enabled(invert); }
	static void sniffer_set_output_reverse_enabled(bool reverse) { ::dma_sniffer_set_output_reverse_enabled(reverse); }
	static void sniffer_disable(void) { ::dma_sniffer_disable(); }
	static void sniffer_set_data_accumulator(uint32_t seed_value) { ::dma_sniffer_set_data_accumulator(seed_value); }
	static uint32_t sniffer_get_data_accumulator(void) { return ::dma_sniffer_get_data_accumulator(); }
	static int claim_unused_timer(bool required) { return ::dma_claim_unused_timer(required); }
	static uint get_timer_dreq(uint timer_num) { return ::dma_get_timer_dreq(timer_num); }
};

}

int main()
{
	//channel_config_set_transfer_data_size();
	::stdio_init_all();

	while (true) {
		::printf("Hello, world!\n");
		::sleep_ms(1000);
	}
}
