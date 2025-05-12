//==============================================================================
// LFS.cpp
//==============================================================================
#include "jxglib/Flash.h"
#include "jxglib/LFS.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LFS
//------------------------------------------------------------------------------
LFS::LFS(uint32_t offsetXIP, uint32_t bytesXIP, const char* driveName) : offsetXIP_{offsetXIP},
	cfg_ {
		context:			this,
		read:				user_provided_block_device_read,
		prog:				user_provided_block_device_prog,
		erase:				user_provided_block_device_erase,
		sync:				user_provided_block_device_sync,
		read_size:			FLASH_PAGE_SIZE,	// Minimum size of a block read in bytes
		prog_size:			FLASH_PAGE_SIZE,	// Minimum size of a block program in bytes
		block_size:			FLASH_SECTOR_SIZE,	// Size of an erasable block in bytes
		block_count:		bytesXIP / FLASH_SECTOR_SIZE,
												// Number of erasable blocks on the device
		block_cycles:		500,				// Number of erase cycles before littlefs evicts metadata logs and moves the metadata to another block
		cache_size:			FLASH_SECTOR_SIZE,	// Size of block caches in bytes
		lookahead_size:		FLASH_SECTOR_SIZE,	// Size of the lookahead buffer in bytes
		compact_thresh:		0,					// Threshold for compacting the filesystem. Default is set if zero
		read_buffer:		nullptr,			// Pointer to the read buffer. lfs will allocate this if nullptr
		prog_buffer:		nullptr,			// Pointer to the program buffer. lfs will allocate this if nullptr
		lookahead_buffer:	nullptr,			// Pointer to the lookahead buffer. lfs will allocate this if nullptr
		name_max:			0,					// Maximum length of file names. Default is set if zero
		file_max:			0,					// Maximum number of open files. Default is set if zero
		attr_max:			0,					// Maximum number of attributes per file. Default is set if zero
		metadata_max:		0,					// Maximum number of metadata entries per file. Default is set if zero
		inline_max:			0,					// Maximum size of inline data. Default is set if zero
	}, driveName_{driveName}
{
}

FS::File* LFS::OpenFile(const char* fileName, const char* mode)
{
	RefPtr<File> pFile(new File(lfs_));
	int flags = 0;
	if (mode[0] == 'r') {
		flags = LFS_O_RDONLY;
	} else if (mode[0] == 'w') {
		flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_TRUNC;
	} else if (mode[0] == 'a') {
		flags = LFS_O_WRONLY | LFS_O_CREAT | LFS_O_APPEND;
	} else {
		flags = LFS_O_RDONLY;
	}
	return (::lfs_file_open(&lfs_, pFile->GetEntity(), fileName, flags) == LFS_ERR_OK)? pFile.release() : nullptr;
}

FS::Dir* LFS::OpenDir(const char* dirName)
{
	RefPtr<Dir> pDir(new Dir(lfs_));
	return (::lfs_dir_open(&lfs_, pDir->GetEntity(), dirName) == LFS_ERR_OK)? pDir.release() : nullptr;
}

bool LFS::RemoveFile(const char* fileName)
{
	return ::lfs_remove(&lfs_, fileName) == LFS_ERR_OK;
}

bool LFS::RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	return ::lfs_rename(&lfs_, fileNameOld, fileNameNew) == LFS_ERR_OK;
}

bool LFS::CreateDir(const char* dirName)
{
	return ::lfs_mkdir(&lfs_, dirName) == LFS_ERR_OK;
}

bool LFS::RemoveDir(const char* dirName)
{
	return ::lfs_remove(&lfs_, dirName) == LFS_ERR_OK;
}

bool LFS::RenameDir(const char* dirNameOld, const char* dirNameNew)
{
	return ::lfs_rename(&lfs_, dirNameOld, dirNameNew) == LFS_ERR_OK;
}

bool LFS::Format()
{
	return ::lfs_format(&lfs_, &cfg_) == LFS_ERR_OK && ::lfs_mount(&lfs_, &cfg_) == LFS_ERR_OK;
}

int LFS::user_provided_block_device_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	uint32_t offsetXIP = reinterpret_cast<LFS*>(cfg->context)->GetOffsetXIP() + block * cfg->block_size + off;
	::printf("Read 0x%08x %d\n", offsetXIP, size);
	::memcpy(buffer, reinterpret_cast<const void*>(XIP_BASE + offsetXIP), size);
	return LFS_ERR_OK;
}

