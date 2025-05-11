//==============================================================================
// LFS.cpp
//==============================================================================
#include "jxglib/Flash.h"
#include "jxglib/LFS.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LFS
//------------------------------------------------------------------------------
LFS::LFS() :
	cfg_ {
		context:			nullptr,
		read:				user_provided_block_device_read,
		prog:				user_provided_block_device_prog,
		erase:				user_provided_block_device_erase,
		sync:				user_provided_block_device_sync,
		read_size:			16,
		prog_size:			16,
		block_size:			4096,
		block_count:		128,
		block_cycles:		500,
		cache_size:			16,
		lookahead_size:		16,
		compact_thresh:		0,
		read_buffer:		nullptr,
		prog_buffer:		nullptr,
		lookahead_buffer:	nullptr,
		name_max:			0,
		file_max:			0,
		attr_max:			0,
		metadata_max:		0,
		inline_max:			0,
	}
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
	return (::lfs_file_open(&lfs_, pFile->GetEntity(), fileName, flags) == 0)? pFile.release() : nullptr;
}

FS::Dir* LFS::OpenDir(const char* dirName)
{
	RefPtr<Dir> pDir(new Dir(lfs_));
	return (::lfs_dir_open(&lfs_, pDir->GetEntity(), dirName) == 0)? pDir.release() : nullptr;
}

bool LFS::RemoveFile(const char* fileName)
{
	return ::lfs_remove(&lfs_, fileName) == 0;
}

bool LFS::RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	return ::lfs_rename(&lfs_, fileNameOld, fileNameNew) == 0;
}

bool LFS::CreateDir(const char* dirName)
{
	return ::lfs_mkdir(&lfs_, dirName) == 0;
}

bool LFS::RemoveDir(const char* dirName)
{
	return ::lfs_remove(&lfs_, dirName) == 0;
}

bool LFS::RenameDir(const char* dirNameOld, const char* dirNameNew)
{
	return ::lfs_rename(&lfs_, dirNameOld, dirNameNew) == 0;
}

bool LFS::Format()
{
	return ::lfs_format(&lfs_, &cfg_) == 0 && ::lfs_mount(&lfs_, &cfg_) == 0;
}

uint32_t offsetXIP = 0x00100000;

int LFS::user_provided_block_device_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	::memcpy(buffer, reinterpret_cast<const void*>(offsetXIP + block * cfg->block_size + off), size);
	return 0; // Return 0 on success
}

int LFS::user_provided_block_device_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	Flash::Instance.Program(offsetXIP + block * cfg->block_size + off, buffer, size);
	return 0; // Return 0 on success
}

int LFS::user_provided_block_device_erase(const struct lfs_config* cfg, lfs_block_t block)
{
	Flash::Instance.Erase(offsetXIP + block * cfg->block_size, cfg->block_size);
	return 0; // Return 0 on success
}

int LFS::user_provided_block_device_sync(const struct lfs_config* cfg)
{
	return 0; // Return 0 on success
}

//------------------------------------------------------------------------------
// LFS::File
//------------------------------------------------------------------------------
int LFS::File::Read(void* buffer, int bytes)
{
	// Read data from the file
	int bytesRead = ::lfs_file_read(&lfs_, &file_, buffer, bytes);
	if (bytesRead < 0) {
		return -1; // Error occurred
	}
	return bytesRead; // Return number of bytes read
}

int LFS::File::Write(const void* buffer, int bytes)
{
	// Write data to the file
	int bytesWritten = ::lfs_file_write(&lfs_, &file_, buffer, bytes);
	if (bytesWritten < 0) {
		return -1; // Error occurred
	}
	return bytesWritten; // Return number of bytes written
}

void LFS::File::Close()
{
	// Close the file
	::lfs_file_close(&lfs_, &file_);
}

bool LFS::File::Seek(int position)
{
	// Seek to the specified position in the file
	int err = ::lfs_file_seek(&lfs_, &file_, position, LFS_SEEK_SET);
	return (err == 0);
}

int LFS::File::Tell()
{
	// Get the current position in the file
	int err = ::lfs_file_tell(&lfs_, &file_);
	if (err < 0) {
		return -1; // Error occurred
	}
	return err; // Return current position
}

int LFS::File::Size()
{
	// Get the size of the file
	int err = ::lfs_file_size(&lfs_, &file_);
	if (err < 0) {
		return -1; // Error occurred
	}
	return err; // Return size of the file
}

bool LFS::File::Flush()
{
	// Flush the file to ensure all data is written
	int err = ::lfs_file_sync(&lfs_, &file_);
	return (err == 0);
}

bool LFS::File::Truncate(int bytes)
{
	// Truncate the file to the specified size
	int err = ::lfs_file_truncate(&lfs_, &file_, bytes);
	return (err == 0);
}

bool LFS::File::Sync()
{
	// Sync the file to ensure all data is written
	int err = ::lfs_file_sync(&lfs_, &file_);
	return (err == 0);
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
	// Read the next entry in the directory
	int err = ::lfs_dir_read(&lfs_, &dir_, &fileInfo_.GetEntity());
	if (err == 0) {
		*ppFileInfo = &fileInfo_;
		return true;
	} else if (err == LFS_ERR_NOENT) {
		return false; // No more entries
	} else {
		return false; // Error occurred
	}
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
