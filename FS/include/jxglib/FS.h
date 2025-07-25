//==============================================================================
// jxglib/FS.h
//==============================================================================
#ifndef PICO_JXGLIB_FS_H
#define PICO_JXGLIB_FS_H
#include <memory>
#include "pico/stdlib.h"
#include "jxglib/Common.h"
#include "jxglib/RTC.h"

namespace jxglib::FS {

class File;
class FileInfo;
class Dir;
class Glob;
class Drive;

constexpr int MaxDriveName = 16;
constexpr int MaxPath = 256;

//------------------------------------------------------------------------------
// FS::File
//------------------------------------------------------------------------------
class File : public Stream {
public:
	enum class SeekStart {
		Begin,	// Seek from the beginning of the file
		Current, // Seek from the current position in the file
		End,	// Seek from the end of the file
	};
protected:
	const Drive& drive_;
public:
	File(const Drive& drive);
	virtual ~File() { Close(); }
public:
	const Drive& GetDrive() const { return drive_; }
public:
	void Close(File* pFile) { pFile->Close(); delete pFile; }
	void Close(std::unique_ptr<File>& pFile) { pFile->Close(); pFile.reset(); }
public:
	virtual void Close() {}
	virtual bool Seek(int position, SeekStart seekStart = SeekStart::Begin) = 0;
	virtual int Tell() = 0;
	virtual int Size() = 0;
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
	std::unique_ptr<FileInfo> pFileInfoChild_;
public:
	static Cmp_Combine CmpDefault;
public:
	FileInfo();
	FileInfo(const char* name, uint8_t attr, uint32_t size, const DateTime& dateTime);
public:
	void PrintList(Printable& tout, bool slashForDirFlag = true) const;
	void PrintTree(Printable& tout, bool dirOnlyFlag, bool slashForDirFlag, char* strBranch, int lenBranchMax) const;
	void SetNext(FileInfo* pFileInfoNext) { pFileInfoNext_.reset(pFileInfoNext); }
	FileInfo* GetNext() { return pFileInfoNext_.get(); }
	const FileInfo* GetNext() const { return pFileInfoNext_.get(); }
	FileInfo* ReleaseNext() { return pFileInfoNext_.release(); }
	void SetChild(FileInfo* pFileInfoChild) { pFileInfoChild_.reset(pFileInfoChild); }
	FileInfo* GetChild() { return pFileInfoChild_.get(); }
	const FileInfo* GetChild() const { return pFileInfoChild_.get(); }
	FileInfo* ReleaseChild() { return pFileInfoChild_.release(); }
public:
	const char* GetName() const { return name_.get(); }
	uint8_t GetAttr() const { return attr_; }
	uint32_t GetSize() const { return size_; }
	const DateTime& GetDateTime() const { return dateTime_; }
	const char* JoinPathName(char* buff, int lenBuff) const;
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
	FileInfo* ReadAll(const FileInfo::Cmp& cmp = FileInfo::Cmp::Zero, const char* pattern = nullptr);
public:
	virtual FileInfo* Read() = 0;
};

//------------------------------------------------------------------------------
// FS::Dir
//------------------------------------------------------------------------------
class Dir : public FileInfoReader {
protected:
	Drive& drive_;
	bool rewindFlag_;
protected:
	// folllwing members are used by FS::Walker
	char dirName_[MaxPath];
	std::unique_ptr<Dir> pDirChild_;
	std::unique_ptr<FileInfo> pFileInfo_;
public:
	Dir(Drive& drive, const char* dirName);
	virtual ~Dir() { Close(); }
public:
	void Close(Dir* pDir) { pDir->Close(); delete pDir; }
	void Close(std::unique_ptr<Dir>& pDir) { pDir->Close(); pDir.reset(); }
public:
	void SetChild(Dir* pDir) { pDirChild_.reset(pDir); }
	Dir* GetChild() const { return pDirChild_.get(); }
	Dir* RemoveBottomChild();
public:
	FileInfo* ReadAllRecursive(const FileInfo::Cmp& cmp = FileInfo::Cmp::Zero, const char* pattern = nullptr, bool* pSuccessFlag = nullptr);
public:
	Drive& GetDrive() const { return drive_; }
	void EnableRewind() { rewindFlag_ = true; }
public:
	const char* GetDirName() const { return dirName_; }
	void SetFileInfo(FileInfo* pFileInfo) { pFileInfo_.reset(pFileInfo); }
	const FileInfo& GetFileInfo() const { return *pFileInfo_; }
	FileInfo* ReleaseFileInfo() { return pFileInfo_.release(); }
public:
	virtual void Close() {}
};

//------------------------------------------------------------------------------
// FS::Glob
//------------------------------------------------------------------------------
class Glob : public FileInfoReader {
protected:
	std::unique_ptr<FileInfo> pFileInfoTop_;
	char dirName_[MaxPath];	// directory name and pattern are stored
	const char* pattern_;
	char pathName_[MaxPath];
public:
	Glob();
public:
	bool Open(const char* pattern, const FileInfo::Cmp& cmp, bool paternAsDirFlag = false, uint8_t attrExclude = 0);
	FileInfo* Read(const char** pPathName);
public:
	// virtual functions of FileInfoReader
	virtual FileInfo* Read() override { return Read(nullptr); }
};

//------------------------------------------------------------------------------
// FS::Walker
//------------------------------------------------------------------------------
class Walker {
protected:
	bool fileFirstFlag_;
	std::unique_ptr<Dir> pDirTop_;
	uint8_t attrExclude_; // attributes to exclude
	Dir* pDirCur_;
	char pathName_[MaxPath];
	const char* pathNameSub_;
public:
	Walker(bool removeModeFlag = false);
	~Walker() { Close(); }
public:
	bool Open(const char* dirName, uint8_t attrExclude = 0);
	void Close() {}
	FileInfo* Read(const char** pPathName);
	const char* GetPathNameSub() const { return pathNameSub_; }
};

//------------------------------------------------------------------------------
// FS::Drive
//------------------------------------------------------------------------------
class Drive {
public:
	class EventHandler {
	public:
		virtual void OnContentChanged() = 0;
	};
protected:
	bool mountedFlag_;
	const char* formatName_;
	char driveNameRaw_[MaxDriveName + 1];	// Raw drive name, e.g. "C:", "*C:"
	const char* driveName_;	 				// Drive name that eliminates the leading '*'
	char dirNameCur_[MaxPath];
	Drive* pDriveNext_;
	EventHandler* pEventHandler_;
public:
	Drive(const char* formatName, const char* driveName);
public:
	Drive* GetNext() const { return pDriveNext_; }
public:
	bool IsPrimary() const { return driveNameRaw_[0] == '*'; }
	bool DoesExist(const char* pathNameN);
	bool IsDirectory(const char* pathNameN);
	const char* GetDriveName() const { return driveName_; }
	void SetDirNameCur(const char* dirName);
	const char* GetDirNameCur() const { return dirNameCur_; }
	const char* RegulatePathName(char* pathNameBuff, int lenBuff, const char* pathName);
public:
	void SetEventHandler(EventHandler* pEventHandler) { pEventHandler_ = pEventHandler; }
	void NotifyContentChanged() { if (pEventHandler_) pEventHandler_->OnContentChanged(); }
public:
	virtual bool CheckMounted() = 0;
	virtual const char* GetFileSystemName() = 0;
	virtual const char* NativePathName(char* pathNameN, int lenBuff, const char* pathName) {
		return RegulatePathName(pathNameN, lenBuff, pathName);
	}
	virtual File* OpenFile(const char* fileNameN, const char* mode) = 0;
	virtual Dir* OpenDir(const char* dirNameN, const char* dirName, uint8_t attrExclude) = 0;
	virtual bool SetTimeStamp(const char* pathNameN, const DateTime& dt) = 0;
	virtual bool RemoveFile(const char* fileNameN) = 0;
	virtual bool Rename(const char* fileNameOldN, const char* fileNameNewN) = 0;
	virtual bool CreateDir(const char* dirNameN) = 0;
	virtual bool RemoveDir(const char* dirNameN) = 0;
	virtual bool Format() = 0;
	virtual bool Mount() = 0;
	virtual bool Unmount() = 0;
	virtual FileInfo* GetFileInfo(const char* pathNameN) = 0;
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
//File* OpenFile(const char* fileName, const char* mode, Drive* pDrive = nullptr);
File* OpenFile(const char* fileName, const char* mode);
Dir* OpenDir(const char* dirName, uint8_t attrExclude = 0);
Glob* OpenGlob(const char* pattern, const FileInfo::Cmp& cmp = FS::FileInfo::CmpDefault, bool patternAsDirFlag = false, uint8_t attrExclude = 0);
bool Touch(Printable& terr, const char* pathName, const DateTime& dt);
bool PrintFile(Printable& terr, Printable& tout, const char* fileName);

bool ListDrives(Printable& tout, const char* driveName = nullptr, bool remarksFlag = true);

bool ListFiles(Printable& terr, Printable& tout, const char* pathName,
	const FileInfo::Cmp& cmp = FileInfo::Cmp::Zero, uint8_t attrExclude = 0, bool slashForDirFlag = true);

bool Copy(Printable& terr, const char* pathNameSrc, const char* pathNameDst, bool recursiveFlag, bool verboseFlag, bool forceFlag);
inline bool Copy(const char* pathNameSrc, const char* pathNameDst, bool recursiveFlag, bool verboseFlag, bool forceFlag) {
	return Copy(PrintableDumb::Instance, pathNameSrc, pathNameDst, recursiveFlag, verboseFlag, forceFlag);
}

bool CopyFile(Printable& terr, const char* fileNameSrc, const char* pathNameDst, bool forceFlag);
inline bool CopyFile(const char* fileNameSrc, const char* pathNameDst, bool forceFlag) {
	return CopyFile(PrintableDumb::Instance, fileNameSrc, pathNameDst, forceFlag);
}

bool Remove(Printable& terr, const char* pathName, bool recursiveFlag, bool verboseFlag);
inline bool Remove(const char* pathName, bool recursiveFlag, bool verboseFlag) {
	return Remove(PrintableDumb::Instance, pathName, recursiveFlag, verboseFlag);
}

bool RemoveFile(Printable& terr, const char* fileName);
inline bool RemoveFile(const char* fileName) { return RemoveFile(PrintableDumb::Instance, fileName); }

bool RemoveDir(Printable& terr, const char* dirName);
inline bool RemoveDir(const char* dirName) { return RemoveDir(PrintableDumb::Instance, dirName); }

bool Move(Printable& terr, const char* pathNameOld, const char* pathNameNew, bool verboseFlag, bool forceFlag);
inline bool Move(const char* pathNameOld, const char* pathNameNew, bool verboseFlag, bool forceFlag) {
	return Move(PrintableDumb::Instance, pathNameOld, pathNameNew, verboseFlag, forceFlag);
}

bool CreateDir(Printable& terr, const char* dirName);
inline bool CreateDir(const char* dirName) { return CreateDir(PrintableDumb::Instance, dirName); }

bool ChangeCurDir(Printable& terr, const char* dirName);
inline bool ChangeCurDir(const char* dirName) { return ChangeCurDir(PrintableDumb::Instance, dirName); }

bool Format(Printable& terr, const char* driveName);
inline bool Format(const char* driveName) { return Format(PrintableDumb::Instance, driveName); }

bool Mount(Printable& terr, const char* driveName);
inline bool Mount(const char* driveName) { return Mount(PrintableDumb::Instance, driveName); }

bool Unmount(Printable& terr, const char* driveName);
inline bool Unmount(const char* driveName) { return Unmount(PrintableDumb::Instance, driveName); }

bool DoesExist(const char* pathName);
bool IsDirectory(const char* pathName);
bool CheckMounted(const char* driveName);
FileInfo* GetFileInfo(const char* pathName);
bool IsLegalDriveName(const char* driveName);
bool GetDirNameCur(const char** pDriveName, const char** pDirName);

const char* ExtractDriveName(const char* pathName, char* driveName, int lenMax);
const char* ExtractFileName(const char* pathName);
const char* ExtractBottomName(const char* pathName);
void SplitDirName(const char* pathName, char* dirName, int lenMax, const char** pFileName);
const char* AppendPathName(char* pathName, int lenMax, const char* pathNameSub);
const char* JoinPathName(char* pathName, int lenMax, const char* dirName, const char* fileName);
bool DoesContainWildcard(const char* str);
bool DoesMatchWildcard(const char* pattern, const char* str);
bool DoesMatchElemName(const char* elemName1, const char* elemName2);
bool DoesMatchDriveName(const char* driveName1, const char* driveName2);

}

#endif
