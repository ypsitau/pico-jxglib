//==============================================================================
// USBMSC.cpp
//==============================================================================
#include "jxglib/FAT/USBMSC.h"

namespace jxglib::FAT {

//-----------------------------------------------------------------------------
// FAT::USBMSC
//-----------------------------------------------------------------------------
USBMSC::USBMSC(const char* driveName, uint8_t orderHint) : Drive(driveName), msc_(orderHint)
{
}	

const char* USBMSC::GetRemarks(char* buff, int lenMax) const
{
	::snprintf(buff, lenMax, "FAT::USBMSC");
	return buff;
}

DSTATUS USBMSC::status()
{
	//::printf("status\n");
	//return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
	return msc_.IsMounted()? 0x00 : STA_NODISK;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS USBMSC::initialize()
{
	//::printf("initialize\n");
	return 0x00;
}

DRESULT USBMSC::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	if (!msc_.IsMounted()) return RES_NOTRDY;
	return msc_.read10(buff, sector, count)? RES_OK : RES_ERROR;
}

DRESULT USBMSC::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	if (!msc_.IsMounted()) return RES_NOTRDY;
	return msc_.write10(buff, sector, count)? RES_OK : RES_ERROR;
}

DRESULT USBMSC::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	if (!msc_.IsMounted()) return RES_NOTRDY;
	return RES_OK;
}

DRESULT USBMSC::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = 0;
	if (!msc_.IsMounted()) return RES_NOTRDY;
	*pSectorCount = msc_.get_block_count();
	return RES_OK;
}

DRESULT USBMSC::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = 0;
	if (!msc_.IsMounted()) return RES_NOTRDY;
	*pSectorSize = msc_.get_block_size();
	return RES_OK;
}

DRESULT USBMSC::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE)\n");
	*pBlockSize = 0;
	if (!msc_.IsMounted()) return RES_NOTRDY;
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT USBMSC::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return msc_.IsMounted()? RES_OK : RES_NOTRDY;
}

}
