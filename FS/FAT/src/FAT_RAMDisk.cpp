//==============================================================================
// FAT_RAMDisk.cpp
//==============================================================================
#include <malloc.h>
#include "jxglib/FAT_RAMDisk.h"

namespace jxglib {

//-----------------------------------------------------------------------------
// FAT_RAMDisk
//-----------------------------------------------------------------------------
FAT_RAMDisk::FAT_RAMDisk(uint32_t bytesDisk, BYTE pdrv) :
	FAT::PhysicalDriveT<>{pdrv}, buffDisk_{nullptr}, bytesDisk_{bytesDisk}
{
}

FAT_RAMDisk::~FAT_RAMDisk()
{
	::free(buffDisk_);
}

DSTATUS FAT_RAMDisk::status()
{
	//::printf("status\n");
	return buffDisk_? 0x00 : STA_NOINIT;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS FAT_RAMDisk::initialize()
{
	//::printf("initialize\n");
	if (!buffDisk_) {
		buffDisk_ = reinterpret_cast<uint8_t*>(::malloc(bytesDisk_));
	}
	return buffDisk_? 0x00 : STA_NOINIT;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DRESULT FAT_RAMDisk::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	::memcpy(buff, buffDisk_ + sector * bytesSector, count * bytesSector);
	return RES_OK;
}

DRESULT FAT_RAMDisk::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	::memcpy(buffDisk_ + sector * bytesSector, buff, count * bytesSector);
	return RES_OK;
}

DRESULT FAT_RAMDisk::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	return RES_OK;
}

DRESULT FAT_RAMDisk::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = bytesDisk_ / bytesSector;
	return RES_OK;
}

DRESULT FAT_RAMDisk::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = bytesSector;
	return RES_OK;
}

DRESULT FAT_RAMDisk::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE)\n");
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT FAT_RAMDisk::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

}
