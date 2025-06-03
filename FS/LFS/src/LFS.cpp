//==============================================================================
// LFS.cpp
//==============================================================================
#include <memory>
#include "jxglib/LFS.h"

namespace jxglib::LFS {

//------------------------------------------------------------------------------
// Functions
//------------------------------------------------------------------------------
FS::FileInfo* MakeFileInfo(const lfs_info& info)
{
	return new FS::FileInfo(info.name, 
		(info.type == LFS_TYPE_DIR)? FS::FileInfo::Attr::Directory : FS::FileInfo::Attr::Archive, 
		static_cast<uint32_t>(info.size));
}

//------------------------------------------------------------------------------
// LFS::Drive
//------------------------------------------------------------------------------
Drive::Drive(const char* driveName) : FS::Drive("lfs", driveName),
	cfg_ {
		context:			this,
		read:				Callback_read,
		prog:				Callback_prog,
		erase:				Callback_erase,
		sync:				Callback_sync,
		read_size:			0,				// Minimum size of a block read in bytes (initialized in derived class)
		prog_size:			0,				// Minimum size of a block program in bytes (initialized in derived class)
		block_size:			0,				// Size of an erasable block in bytes (initialized in derived class)
		block_count:		0,				// Number of erasable blocks on the device (initialized in derived class)
		block_cycles:		500,			// Number of erase cycles before littlefs evicts metadata logs and moves the metadata to another block
		cache_size:			0,				// Size of block caches in bytes (initialized in derived class)
		lookahead_size:		0,				// Size of the lookahead buffer in bytes (initialized in derived class)
		compact_thresh:		0,				// Threshold for compacting the filesystem. Default is set if zero
		read_buffer:		nullptr,		// Pointer to the read buffer. lfs will allocate this if nullptr
		prog_buffer:		nullptr,		// Pointer to the program buffer. lfs will allocate this if nullptr
		lookahead_buffer:	nullptr,		// Pointer to the lookahead buffer. lfs will allocate this if nullptr
		name_max:			0,				// Maximum length of file names. Default is set if zero
		file_max:			0,				// Maximum number of open files. Default is set if zero
		attr_max:			0,				// Maximum number of attributes per file. Default is set if zero
		metadata_max:		0,				// Maximum number of metadata entries per file. Default is set if zero
		inline_max:			0,				// Maximum size of inline data. Default is set if zero
	}
{
}

const char* Drive::GetFileSystemName()
{
	return Mount()? "LittleFS" : "unmounted";
}

FS::File* Drive::OpenFile(const char* fileName, const char* mode)
{
	if (!Mount()) return nullptr;
	std::unique_ptr<File> pFile(new File(*this, lfs_));
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
	if (::lfs_file_open(&lfs_, pFile->GetEntity(), fileName, flags) == LFS_ERR_OK) {
		pFile->SetOpenedFlag();
		return pFile.release();
	}
	return nullptr;	
}

FS::Dir* Drive::OpenDir(const char* dirName, uint8_t attrExclude)
{
	if (!Mount()) return nullptr;
	std::unique_ptr<Dir> pDir(new Dir(*this, lfs_));
	return (::lfs_dir_open(&lfs_, pDir->GetEntity(), dirName) == LFS_ERR_OK)? pDir.release() : nullptr;
}

bool Drive::RemoveFile(const char* fileName)
{
	if (!Mount()) return false;
	return ::lfs_remove(&lfs_, fileName) == LFS_ERR_OK;
}

bool Drive::RenameFile(const char* fileNameOld, const char* fileNameNew)
{
	if (!Mount()) return false;
	return ::lfs_rename(&lfs_, fileNameOld, fileNameNew) == LFS_ERR_OK;
}

bool Drive::CreateDir(const char* dirName)
{
	if (!Mount()) return false;
	return ::lfs_mkdir(&lfs_, dirName) == LFS_ERR_OK;
}

bool Drive::RemoveDir(const char* dirName)
{
	if (!Mount()) return false;
	return ::lfs_remove(&lfs_, dirName) == LFS_ERR_OK;
}

bool Drive::RenameDir(const char* dirNameOld, const char* dirNameNew)
{
	if (!Mount()) return false;
	return ::lfs_rename(&lfs_, dirNameOld, dirNameNew) == LFS_ERR_OK;
}

bool Drive::Format()
{
	return ::lfs_format(&lfs_, &cfg_) == LFS_ERR_OK && ::lfs_mount(&lfs_, &cfg_) == LFS_ERR_OK;
}

bool Drive::Mount()
{
	if (mountedFlag_) return true;
	if (::lfs_mount(&lfs_, &cfg_) != LFS_ERR_OK) return false;
	mountedFlag_ = true;
	return true;
}

bool Drive::Unmount()
{
	if (!mountedFlag_) return true;
	if (::lfs_unmount(&lfs_) != LFS_ERR_OK) return false;
	mountedFlag_ = false;
	return true;
}

FS::FileInfo* Drive::GetFileInfo(const char* pathName)
{
	if (!Mount()) return nullptr;
	lfs_info info;
	return (::lfs_stat(&lfs_, pathName, &info) == LFS_ERR_OK)? MakeFileInfo(info) : nullptr;
}

uint64_t Drive::GetBytesTotal()
{
	return Mount()? static_cast<uint64_t>(cfg_.block_count) * cfg_.block_size : 0;
}

uint64_t Drive::GetBytesUsed()
{
	if (!Mount()) return 0;
	lfs_ssize_t nBlocks = ::lfs_fs_size(&lfs_);
	return (nBlocks < 0)? 0 : static_cast<uint64_t>(nBlocks) * cfg_.block_size;
}

int Drive::Callback_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	return reinterpret_cast<Drive*>(cfg->context)->On_read(cfg, block, off, buffer, size);
}

