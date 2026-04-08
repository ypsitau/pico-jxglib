//==============================================================================
// jxglib/USBHost/MSC.h
//==============================================================================
#ifndef PICO_JXGLIB_USBHOST_MSC_H
#define PICO_JXGLIB_USBHOST_MSC_H
#include "jxglib/USBHost.h"

#if CFG_TUH_MSC > 0

namespace jxglib::USBHost {

//-----------------------------------------------------------------------------
// USBHost::MSC
//-----------------------------------------------------------------------------
class MSC {
private:
	uint8_t orderHint_;
	uint8_t dev_addr_;
	uint8_t lun_;
	MSC* pMSCNext_;
public:
	static MSC* pMSCTop;
public:
	MSC(uint8_t orderHint = UINT8_MAX);
	~MSC();
public:
	void SetDevAddr(uint8_t dev_addr) { dev_addr_ = dev_addr; }
	uint16_t GetDevAddr() const { return dev_addr_; }
	uint8_t GetLun() const { return lun_; }
	MSC* GetNext() const { return pMSCNext_; }
	bool IsMounted() const { return dev_addr_ != UINT8_MAX; }
public:
	//bool mounted() { return ::tuh_msc_mounted(dev_addr_); }
	bool ready() { return ::tuh_msc_ready(dev_addr_); }
	uint8_t get_maxlun() { return ::tuh_msc_get_maxlun(dev_addr_); }
	uint32_t get_block_count() { return ::tuh_msc_get_block_count(dev_addr_, lun_); }
	uint32_t get_block_size() { return ::tuh_msc_get_block_size(dev_addr_, lun_); }
	uint32_t get_block() { return ::tuh_msc_get_block_count(dev_addr_, lun_); }
	bool scsi_command_NB(msc_cbw_t const* cbw, void* data, bool* pCompleteFlag);
	bool scsi_command(msc_cbw_t const* cbw, void* data);
	bool inquiry_NB(scsi_inquiry_resp_t* response, bool* pCompleteFlag);
	bool inquiry(scsi_inquiry_resp_t* response);
	bool test_unit_ready_NB(bool* pCompleteFlag);
	bool test_unit_ready();
	bool request_sense_NB(void* response, bool* pCompleteFlag);
	bool request_sense(void* response);
	bool read10_NB(void* buffer, uint32_t lba, uint16_t block_count, bool* pCompleteFlag);
	bool read10(void* buffer, uint32_t lba, uint16_t block_count);
	bool write10_NB(void const* buffer, uint32_t lba, uint16_t block_count, bool* pCompleteFlag);
	bool write10(void const* buffer, uint32_t lba, uint16_t block_count);
	bool read_capacity_NB(scsi_read_capacity10_resp_t* response, bool* pCompleteFlag);
	bool read_capacity(scsi_read_capacity10_resp_t* response);
private:
	static bool complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data);
};

}

#endif

#endif
