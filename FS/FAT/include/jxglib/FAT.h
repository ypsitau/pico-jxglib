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

namespace jxglib {

//------------------------------------------------------------------------------
// FAT
//------------------------------------------------------------------------------
class FAT : public FS::Manager {
public:
	class File : public FS::File {
	public:
		DeclareReferable(File);
	private:
		FIL fil_;
	public:
		File();
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
	class FileInfo : public FS::FileInfo {
	private:
		FILINFO filInfo_;
	public:
		FileInfo() {}
		~FileInfo() {}
	public:
		FILINFO& GetEntity() { return filInfo_; }
		const FILINFO& GetEntity() const { return filInfo_; }
	public:
		virtual const char* GetName() const { return filInfo_.fname; }
		virtual uint32_t GetSize() const { return filInfo_.fsize; }
		virtual bool IsDirectory() const { return (filInfo_.fattrib & AM_DIR) != 0; }
		virtual bool IsFile() const { return (filInfo_.fattrib & AM_DIR) == 0; }
		//virtual bool IsHidden() const { return (filInfo_.fattrib & AM_HID) != 0; }
		//virtual bool IsReadOnly() const { return (filInfo_.fattrib & AM_RDO) != 0; }
		//virtual bool IsSystem() const { return (filInfo_.fattrib & AM_SYS) != 0; }
		//virtual bool IsArchive() const { return (filInfo_.fattrib & AM_ARC) != 0; }
		//virtual bool IsNormal() const { return (filInfo_.fattrib & AM_DIR) == 0 && (filInfo_.fattrib & AM_HID) == 0; }
	};
	class Dir : public FS::Dir {
	private:
		DIR dir_;
		FileInfo fileInfo_;
	public:
		Dir();
		~Dir();
	public:
		DIR* GetEntity() { return &dir_; }
		const DIR* GetEntity() const { return &dir_; }
	public:
		bool Read(FS::FileInfo** ppFileInfo) override;
		void Close() override;
	};
	enum class MountMode { Normal, Forced, };
	static const int SectorSize = FF_MIN_SS;
private:
	const char* driveName_;
	FATFS fatFs_;
	FAT* pFATNext_;
private:
	static FAT* pFATHead_;
public:
	FAT(const char* driveName);
public:
	void Mount(MountMode mountMode = MountMode::Normal);
public:
	// virtual functions of FS::Manager
	virtual const char* GetDriveName() const override { return driveName_; }
	virtual FS::File* OpenFile(const char* fileName, const char* mode) override;
	virtual FS::Dir* OpenDir(const char* dirName) override;
	virtual bool RemoveFile(const char* fileName) override;
	virtual bool RenameFile(const char* fileNameOld, const char* fileNameNew) override;
	virtual bool CreateDir(const char* dirName) override;
	virtual bool RemoveDir(const char* dirName) override;
	virtual bool RenameDir(const char* fileNameOld, const char* fileNameNew) override;
	virtual bool Format() override;
public:
	static FAT* GetFAT(BYTE pdrv);
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
