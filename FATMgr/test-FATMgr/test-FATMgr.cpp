#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/FATMgr.h"

using namespace jxglib;

class PhysicalDrive : public FATMgr::PhysicalDrive {
public:
	PhysicalDrive(BYTE pdrv = 0) : FATMgr::PhysicalDrive{pdrv} {}
public:
	virtual DSTATUS initialize() override;
	virtual DSTATUS status() override;
	virtual DRESULT read(BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT write(const BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT ioctl(BYTE cmd, void* buff) override;
};

DSTATUS PhysicalDrive::initialize()
{
	::printf("initialize\n");
	return RES_OK;
}

DSTATUS PhysicalDrive::status()
{
	::printf("status\n");
	return RES_OK;
}

DRESULT PhysicalDrive::read(BYTE* buff, LBA_t sector, UINT count)
{
	::printf("read(sector=%d, count=%d)\n", sector, count);
	return RES_OK;
}

DRESULT PhysicalDrive::write(const BYTE* buff, LBA_t sector, UINT count)
{
	::printf("write(sector=%d, count=%d)\n", sector, count);
	return RES_OK;
}

DRESULT PhysicalDrive::ioctl(BYTE cmd, void* buff)
{
	::printf("ioctl(cmd=%d)\n", cmd);
	return RES_OK;
}

FATFS FatFs;

int main()
{
	::stdio_init_all();
	::printf("check\n");
#if 1
	FIL fil;
	char line[100];
	PhysicalDrive physicalDrive(0);
	FATMgr::RegisterPhysicalDrive(physicalDrive);
	::f_mount(&FatFs, "", 0);
	::f_open(&fil, "hoge.txt", 0);
	while (::f_gets(line, sizeof(line), &fil)) {
		::printf(line);
	}
	::f_close(&fil);
#endif
}
