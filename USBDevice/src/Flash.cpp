//==============================================================================
// Flash.cpp
//==============================================================================
#include "jxglib/USBDevice/Flash.h"

namespace jxglib::USBDevice {

Flash::Flash(USBDevice::Controller& deviceController, uint32_t addrXIP, uint32_t bytesXIP, uint8_t endpBulkOut, uint8_t endpBulkIn) :
	USBDevice::MSC(deviceController, "Flash Interface", endpBulkOut, endpBulkIn),
	offsetXIP_{addrXIP & 0x0fff'ffff}, bytesXIP_{bytesXIP}
{}

void Flash::On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	//::printf("On_msc_inquiry\n");
	static const char vid[] = "TinyUSB";
	static const char pid[] = "Mass Storage";
	static const char rev[] = "1.0";
	memcpy(vendor_id,	vid, strlen(vid));
	memcpy(product_id,	pid, strlen(pid));
	memcpy(product_rev,	rev, strlen(rev));
}

bool Flash::On_msc_test_unit_ready(uint8_t lun)
{
	::printf("On_msc_test_unit_ready\n");
	return true;
}

void Flash::On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	//::printf("On_msc_capacity\n");
	*block_count = bytesXIP_ / BlockSize;
	*block_size  = BlockSize;
}

bool Flash::On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	//::printf("On_msc_start_stop\n");
	if (!load_eject) {
		// nothing to do
	} else if (start) {
		// load disk storage
	} else {
		jxglib::Flash::Sync();
		// unload disk storage
	}
	return true;
}

int32_t Flash::On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	//::printf("On_msc_read10(lba=%d, bufsize=%d)\n", lba, bufsize);
	if (lba >= bytesXIP_ / BlockSize) return -1;
	//sdCard_.ReadBlock(lba, buffer, bufsize / BlockSize);
	jxglib::Flash::Read(offsetXIP_ + lba * BlockSize, buffer, bufsize);
	return bufsize;
}

bool Flash::On_msc_is_writable(uint8_t lun)
{
	//::printf("On_msc_is_writable\n");
	return true;
}

int32_t Flash::On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	//::printf("On_msc_write10(lba=%d)\n", lba);
	if (lba >= bytesXIP_ / BlockSize) return -1;
	jxglib::Flash::Write(offsetXIP_ + lba * BlockSize, buffer, bufsize);
	syncAgent_.Start();
	return bufsize;
}

int32_t Flash::On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	//::printf("On_msc_scsi\n");
	switch (scsi_cmd[0]) {
	default:
		// Set Sense = Invalid Command Operation
		::tud_msc_set_sense(lun, SCSI_SENSE_ILLEGAL_REQUEST, 0x20, 0x00);
		// negative means error -> tinyusb could stall and/or response with failed status
		return -1;
	}
	return 0;
}

void Flash::SyncAgent::OnTick()
{
	if (cntUntilSync_ > 0) {
		--cntUntilSync_;
		if (cntUntilSync_ == 0) jxglib::Flash::Sync();
	}
}

}
