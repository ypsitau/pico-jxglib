//==============================================================================
// FAT_SDCard.cpp
//==============================================================================
#include "jxglib/FAT_SDCard.h"

namespace jxglib {

DSTATUS FAT_SDCard::status()
{
	//::printf("status\n");
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS FAT_SDCard::initialize()
{
	//::printf("initialize\n");
	sdCard_.Initialize();
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DRESULT FAT_SDCard::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	return sdCard_.ReadBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT FAT_SDCard::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	return sdCard_.WriteBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT FAT_SDCard::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	return RES_OK;
}

DRESULT FAT_SDCard::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = sdCard_.GetSectorCount();
	return RES_OK;
}

DRESULT FAT_SDCard::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = 512;
	return RES_OK;
}

DRESULT FAT_SDCard::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE)\n");
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT FAT_SDCard::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

}
