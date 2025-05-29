#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBHost.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// USBHost::MSC
//-----------------------------------------------------------------------------
class MSC {
private:
	uint8_t dev_addr_;
	uint8_t lun_;
	MSC* pMSCNext_;
public:
	static MSC* pMSCTop;
public:
	MSC();
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

//-----------------------------------------------------------------------------
// USBHost::MSC
//-----------------------------------------------------------------------------
MSC* MSC::pMSCTop = nullptr;

MSC::MSC() : dev_addr_(UINT8_MAX), lun_(0), pMSCNext_{nullptr}
{
	if (pMSCTop) {
		MSC* pMSC = pMSCTop;
		for ( ; pMSC->pMSCNext_; pMSC = pMSC->pMSCNext_) ;
		pMSC->pMSCNext_ = this;
	} else {
		pMSCTop = this;
	}
}

MSC::~MSC()
{
	if (pMSCTop == this) {
		pMSCTop = pMSCNext_;
	} else {
		for (MSC* pMSC = pMSCTop; pMSC; pMSC = pMSC->pMSCNext_) {
			if (pMSC->pMSCNext_ == this) {
				pMSC->pMSCNext_ = pMSCNext_;
				break;
			}
		}
	}
}

bool MSC::scsi_command_NB(msc_cbw_t const* cbw, void* data, bool* pCompleteFlag)
{
	return IsMounted() && ::tuh_msc_scsi_command(dev_addr_, cbw, data, complete_cb, reinterpret_cast<uintptr_t>(pCompleteFlag));
}

bool MSC::scsi_command(msc_cbw_t const* cbw, void* data)
{
	bool completeFlag = false;
	if (!scsi_command_NB(cbw, data, &completeFlag)) return false;
	while (!completeFlag) Tickable::Tick();
	return true;
}

bool MSC::inquiry_NB(scsi_inquiry_resp_t* response, bool* pCompleteFlag)
{
	return IsMounted() && ::tuh_msc_inquiry(dev_addr_, lun_, response, complete_cb, reinterpret_cast<uintptr_t>(pCompleteFlag));
}

bool MSC::inquiry(scsi_inquiry_resp_t* response)
{
	bool completeFlag = false;
	if (!inquiry_NB(response, &completeFlag)) return false;
	while (!completeFlag) Tickable::Tick();
	return true;
}

bool MSC::test_unit_ready_NB(bool* pCompleteFlag)
{
	return IsMounted() && ::tuh_msc_test_unit_ready(dev_addr_, lun_, complete_cb, reinterpret_cast<uintptr_t>(pCompleteFlag));
}

bool MSC::test_unit_ready()
{
	bool completeFlag = false;
	if (!test_unit_ready_NB(&completeFlag)) return false;
	while (!completeFlag) Tickable::Tick();
	return true;
}

bool MSC::request_sense_NB(void* response, bool* pCompleteFlag)
{
	// Note: The callback will set the complete flag to true when the operation is done
	return IsMounted() && ::tuh_msc_request_sense(dev_addr_, lun_, response, complete_cb, reinterpret_cast<uintptr_t>(pCompleteFlag));
}

bool MSC::request_sense(void* response)
{
	bool completeFlag = false;
	if (!request_sense_NB(response, &completeFlag)) return false;
	while (!completeFlag) Tickable::Tick();
	return true;
}

bool MSC::read10_NB(void* buffer, uint32_t lba, uint16_t block_count, bool* pCompleteFlag)
{
	// Note: The callback will set the complete flag to true when the operation is done
	return IsMounted() && ::tuh_msc_read10(dev_addr_, lun_, buffer, lba, block_count, complete_cb, reinterpret_cast<uintptr_t>(pCompleteFlag));
}

bool MSC::read10(void* buffer, uint32_t lba, uint16_t block_count)
{
	bool completeFlag = false;
	if (!read10_NB(buffer, lba, block_count, &completeFlag)) return false;
	while (!completeFlag) Tickable::Tick();
	return true;
}

bool MSC::write10_NB(void const* buffer, uint32_t lba, uint16_t block_count, bool* pCompleteFlag)
{
	// Note: The callback will set the complete flag to true when the operation is done
	return IsMounted() && ::tuh_msc_write10(dev_addr_, lun_, buffer, lba, block_count, complete_cb, reinterpret_cast<uintptr_t>(pCompleteFlag));
}

bool MSC::write10(void const* buffer, uint32_t lba, uint16_t block_count)
{
	bool completeFlag = false;
	if (!write10_NB(buffer, lba, block_count, &completeFlag)) return false;
	while (!completeFlag) Tickable::Tick();
	return true;
}

bool MSC::read_capacity_NB(scsi_read_capacity10_resp_t* response, bool* pCompleteFlag)
{
	// Note: The callback will set the complete flag to true when the operation is done
	return IsMounted() && ::tuh_msc_read_capacity(dev_addr_, lun_, response, complete_cb, reinterpret_cast<uintptr_t>(pCompleteFlag));
}

bool MSC::read_capacity(scsi_read_capacity10_resp_t* response)
{
	bool completeFlag = false;
	if (!read_capacity_NB(response, &completeFlag)) return false;
	while (!completeFlag) Tickable::Tick();
	return true;
}

bool MSC::complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const* cb_data)
{
	bool* pCompleteFlag = reinterpret_cast<bool*>(cb_data->user_arg);
	if (cb_data->csw->status != 0) {
		::printf("MSC command failed with status %d\r\n", cb_data->csw->status);
		return false; // Indicate failure
	}
	*pCompleteFlag = true;
	return true; // Indicate success
}

extern "C" void tuh_msc_mount_cb(uint8_t dev_addr)
{
	printf("A MassStorage device is mounted\r\n");
	for (MSC* pMSC = MSC::pMSCTop; pMSC; pMSC = pMSC->GetNext()) {
		if (!pMSC->IsMounted()) {
			pMSC->SetDevAddr(dev_addr);
			break;
		}
	}
}

extern "C" void tuh_msc_umount_cb(uint8_t dev_addr)
{
	printf("A MassStorage device is unmounted\r\n");
	for (MSC* pMSC = MSC::pMSCTop; pMSC; pMSC = pMSC->GetNext()) {
		// If the device is mounted, we set the address to UINT8_MAX to indicate it's unmounted
		if (pMSC->GetDevAddr() == dev_addr) {
			pMSC->SetDevAddr(UINT8_MAX);
			break;
		}
	}
}

#if 0
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
#endif

MSC msc;


ShellCmd_Named(msc_inquiry, "msc-inquiry", "Inquiry the connected MSC device")
{
	scsi_inquiry_resp_t inquiry_resp;
	if (!msc.inquiry(&inquiry_resp)) {
		tout.Printf("Inquiry failed\r\n");
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

int main()
{
	::stdio_init_all();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	USBHost::Initialize();
	while (true) Tickable::Tick();
}
