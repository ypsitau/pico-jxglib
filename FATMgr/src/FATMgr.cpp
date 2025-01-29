//==============================================================================
// FATMgr.cpp
//==============================================================================
#include "jxglib/FATMgr.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FATMgr
//------------------------------------------------------------------------------
FATMgr FATMgr::Instance;

}

//------------------------------------------------------------------------------
// Callback Function
//------------------------------------------------------------------------------
DSTATUS disk_initialize(BYTE pdrv)
{
	using namespace jxglib;
	FATMgr::PhysicalDrive* pPhysicalDrive = FATMgr::Instance.GetPhysicalDrive(pdrv);
	if (!pPhysicalDrive) return RES_PARERR;
	return pPhysicalDrive->initialize();
}

DSTATUS disk_status(BYTE pdrv)
{
	using namespace jxglib;
	FATMgr::PhysicalDrive* pPhysicalDrive = FATMgr::Instance.GetPhysicalDrive(pdrv);
	if (!pPhysicalDrive) return RES_PARERR;
	return pPhysicalDrive->status();
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	using namespace jxglib;
	FATMgr::PhysicalDrive* pPhysicalDrive = FATMgr::Instance.GetPhysicalDrive(pdrv);
	if (!pPhysicalDrive) return RES_PARERR;
	return pPhysicalDrive->read(buff, sector, count);
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	using namespace jxglib;
	FATMgr::PhysicalDrive* pPhysicalDrive = FATMgr::Instance.GetPhysicalDrive(pdrv);
	if (!pPhysicalDrive) return RES_PARERR;
	return pPhysicalDrive->write(buff, sector, count);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	using namespace jxglib;
	FATMgr::PhysicalDrive* pPhysicalDrive = FATMgr::Instance.GetPhysicalDrive(pdrv);
	if (!pPhysicalDrive) return RES_PARERR;
	switch (cmd) {
	case CTRL_SYNC: {
		return pPhysicalDrive->ioctl_CTRL_SYNC();
	}
	case GET_SECTOR_COUNT: {
		LBA_t* pSectorCount = reinterpret_cast<LBA_t*>(buff);
		return pPhysicalDrive->ioctl_GET_SECTOR_COUNT(pSectorCount);
	}
	case GET_SECTOR_SIZE: {
		WORD* pSectorSize = reinterpret_cast<WORD*>(buff);
		return pPhysicalDrive->ioctl_GET_SECTOR_SIZE(pSectorSize);
	}
	case GET_BLOCK_SIZE: {
		DWORD* pBlockSize = reinterpret_cast<DWORD*>(buff);
		return pPhysicalDrive->ioctl_GET_BLOCK_SIZE(pBlockSize);
	}
	case CTRL_TRIM: {
		LBA_t* args = reinterpret_cast<LBA_t*>(buff);
		return pPhysicalDrive->ioctl_CTRL_TRIM(args[0], args[1]);
	}
	default:
		break;
	}
	return RES_PARERR;
}

DWORD get_fattime()
{
	DWORD year = 2025;
	DWORD month = 1;
	DWORD dayOfMonth = 1;
	DWORD hour = 0;
	DWORD minute = 0;
	DWORD second = 0;
	return ((year - 1980) << 25) | (month << 21) | (dayOfMonth << 16) |
			(hour << 11) | (minute << 5) | (second >> 1);
}
