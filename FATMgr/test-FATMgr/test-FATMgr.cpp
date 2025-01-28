#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/FATMgr.h"

using namespace jxglib;

class DiskDummy : public FATMgr::Disk {
public:
	DiskDummy(BYTE pdrv = 0) : FATMgr::Disk{pdrv} {}
public:
	virtual DSTATUS initialize() override;
	virtual DSTATUS status() override;
	virtual DRESULT read(BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT write(const BYTE* buff, LBA_t sector, UINT count) override;
	virtual DRESULT ioctl(BYTE cmd, void* buff) override;
};

DSTATUS DiskDummy::initialize()
{
	::printf("initialize\n");
	return RES_OK;
}

DSTATUS DiskDummy::status()
{
	::printf("status\n");
	return RES_OK;
}

DRESULT DiskDummy::read(BYTE* buff, LBA_t sector, UINT count)
{
	::printf("read(sector=%d, count=%d)\n", sector, count);
	return RES_OK;
}

DRESULT DiskDummy::write(const BYTE* buff, LBA_t sector, UINT count)
{
	::printf("write(sector=%d, count=%d)\n", sector, count);
	return RES_OK;
}

DRESULT DiskDummy::ioctl(BYTE cmd, void* buff)
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
	DiskDummy disk(0);
	FATMgr::RegisterDisk(disk);
	::f_mount(&FatFs, "", 0);
	::f_open(&fil, "hoge.txt", 0);
	while (::f_gets(line, sizeof(line), &fil)) {
		::printf(line);
	}
	::f_close(&fil);
#endif
}
