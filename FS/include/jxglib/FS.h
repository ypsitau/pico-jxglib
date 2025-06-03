//==============================================================================
// jxglib/FS.h
//==============================================================================
#ifndef PICO_JXGLIB_FS_H
#define PICO_JXGLIB_FS_H
#include <memory>
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
// FS::File
//------------------------------------------------------------------------------
class File : public Stream {
protected:
	const Drive& drive_;
public:
	File(const Drive& drive);
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
public:
	struct Attr {
		static const uint8_t Directory	= (1u << 0);
		static const uint8_t Archive	= (1u << 1);
		static const uint8_t ReadOnly	= (1u << 2);
		static const uint8_t Hidden		= (1u << 3);
		static const uint8_t System		= (1u << 4);
		static const uint8_t Link		= (1u << 5);
	};
	class Cmp {
	private:
		int multiplier_;
	public:
		static const Cmp Zero;
	public:
		Cmp(int multiplier) : multiplier_(multiplier) {}
	public:
		int Compare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const;
	public:
		virtual int DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const;
	};
	class Cmp_Type : public Cmp {
	public:
		static const Cmp_Type Ascent;
		static const Cmp_Type Descent;
	public:
		Cmp_Type(int multiplier) : Cmp(multiplier) {}
	public:
		virtual int DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const override;
	};
	class Cmp_Name : public Cmp {
	public:
		static const Cmp_Name Ascent;
		static const Cmp_Name Descent;
	public:
		Cmp_Name(int multiplier) : Cmp(multiplier) {}
	public:
		virtual int DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const override;
	};
	class Cmp_Size : public Cmp {
	public:
		static const Cmp_Size Ascent;
		static const Cmp_Size Descent;
	public:
		Cmp_Size(int multiplier) : Cmp(multiplier) {}
	public:
		virtual int DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const override;
	};
	class Cmp_DateTime : public Cmp {
	public:
		static const Cmp_DateTime Ascent;
		static const Cmp_DateTime Descent;
	public:
		Cmp_DateTime(int multiplier) : Cmp(multiplier) {}
	public:
		virtual int DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const override;
	};
	class Cmp_Combine : public Cmp {
	private:
		const Cmp* pCmp1_;
		const Cmp* pCmp2_;
		const Cmp* pCmp3_;
	public:
		Cmp_Combine(const Cmp& cmp1, const Cmp& cmp2, const Cmp& cmp3 = Cmp::Zero) : Cmp(1), pCmp1_{&cmp1}, pCmp2_{&cmp2}, pCmp3_{&cmp3} {}
	public:
		void Set(const Cmp* pCmp1, const Cmp* pCmp2, const Cmp* pCmp3) { pCmp1_ = pCmp1; pCmp2_ = pCmp2; pCmp3_ = pCmp3; }
	public:
		virtual int DoCompare(const FileInfo& fileInfo1, const FileInfo& fileInfo2) const override;
	};
protected:
	std::unique_ptr<char[]> name_; // Use unique_ptr for automatic memory management
	uint8_t attr_;
	uint32_t size_;
	DateTime dateTime_;
	std::unique_ptr<FileInfo> pFileInfoNext_;
public:
	static Cmp_Combine CmpDefault;
public:
	FileInfo();
	FileInfo(const char* name, uint8_t attr, uint32_t size, const DateTime& dateTime);
public:
	void PrintList(Printable& tout) const;
	void SetNext(FileInfo* pFileInfoNext) { pFileInfoNext_.reset(pFileInfoNext); }
	FileInfo* GetNext() const { return pFileInfoNext_.get(); }
	FileInfo* ReleaseNext() { return pFileInfoNext_.release(); }
public:
	const char* GetName() const { return name_.get(); }
	uint8_t GetAttr() const { return attr_; }
	uint32_t GetSize() const { return size_; }
	const DateTime& GetDateTime() const { return dateTime_; }
	const char* MakeAttrString(char* buff, int lenBuff) const;
	const char* MakeDateTimeString(char* buff, int lenBuff) const;
	bool IsDirectory() const { return !!(attr_ & Attr::Directory); }
	bool IsFile() const { return !(attr_ & Attr::Directory); }
	bool IsArchive() const { return !!(attr_ & Attr::Archive); }
	bool IsReadOnly() const { return !!(attr_ & Attr::ReadOnly); }
	bool IsHidden() const { return !!(attr_ & Attr::Hidden); }
	bool IsSystem() const { return !!(attr_ & Attr::System); }
	bool IsLink() const { return !!(attr_ & Attr::Link); }
};

//------------------------------------------------------------------------------
// FS::FileInfoReader
//------------------------------------------------------------------------------
class FileInfoReader {
public:
	FileInfo* ReadAll(const FileInfo::Cmp& cmp = FileInfo::Cmp::Zero);
public:
	virtual FileInfo* Read() = 0;
};

//------------------------------------------------------------------------------
// FS::Dir
//------------------------------------------------------------------------------
class Dir : public FileInfoReader {
protected:
	const Drive& drive_;
	bool rewindFlag_;
public:
	Dir(const Drive& drive);
	virtual ~Dir() { Close(); }
public:
	const Drive& GetDrive() const { return drive_; }
	void EnableRewind() { rewindFlag_ = true; }
public:
	virtual void Close() {}
};

//------------------------------------------------------------------------------
// FS::Glob
//------------------------------------------------------------------------------
class Glob : public FileInfoReader {
protected:
	std::unique_ptr<Dir> pDir_;
	char dirName_[MaxPath];
	const char* pattern_;
	char pathName_[MaxPath];
public:
	Glob();
	~Glob() { Close(); }
public:
	bool Open(const char* pattern, bool paternAsDirFlag = false, uint8_t attrExclude = 0);
	FileInfo* Read(const char** pPathName);
	void Close();
public:
	// virtual functions of FileInfoReader
	virtual FileInfo* Read() override { return Read(nullptr); }
};

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
class Drive {
protected:
	bool mountedFlag_;
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
	bool IsMounted() const { return mountedFlag_; }
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
	virtual Dir* OpenDir(const char* dirName, uint8_t attrExclude) = 0;
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
	virtual bool GetFlashInfo(uint32_t* pAddr, uint32_t* pBytes) { return false; }
};

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
Dir* OpenDir(const char* dirName, uint8_t attrExclude = 0);
Glob* OpenGlob(const char* pattern, bool patternAsDirFlag = false, uint8_t attrExclude = 0);
bool PrintFile(Printable& terr, Printable& tout, const char* fileName);
bool ListFiles(Printable& terr, Printable& tout, const char* pathName,
	const FileInfo::Cmp& cmp = FileInfo::Cmp::Zero, uint8_t attrExclude = 0);
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
bool DoesMatchDriveName(const char* driveName1, const char* driveName2);

}

#endif
