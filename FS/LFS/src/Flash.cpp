//==============================================================================
// Flash.cpp
//==============================================================================
#include "jxglib/Flash.h"
#include "jxglib/LFS/Flash.h"

namespace jxglib::LFS {

//------------------------------------------------------------------------------
// LFS::Flash
//------------------------------------------------------------------------------
Flash::Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP) :
	Drive(driveName), offsetXIP_{addrXIP & 0x0fff'ffff}
{
	cfg_.read_size		= FLASH_PAGE_SIZE;				// Minimum size of a block read in bytes
	cfg_.prog_size		= FLASH_PAGE_SIZE;				// Minimum size of a block program in bytes
	cfg_.block_size		= FLASH_SECTOR_SIZE;			// Size of an erasable block in bytes
	cfg_.block_count	= bytesXIP / FLASH_SECTOR_SIZE;	// Number of erasable blocks on the device (initialized in derived class)
	cfg_.cache_size		= FLASH_SECTOR_SIZE;			// Size of block caches in bytes
	cfg_.lookahead_size	= FLASH_SECTOR_SIZE;			// Size of the lookahead buffer in bytes
}

Flash::Flash(const char* driveName, uint32_t bytesXIP) :
	Flash(driveName, XIP_BASE + PICO_FLASH_SIZE_BYTES - bytesXIP, bytesXIP)
{
}

const char* Flash::GetRemarks(char* buff, int lenMax) const
{
	::snprintf(buff, lenMax, "LFS::Flash 0x%08x-0x%08x %dkB",
		XIP_BASE + offsetXIP_, XIP_BASE + offsetXIP_ + cfg_.block_count * cfg_.block_size,
		cfg_.block_count * cfg_.block_size / 1024);
	return buff;
}

int Flash::On_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	uint32_t offsetXIP = offsetXIP_ + block * cfg->block_size + off;
	//::printf("Read 0x%08x %d\n", offsetXIP, size);
	::memcpy(buffer, reinterpret_cast<const void*>(XIP_BASE + offsetXIP), size);
	return LFS_ERR_OK;
}

int Flash::On_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	uint32_t offsetXIP = offsetXIP_ + block * cfg->block_size + off;
	//::printf("Program 0x%08x %d\n", offsetXIP, size);
	jxglib::Flash::Program(offsetXIP, buffer, size);
	return LFS_ERR_OK;
}

int Flash::On_erase(const struct lfs_config* cfg, lfs_block_t block)
{
	uint32_t offsetXIP = offsetXIP_ + block * cfg->block_size;
	//::printf("Erase 0x%08x %d\n", offsetXIP, cfg->block_size);
	jxglib::Flash::Erase(offsetXIP, cfg->block_size);
	return LFS_ERR_OK;
}

int Flash::On_sync(const struct lfs_config* cfg)
{
	return LFS_ERR_OK;
}

}
