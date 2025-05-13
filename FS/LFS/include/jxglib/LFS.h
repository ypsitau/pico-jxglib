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
		lfs_t& lfs_;
		lfs_file_t file_;
	public:
		File(lfs_t& lfs);
		~File() {}
	public:
		lfs_file_t* GetEntity() { return &file_; }
		const lfs_file_t* GetEntity() const { return &file_; }
	public:
		virtual int Read(void* buffer, int bytes) override;
		virtual int Write(const void* buffer, int bytes) override;
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
		lfs_info info_;
	public:
		FileInfo() {}
		~FileInfo() {}
	public:
		lfs_info& GetEntity() { return info_; }
		const lfs_info& GetEntity() const { return info_; }
	public:
		virtual const char* GetName() const override { return info_.name; }
		virtual uint32_t GetSize() const override { return info_.size; }
		virtual bool IsDirectory() const override { return (info_.type & LFS_TYPE_DIR) != 0; }
		virtual bool IsFile() const override { return (info_.type & LFS_TYPE_REG) != 0; }
		//virtual bool IsHidden() const override { return (info_.fattrib & AM_HID) != 0; }
		//virtual bool IsReadOnly() const override { return (info_.fattrib & AM_RDO) != 0; }
		//virtual bool IsSystem() const override { return (info_.fattrib & AM_SYS) != 0; }
		//virtual bool IsArchive() const override { return (info_.fattrib & AM_ARC) != 0; }
	};
	class Dir : public FS::Dir {
	public:
		DeclareReferable(Dir);
	private:
		lfs_t& lfs_;
		lfs_dir_t dir_;
		FileInfo fileInfo_;
	public:
		Dir(lfs_t& lfs);
		~Dir() {}
	public:
		lfs_dir_t* GetEntity() { return &dir_; }
		const lfs_dir_t* GetEntity() const { return &dir_; }
	public:
		bool Read(FS::FileInfo** ppFileInfo) override;
		void Close() override;
	};
private:
	lfs_t lfs_;
	lfs_config cfg_;
	const char* driveName_;
	uint32_t offsetXIP_;
	bool mountedFlag_;
public:
	LFS(uint32_t offsetXIP, uint32_t bytesXIP, const char* driveName = "flash");
public:
	uint32_t GetOffsetXIP() const { return offsetXIP_; }
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
	static int user_provided_block_device_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
	static int user_provided_block_device_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
	static int user_provided_block_device_erase(const struct lfs_config* cfg, lfs_block_t block);
	static int user_provided_block_device_sync(const struct lfs_config* cfg);
};

}

#endif