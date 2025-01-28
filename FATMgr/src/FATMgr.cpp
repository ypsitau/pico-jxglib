//==============================================================================
// FATMgr.cpp
//==============================================================================
#include "jxglib/FATMgr.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FATMgr
//------------------------------------------------------------------------------
FATMgr FATMgr::Instance;

void FATMgr::MountDrive(PhysicalDrive& physicalDrive)
{
	physicalDriveTbl_[physicalDrive.GetPDRV()] = &physicalDrive;
}

}

//------------------------------------------------------------------------------
// Callback Function
//------------------------------------------------------------------------------
DSTATUS disk_initialize(BYTE pdrv)
{
	return jxglib::FATMgr::Instance.GetPhysicalDrive(pdrv).initialize();
}

DSTATUS disk_status(BYTE pdrv)
{
	return jxglib::FATMgr::Instance.GetPhysicalDrive(pdrv).status();
}

DRESULT disk_read(BYTE pdrv, BYTE* buff, LBA_t sector, UINT count)
{
	return jxglib::FATMgr::Instance.GetPhysicalDrive(pdrv).read(buff, sector, count);
}

DRESULT disk_write(BYTE pdrv, const BYTE* buff, LBA_t sector, UINT count)
{
	return jxglib::FATMgr::Instance.GetPhysicalDrive(pdrv).write(buff, sector, count);
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void* buff)
{
	return jxglib::FATMgr::Instance.GetPhysicalDrive(pdrv).ioctl(cmd, buff);
}

DWORD get_fattime()
{
	return 0;
}
