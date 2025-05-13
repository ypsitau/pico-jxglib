//==============================================================================
// jxglib/FS.h
//==============================================================================
#ifndef PICO_JXGLIB_FS_H
#define PICO_JXGLIB_FS_H
#include "pico/stdlib.h"
#include "jxglib/Stream.h"

namespace jxglib {

//------------------------------------------------------------------------------
// FS
//------------------------------------------------------------------------------
class FS {
public:
	static const int MaxLenPathName = 256;
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
		char dirNameCur_[FS::MaxLenPathName];
		Manager* pManagerNext_;
	public:
		Manager();
	public:
		Manager* GetNext() const { return pManagerNext_; }
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
	class DirDrive : public Dir {
	public:
		class FileInfo : public FS::FileInfo {
		private:
			const Manager* pManager_;
		public:
			FileInfo() : pManager_(nullptr) {}
		public:
			void SetManager(const Manager* pManager) { pManager_ = pManager; }
		public:
			virtual const char* GetName() const override { return pManager_? pManager_->GetDriveName() : ""; }
			virtual uint32_t GetSize() const override { return 0; }
			virtual bool IsDirectory() const override { return false; }
			virtual bool IsFile() const override { return false; }
		};
	private:
		Manager* pManager_;
		FileInfo fileInfo_;
	public:
		DirDrive(Manager* pManager) : pManager_(pManager) {}
	public:
		virtual bool Read(FS::FileInfo** ppFileInfo) override;
	};
	class Stream : public jxglib::Stream {
	private:
		RefPtr<File> pFile_;
	public:
		Stream() {}
	public:
		void SetFile(File* pFile) { pFile_ = pFile; }
	public:
		// virtual functions of jxglib::Stream
		virtual bool Read(void* buff, int bytesBuff, int* pBytesRead) override;
		virtual bool Write(const void* buff, int bytesBuff) override;
	};			
public:
	static Manager* pManagerTop;
	static Manager* pManagerCur;
public:
	FS();
public:
	static Manager* FindManager(const char* pathName);
	static Manager* GetManagerCur();
public:
	static const char* SkipDriveName(const char* pathName);
	static const char* ExtractDriveName(const char* pathName, char* driveName, int lenMax);
public:
	static bool SetDriveCur(const char* driveName);
	static Dir* OpenDirDrive();
	static File* OpenFile(const char* fileName, const char* mode);
	static Dir* OpenDir(const char* dirName);
	static bool RemoveFile(const char* fileName);
	static bool RenameFile(const char* fileNameOld, const char* fileNameNew);
	static bool CreateDir(const char* dirName);
	static bool RemoveDir(const char* dirName);
	static bool RenameDir(const char* fileNameOld, const char* fileNameNew);
	static bool ChangeCurDir(const char* dirName);
	static bool Format(const char* driveName, Printable& out);
	static bool IsLegalDriveName(const char* driveName);
	static const char* JoinPathName(char* pathName, const char* dirName, const char* fileName);
};

}

#endif
