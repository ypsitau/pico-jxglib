//==============================================================================
// jxglib/FAT.h
//==============================================================================
#ifndef PICO_JXGLIB_FAT_H
#define PICO_JXGLIB_FAT_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "ff.h"
#include "diskio.h"
#include "jxglib/FS.h"

namespace jxglib::FAT {

//------------------------------------------------------------------------------
// FAT::File
//------------------------------------------------------------------------------
class File : public FS::File {
private:
	FIL fil_;
public:
	File(FS::Drive& drive);
	~File();
public:
	FIL* GetEntity() { return &fil_; }
	const FIL* GetEntity() const { return &fil_; }
public:
	// virtual functions of Stream
	virtual int Read(void* buff, int bytesBuff) override;
	virtual int Write(const void* buff, int bytesBuff) override;
	// virtual functions of FS::File
	virtual void Close() override;
	virtual bool Seek(int position) override;
	virtual int Tell() override;
	virtual int Size() override;
	virtual bool Flush() override;
	virtual bool Truncate(int bytes) override;
	virtual bool Sync() override;
};

//------------------------------------------------------------------------------
// FAT::Dir
//------------------------------------------------------------------------------
class Dir : public FS::Dir {
private:
	DIR dir_;
	BYTE fattribSkip_;
	int nItems_;
public:
	Dir(FS::Drive& drive, uint8_t attrExclude);
	~Dir();
public:
	DIR* GetEntity() { return &dir_; }
	const DIR* GetEntity() const { return &dir_; }
public:
	// virtual functions of FS::FileInfoReader
	virtual FS::FileInfo* Read() override;
	// virtual functions of FS::Dir
	virtual void Close() override;
};

//------------------------------------------------------------------------------
// FAT::Drive
//------------------------------------------------------------------------------
class Drive : public FS::Drive {
public:
	enum class MountMode { Normal, Forced, };
	static const int SectorSize = FF_MIN_SS;
protected:
	BYTE pdrv_;
	Drive* pDriveNext_;
	FATFS fatFs_;
private:
	static Drive* pDriveHead_;
public:
	Drive(const char* driveName);
public:
	// virtual functions of FS::Drive
	virtual bool CheckMounted() override;
	virtual const char* GetFileSystemName() override;
	virtual const char* NativePathName(char* pathNameN, int lenBuff, const char* pathName) override;
	virtual FS::File* OpenFile(const char* fileNameN, const char* mode) override;
	virtual FS::Dir* OpenDir(const char* dirNameN, uint8_t attrExclude) override;
	virtual bool SetTimeStamp(const char* pathNameN, const DateTime& dt) override;
	virtual bool RemoveFile(const char* fileNameN) override;
	virtual bool Rename(const char* fileNameOldN, const char* fileNameNewN) override;
	virtual bool CreateDir(const char* dirNameN) override;
	virtual bool RemoveDir(const char* dirNameN) override;
	virtual bool Format() override;
	virtual bool Mount() override;
	virtual bool Unmount() override;
	virtual FS::FileInfo* GetFileInfo(const char* pathNameN) override;
	virtual uint64_t GetBytesTotal() override;
	virtual uint64_t GetBytesUsed() override;
public:
	static Drive* LookupDrive(BYTE pdrv);
	static const char* FRESULTToStr(FRESULT result);
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

}

#endif
