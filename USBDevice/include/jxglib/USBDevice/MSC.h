//==============================================================================
// jxglib/USBDevice/MSC.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_MSC_H
#define PICO_JXGLIB_USBDEVICE_MSC_H
#include "jxglib/USBDevice.h"

#if CFG_TUD_MSC > 0

namespace jxglib::USBDevice {

class MSC : public Interface {
public:
	MSC(Controller& deviceController, const char* str, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize = 64);
public:
	virtual const char* GetTickableName() const override { return "Controller::MSC"; }
	virtual void OnTick() override {}
public:
	virtual void On_msc_inquiry(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4]) = 0;
	virtual bool On_msc_test_unit_ready(uint8_t lun) = 0;
	virtual void On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) = 0;
	virtual bool On_msc_start_stop(uint8_t lun, uint8_t power_condition, bool start, bool load_eject) = 0;
	virtual int32_t On_msc_read10(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize) = 0;
	virtual bool On_msc_is_writable(uint8_t lun) = 0;
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) = 0;
	virtual int32_t On_msc_scsi(uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize) = 0;

};

}

#endif

#endif
