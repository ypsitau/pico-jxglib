//==============================================================================
// jxglib/LFS.h
//==============================================================================
#ifndef PICO_JXGLIB_LFS_H
#define PICO_JXGLIB_LFS_H
#include <stdio.h>
#include "pico/stdlib.h"
#include "littlefs/lfs.h"
#include "jxglib/FS.h"

namespace jxglib::LFS {

//------------------------------------------------------------------------------
// LFS::File
//------------------------------------------------------------------------------
class File : public FS::File {
private:
	lfs_t& lfs_;
	lfs_file_t file_;
	bool openedFlag_;
	lfs_attr attrs_[1];
	lfs_file_config cfg_;
	uint64_t unixtime_;
public:
	File(FS::Drive& drive, lfs_t& lfs);
	~File() { Close(); }
public:
	void SetTimeStamp(const DateTime& dt) { unixtime_ = dt.ToUnixTime(); }
	lfs_file_config* GetConfig() { return &cfg_; }
	lfs_file_t* GetEntity() { return &file_; }
	const lfs_file_t* GetEntity() const { return &file_; }
	void SetOpenedFlag(bool openedFlag = true) { openedFlag_ = openedFlag; }
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

//------------------------------------------------------------------------------
// LFS::Dir
//------------------------------------------------------------------------------
class Dir : public FS::Dir {
private:
	lfs_t& lfs_;
	lfs_dir_t dir_;
	bool openedFlag_;
	int nItems_;
public:
	Dir(FS::Drive& drive, const char* dirName, lfs_t& lfs);
	~Dir() { Close(); }
public:
	lfs_dir_t* GetEntity() { return &dir_; }
	const lfs_dir_t* GetEntity() const { return &dir_; }
public:
	// virtual functions of FS::FileInfoReader
	virtual FS::FileInfo* Read() override;
	// virtual functions of FS::Dir
	virtual void Close() override;
};

//------------------------------------------------------------------------------
// LFS::Drive
//------------------------------------------------------------------------------
class Drive : public FS::Drive {
protected:
	lfs_t lfs_;
	lfs_config cfg_;
public:
	Drive(const char* driveName = "flash");
public:
	// virtual functions of FS::Drive
	virtual bool CheckMounted() override;
	virtual const char* GetFileSystemName() override;
	virtual FS::File* OpenFile(const char* fileNameN, const char* mode) override;
	virtual FS::Dir* OpenDir(const char* dirNameN, const char* dirName, uint8_t attrExclude) override;
	virtual bool SetTimeStamp(const char* pathNameN, const DateTime& dt) override;
	virtual bool RemoveFile(const char* fileNameN) override;
	virtual bool Rename(const char* fileNameOldN, const char* fileNameNewN) override;
	virtual bool CreateDir(const char* dirNameN) override;
	virtual bool RemoveDir(const char* dirNameN) override;
	virtual bool Format() override;
	virtual bool Mount() override;
	virtual bool Unmount() override;
	virtual FS::FileInfo* GetFileInfo(const char* pathNameN) override;
	virtual uint64_t GetBytesTotal() override;
	virtual uint64_t GetBytesUsed() override;
public:
	virtual int On_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) = 0;
	virtual int On_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size) = 0;
	virtual int On_erase(const struct lfs_config* cfg, lfs_block_t block) = 0;
	virtual int On_sync(const struct lfs_config* cfg) = 0;
public:
	static int Callback_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size);
	static int Callback_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size);
	static int Callback_erase(const struct lfs_config* cfg, lfs_block_t block);
	static int Callback_sync(const struct lfs_config* cfg);
};

}

#endif
