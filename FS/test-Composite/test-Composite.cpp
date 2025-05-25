#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/FAT/SDCard.h"
#include "jxglib/FAT/RAMDisk.h"
#include "jxglib/LFS/Flash.h"

using namespace jxglib;

ShellCmd_Named(test_rewind, "test-rewind", "tests rewind of directory")
{
	RefPtr<FS::Dir> pDir(FS::OpenDir("."));
	if (!pDir) {
		tout.Printf("failed to open current directory\n");
		return 1;
	}
	pDir->EnableRewind();
	FS::FileInfo* pFileInfo;
	for (int i = 0; i < 20 && pDir->Read(&pFileInfo); i++) {
		if (pFileInfo->IsDirectory()) {
			tout.Printf("%-20s <DIR>\n", pFileInfo->GetName());
		} else if (pFileInfo->IsFile()) {
			tout.Printf("%-20s %d\n", pFileInfo->GetName(), pFileInfo->GetSize());
		}
	}
	return 0;
}

int main()
{
	::stdio_init_all();
	GPIO2.set_function_SPI0_SCK();
	GPIO3.set_function_SPI0_TX();
	GPIO4.set_function_SPI0_RX();
	LFS::Flash		drive_A("a", 0x1010'0000, 0x0004'0000); 	// Flash address and size 256kB
	LFS::Flash		drive_B("b", 0x1014'0000, 0x0004'0000); 	// Flash address and size 256kB
	FAT::Flash		drive_C("c", 0x1018'0000, 0x0004'0000); 	// Flash address and size 256kB
	FAT::Flash		drive_D("d", 0x101c'0000, 0x0004'0000); 	// Flash address and size 256kB
	FAT::SDCard		drive_E("e", spi0, 10'000'000, {CS: GPIO5});// SDCard on SPI0 10MHz
	FAT::RAMDisk	drive_F("f", 0x2'0000);						// RAM Disk 128kB
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) Tickable::Tick();
}
