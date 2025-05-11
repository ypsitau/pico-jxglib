//==============================================================================
// jxglib/LFS.h
//==============================================================================
#ifndef PICO_JXGLIB_LFS_H
#define PICO_JXGLIB_LFS_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "littlefs/lfs.h"
#include "jxglib/FS.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LFS
//------------------------------------------------------------------------------
class LFS : public FS::Manager {
public:
	class File : public FS::File {
	public:
		DeclareReferable(File);
	private:
		lfs_file_t file_;
	public:
		File();
		~File();
	public:
		lfs_file_t* GetEntity() { return &file_; }
		const lfs_file_t* GetEntity() const { return &file_; }
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
	class FileInfo : public FS::FileInfo {
	private:
		lfs_info fileInfo_;
	public:
		FileInfo() {}
		~FileInfo() {}
	public:
		lfs_info& GetEntity() { return fileInfo_; }
		const lfs_info& GetEntity() const { return fileInfo_; }
	public:
		virtual const char* GetName() const { return fileInfo_.name; }
		virtual uint32_t GetSize() const { return fileInfo_.size; }
		virtual bool IsDirectory() const { return (fileInfo_.type & LFS_TYPE_DIR) != 0; }
		virtual bool IsFile() const { return (fileInfo_.type & LFS_TYPE_REG) != 0; }
		//virtual bool IsHidden() const { return (fileInfo_.fattrib & AM_HID) != 0; }
		//virtual bool IsReadOnly() const { return (fileInfo_.fattrib & AM_RDO) != 0; }
		//virtual bool IsSystem() const { return (fileInfo_.fattrib & AM_SYS) != 0; }
		//virtual bool IsArchive() const { return (fileInfo_.fattrib & AM_ARC) != 0; }
	};
	class Dir : public FS::Dir {
	public:
		DeclareReferable(Dir);
	private:
		lfs_dir_t dir_;
		FileInfo fileInfo_;
	public:
		Dir();
		~Dir();
	public:
		lfs_dir_t* GetEntity() { return &dir_; }
		const lfs_dir_t* GetEntity() const { return &dir_; }
	public:
		bool Read(FS::FileInfo** ppFileInfo) override;
		void Close() override;
	};
public:
	// virtual functions of FS::Manager
	virtual File* OpenFile(const char* fileName, const char* mode) override;
	virtual Dir* OpenDir(const char* dirName) override;
	virtual bool RemoveFile(const char* fileName) override;
	virtual bool RenameFile(const char* fileNameOld, const char* fileNameNew) override;
	virtual bool CreateDir(const char* dirName) override;
	virtual bool RemoveDir(const char* dirName) override;
	virtual bool RenameDir(const char* fileNameOld, const char* fileNameNew) override;
};

}

#endif