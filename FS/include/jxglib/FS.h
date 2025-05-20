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
Drive* GetDriveCur();
const char* SkipDriveName(const char* pathName);
const char* ExtractDriveName(const char* pathName, char* driveName, int lenMax);
bool SetDriveCur(const char* driveName);
Dir* OpenDirDrive();
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
	virtual ~File() { Close(); }
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
	virtual ~Dir() { Close(); }
public:
	virtual bool Read(FileInfo** ppFileInfo) = 0;
	virtual void Close() {}
};

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
class Drive {
private:
	char dirNameCur_[FS::MaxLenPathName];
	Drive* pDriveNext_;
public:
	Drive();
public:
	Drive* GetNext() const { return pDriveNext_; }
public:
	void SetDirNameCur(const char* dirName) { ::strcpy(dirNameCur_, dirName); }
	const char* GetDirNameCur() const { return dirNameCur_; }
	const char* RegulatePathName(char* pathNameBuff, const char* pathName);
public:
	virtual const char* GetDriveName() const = 0;
public:
	virtual File* OpenFile(const char* fileName, const char* mode) = 0;
	virtual Dir* OpenDir(const char* dirName) = 0;
	virtual bool RemoveFile(const char* fileName) = 0;
	virtual bool RenameFile(const char* fileNameOld, const char* fileNameNew) = 0;
	virtual bool CreateDir(const char* dirName) = 0;
	virtual bool RemoveDir(const char* dirName) = 0;
	virtual bool RenameDir(const char* fileNameOld, const char* fileNameNew) = 0;
	virtual bool Format() = 0;
	//virtual bool Sync() = 0;
	//virtual bool GetFreeSpace(uint32_t* pFreeSpace) = 0;
	//virtual bool GetTotalSpace(uint32_t* pTotalSpace) = 0;
	//virtual bool GetUsedSpace(uint32_t* pUsedSpace) = 0;
};

//------------------------------------------------------------------------------
// FS::DirDrive
//------------------------------------------------------------------------------
class DirDrive : public Dir {
public:
	class FileInfo : public FS::FileInfo {
	private:
		const Drive* pDrive_;
	public:
		FileInfo() : pDrive_(nullptr) {}
	public:
		void SetDrive(const Drive* pDrive) { pDrive_ = pDrive; }
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

}

#endif
