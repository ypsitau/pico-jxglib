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
	class Disk {
	private:
		BYTE pdrv_;
	public:
		Disk(BYTE pdrv = 0) : pdrv_{pdrv} {}
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
	int nDisks_;
	Disk* diskTbl_[16];
public:
	static FATMgr Instance;
public:
	FATMgr() {}
	Disk& GetDisk(BYTE pdrv) { return *diskTbl_[pdrv]; }
public:
	static void RegisterDisk(Disk& disk) { Instance.RegisterDisk_(disk); }
private:
	void RegisterDisk_(Disk& disk);
};

}

#endif
