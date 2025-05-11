#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/FAT.h"
#include "jxglib/SDCard.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"

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
	//::printf("initialize\n");
	sdCard_.Initialize();
	return 0x00;	// STA_NOINIT, STA_NODISK, STA_PROTECT
}

DRESULT SDDrive::read(BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("read(sector=%d, count=%d)\n", sector, count);
	return sdCard_.ReadBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT SDDrive::write(const BYTE* buff, LBA_t sector, UINT count)
{
	//::printf("write(sector=%d, count=%d)\n", sector, count);
	return sdCard_.WriteBlock(sector, buff, count)? RES_OK : RES_ERROR;
}

DRESULT SDDrive::ioctl_CTRL_SYNC()
{
	//::printf("ioctl(CTRL_SYNC)\n");
	return RES_OK;
}

DRESULT SDDrive::ioctl_GET_SECTOR_COUNT(LBA_t* pSectorCount)
{
	//::printf("ioctl(GET_SECTOR_COUNT)\n");
	*pSectorCount = 0;
	return RES_OK;
}

DRESULT SDDrive::ioctl_GET_SECTOR_SIZE(WORD* pSectorSize)
{
	//::printf("ioctl(GET_SECTOR_SIZE)\n");
	*pSectorSize = 512;
	return RES_OK;
}

DRESULT SDDrive::ioctl_GET_BLOCK_SIZE(DWORD* pBlockSize)
{
	//::printf("ioctl(GET_BLOCK_SIZE)\n");
	*pBlockSize = 1 << 0;
	return RES_OK;
}

DRESULT SDDrive::ioctl_CTRL_TRIM(LBA_t startLBA, LBA_t endLBA)
{
	//::printf("ioctl(CTRL_TRIM)\n");
	return RES_OK;
}

ShellCmd(cp, "copies a file")
{
	if (argc < 3) {
		out.Printf("Usage: %s <src> <dst>\n", argv[0]);
		return 1;
	}
	const char* fileNameSrc = argv[1];
	const char* fileNameDst = argv[2];
	FIL filSrc, filDst;
	FRESULT result = ::f_open(&filSrc, fileNameSrc, FA_READ);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	result = ::f_open(&filDst, fileNameDst, FA_WRITE | FA_CREATE_ALWAYS);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		::f_close(&filSrc);
		return 1;
	}
	char buff[512];
	UINT bytesRead, bytesWritten;
	while (::f_read(&filSrc, buff, sizeof(buff), &bytesRead) == FR_OK && bytesRead > 0) {
		if (::f_write(&filDst, buff, bytesRead, &bytesWritten) != FR_OK || bytesWritten != bytesRead) {
			out.Printf("Error writing to destination file\n");
			break;
		}
	}
	::f_close(&filSrc);
	::f_close(&filDst);
	return 0;
}

ShellCmd(mkdir, "creates a directory")
{
	if (argc < 2) {
		out.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	FRESULT result = ::f_mkdir(dirName);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	return 0;
}

ShellCmd(rm, "removes a file")
{
	if (argc < 2) {
		out.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	FRESULT result = ::f_unlink(fileName);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	return 0;
}

ShellCmd(rmdir, "removes a directory")
{
	if (argc < 2) {
		out.Printf("Usage: %s <directory>\n", argv[0]);
		return 1;
	}
	const char* dirName = argv[1];
	FRESULT result = ::f_rmdir(dirName);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	return 0;
}

ShellCmd(mv, "moves a file")
{
	if (argc < 3) {
		out.Printf("Usage: %s <src> <dst>\n", argv[0]);
		return 1;
	}
	const char* fileNameSrc = argv[1];
	const char* fileNameDst = argv[2];
	FRESULT result = ::f_rename(fileNameSrc, fileNameDst);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	return 0;
}

ShellCmd(touch, "creates an empty file")
{
	if (argc < 2) {
		out.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	FIL fil;
	FRESULT result = ::f_open(&fil, fileName, FA_WRITE | FA_CREATE_ALWAYS);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	::f_close(&fil);
	return 0;
}

ShellCmd(format, "formats the drive")
{
	const char*path = "";
	MKFS_PARM opt;
	opt.fmt = 0;	// FAT12
	opt.n_fat = 1;
	opt.n_root = 0;	// auto
	//opt.n_clst = 0;	// auto
	//opt.n_sect = 0;	// auto
	//opt.n_size = 0;	// auto
	opt.align = 0;	// auto
	opt.au_size = 0;	// auto
	//opt.use_trimming = 0;	// no trimming
	//opt.use_lfn = 1;	// use LFN
	FRESULT result = ::f_mkfs(path, &opt, nullptr, 0);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	return 0;
}

/*
ShellCmd(df, "shows free space on the drive")
{
	DWORD nFreeClusters;
	FRESULT result = ::f_getfree("", &nFreeClusters, &volInfo.fs);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	out.Printf("Free clusters: %lu\n", volInfo.freeClusters);
	out.Printf("Total clusters: %lu\n", volInfo.fs->n_fatent - 2);
	return 0;
}
*/

ShellCmd(cat, "prints the contents of a file")
{
	if (argc < 2) {
		out.Printf("Usage: %s <filename>\n", argv[0]);
		return 1;
	}
	const char* fileName = argv[1];
	RefPtr<FS::File> pFile(FS::OpenFile(fileName, "r"));
	if (!pFile) {
		err.Printf("failed to open %s\n", fileName);
		return 1;
	}
	int bytes;
	char buff[128];
	while ((bytes = pFile->Read(buff, sizeof(buff) - 1)) > 0) {
		buff[bytes] = '\0';
		out.Print(buff);
	}
	pFile->Close();
	return 0;
}

ShellCmd(sync, "syncs the file system")
{
	FRESULT result = ::f_sync(NULL);
	if (result != FR_OK) {
		out.Printf("Error: %s\n", FAT::FRESULTToStr(result));
		return 1;
	}
	return 0;
}

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	SDDrive sdDrive(spi0, 10 * 1000 * 1000, {CS: GPIO5});
	sdDrive.Mount();
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal);
	for (;;) Tickable::Tick();
}
