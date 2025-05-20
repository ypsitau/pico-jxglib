//==============================================================================
// jxglib/FAT/SDCard.h
//==============================================================================
#ifndef PICO_JXGLIB_FAT_SDCARD_H
#define PICO_JXGLIB_FAT_SDCARD_H
#include "jxglib/FAT.h"
#include "jxglib/SDCard.h"

namespace jxglib::FAT {

//-----------------------------------------------------------------------------
// FAT::SDCard
//-----------------------------------------------------------------------------
class SDCard : public Drive {
private:
	jxglib::SDCard sdCard_;
public:
	SDCard(spi_inst_t* spi, uint baudrate, const jxglib::SDCard::PinAssign& pinAssign, const char* driveName = "SD");
public:
	// virtual functions of FAT
	virtual DSTATUS status() override;
	virtual DSTATUS initialize() override;
	virtual DRESULT read(BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT write(const BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT ioctl_CTRL_SYNC() override;
	virtual DRESULT ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount) override;
	virtual DRESULT ioctl_GET_SECTOR_SIZE(WORD* pSectorSize) override;
	virtual DRESULT ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize) override;
	virtual DRESULT ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA) override;
};

}

#endif