int Drive::Callback_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	return reinterpret_cast<Drive*>(cfg->context)->On_prog(cfg, block, off, buffer, size);
}

int Drive::Callback_erase(const struct lfs_config* cfg, lfs_block_t block)
{
	return reinterpret_cast<Drive*>(cfg->context)->On_erase(cfg, block);
}

int Drive::Callback_sync(const struct lfs_config* cfg)
{
	return reinterpret_cast<Drive*>(cfg->context)->On_sync(cfg);
}

//------------------------------------------------------------------------------
// LFS::File
//------------------------------------------------------------------------------
File::File(FS::Drive& drive, lfs_t& lfs) : FS::File(drive), lfs_(lfs), openedFlag_{false}
{
}

int File::Read(void* buff, int bytesBuff)
{
	return ::lfs_file_read(&lfs_, &file_, buff, bytesBuff);
}

int File::Write(const void* buff, int bytesBuff)
{
	return ::lfs_file_write(&lfs_, &file_, buff, bytesBuff);
}

void File::Close()
{
	if (openedFlag_) {
		::lfs_file_close(&lfs_, &file_);
		openedFlag_ = false;
	}
}

bool File::Seek(int position)
{
	// Seek to the specified position in the file
	return ::lfs_file_seek(&lfs_, &file_, position, LFS_SEEK_SET) == LFS_ERR_OK;
}

int File::Tell()
{
	return ::lfs_file_tell(&lfs_, &file_);
}

int File::Size()
{
	return ::lfs_file_size(&lfs_, &file_);
}

bool File::Flush()
{
	return ::lfs_file_sync(&lfs_, &file_) == LFS_ERR_OK;
}

bool File::Truncate(int bytes)
{
	return ::lfs_file_truncate(&lfs_, &file_, bytes) == LFS_ERR_OK;
}

bool File::Sync()
{
	return ::lfs_file_sync(&lfs_, &file_) == LFS_ERR_OK;
}

//------------------------------------------------------------------------------
// LFS::Dir
//------------------------------------------------------------------------------
Dir::Dir(FS::Drive& drive, lfs_t& lfs) : FS::Dir(drive), lfs_(lfs), openedFlag_{true}, nItems_{0}
{
}

FS::FileInfo* Dir::Read()
{
	lfs_info info;
	for (;;) {
		int rtn = ::lfs_dir_read(&lfs_, &dir_, &info);
		if (rtn < 0) return nullptr;  // Error reading directory
		if (rtn == 0) {  // End of directory
			if (rewindFlag_ && nItems_ > 0) {
				::lfs_dir_rewind(&lfs_, &dir_);
				nItems_ = 0;
			} else {
				return nullptr;  // No more items
			}
		} else if (::strcmp(info.name, ".") != 0 && ::strcmp(info.name, "..") != 0) {
			nItems_++;
			break;  // Found a valid file or directory entry
		}
	}
	return MakeFileInfo(info);
}

void Dir::Close()
{
	if (openedFlag_) {
		::lfs_dir_close(&lfs_, &dir_);
		openedFlag_ = false;
	}
}

}
