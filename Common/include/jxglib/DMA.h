//==============================================================================
// jxglib/DMA.h
//==============================================================================
#ifndef PICO_JXGLIB_DMA_H
#define PICO_JXGLIB_DMA_H
#include "pico/stdlib.h"
#include "hardware/dma.h"

namespace jxglib {

class DMA {
public:
	class ChannelConfig {
	private:
		dma_channel_config config_;
	public:
		ChannelConfig() : config_{0} {}
		ChannelConfig(const dma_channel_config& config) : config_{config} {}
		ChannelConfig(const ChannelConfig& channelConfig) : config_{channelConfig.config_} {}
	public:
		dma_channel_config& GetEntity() { return config_; }
		const dma_channel_config& GetEntity() const { return config_; }
		dma_channel_config* GetEntityPtr() { return &config_; }
		const dma_channel_config* GetEntityPtr() const { return &config_; }
	public:
		ChannelConfig& set_read_increment(bool incr) {
			::channel_config_set_read_increment(&config_, incr); return *this;
		}
		ChannelConfig& set_write_increment(bool incr) {
			::channel_config_set_write_increment(&config_, incr); return *this;
		}
		ChannelConfig& set_dreq(uint dreq) {
			::channel_config_set_dreq(&config_, dreq); return *this;
		}
		ChannelConfig& set_chain_to(uint chain_to) {
			::channel_config_set_chain_to(&config_, chain_to); return *this;
		}
		ChannelConfig& set_transfer_data_size(enum dma_channel_transfer_size size) {
			::channel_config_set_transfer_data_size(&config_, size); return *this;
		}
		ChannelConfig& set_ring(bool write, uint size_bits) {
			::channel_config_set_ring(&config_, write, size_bits); return *this;
		}
		ChannelConfig& set_ring_to_read(uint size_bits) {
			::channel_config_set_ring(&config_, false, size_bits); return *this;
		}
		ChannelConfig& set_ring_to_write(uint size_bits) {
			::channel_config_set_ring(&config_, true, size_bits); return *this;
		}
		ChannelConfig& set_bswap(bool bswap) {
			::channel_config_set_bswap(&config_, bswap); return *this;
		}
		ChannelConfig& set_irq_quiet(bool irq_quiet) {
			::channel_config_set_irq_quiet(&config_, irq_quiet); return *this;
		}
		ChannelConfig& set_high_priority(bool high_priority) {
			::channel_config_set_high_priority(&config_, high_priority); return *this;
		}
		ChannelConfig& set_enable(bool enable) {
			::channel_config_set_enable(&config_, enable); return *this;
		}
		ChannelConfig& set_sniff_enable(bool sniff_enable) {
			::channel_config_set_sniff_enable(&config_, sniff_enable); return *this;
		}
		uint32_t get_ctrl_value() const { return ::channel_config_get_ctrl_value(&config_); }
	};
	class Channel {
	private:
		uint channel_;
	public:
		Channel(uint channel) : channel_{channel} {}
		Channel(const Channel& channel) : channel_{channel.channel_} {}
	public:
		operator uint() const { return channel_; }
	public:
		const Channel& claim() const { ::dma_channel_claim(channel_); return *this; }
		const Channel& unclaim() const { ::dma_channel_unclaim(channel_); return *this; }
		bool is_claimed() const { return ::dma_channel_is_claimed(channel_); }
		dma_channel_hw_t* get_hw() { return ::dma_channel_hw_addr(channel_); }
		const Channel& set_config(const ChannelConfig& channelConfig, bool trigger) const {
			::dma_channel_set_config(channel_, channelConfig.GetEntityPtr(), trigger);
			return *this;
		}
		const Channel& set_config(const ChannelConfig& channelConfig) const {
			::dma_channel_hw_addr(channel_)->al1_ctrl = channelConfig.get_ctrl_value();
			return *this;
		}
		const Channel& set_config_trig(const ChannelConfig& channelConfig) const {
			::dma_channel_hw_addr(channel_)->ctrl_trig = channelConfig.get_ctrl_value();
			return *this;
		}
		const Channel& set_read_addr(const volatile void* read_addr, bool trigger) const {
			::dma_channel_set_read_addr(channel_, read_addr, trigger);
			return *this;
		}
		const Channel& set_read_addr(const volatile void* read_addr) const {
			::dma_channel_hw_addr(channel_)->read_addr = reinterpret_cast<uintptr_t>(read_addr);
			return *this;
		}
		const Channel& set_read_addr_trig(const volatile void* read_addr) const {
			::dma_channel_hw_addr(channel_)->al3_read_addr_trig = reinterpret_cast<uintptr_t>(read_addr);
			return *this;
		}
		const Channel& set_write_addr(volatile void* write_addr, bool trigger) const {
			::dma_channel_set_write_addr(channel_, write_addr, trigger);
			return *this;
		}
		const Channel& set_write_addr(volatile void* write_addr) const {
			::dma_channel_hw_addr(channel_)->write_addr = reinterpret_cast<uintptr_t>(write_addr);
			return *this;
		}
		const Channel& set_write_addr_trig(volatile void* write_addr) const {
			::dma_channel_hw_addr(channel_)->al2_write_addr_trig = reinterpret_cast<uintptr_t>(write_addr);
			return *this;
		}
		const Channel& set_trans_count(uint32_t trans_count, bool trigger) const {
			::dma_channel_set_trans_count(channel_, trans_count, trigger);
			return *this;
		}
		const Channel& set_trans_count(uint32_t trans_count) const {
			::dma_channel_hw_addr(channel_)->transfer_count = trans_count;
			return *this;
		}
		const Channel& set_trans_count_trig(uint32_t trans_count) const {
			::dma_channel_hw_addr(channel_)->al1_transfer_count_trig = trans_count;
			return *this;
		}
		const Channel& configure(const ChannelConfig& channelConfig, volatile void* write_addr, const volatile void* read_addr, uint transfer_count, bool trigger) const {
			::dma_channel_configure(channel_, channelConfig.GetEntityPtr(), write_addr, read_addr, transfer_count, trigger);
			return *this;
		}
		const Channel& transfer_from_buffer_now(const volatile void* read_addr, uint32_t transfer_count) const {
			::dma_channel_transfer_from_buffer_now(channel_, read_addr, transfer_count);
			return *this;
		}
		const Channel& transfer_to_buffer_now(volatile void* write_addr, uint32_t transfer_count) const {
			::dma_channel_transfer_to_buffer_now(channel_, write_addr, transfer_count);
			return *this;
		}
		const Channel& start() const { ::dma_channel_start(channel_); return *this; }
		const Channel& abort() const { ::dma_channel_abort(channel_); return *this; }
		const Channel& set_irq0_enabled(bool enabled) const { ::dma_channel_set_irq0_enabled(channel_, enabled); return *this; }
		const Channel& set_irq1_enabled(bool enabled) const { ::dma_channel_set_irq1_enabled(channel_, enabled); return *this; }
		bool get_irq0_status() const { return ::dma_channel_get_irq0_status(channel_); }
		bool get_irq1_status() const { return ::dma_channel_get_irq1_status(channel_); }
		const Channel& acknowledge_irq0() const { ::dma_channel_acknowledge_irq0(channel_); return *this; }
		const Channel& acknowledge_irq1() const { ::dma_channel_acknowledge_irq1(channel_); return *this; }
		bool is_busy() const { return ::dma_channel_is_busy(channel_); }
		const Channel& wait_for_finish_blocking() const { ::dma_channel_wait_for_finish_blocking(channel_); return *this; }
		const Channel& cleanup() const { ::dma_channel_cleanup(channel_); return *this; }
		ChannelConfig get_default_config() const { return ChannelConfig(::dma_channel_get_default_config(channel_)); }
		ChannelConfig get_config() const { return ChannelConfig(::dma_get_channel_config(channel_)); }
		const Channel& sniffer_enable(uint mode, bool force_channel_enabled = true) const {
			::dma_sniffer_enable(channel_, mode, force_channel_enabled);
			return *this;
		}
		const Channel& sniffer_enable_CALC_VALUE_CRC32(bool force_channel_enabled = true) const {
			return sniffer_enable(DMA_SNIFF_CTRL_CALC_VALUE_CRC32, force_channel_enabled);	// 0x0
		}
		const Channel& sniffer_enable_CALC_VALUE_CRC32R(bool force_channel_enabled = true) const {
			return sniffer_enable(DMA_SNIFF_CTRL_CALC_VALUE_CRC32R, force_channel_enabled);	// 0x1
		}
		const Channel& sniffer_enable_CALC_VALUE_CRC16(bool force_channel_enabled = true) const {
			return sniffer_enable(DMA_SNIFF_CTRL_CALC_VALUE_CRC16, force_channel_enabled);	// 0x2
		}
		const Channel& sniffer_enable_CALC_VALUE_CRC16R(bool force_channel_enabled = true) const {
			return sniffer_enable(DMA_SNIFF_CTRL_CALC_VALUE_CRC16R, force_channel_enabled);	// 0x3
		}
		const Channel& sniffer_enable_CALC_VALUE_EVEN(bool force_channel_enabled = true) const {
			return sniffer_enable(DMA_SNIFF_CTRL_CALC_VALUE_EVEN, force_channel_enabled);	// 0xe
		}
		const Channel& sniffer_enable_CALC_VALUE_SUM(bool force_channel_enabled = true) const {
			return sniffer_enable(DMA_SNIFF_CTRL_CALC_VALUE_SUM, force_channel_enabled);	// 0xf
		}
		bool get_raw_interrupt_status() const { return !!(dma_hw->intr & (1u << channel_)); }
	};
	class Timer {
	private:
		uint timer_;
	public:
		Timer(uint timer) : timer_{timer} {}
		Timer(const Timer& timer) : timer_{timer.timer_} {}
	public:
		operator uint() const { return timer_; }
	public:
		const Timer& claim() const { ::dma_timer_claim(timer_); return *this; }
		const Timer& unclaim() const { ::dma_timer_unclaim(timer_); return *this; }
		bool is_claimed() const { return ::dma_timer_is_claimed(timer_); }
		const Timer& set_fraction(uint16_t numerator, uint16_t denominator) const {
			::dma_timer_set_fraction(timer_, numerator, denominator);
			return *this;
		}
		uint get_dreq() const { return ::dma_get_timer_dreq(timer_); }
	};
	class IRQ_n {
	private:
		uint irq_index_;
	public:
		IRQ_n(uint irq_index) : irq_index_{irq_index} {}
		IRQ_n(const IRQ_n& irqn) : irq_index_{irqn.irq_index_} {}
	public:
		operator uint() const { return irq_index_; }
	public:
		const IRQ_n& set_channel_enabled(uint channel, bool enabled) const {
			::dma_irqn_set_channel_enabled(irq_index_, channel, enabled);
			return *this;
		}
		const IRQ_n& set_channel_mask_enabled(uint32_t channel_mask, bool enabled) const {
			::dma_irqn_set_channel_mask_enabled(irq_index_, channel_mask, enabled);
			return *this;
		}
		bool get_channel_status(uint channel) const {
			return ::dma_irqn_get_channel_status(irq_index_, channel);
		}
		const IRQ_n& acknowledge_channel(uint channel) const {
			::dma_irqn_acknowledge_channel(irq_index_, channel);
			return *this;
		}
	};
	class Sniffer {
	public:
		static void set_byte_swap_enabled(bool swap) { ::dma_sniffer_set_byte_swap_enabled(swap); }
		static void set_output_invert_enabled(bool invert) { ::dma_sniffer_set_output_invert_enabled(invert); }
		static void set_output_reverse_enabled(bool reverse) { ::dma_sniffer_set_output_reverse_enabled(reverse); }
		static void disable(void) { ::dma_sniffer_disable(); }
		static void set_data_accumulator(uint32_t seed_value) { ::dma_sniffer_set_data_accumulator(seed_value); }
		static uint32_t get_data_accumulator(void) { return ::dma_sniffer_get_data_accumulator(); }
	};
public:
	static const Channel Channel0;
	static const Channel Channel1;
	static const Channel Channel2;
	static const Channel Channel3;
	static const Channel Channel4;
	static const Channel Channel5;
	static const Channel Channel6;
	static const Channel Channel7;
	static const Channel Channel8;
	static const Channel Channel9;
	static const Channel Channel10;
	static const Channel Channel11;
	static const Timer Timer0;
	static const Timer Timer1;
	static const Timer Timer2;
	static const Timer Timer3;
	static const IRQ_n IRQ_0;
	static const IRQ_n IRQ_1;
public:
	static Channel claim_unused_channel(bool required) { return Channel(::dma_claim_unused_channel(required)); }
	static Timer claim_unused_timer(bool required) { return Timer(::dma_claim_unused_timer(required)); }
public:
	static void claim_mask(uint32_t channel_mask) { ::dma_claim_mask(channel_mask); }
	static void unclaim_mask(uint32_t channel_mask) { ::dma_unclaim_mask(channel_mask); }
	static void start_channel_mask(uint32_t channel_mask) { ::dma_start_channel_mask(channel_mask); }
	static void set_irq0_channel_mask_enabled(uint32_t channel_mask, bool enabled) { ::dma_set_irq0_channel_mask_enabled(channel_mask, enabled); }
	static void set_irq1_channel_mask_enabled(uint32_t channel_mask, bool enabled) { ::dma_set_irq1_channel_mask_enabled(channel_mask, enabled); }
};

}

#endif
