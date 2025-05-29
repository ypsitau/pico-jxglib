#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBHost/MSC.h"

using namespace jxglib;

USBHost::MSC msc;

ShellCmd_Named(msc_inquiry, "msc-inquiry", "Inquiry the connected MSC device")
{
	scsi_inquiry_resp_t inquiry_resp;
	if (!msc.inquiry(&inquiry_resp)) {
		tout.Printf("Inquiry failed\r\n");
		return 1;
	}
	// Print out Vendor ID, Product ID and Rev
	printf("%.8s %.16s rev %.4s\r\n", inquiry_resp.vendor_id, inquiry_resp.product_id, inquiry_resp.product_rev);
	uint32_t const block_count = msc.get_block_count();
	uint32_t const block_size = msc.get_block_size();
	printf("Disk Size: %" PRIu32 " MB\r\n", block_count / ((1024 * 1024) / block_size));
	printf("Block Count = %" PRIu32 ", Block Size: %" PRIu32 "\r\n", block_count, block_size);
	return 0;
}

int main()
{
	::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	USBHost::Initialize();
	while (true) Tickable::Tick();
}
