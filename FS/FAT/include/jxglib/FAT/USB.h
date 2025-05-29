//==============================================================================
// jxglib/FAT/USB.h
//==============================================================================
#ifndef PICO_JXGLIB_FAT_USB_H
#define PICO_JXGLIB_FAT_USB_H
#include "jxglib/FAT.h"
#include "jxglib/USBHost/MSC.h"

namespace jxglib::FAT {

//-----------------------------------------------------------------------------
// FAT::USB
//-----------------------------------------------------------------------------
class USB : public Drive {
private:
	jxglib::USBHost::MSC msc_;
public:
	USB(const char* driveName, uint8_t orderHint = UINT8_MAX);
public:
	jxglib::USBHost::MSC& GetMSC() { return msc_; }
public:
	// virtual functions of FS::Drive
	const char* GetRemarks(char* buff, int lenMax) const;
public:
	// virtual functions of FAT
	virtual DSTATUS status() override;
	virtual DSTATUS initialize() override;
	virtual DRESULT read(BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT write(const BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT ioctl_CTRL_SYNC() override;
	virtual DRESULT ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount) override;
	virtual DRESULT ioctl_GET_SECTOR_SIZE(WORD* pSectorSize) override;
	virtual DRESULT ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize) override;
	virtual DRESULT ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA) override;
};

}

#endif
