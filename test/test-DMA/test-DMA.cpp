#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/DMA.h"

int main()
{
	::stdio_init_all();
#if 0
	DMA::Channel chan(DMA::claim_unused_channel(true));
	dma_channel_config c = dma_channel_get_default_config(chan);
	channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
	channel_config_set_read_increment(&c, true);
	channel_config_set_write_increment(&c, true);
	dma_channel_configure(
		chan,          // Channel to be configured
		&c,            // The configuration we just created
		dst,           // The initial write address
		src,           // The initial read address
		count_of(src), // Number of transfers; in this case each is 1 byte.
		true           // Start immediately.
	);
	// We could choose to go and do something else whilst the DMA is doing its
	// thing. In this case the processor has nothing else to do, so we just
	// wait for the DMA to finish.
	dma_channel_wait_for_finish_blocking(chan);
	// The DMA has now copied our text from the transmit buffer (src) to the
	// receive buffer (dst), so we can print it out from there.
	puts(dst);
#endif
}
