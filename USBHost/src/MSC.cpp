//==============================================================================
// MSC.cpp
//==============================================================================
#include "jxglib/USBHost/MSC.h"

#if CFG_TUH_MSC > 0

namespace jxglib::USBHost {

//-----------------------------------------------------------------------------
// USBHost::MSC
//-----------------------------------------------------------------------------
MSC* MSC::pMSCTop = nullptr;

MSC::MSC(uint8_t orderHint) : orderHint_{orderHint}, dev_addr_(UINT8_MAX), lun_(0), pMSCNext_{nullptr}
{
    // Initialize the MSC instance
    if (pMSCTop) {
        MSC* pMSCPrev = nullptr;
        for (MSC* pMSC = pMSCTop; pMSC; pMSC = pMSC->pMSCNext_) {
			if (orderHint < pMSC->orderHint_) break;
			pMSCPrev = pMSC;
		}
        if (pMSCPrev) {
            pMSCNext_ = pMSCPrev->pMSCNext_;
            pMSCPrev->pMSCNext_ = this;
        } else {
            pMSCNext_ = pMSCTop;
            pMSCTop = this;
        }
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
		//::printf("MSC command failed with status %d\r\n", cb_data->csw->status);
		return false; // Indicate failure
	}
	*pCompleteFlag = true;
	return true; // Indicate success
}

extern "C" void tuh_msc_mount_cb(uint8_t dev_addr)
{
	//printf("A MassStorage device is mounted\r\n");
	for (MSC* pMSC = MSC::pMSCTop; pMSC; pMSC = pMSC->GetNext()) {
		if (!pMSC->IsMounted()) {
			pMSC->SetDevAddr(dev_addr);
			break;
		}
	}
}

extern "C" void tuh_msc_umount_cb(uint8_t dev_addr)
{
	//printf("A MassStorage device is unmounted\r\n");
	for (MSC* pMSC = MSC::pMSCTop; pMSC; pMSC = pMSC->GetNext()) {
		// If the device is mounted, we set the address to UINT8_MAX to indicate it's unmounted
		if (pMSC->GetDevAddr() == dev_addr) {
			pMSC->SetDevAddr(UINT8_MAX);
			break;
		}
	}
}

}

#endif
