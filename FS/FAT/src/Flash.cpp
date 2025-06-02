//==============================================================================
// Flash.cpp
//==============================================================================
#include "jxglib/Flash.h"
#include "jxglib/FAT/Flash.h"

namespace jxglib::FAT {

//-----------------------------------------------------------------------------
// FAT::Flash
//-----------------------------------------------------------------------------
Flash::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP) :
	Drive(driveName), offsetXIP_{addrXIP & 0x0fff'ffff}, bytesXIP_{bytesXIP}
{
	if (bytesXIP % FLASH_SECTOR_SIZE != 0) {
		::panic("bytesXIP must be multiple of %d", FLASH_SECTOR_SIZE);
	}
}

Flash::Flash(const char* driveName, uint32_t bytesXIP) :
	Flash(driveName, XIP_BASE + PICO_FLASH_SIZE_BYTES - bytesXIP, bytesXIP)
{
}

const char* Flash::GetRemarks(char* buff, int lenMax) const
{
	::snprintf(buff, lenMax, "FAT::Flash 0x%08x-0x%08x %dkB",
		XIP_BASE + offsetXIP_, XIP_BASE + offsetXIP_ + bytesXIP_, bytesXIP_ / 1024);
	return buff;
}

DSTATUS Flash::status()
{
	//::printf("status\n");
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS Flash::initialize()
{
	//::printf("initialize\n");
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DRESULT Flash::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	jxglib::Flash::Read(offsetXIP_ + sector * bytesSector, buff, count * bytesSector);
	return RES_OK;
}

DRESULT Flash::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	jxglib::Flash::Write(offsetXIP_ + sector * bytesSector, buff, count * bytesSector);
	return RES_OK;
}

DRESULT Flash::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	jxglib::Flash::Sync();
	return RES_OK;
}

DRESULT Flash::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = bytesXIP_ / bytesSector;
	return RES_OK;
}

DRESULT Flash::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = bytesSector;
	return RES_OK;
}

DRESULT Flash::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE) %d\n", FLASH_SECTOR_SIZE / bytesSector);
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT Flash::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

}
