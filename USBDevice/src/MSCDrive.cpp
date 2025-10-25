//==============================================================================
// MSCDrive.cpp
//==============================================================================
#include "jxglib/USBDevice/MSCDrive.h"

namespace jxglib::USBDevice {

MSCDrive::MSCDrive(USBDevice::Controller& deviceController, uint8_t endpBulkOut, uint8_t endpBulkIn) :
	USBDevice::MSC(deviceController, "Flash Interface", endpBulkOut, endpBulkIn),
	pDriveAttached_{nullptr}, offsetXIP_{0}, bytesXIP_{0}, syncAgent_(*this, 0), unitReadyFlag_{true}, pHookHandler_{nullptr}
{}

void MSCDrive::Initialize(FS::Drive& drive, int msecTimeoutSync)
{
	pDriveAttached_ = &drive;
	if (!drive.GetFlashInfo(&offsetXIP_, &bytesXIP_)) {
		::panic("MSCDrive::AttachDrive: Drive does not support flash info");
	}
	drive.SetEventHandler(this);
	syncAgent_.SetTimeoutSync(msecTimeoutSync);
	offsetXIP_ &= 0x0fff'ffff;
	MSC::Initialize();
}

void MSCDrive::On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	//::printf("On_msc_inquiry\n");
	static const char vid[] = "jxglib";
	static const char pid[] = "MSCDrive";
	static const char rev[] = "1.0";
	::memcpy(vendor_id,	vid, strlen(vid));
	::memcpy(product_id,	pid, strlen(pid));
	::memcpy(product_rev,	rev, strlen(rev));
}

bool MSCDrive::On_msc_test_unit_ready(uint8_t lun)
{
	//::printf("On_msc_test_unit_ready\n");
	if (unitReadyFlag_) return true;
	unitReadyFlag_ = true;
	return false;
}

void MSCDrive::On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	//::printf("On_msc_capacity\n");
	if (pHookHandler_) {
		pHookHandler_->On_msc_capacity(lun, block_count, block_size);
		return;
	}
	*block_count = bytesXIP_ / BlockSize;
	*block_size  = BlockSize;
}

bool MSCDrive::On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	//::printf("On_msc_start_stop\n");
	if (!load_eject) {
		// nothing to do
	} else if (start) {
		// load disk storage
	} else {
		Flash::Sync();
		// unload disk storage
	}
	return true;
}

int32_t MSCDrive::On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	if (pHookHandler_) return pHookHandler_->On_msc_read10(lun, lba, offset, buffer, bufsize);
	//::printf("On_msc_read10(lba=%d, bufsize=%d)\n", lba, bufsize);
	if (lba >= bytesXIP_ / BlockSize) return -1;
	Flash::Read(offsetXIP_ + lba * BlockSize, buffer, bufsize);
	return bufsize;
}

bool MSCDrive::On_msc_is_writable(uint8_t lun)
{
	//::printf("On_msc_is_writable\n");
	return true;
}

int32_t MSCDrive::On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	//::printf("On_msc_write10(lba=%d)\n", lba);
	if (pHookHandler_) return pHookHandler_->On_msc_write10(lun, lba, offset, buffer, bufsize);
	if (lba >= bytesXIP_ / BlockSize) return -1;
	Flash::Write(offsetXIP_ + lba * BlockSize, buffer, bufsize);
	syncAgent_.StartDevice();
	return bufsize;
}

int32_t MSCDrive::On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
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

void MSCDrive::SyncAgent::OnTick()
{
	if (timeoutCntDevice_ > 0) {
		--timeoutCntDevice_;
		if (timeoutCntDevice_ == 0) {
			Flash::Sync();
			mscDrive_.GetDriveAttached().Unmount();
		}
	}
	if (timeoutCntHost_ > 0) {
		--timeoutCntHost_;
		if (timeoutCntHost_ == 0) {
			mscDrive_.NotifyContentChanged();
		}
	}
}

}
