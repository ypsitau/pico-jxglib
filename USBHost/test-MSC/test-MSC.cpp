#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

static scsi_inquiry_resp_t inquiry_resp;

bool inquiry_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const * cb_data)
{
	msc_cbw_t const* cbw = cb_data->cbw;
	msc_csw_t const* csw = cb_data->csw;

	if (csw->status != 0)
	{
		printf("Inquiry failed\r\n");
		return false;
	}

	// Print out Vendor ID, Product ID and Rev
	printf("%.8s %.16s rev %.4s\r\n", inquiry_resp.vendor_id, inquiry_resp.product_id, inquiry_resp.product_rev);

	// Get capacity of device
	uint32_t const block_count = tuh_msc_get_block_count(dev_addr, cbw->lun);
	uint32_t const block_size = tuh_msc_get_block_size(dev_addr, cbw->lun);

	printf("Disk Size: %" PRIu32 " MB\r\n", block_count / ((1024*1024)/block_size));
	printf("Block Count = %" PRIu32 ", Block Size: %" PRIu32 "\r\n", block_count, block_size);

	return true;
}

extern "C" void tuh_msc_mount_cb(uint8_t dev_addr)
{
	printf("A MassStorage device is mounted\r\n");

	uint8_t const lun = 0;
	::tuh_msc_inquiry(dev_addr, lun, &inquiry_resp, inquiry_complete_cb, 0);
}

extern "C" void tuh_msc_umount_cb(uint8_t dev_addr)
{
	printf("A MassStorage device is unmounted\r\n");
}

int main()
{
	::stdio_init_all();
	USBHost::Initialize();
	while (true) Tickable::Tick();
}
