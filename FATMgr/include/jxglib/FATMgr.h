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
	public:
		static const int SectorSize = FF_MIN_SS;
	private:
		BYTE pdrv_;
	public:
		PhysicalDrive(BYTE pdrv) : pdrv_{pdrv} {}
	public:
		BYTE GetPDRV() const { return pdrv_; }
	public:
		virtual void Mount() = 0;
	public:
		virtual DSTATUS initialize() = 0;
		virtual DSTATUS status() = 0;
		virtual DRESULT read(BYTE* buff, LBA_t sector, UINT count) = 0;
		virtual DRESULT write(const BYTE* buff, LBA_t sector, UINT count) = 0;
		virtual DRESULT ioctl(BYTE cmd, void* buff) = 0;
	};
	template<int cntLogicalDrive = 1> class PhysicalDriveT : public PhysicalDrive {
	public:
		FATFS fatFsTbl_[cntLogicalDrive];
	public:
		PhysicalDriveT(BYTE pdrv) : PhysicalDrive(pdrv) {}
	public:
		virtual void Mount() override {
			FATMgr::Instance.MountDrive(*this);
			for (int i = 0; i < cntLogicalDrive; i++) {
				//***********************************
				::f_mount(&fatFsTbl_[i], "", i);
			}
		}
	};
private:
	PhysicalDrive* physicalDriveTbl_[16];
public:
	static FATMgr Instance;
public:
	FATMgr() {}
public:
	PhysicalDrive& GetPhysicalDrive(BYTE pdrv) { return *physicalDriveTbl_[pdrv]; }
	void MountDrive(PhysicalDrive& physicalDrive);
};

}

#endif
