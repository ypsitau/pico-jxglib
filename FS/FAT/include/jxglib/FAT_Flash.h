//==============================================================================
// jxglib/FAT_Flash.h
//==============================================================================
#ifndef PICO_JXGLIB_FAT_FLASH_H
#define PICO_JXGLIB_FAT_FLASH_H
#include "jxglib/FAT.h"

namespace jxglib::FAT {

//-----------------------------------------------------------------------------
// FAT::Flash
//-----------------------------------------------------------------------------
class Flash : public Drive {
public:
	static const uint32_t bytesSector = 512;
private:
	uint32_t offsetXIP_;
	uint32_t bytesXIP_; 	// must be multiple of bytesSector
public:
	Flash(uint32_t addrXIP, uint32_t bytesXIP, const char* driveName = "Flash");
public:
	// virtual functions of FAT::Drive
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
