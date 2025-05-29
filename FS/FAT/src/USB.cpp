//==============================================================================
// USB.cpp
//==============================================================================
#include "jxglib/FAT/USB.h"

namespace jxglib::FAT {

//-----------------------------------------------------------------------------
// FAT::USB
//-----------------------------------------------------------------------------
USB::USB(const char* driveName, uint8_t orderHint) : Drive(driveName), msc_(orderHint)
{
}	

const char* USB::GetRemarks(char* buff, int lenMax) const
{
	::snprintf(buff, lenMax, "FAT::USB");
	return buff;
}

DSTATUS USB::status()
{
	//::printf("status\n");
	//return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
	return msc_.IsMounted()? 0x00 : STA_NODISK;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS USB::initialize()
{
	//::printf("initialize\n");
	//sdCard_.Initialize();
	return 0x00;
}

DRESULT USB::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	//return sdCard_.ReadBlock(sector, buff, count)? RES_OK : RES_ERROR;
	return msc_.read10(buff, sector, count)? RES_OK : RES_ERROR;
}

DRESULT USB::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	//return sdCard_.WriteBlock(sector, buff, count)? RES_OK : RES_ERROR;
	return msc_.write10(buff, sector, count)? RES_OK : RES_ERROR;
}

DRESULT USB::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	return RES_OK;
}

DRESULT USB::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	//*pSectorCount = sdCard_.GetSectorCount();
	//*pSectorCount = msc_.getCapacity();
	*pSectorCount = msc_.get_block_count();
	return RES_OK;
}

DRESULT USB::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = msc_.get_block_size();
	return RES_OK;
}

DRESULT USB::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE)\n");
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT USB::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

}
