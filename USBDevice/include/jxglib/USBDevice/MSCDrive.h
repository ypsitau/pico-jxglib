//==============================================================================
// jxglib/USBDevice/MSCDrive.h
//==============================================================================
#ifndef PICO_JXdGLIB_USBDEVICE_MSCDRIVE_H
#define PICO_JXGLIB_USBDEVICE_MSCDRIVE_H
#include "pico/stdlib.h"
#include "jxglib/USBDevice/MSC.h"
#include "jxglib/Flash.h"

#if CFG_TUD_MSC > 0

namespace jxglib::USBDevice {

//-----------------------------------------------------------------------------
// MSCDrive
//-----------------------------------------------------------------------------
class MSCDrive : public USBDevice::MSC {
public:
	class SyncAgent : public Tickable {
	private:
		int cntUntilSync_ ;
	public:
		SyncAgent() : Tickable(500, Tickable::Priority::Lowest), cntUntilSync_{0} {}
	public:
		void Start() { cntUntilSync_ = 2; }
	public:
		// virual functions of Tickable
		virtual void OnTick() override;
	};
public:
	static const int BlockSize = 512;
private:
	uint32_t offsetXIP_;
	uint32_t bytesXIP_;
	SyncAgent syncAgent_;
	bool unitReadyFlag_;
public:
	MSCDrive(USBDevice::Controller& deviceController, uint32_t addrXIP, uint32_t bytesXIP, uint8_t endpBulkOut, uint8_t endpBulkIn);
public:
	void NotifyMediaChange() { unitReadyFlag_ = false; }
public:
	// virual functions of USBDevice::MSC
	virtual void On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) override;
	virtual bool On_msc_test_unit_ready(uint8_t lun) override;
	virtual void On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) override;
	virtual bool On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) override;
	virtual int32_t On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) override;
	virtual bool On_msc_is_writable(uint8_t lun) override;
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) override;
	virtual int32_t On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) override;
};

}

#endif

#endif
