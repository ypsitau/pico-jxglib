# DMA

```cpp
DMA::Channel& channel = *DMA::claim_unused_channel();
DMA::ChannelConfig config;
absolute_time_t absTimeStart = ::get_absolute_time();
config.set_enable(true)
    .set_transfer_data_size(DMA_SIZE_8)
    .set_read_increment(true)
    .set_write_increment(true)
    .set_dreq(DREQ_FORCE) // see RP2040 Datasheet 2.5.3.1 System DREQ Table
    .set_chain_to(channel) // disable by setting chain_to to itself
    .set_ring_read(0)
    .set_bswap(false)
    .set_irq_quiet(false)
    .set_sniff_enable(false)
    .set_high_priority(false);
channel.set_config(config)
    .set_read_addr(src)
    .set_write_addr(dst)
    .set_trans_count_trig(count_of(src))
    .wait_for_finish_blocking();
```
