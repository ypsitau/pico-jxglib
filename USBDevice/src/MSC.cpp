//==============================================================================
// MSC.cpp
//==============================================================================
#include "jxglib/USBDevice/MSC.h"

#if CFG_TUD_MSC > 0

namespace jxglib::USBDevice {

//-----------------------------------------------------------------------------
// USBDevice::MSC
//-----------------------------------------------------------------------------
MSC::MSC(Controller& deviceController, const char* str, uint8_t endpBulkOut, uint8_t endpBulkIn, uint16_t endpSize) : Interface(deviceController, 1, 0)
{
	uint8_t configDesc[] = {
		// Interface number, string index, EP Out & EP In address, EP size
		TUD_MSC_DESCRIPTOR(interfaceNum_, deviceController.RegisterStringDesc(str), endpBulkOut, endpBulkIn, endpSize),
	};
	iInstance_ = deviceController.AddInterface_MSC(this);
	RegisterConfigDesc(configDesc, sizeof(configDesc));
}

}

// Invoked when received SCSI_CMD_INQUIRY
// Application fill vendor id, product id and revision with string up to 8, 16, 4 characters respectively
void tud_msc_inquiry_cb(uint8_t lun, uint8_t vendor_id[8], uint8_t product_id[16], uint8_t product_rev[4])
{
	using namespace jxglib;
	Controller::GetInterface_MSC()->On_msc_inquiry(lun, vendor_id, product_id, product_rev);
}

// Invoked when received Test Unit Ready command.
// return true allowing host to read/write this LUN e.g SD card inserted
bool tud_msc_test_unit_ready_cb(uint8_t lun)
{
	using namespace jxglib;
	return Controller::GetInterface_MSC()->On_msc_test_unit_ready(lun);
}

// Invoked when received SCSI_CMD_READ_CAPACITY_10 and SCSI_CMD_READ_FORMAT_CAPACITY to determine the disk size
// Application update block count and block size
void tud_msc_capacity_cb(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	using namespace jxglib;
	return Controller::GetInterface_MSC()->On_msc_capacity(lun, block_count, block_size);
}

// Invoked when received Start Stop Unit command
// - Start = 0 : stopped power mode, if load_eject = 1 : unload disk storage
// - Start = 1 : active mode, if load_eject = 1 : load disk storage
bool tud_msc_start_stop_cb(uint8_t lun, uint8_t power_condition, bool start, bool load_eject)
{
	using namespace jxglib;
	return Controller::GetInterface_MSC()->On_msc_start_stop(lun, power_condition, start, load_eject);
}

// Callback invoked when received READ10 command.
// Copy disk's data to buffer (up to bufsize) and return number of copied bytes.
int32_t tud_msc_read10_cb(uint8_t lun, uint32_t lba, uint32_t offset, void* buffer, uint32_t bufsize)
{
	using namespace jxglib;
	return Controller::GetInterface_MSC()->On_msc_read10(lun, lba, offset, buffer, bufsize);
}

bool tud_msc_is_writable_cb(uint8_t lun)
{
	using namespace jxglib;
	return Controller::GetInterface_MSC()->On_msc_is_writable(lun);
}

// Callback invoked when received WRITE10 command.
// Process data in buffer to disk's storage and return number of written bytes
int32_t tud_msc_write10_cb(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	using namespace jxglib;
	return Controller::GetInterface_MSC()->On_msc_write10(lun, lba, offset, buffer, bufsize);
}

// Callback invoked when received an SCSI command not in built-in list below
// - READ_CAPACITY10, READ_FORMAT_CAPACITY, INQUIRY, MODE_SENSE6, REQUEST_SENSE
// - READ10 and WRITE10 has their own callbacks
int32_t tud_msc_scsi_cb (uint8_t lun, uint8_t const scsi_cmd[16], void* buffer, uint16_t bufsize)
{
	using namespace jxglib;
	return Controller::GetInterface_MSC()->On_msc_scsi(lun, scsi_cmd, buffer, bufsize);
}

#endif
