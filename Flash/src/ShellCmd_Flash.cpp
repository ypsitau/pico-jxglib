//==============================================================================
// ShellCmd_Flash.cpp
//==============================================================================
#include "jxglib/Shell.h"
#include "jxglib/Flash.h"

namespace jxglib {

ShellCmd_Named(flash_erase, "flash-erase", "erases flash memory")
{
	if (argc < 2) {
		tout.Printf("Usage: %s <address> <size>\n", argv[0]);
		return 1;
	}
	const char* strAddr = argv[1];
	const char* strSize = argv[2];
	char* endPtr;
	uint32_t addr = ::strtoul(strAddr, &endPtr, 0);
	if (*endPtr != '\0') {
		tout.Printf("invalid addr: %s\n", strAddr);
		return 1;
	}
	uint32_t size = ::strtoul(strSize, &endPtr, 0);
	if (*endPtr != '\0') {
		tout.Printf("invalid size: %s\n", strSize);
		return 1;
	}
	Flash::Erase(addr & 0x0fff'ffff, size);
	tout.Printf("erased flash at %s for %s bytes\n", strAddr, strSize);
	return 0;
}

}
