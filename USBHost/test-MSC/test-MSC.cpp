#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

static scsi_inquiry_resp_t inquiry_resp;

#if 0
//typedef bool (*tuh_msc_complete_cb_t)(uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data);

// This function true after tuh_msc_mounted_cb() and false after tuh_msc_unmounted_cb()
bool tuh_msc_mounted(uint8_t dev_addr);
// Check if the interface is currently ready or busy transferring data
bool tuh_msc_ready(uint8_t dev_addr);
uint8_t tuh_msc_get_maxlun(uint8_t dev_addr);
uint32_t tuh_msc_get_block_count(uint8_t dev_addr, uint8_t lun);
uint32_t tuh_msc_get_block_size(uint8_t dev_addr, uint8_t lun);
// NOTE: buffer must be accessible by USB/DMA controller, aligned correctly and multiple of cache line if enabled
bool tuh_msc_scsi_command(uint8_t daddr, msc_cbw_t const* cbw, void* data, tuh_msc_complete_cb_t complete_cb, uintptr_t arg);
// NOTE: response must be accessible by USB/DMA controller, aligned correctly and multiple of cache line if enabled
bool tuh_msc_inquiry(uint8_t dev_addr, uint8_t lun, scsi_inquiry_resp_t* response, tuh_msc_complete_cb_t complete_cb, uintptr_t arg);
bool tuh_msc_test_unit_ready(uint8_t dev_addr, uint8_t lun, tuh_msc_complete_cb_t complete_cb, uintptr_t arg);
// NOTE: response must be accessible by USB/DMA controller, aligned correctly and multiple of cache line if enabled
bool tuh_msc_request_sense(uint8_t dev_addr, uint8_t lun, void *response, tuh_msc_complete_cb_t complete_cb, uintptr_t arg);
// NOTE: buffer must be accessible by USB/DMA controller, aligned correctly and multiple of cache line if enabled
bool tuh_msc_read10(uint8_t dev_addr, uint8_t lun, void * buffer, uint32_t lba, uint16_t block_count, tuh_msc_complete_cb_t complete_cb, uintptr_t arg);
// NOTE: buffer must be accessible by USB/DMA controller, aligned correctly and multiple of cache line if enabled
bool tuh_msc_write10(uint8_t dev_addr, uint8_t lun, void const * buffer, uint32_t lba, uint16_t block_count, tuh_msc_complete_cb_t complete_cb, uintptr_t arg);
bool tuh_msc_read_capacity(uint8_t dev_addr, uint8_t lun, scsi_read_capacity10_resp_t* response, tuh_msc_complete_cb_t complete_cb, uintptr_t arg);

extern "C" void tuh_msc_mount_cb(uint8_t dev_addr)
{
}

extern "C" void tuh_msc_umount_cb(uint8_t dev_addr)
{
}
#endif

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
