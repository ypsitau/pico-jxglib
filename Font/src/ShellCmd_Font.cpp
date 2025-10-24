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

const FontSet& InstallFontFromMSCDrive(Printable& terr, uint32_t* pAddr, uint32_t* pBytes);
bool UninstallFont(Printable& terr);
void PrintFontSet(Printable& tout, const FontSet& fontSet, uint32_t flashAddr, uint32_t flashBytes);
void PrintInstalledFonts(Printable& tout, Printable& terr);

//-----------------------------------------------------------------------------
// UF2Installer
//-----------------------------------------------------------------------------
class UF2Installer : public USBDevice::MSCDrive::HookHandler {
private:
	Printable& terr_;
	bool completeFlag_;
	uint32_t flashBytes_;
	uint32_t flashAddrTop_;
	uint32_t flashAddrBtm_;
	uint32_t flashAddrTarget_;
public:
	UF2Installer(Printable& terr, uint32_t flashAddrTop) : terr_{terr}, completeFlag_{false},
		flashBytes_{0}, flashAddrTop_{flashAddrTop}, flashAddrBtm_{flashAddrTop}, flashAddrTarget_{0} {}
public:
	uint32_t GetFlashBytes() const { return flashBytes_; }
	uint32_t GetFlashAddrTop() const { return flashAddrTop_; }
	bool IsComplete() const { return completeFlag_; }
public:
	virtual void On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size) override;
	virtual int32_t On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize) override;
};

void UF2Installer::On_msc_capacity(uint8_t lun, uint32_t* block_count, uint16_t* block_size)
{
	*block_count = (flashAddrBtm_ - XIP_BASE) / USBDevice::MSCDrive::BlockSize;
	*block_size  = USBDevice::MSCDrive::BlockSize;
}

int32_t UF2Installer::On_msc_write10(uint8_t lun, uint32_t lba, uint32_t offset, uint8_t* buffer, uint32_t bufsize)
{
	const uint32_t bytesSector = 512;
	for (uint32_t bytesRest = bufsize; bytesRest >= bytesSector; bytesRest -= bytesSector, buffer += bytesSector) {
		uf2_block& block = *reinterpret_cast<uf2_block*>(buffer);
		if (block.magic_start0 != UF2_MAGIC_START0 || block.magic_start1 != UF2_MAGIC_START1 ||
			block.magic_end != UF2_MAGIC_END) continue;
		if (block.block_no == 0) {
			flashBytes_ = block.num_blocks * 256;
			flashAddrTop_ = flashAddrBtm_ - flashBytes_;
			flashAddrTarget_ = flashAddrTop_;
		}
		if (flashAddrTarget_ > 0) {
			uint32_t offsetXIP = flashAddrTarget_ - XIP_BASE;
			Flash::Write(offsetXIP, block.data, block.payload_size);
			terr_.Printf("%3d/%3d 0x%08x\r", block.block_no + 1, block.num_blocks, flashAddrTarget_);
			flashAddrTarget_ += block.payload_size;
		}
		if (block.block_no + 1 == block.num_blocks) {
			// last block
			terr_.Printf("                  \r");
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
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... CMDS...\n", GetName());
		arg.PrintHelp(terr);
		terr.Printf("Sub Commands:\n");
		terr.Printf(" install    install font via MSC drive\n");
		terr.Printf(" uninstall  uninstall all installed fonts\n");
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	bool printInstalledFontsFlag = true;
	for (int iArg = 1; iArg < argc; ++iArg) {
		const char* subcmd = argv[iArg];
		if (::strcasecmp(subcmd, "install") == 0) {
			terr.Printf("Please copy a font UF2 file to MSC drive\n");
			uint32_t flashAddr, flashBytes;
			const FontSet& fontSet = InstallFontFromMSCDrive(terr, &flashAddr, &flashBytes);
			if (!fontSet.IsNone()) PrintFontSet(terr, fontSet, flashAddr, flashBytes);
			printInstalledFontsFlag = false;
		} else if (::strcasecmp(subcmd, "uninstall") == 0) {
			UninstallFont(terr);
			printInstalledFontsFlag = false;
		} else {
			terr.Printf("Unknown sub command: %s\n", subcmd);
			return Result::Error;
		}
	}	
	if (printInstalledFontsFlag) PrintInstalledFonts(tout, terr);
	return Result::Success;
}

const FontSet& InstallFontFromMSCDrive(Printable& terr, uint32_t* pAddr, uint32_t* pBytes)
{
	uint32_t flashAddrTop = 0;
	for (int iFont = 0; ; ++iFont) {
		uint32_t flashAddr, flashBytes;
		if (!FontSet::GetInstanceRange(iFont, &flashAddr, &flashBytes)) break;
		flashAddrTop = flashAddr;
	}
	if (flashAddrTop == 0) flashAddrTop = FontSet::flashAddrBtm;
	UF2Installer uf2Installer(terr, flashAddrTop);
	USBDevice::MSCDrive& mscDrive = ShellCmd_Font_GetMSCDrive();
	mscDrive.SetHookHandler(uf2Installer);
	while (!uf2Installer.IsComplete()) {
		if (Tickable::TickSub()) {
			mscDrive.ClearHookHandler();
			return FontSet::None;
		}
	}
	Tickable::Sleep(500); // ignore remaining write10 requests
	mscDrive.ClearHookHandler();
	if (pAddr) *pAddr = uf2Installer.GetFlashAddrTop();
	if (pBytes) *pBytes = uf2Installer.GetFlashBytes();
	return *reinterpret_cast<const FontSet*>(uf2Installer.GetFlashAddrTop() + 8);
}

bool UninstallFont(Printable& terr)
{
	for (int iFont = 0; ; ++iFont) {
		uint32_t flashAddr, flashBytes;
		if (!FontSet::GetInstanceRange(iFont, &flashAddr, &flashBytes)) break;
		uint32_t offsetXIP = flashAddr - XIP_BASE;
		uint8_t blank[256];
		::memset(blank, 0xff, sizeof(blank));
		for (uint32_t offset = 0; offset < flashBytes; offset += sizeof(blank)) {
			uint32_t bytesToWrite = ChooseMin(sizeof(blank), flashBytes - offset);
			Flash::Write(offsetXIP + offset, blank, bytesToWrite);
		}
	}
	Flash::Sync();
	terr.Printf("Font uninstallation completed.\n");
	return true;
}

void PrintFontSet(Printable& tout, const FontSet& fontSet, uint32_t flashAddr, uint32_t flashBytes)
{
	tout.Printf("%-16s y-advance:%d basic-chars:%d extra-chars:%d %d bytes\n",
		fontSet.name, fontSet.yAdvance, fontSet.nFontEntries_Basic, fontSet.nFontEntries_Extra, flashBytes);
}

void PrintInstalledFonts(Printable& tout, Printable& terr)
{
	uint32_t flashBytesSum = 0;
	for (int iFont = 0; ; ++iFont) {
		uint32_t flashAddr, flashBytes;
		const FontSet& fontSet = FontSet::GetInstance(iFont, &flashAddr, &flashBytes);
		if (fontSet.IsNone()) {
			if (iFont == 0) terr.Printf("no font installed\n");
			break;
		}
		tout.Printf("%d: ", iFont);
		PrintFontSet(tout, fontSet, flashAddr, flashBytes);
		flashBytesSum += flashBytes;
	}
	terr.Printf("Total flash usage: %d bytes\n", flashBytesSum);
}

}
