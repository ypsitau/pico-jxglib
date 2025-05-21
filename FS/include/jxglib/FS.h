//==============================================================================
// jxglib/FS.h
//==============================================================================
#ifndef PICO_JXGLIB_FS_H
#define PICO_JXGLIB_FS_H
#include "pico/stdlib.h"
#include "jxglib/Stream.h"

namespace jxglib::FS {

class File;
class FileInfo;
class Dir;
class Drive;

constexpr int MaxLenPathName = 256;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
Drive* FindDrive(const char* pathName);
Drive* GetDriveHead();
Drive* GetDriveCur();
const char* SkipDriveName(const char* pathName);
const char* ExtractDriveName(const char* pathName, char* driveName, int lenMax);
bool SetDriveCur(const char* driveName);
File* OpenFile(const char* fileName, const char* mode);
Dir* OpenDir(const char* dirName);
bool RemoveFile(const char* fileName);
bool RenameFile(const char* fileNameOld, const char* fileNameNew);
bool CreateDir(const char* dirName);
bool RemoveDir(const char* dirName);
bool RenameDir(const char* fileNameOld, const char* fileNameNew);
bool ChangeCurDir(const char* dirName);
bool Format(const char* driveName, Printable& out);
bool IsLegalDriveName(const char* driveName);
const char* JoinPathName(char* pathName, const char* dirName, const char* fileName);

//------------------------------------------------------------------------------
// FS::File
//------------------------------------------------------------------------------
class File : public Referable, public Stream {
public:
	DeclareReferable(File);
protected:
	const Drive& drive_;
public:
	File(const Drive& drive) : drive_(drive) {}
protected:
	virtual ~File() { Close(); }
public:
	const Drive& GetDrive() const { return drive_; }
public:
	virtual void Close() {}
	virtual bool Seek(int position) = 0;
	virtual int Tell() = 0;
	virtual int Size() = 0;
	virtual bool Flush() = 0;
	virtual bool Truncate(int bytes) = 0;
	virtual bool Sync() = 0;
};

//------------------------------------------------------------------------------
// FS::FileInfo
//------------------------------------------------------------------------------
class FileInfo {
protected:
	const Drive* pDrive_;
public:
	FileInfo(const Drive* pDrive = nullptr) : pDrive_(pDrive) {}
public:
	void SetDrive(const Drive* pDrive) { pDrive_ = pDrive; }
public:
	virtual const char* GetName() const = 0;
	virtual uint32_t GetSize() const = 0;
	virtual bool IsDirectory() const = 0;
	virtual bool IsFile() const = 0;
};

//------------------------------------------------------------------------------
// FS::Dir
//------------------------------------------------------------------------------
class Dir : public Referable {
public:
	DeclareReferable(Dir);
protected:
	const Drive& drive_;
public:
	Dir(const Drive& drive) : drive_(drive) {}
protected:
	virtual ~Dir() { Close(); }
public:
	const Drive& GetDrive() const { return drive_; }
public:
	virtual bool Read(FileInfo** ppFileInfo) = 0;
	virtual void Close() {}
};

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
class Drive {
private:
	const char* formatName_;
	const char* driveName_;
	char dirNameCur_[FS::MaxLenPathName];
	Drive* pDriveNext_;
public:
	Drive(const char* formatName, const char* driveName);
public:
	Drive* GetNext() const { return pDriveNext_; }
public:
	const char* GetFormatName() const { return formatName_; }
	const char* GetDriveName() const { return driveName_; }
	void SetDirNameCur(const char* dirName);
	const char* GetDirNameCur() const { return dirNameCur_; }
	const char* RegulatePathName(char* pathNameBuff, int lenBuff, const char* pathName);
public:
	virtual const char* NativePathName(char* pathNameBuff, int lenBuff, const char* pathName) {
		return RegulatePathName(pathNameBuff, lenBuff, pathName);
	}
	virtual File* OpenFile(const char* fileName, const char* mode) = 0;
	virtual Dir* OpenDir(const char* dirName) = 0;
	virtual bool RemoveFile(const char* fileName) = 0;
	virtual bool RenameFile(const char* fileNameOld, const char* fileNameNew) = 0;
	virtual bool CreateDir(const char* dirName) = 0;
	virtual bool RemoveDir(const char* dirName) = 0;
	virtual bool RenameDir(const char* fileNameOld, const char* fileNameNew) = 0;
	virtual bool Format() = 0;
	//virtual bool Sync() = 0;
	//virtual bool GetFreeSpace(uint32_t* bytesFree) = 0;
	//virtual bool GetTotalSpace(uint32_t* bytesTotal) = 0;
	//virtual bool GetUsedSpace(uint32_t* bytesUsed) = 0;
};

#if 0
//------------------------------------------------------------------------------
// FS::DirDrive
//------------------------------------------------------------------------------
class DirDrive : public Dir {
public:
	class FileInfo : public FS::FileInfo {
	public:
		FileInfo() {}
	public:
		virtual const char* GetName() const override { return pDrive_? pDrive_->GetDriveName() : ""; }
		virtual uint32_t GetSize() const override { return 0; }
		virtual bool IsDirectory() const override { return false; }
		virtual bool IsFile() const override { return false; }
	};
private:
	Drive* pDrive_;
	FileInfo fileInfo_;
public:
	DirDrive(Drive* pDrive) : pDrive_(pDrive) {}
public:
	virtual bool Read(FS::FileInfo** ppFileInfo) override;
};
#endif

}

#endif
