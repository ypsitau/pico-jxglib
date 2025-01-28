//==============================================================================
// jxglib/FATMgr.h
//==============================================================================
#ifndef PICO_JXGLIB_FATMGR_H
#define PICO_JXGLIB_FATMGR_H
#include "pico/stdlib.h"
#include "ff15a/source/ff.h"
#include "ff15a/source/diskio.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FATMgr
//------------------------------------------------------------------------------
class FATMgr {
public:
	
	class PhysicalDrive {
	private:
		BYTE pdrv_;
	public:
		PhysicalDrive(BYTE pdrv = 0) : pdrv_{pdrv} {}
	public:
		BYTE GetPDRV() const { return pdrv_; }
	public:
		virtual DSTATUS initialize() = 0;
		virtual DSTATUS status() = 0;
		virtual DRESULT read(BYTE* buff, LBA_t sector, UINT count) = 0;
		virtual DRESULT write(const BYTE* buff, LBA_t sector, UINT count) = 0;
		virtual DRESULT ioctl(BYTE cmd, void* buff) = 0;
	};
private:
	int nDis;
	PhysicalDrive* physicalDriveTbl_[16];
public:
	static FATMgr Instance;
public:
	FATMgr() {}
public:
	PhysicalDrive& GetPhysicalDrive(BYTE pdrv) { return *physicalDriveTbl_[pdrv]; }
public:
	static FATMgr& RegisterPhysicalDrive(PhysicalDrive& physicalDrive) {
		return Instance.RegisterPhysicalDrive_(physicalDrive);
	}
private:
	FATMgr& RegisterPhysicalDrive_(PhysicalDrive& physicalDrive);
};

}

#endif
