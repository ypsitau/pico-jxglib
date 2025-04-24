//==============================================================================
// jxglib/FAT.h
//==============================================================================
#ifndef PICO_JXGLIB_FAT_H
#define PICO_JXGLIB_FAT_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "ff15a/source/ff.h"
#include "ff15a/source/diskio.h"
#include "jxglib/FS.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FAT
//------------------------------------------------------------------------------
class FAT : public FS {
public:
	class File : public FS::File {
	private:
		FIL file; // FATFS file object
	public:
		File(FIL f);
		~File();
	public:
		int Read(void* buffer, int bytes) override;
		int Write(const void* buffer, int bytes) override;
		void Close() override;
		bool Seek(int position) override;
		int Tell() override;
		int Size() override;
		bool Remove() override;
		bool Rename(const char* newName) override;
		bool Flush() override;
		bool Truncate(int bytes) override;
		bool Sync() override;
	};
	class Dir : public FS::Dir {
	private:
		DIR dir; // FATFS directory object
	public:
		Dir(DIR d);
		~Dir();

		bool First(FileInfo& info) override;
		bool Next(FileInfo& info) override;
		void Close() override;
		bool Remove() override;
		bool Exists() override;
		bool Rename(const char* newName) override;
		bool Create() override;
	};
	enum class MountMode { Normal, Forced, };
	static const int SectorSize = FF_MIN_SS;
	class PhysicalDrive {
	private:
		BYTE pdrv_;
	public:
		PhysicalDrive(BYTE pdrv) : pdrv_{pdrv} {}
	public:
		BYTE GetPDRV() const { return pdrv_; }
	public:
		virtual void Mount(MountMode mountMode = MountMode::Normal) = 0;
	public:
		virtual DSTATUS status() = 0;
		virtual DSTATUS initialize() = 0;
		virtual DRESULT read(BYTE* buff, LBA_t sector, UINT count) = 0;
		virtual DRESULT write(const BYTE* buff, LBA_t sector, UINT count) = 0;
		virtual DRESULT ioctl_CTRL_SYNC() = 0;
		virtual DRESULT ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount) = 0;
		virtual DRESULT ioctl_GET_SECTOR_SIZE(WORD* pSectorSize) = 0;
		virtual DRESULT ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize) = 0;
		virtual DRESULT ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA) = 0;
	};
	template<int cntLogicalDrive = 1> class PhysicalDriveT : public PhysicalDrive {
	public:
		FATFS fatFsTbl_[cntLogicalDrive];
	public:
		PhysicalDriveT(BYTE pdrv) : PhysicalDrive(pdrv) {}
	public:
		virtual void Mount(MountMode mountMode = MountMode::Normal) override {
			FAT::Instance.RegisterPhysicalDrive(*this);
			for (int i = 0; i < cntLogicalDrive; i++) {
				TCHAR path[16];
				::snprintf(path, sizeof(path), "%d:", FAT::Instance.AssignLogialDrive());
				::f_mount(&fatFsTbl_[i], path, (mountMode == MountMode::Forced)? 1 : 0);
			}
		}
	};
private:
	int numLogicalDrive_;
	PhysicalDrive* physicalDriveTbl_[16];
public:
	static FAT Instance;
public:
	FAT() : numLogicalDrive_{0} {}
public:
	static File* OpenFile(const char* path, const char* mode);
	static Dir* OpenDir(const char* path);
public:
	int AssignLogialDrive() { numLogicalDrive_++; return numLogicalDrive_ - 1; }
	void RegisterPhysicalDrive(PhysicalDrive& physicalDrive) {
		physicalDriveTbl_[physicalDrive.GetPDRV()] = &physicalDrive;
	}
	PhysicalDrive* GetPhysicalDrive(BYTE pdrv) { return physicalDriveTbl_[pdrv]; }
public:
	static const char* FRESULTToStr(FRESULT result);
};

}

#endif
