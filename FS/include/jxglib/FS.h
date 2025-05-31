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
class Glob;
class Drive;

constexpr int MaxPath = 256;

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
Drive* FindDrive(const char* pathName);
Drive* GetDriveHead();
Drive* GetDriveCur();
const char* SkipDriveName(const char* pathName);
bool SetDriveCur(const char* driveName);
File* OpenFile(const char* fileName, const char* mode, Drive* pDrive = nullptr);
File* OpenFileForCopy(const char* fileNameSrc, const char* fileNameDst);
Dir* OpenDir(const char* dirName);
Glob* OpenGlob(const char* pattern, bool patternAsDirFlag = false);
bool PrintFile(Printable& terr, Printable& tout, const char* fileName);
bool ListFiles(Printable& terr, Printable& tout, const char* dirName);
bool CopyFile(Printable& terr, const char* fileNameSrc, const char* fileNameDst);
bool RemoveFile(const char* fileName);
bool RemoveFile(Printable& terr, const char* fileName);
bool MoveFile(Printable& terr, const char* fileNameOld, const char* fileNameNew);
bool CreateDir(const char* dirName);
bool RemoveDir(const char* dirName);
bool RenameDir(const char* fileNameOld, const char* fileNameNew);
bool ChangeCurDir(const char* dirName);
bool IsDirectory(const char* pathName);
bool Format(Printable& terr, const char* driveName);
bool Mount(Printable& terr, const char* driveName);
bool Unmount(Printable& terr, const char* driveName);
FileInfo* GetFileInfo(const char* pathName);
bool IsLegalDriveName(const char* driveName);
bool GetDirNameCur(const char** pDriveName, const char** pDirName);

const char* ExtractDriveName(const char* pathName, char* driveName, int lenMax);
const char* ExtractFileName(const char* pathName);
void SplitDirName(const char* pathName, char* dirName, int lenMax, const char** pFileName);
const char* AppendPathName(char* pathName, int lenMax, const char* pathNameSub);
const char* JoinPathName(char* pathName, int lenMax, const char* dirName, const char* fileName);
bool DoesContainWildcard(const char* str);
bool DoesMatchWildcard(const char* pattern, const char* str);
bool DoesMatchElemName(const char* elemName1, const char* elemName2);

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
	bool rewindFlag_;
public:
	Dir(const Drive& drive) : drive_(drive), rewindFlag_{false} {}
protected:
	virtual ~Dir() { Close(); }
public:
	const Drive& GetDrive() const { return drive_; }
	void EnableRewind() { rewindFlag_ = true; }
public:
	virtual bool Read(FileInfo** ppFileInfo) = 0;
	virtual void Close() {}
};

//------------------------------------------------------------------------------
// FS::Glob
//------------------------------------------------------------------------------
class Glob : public Referable {
public:
	DeclareReferable(Glob);
protected:
	RefPtr<Dir> pDir_;
	char dirName_[MaxPath];
	const char* pattern_;
	char pathName_[MaxPath];
public:
	Glob();
protected:
	~Glob() { Close(); }
public:
	bool Open(const char* pattern, bool paternAsDirFlag = false);
	bool Read(FileInfo** ppFileInfo, const char** pPathName = nullptr);
	void Close();
};

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
class Drive {
private:
	const char* formatName_;
	const char* driveName_;
	const char* driveNameRaw_;
	char dirNameCur_[MaxPath];
	Drive* pDriveNext_;
public:
	Drive(const char* formatName, const char* driveName);
public:
	Drive* GetNext() const { return pDriveNext_; }
public:
	bool IsPrimary() const { return *driveNameRaw_ == '*'; }
	bool IsDirectory(const char* pathName);
	const char* GetDriveName() const { return driveName_; }
	void SetDirNameCur(const char* dirName);
	const char* GetDirNameCur() const { return dirNameCur_; }
	const char* RegulatePathName(char* pathNameBuff, int lenBuff, const char* pathName);
public:
	virtual const char* GetFileSystemName() = 0;
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
	virtual bool Mount() = 0;
	virtual bool Unmount() = 0;
	virtual FileInfo* GetFileInfo(const char* pathName) = 0;
	virtual uint64_t GetBytesTotal() = 0;
	virtual uint64_t GetBytesUsed() = 0;
	virtual const char* GetRemarks(char* buff, int lenMax) const { return buff;}
};

}

#endif
