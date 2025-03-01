#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/FAT.h"
#include "jxglib/SDCard.h"

using namespace jxglib;

class SDDrive : public FAT::PhysicalDriveT<> {
private:
	SDCard sdCard_;
public:
	SDDrive(spi_inst_t* spi, uint baudrate, const SDCard::PinAssign& pinAssign, BYTE pdrv = 0) :
		FAT::PhysicalDriveT<>{pdrv}, sdCard_(spi, baudrate, pinAssign) {}
public:
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

DSTATUS SDDrive::status()
{
	//::printf("status\n");
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DSTATUS SDDrive::initialize()
{
	::printf("initialize\n");
	sdCard_.Initialize();
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DRESULT SDDrive::read(BYTE* buff, LBA_t sector, UINT count)
{
	::printf("read(sector=%d, count=%d)\n", sector, count);
	return sdCard_.ReadBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT SDDrive::write(const BYTE* buff, LBA_t sector, UINT count)
{
	::printf("write(sector=%d, count=%d)\n", sector, count);
	return sdCard_.WriteBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT SDDrive::ioctl_CTRL_SYNC()
{
	::printf("ioctl(CTRL_SYNC)\n");
	return RES_OK;
}

DRESULT SDDrive::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = 0;
	return RES_OK;
}

DRESULT SDDrive::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = 512;
	return RES_OK;
}

DRESULT SDDrive::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	::printf("ioctl(GET_BLOCK_SIZE)\n");
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT SDDrive::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	SDDrive sdDrive(spi0, 10 * 1000 * 1000, {CS: GPIO5});
	sdDrive.Mount();
#if 1
	FIL fil;
	char buff[80];
	FRESULT result = ::f_open(&fil, "/SAMPLE.TXT", FA_READ);
	if (result != FR_OK) {
		::printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	while (::f_gets(buff, sizeof(buff), &fil)) {
		::printf(buff);
	}
	::f_close(&fil);
#endif
}
