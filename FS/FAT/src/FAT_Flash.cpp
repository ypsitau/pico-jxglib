//==============================================================================
// FAT_Flash.cpp
//==============================================================================
#include "jxglib/FAT_Flash.h"

namespace jxglib {

//-----------------------------------------------------------------------------
// FAT_Flash
//-----------------------------------------------------------------------------
FAT_Flash::FAT_Flash(uint32_t offsetXIP, uint32_t bytesXIP, const char* driveName) :
	FAT(driveName), offsetXIP_{offsetXIP & 0x0fff'ffff}, bytesXIP_{bytesXIP}
{
}

DSTATUS FAT_Flash::status()
{
	//::printf("status\n");
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS FAT_Flash::initialize()
{
	//::printf("initialize\n");
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DRESULT FAT_Flash::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	Flash::Read(offsetXIP_ + sector * bytesSector, buff, count * bytesSector);
	return RES_OK;
}

DRESULT FAT_Flash::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	Flash::Write(offsetXIP_ + sector * bytesSector, buff, count * bytesSector);
	return RES_OK;
}

DRESULT FAT_Flash::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	Flash::Sync();
	return RES_OK;
}

DRESULT FAT_Flash::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = bytesXIP_ / bytesSector;
	return RES_OK;
}

DRESULT FAT_Flash::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = bytesSector;
	return RES_OK;
}

DRESULT FAT_Flash::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE) %d\n", FLASH_SECTOR_SIZE / bytesSector);
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT FAT_Flash::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

}
