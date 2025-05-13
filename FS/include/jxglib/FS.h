//==============================================================================
// jxglib/FS.h
//==============================================================================
#ifndef PICO_JXGLIB_FS_H
#define PICO_JXGLIB_FS_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FS
//------------------------------------------------------------------------------
class FS {
public:
	class File : public Referable {
	public:
		DeclareReferable(File);
	protected:
		virtual ~File() { Close(); }
	public:
		virtual int Read(void* buffer, int bytes) = 0;
		virtual int Write(const void* buffer, int bytes) = 0;
		virtual void Close() {}
		virtual bool Seek(int position) = 0;
		virtual int Tell() = 0;
		virtual int Size() = 0;
		virtual bool Flush() = 0;
		virtual bool Truncate(int bytes) = 0;
		virtual bool Sync() = 0;
	};
	class FileInfo {
	public:
		virtual const char* GetName() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual bool IsDirectory() const = 0;
		virtual bool IsFile() const = 0;
	};
	class Dir : public Referable {
	public:
		DeclareReferable(Dir);
	protected:
		virtual ~Dir() { Close(); }
	public:
		virtual bool Read(FileInfo** ppFileInfo) = 0;
		virtual void Close() {}
	};
	class Manager {
	private:
		Manager* pManagerNext_;
	public:
		Manager();
	public:
		Manager* GetNext() const { return pManagerNext_; }
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
public:
	static Manager* pManagerTop;
public:
	static const int MaxLenDriveName = 16;
	static const int MaxLenPathName = 256;
private:
	char driveNameCur_[MaxLenDriveName + 1];
	char dirNameCur_[MaxLenPathName + 1];
public:
	FS();
public:
	static Manager* FindManager(const char* pathName);
	static const char* SkipDriveName(const char* pathName);
	static const char* ExtractDriveName(const char* pathName, char* driveName, int lenMax);
public:
	static File* OpenFile(const char* fileName, const char* mode);
	static Dir* OpenDir(const char* dirName);
	static bool RemoveFile(const char* fileName);
	static bool RenameFile(const char* fileNameOld, const char* fileNameNew);
	static bool CreateDir(const char* dirName);
	static bool RemoveDir(const char* dirName);
	static bool RenameDir(const char* fileNameOld, const char* fileNameNew);
	static bool Format(const char* driveName);
	static bool IsLegalDriveName(const char* driveName);
	static const char* JoinPathName(char* pathName, const char* dirName, const char* fileName);
};

}

#endif
