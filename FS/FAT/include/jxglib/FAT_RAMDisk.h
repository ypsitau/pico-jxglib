//==============================================================================
// jxglib/FAT_RAMDisk.h
//==============================================================================
#ifndef PICO_JXGLIB_FAT_RAMDISK_H
#define PICO_JXGLIB_FAT_RAMDISK_H
#include "jxglib/FAT.h"

namespace jxglib {

//-----------------------------------------------------------------------------
// FAT_RAMDisk
//-----------------------------------------------------------------------------
class FAT_RAMDisk : public FAT::PhysicalDriveT<> {
public:
	static const uint32_t bytesSector = 512;
private:
	uint8_t* buffDisk_;
	uint32_t bytesDisk_; 	// must be multiple of bytesSector
public:
	FAT_RAMDisk(uint32_t bytesDisk, BYTE pdrv = 0);
	~FAT_RAMDisk();
public:
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
