//==============================================================================
// jxglib/USBDevice/MSCDrive.h
//==============================================================================
#ifndef PICO_JXdGLIB_USBDEVICE_MSCDRIVE_H
#define PICO_JXGLIB_USBDEVICE_MSCDRIVE_H
#include "pico/stdlib.h"
#include "jxglib/USBDevice/MSC.h"
#include "jxglib/FS.h"
#include "jxglib/Flash.h"

#if CFG_TUD_MSC > 0

namespace jxglib::USBDevice {

//-----------------------------------------------------------------------------
// MSCDrive
//-----------------------------------------------------------------------------
class MSCDrive : public USBDevice::MSC, public FS::Drive::EventHandler {
public:
	class SyncAgent : public Tickable {
	private:
		MSCDrive& mscDrive_;
		int timeoutCntInit_;
		int timeoutCntDevice_ ;
		int timeoutCntHost_;
	public:
		SyncAgent(MSCDrive& mscDrive, int msecTimeoutSync) : Tickable(100, Tickable::Priority::Lowest),
				mscDrive_{mscDrive}, timeoutCntInit_{msecTimeoutSync / 100}, timeoutCntDevice_{0}, timeoutCntHost_{0} {}
	public:
		void SetTimeoutSync(int msecTimeoutSync) { timeoutCntInit_ = msecTimeoutSync / 100; }
		void StartDevice() { timeoutCntDevice_ = timeoutCntInit_; }
		void StartHost() { timeoutCntHost_ = timeoutCntInit_; }
	public:
		// virual functions of Tickable
		virtual void OnTick() override;
		virtual const char* GetTickableName() const override { return "USBDevice::MSCDrive::SyncAgent"; }
	};
	class HookHandler {
	public:
		virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) = 0;
	};
public:
	static const int BlockSize = 512;
private:
	FS::Drive* pDriveAttached_;
	uint32_t offsetXIP_;
	uint32_t bytesXIP_;
	SyncAgent syncAgent_;
	bool unitReadyFlag_;
	HookHandler* pHookHandler_;
public:
	MSCDrive(USBDevice::Controller& deviceController, uint8_t endpBulkOut, uint8_t endpBulkIn);
public:
	void Initialize(FS::Drive& drive, int msecTimeoutSync = 2000);
	FS::Drive& GetDriveAttached() { return *pDriveAttached_; }
public:
	void NotifyContentChanged() { unitReadyFlag_ = false; }
public:
	void SetHookHandler(HookHandler& hookHandler) { pHookHandler_ = &hookHandler; }
	void ClearHookHandler() { pHookHandler_ = nullptr; }
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
public:
	// virtual functions of FS::Drive::EventHandler
	virtual void OnContentChanged() override { syncAgent_.StartHost(); }
};

}

#endif

#endif
