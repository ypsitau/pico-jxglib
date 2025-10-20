//==============================================================================
// ShellCmd_Font.cpp
//==============================================================================
#include <stdlib.h>
#include "boot/uf2.h"
#include "jxglib/Shell.h"
#include "jxglib/Font.h"
#include "jxglib/USBDevice/MSCDrive.h"

jxglib::USBDevice::MSCDrive& ShellCmd_Font_GetMSCDrive();

namespace jxglib::ShellCmd_Font {

bool InstallFontFromMSCDrive(Printable& terr);
void PrintInstalledFonts(Printable& tout, Printable& terr);

//-----------------------------------------------------------------------------
// FontInstaller
//-----------------------------------------------------------------------------
class FontInstaller : public USBDevice::MSCDrive::HookHandler {
private:
	bool completeFlag_;
	uint32_t addrTop_;
public:
	FontInstaller(uint32_t addrTop) : completeFlag_{false}, addrTop_{addrTop} {}
public:
	bool IsComplete() const { return completeFlag_; }
public:
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) override;
};

int32_t FontInstaller::On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	uint32_t addrTarget = 0;
	const uint32_t bytesSector = 512;
	for (uint32_t bytesRest = bufsize; bytesRest >= bytesSector; bytesRest -= bytesSector, buffer += bytesSector) {
		uf2_block& block = *reinterpret_cast<uf2_block*>(buffer);
		if (block.magic_start0 != UF2_MAGIC_START0 || block.magic_start1 != UF2_MAGIC_START1 ||
			block.magic_end != UF2_MAGIC_END) continue;
		if (block.block_no == 0) {
			uint32_t bytesWhole = block.num_blocks * 256;
			addrTarget = addrTop_ - bytesWhole;
		}
		if (addrTarget > 0) {
			uint32_t offsetXIP = addrTarget - XIP_BASE;
			Flash::Write(offsetXIP, block.data, block.payload_size);
			::printf("%08x %dbytes %d/%d\n", addrTarget, block.payload_size, block.block_no, block.num_blocks);
			addrTarget += block.payload_size;
		}
		if (block.block_no + 1 == block.num_blocks) {
			// last block
			completeFlag_ = true;
			break;
		}
	}
	Flash::Sync();
	return bufsize;
}

//-----------------------------------------------------------------------------
// font
//-----------------------------------------------------------------------------
ShellCmd(font, "prints information about installed font")
{
	for (int iArg = 1; iArg < argc; ++iArg) {
		const char* subcmd = argv[iArg];
		if (::strcasecmp(subcmd, "install") == 0) {
			InstallFontFromMSCDrive(terr);
		} else {
			terr.Printf("Unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}	
	PrintInstalledFonts(tout, terr);
	return Result::Success;
}

bool InstallFontFromMSCDrive(Printable& terr)
{
	uint32_t addrTop = 0;
	for (int iFont = 0; ; ++iFont) {
		uint32_t rtn = FontSet::GetInstanceAddrTop(iFont);
		if (rtn == 0) break;
		addrTop = rtn;
	}
	if (addrTop == 0) addrTop = XIP_BASE + FontSet::bytesProgramMax;
	FontInstaller fontInstaller(addrTop);
	USBDevice::MSCDrive& mscDrive = ShellCmd_Font_GetMSCDrive();
	mscDrive.SetHookHandler(fontInstaller);
	terr.Printf("Please copy a font UF2 file to MSC drive\n");
	while (!fontInstaller.IsComplete()) {
		if (Tickable::TickSub()) {
			mscDrive.ClearHookHandler();
			return false;
		}
	}
	mscDrive.ClearHookHandler();
	terr.Printf("Font installation via MSC completed.\n");
	return true;
}

void PrintInstalledFonts(Printable& tout, Printable& terr)
{
	for (int iFont = 0; ; ++iFont) {
		const FontSet& fontSet = FontSet::GetInstance(iFont);
		if (fontSet.IsNone()) {
			if (iFont == 0) terr.Printf("no font installed\n");
			break;
		}
		tout.Printf("%s y-advance:%d basic-chars:%d extra-chars:%d\n",
			fontSet.name, fontSet.yAdvance, fontSet.nFontEntries_Basic, fontSet.nFontEntries_Extra);
	}
}

}
