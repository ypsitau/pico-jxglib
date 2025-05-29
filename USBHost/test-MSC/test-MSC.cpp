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
		terr.Printf("Inquiry failed\r\n");
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

ShellCmd_Named(msc_dump, "msc-dump", "Read a block from the connected MSC device")
{
	uint32_t lba = 0;
	if (argc >= 2) {
		char* p = nullptr;
		uint32_t num = ::strtoul(argv[1], &p, 0);
		if (*p != '\0') {
			terr.Printf("invalid number\n");
			return 1;
		}
		lba = num;
	}
	uint8_t buff[512];
	if (!msc.read10(buff, lba, 1)) {
		terr.Printf("failed in read10()\n");
		return 1;
	}
	Printable::DumpT(tout).DigitsAddr(4)(buff, sizeof(buff));
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
