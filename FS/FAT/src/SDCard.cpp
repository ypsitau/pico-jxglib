//==============================================================================
// SDCard.cpp
//==============================================================================
#include "jxglib/FAT/SDCard.h"

namespace jxglib::FAT {

//-----------------------------------------------------------------------------
// FAT::SDCard
//-----------------------------------------------------------------------------
SDCard::SDCard(const char* driveName, spi_inst_t* spi, uint baudrate, const jxglib::SDCard::PinAssign& pinAssign) :
		Drive(driveName), sdCard_(spi, baudrate, pinAssign)
{
}	

const char* SDCard::GetRemarks(char* buff, int lenMax) const
{
	::snprintf(buff, lenMax, "SDCard");
	return buff;
}

DSTATUS SDCard::status()
{
	//::printf("status\n");
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS SDCard::initialize()
{
	//::printf("initialize\n");
	sdCard_.Initialize();
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DRESULT SDCard::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	return sdCard_.ReadBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT SDCard::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	return sdCard_.WriteBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT SDCard::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	return RES_OK;
}

DRESULT SDCard::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = sdCard_.GetSectorCount();
	return RES_OK;
}

DRESULT SDCard::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = 512;
	return RES_OK;
}

DRESULT SDCard::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE)\n");
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT SDCard::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

}