int LFS::user_provided_block_device_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	uint32_t offsetXIP = reinterpret_cast<LFS*>(cfg->context)->GetOffsetXIP() + block * cfg->block_size + off;
	::printf("Program 0x%08x %d\n", offsetXIP, size);
	Flash::Program(offsetXIP, buffer, size);
	return LFS_ERR_OK;
}

int LFS::user_provided_block_device_erase(const struct lfs_config* cfg, lfs_block_t block)
{
	uint32_t offsetXIP = reinterpret_cast<LFS*>(cfg->context)->GetOffsetXIP() + block * cfg->block_size;
	::printf("Erase 0x%08x %d\n", offsetXIP, cfg->block_size);
	Flash::Erase(offsetXIP, cfg->block_size);
	return LFS_ERR_OK;
}

int LFS::user_provided_block_device_sync(const struct lfs_config* cfg)
{
	return LFS_ERR_OK;
}

//------------------------------------------------------------------------------
// LFS::File
//------------------------------------------------------------------------------
LFS::File::File(lfs_t& lfs) : lfs_(lfs)
{
}

int LFS::File::Read(void* buffer, int bytes)
{
	return ::lfs_file_read(&lfs_, &file_, buffer, bytes);
}

int LFS::File::Write(const void* buffer, int bytes)
{
	return ::lfs_file_write(&lfs_, &file_, buffer, bytes);
}

void LFS::File::Close()
{
	::lfs_file_close(&lfs_, &file_);
}

bool LFS::File::Seek(int position)
{
	// Seek to the specified position in the file
	return ::lfs_file_seek(&lfs_, &file_, position, LFS_SEEK_SET) == LFS_ERR_OK;
}

int LFS::File::Tell()
{
	return ::lfs_file_tell(&lfs_, &file_);
}

int LFS::File::Size()
{
	return ::lfs_file_size(&lfs_, &file_);
}

bool LFS::File::Flush()
{
	return ::lfs_file_sync(&lfs_, &file_) == LFS_ERR_OK;
}

bool LFS::File::Truncate(int bytes)
{
	return ::lfs_file_truncate(&lfs_, &file_, bytes) == LFS_ERR_OK;
}

bool LFS::File::Sync()
{
	return ::lfs_file_sync(&lfs_, &file_) == LFS_ERR_OK;
}

//------------------------------------------------------------------------------
// LFS::FileInfo
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// LFS::Dir
//------------------------------------------------------------------------------
LFS::Dir::Dir(lfs_t& lfs) : lfs_(lfs)
{
}

bool LFS::Dir::Read(FS::FileInfo** ppFileInfo)
{
	*ppFileInfo = &fileInfo_;
	// LFS_ERR_NOENT indicates no more entries
	return ::lfs_dir_read(&lfs_, &dir_, &fileInfo_.GetEntity()) == LFS_ERR_OK;
}

void LFS::Dir::Close()
{
	::lfs_dir_close(&lfs_, &dir_);
}

}

#if 0
void initializeLFS()
{
	// LittleFSの初期化
	int err = ::lfs_mount(&lfs, &cfg);
	if (err) {
		// マウントに失敗した場合はフォーマットして再マウント
		::lfs_format(&lfs, &cfg);
		err = ::lfs_mount(&lfs, &cfg);
		if (err) {
			// 再マウント失敗時のエラーハンドリング
			printf("Failed to mount LFS\n");
			return;
		}
	}
	printf("LFS mounted successfully\n");
}

void writeFile(const char* path, const char* data)
{
	// ファイルを書き込む
	lfs_file_t file;
	int err = ::lfs_file_open(&lfs, &file, path, LFS_O_WRONLY | LFS_O_CREAT);
	if (err) {
		printf("Failed to open file for writing\n");
		return;
	}
	::lfs_file_write(&lfs, &file, data, strlen(data));
	::lfs_file_close(&lfs, &file);
	printf("File written successfully\n");
}

void readFile(const char* path)
{
	// ファイルを読み込む
	lfs_file_t file;
	int err = ::lfs_file_open(&lfs, &file, path, LFS_O_RDONLY);
	if (err) {
		printf("Failed to open file for reading\n");
		return;
	}
	char buffer[128];
	int bytesRead = ::lfs_file_read(&lfs, &file, buffer, sizeof(buffer) - 1);
	if (bytesRead > 0) {
		buffer[bytesRead] = '\0'; // Null-terminate the string
		printf("File content: %s\n", buffer);
	}
	::lfs_file_close(&lfs, &file);
}

void unmountLFS()
{
	// LittleFSのアンマウント
	::lfs_unmount(&lfs);
	printf("LFS unmounted successfully\n");
}
#endif
