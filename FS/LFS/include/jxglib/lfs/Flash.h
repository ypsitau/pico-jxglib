//==============================================================================
// jxglib/LFS/Flash.h
//==============================================================================
#ifndef PICO_JXGLIB_LFS_FLASH_H
#define PICO_JXGLIB_LFS_FLASH_H
#include "jxglib/LFS.h"

namespace jxglib::LFS {

//------------------------------------------------------------------------------
// Flash
//------------------------------------------------------------------------------
class Flash : public Drive {
private:
	uint32_t offsetXIP_;
public:
	Flash(const char* driveName, uint32_t addrXIP, uint32_t bytesXIP);
public:
	virtual int On_read(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, void* buffer, lfs_size_t size) override;
	virtual int On_prog(const struct lfs_config* cfg, lfs_block_t block, lfs_off_t off, const void* buffer, lfs_size_t size) override;
	virtual int On_erase(const struct lfs_config* cfg, lfs_block_t block) override;
	virtual int On_sync(const struct lfs_config* cfg) override;
};

}

#endif
