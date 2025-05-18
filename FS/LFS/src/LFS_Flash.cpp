//==============================================================================
// LFS_Flash.cpp
//==============================================================================
#include "jxglib/LFS_Flash.h"
#include "jxglib/Flash.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LFS_Flash
//------------------------------------------------------------------------------
LFS_Flash::LFS_Flash(uint32_t offsetXIP, uint32_t bytesXIP, const char* driveName) :
	LFS(driveName), offsetXIP_{offsetXIP & 0x0fff'ffff}
{
	cfg_.read_size		= FLASH_PAGE_SIZE;				// Minimum size of a block read in bytes
	cfg_.prog_size		= FLASH_PAGE_SIZE;				// Minimum size of a block program in bytes
	cfg_.block_size		= FLASH_SECTOR_SIZE;			// Size of an erasable block in bytes
	cfg_.block_count	= bytesXIP / FLASH_SECTOR_SIZE;	// Number of erasable blocks on the device (initialized in derived class)
	cfg_.cache_size		= FLASH_SECTOR_SIZE;			// Size of block caches in bytes
	cfg_.lookahead_size	= FLASH_SECTOR_SIZE;			// Size of the lookahead buffer in bytes
}

int LFS_Flash::On_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size)
{
	uint32_t offsetXIP = offsetXIP_ + block * cfg->block_size + off;
	//::printf("Read 0x%08x %d\n", offsetXIP, size);
	::memcpy(buffer, reinterpret_cast<const void*>(XIP_BASE + offsetXIP), size);
	return LFS_ERR_OK;
}

int LFS_Flash::On_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size)
{
	uint32_t offsetXIP = offsetXIP_ + block * cfg->block_size + off;
	//::printf("Program 0x%08x %d\n", offsetXIP, size);
	Flash::Program(offsetXIP, buffer, size);
	return LFS_ERR_OK;
}

int LFS_Flash::On_erase(const struct lfs_config* cfg, lfs_block_t block)
{
	uint32_t offsetXIP = offsetXIP_ + block * cfg->block_size;
	//::printf("Erase 0x%08x %d\n", offsetXIP, cfg->block_size);
	Flash::Erase(offsetXIP, cfg->block_size);
	return LFS_ERR_OK;
}

int LFS_Flash::On_sync(const struct lfs_config* cfg)
{
	return LFS_ERR_OK;
}

}
