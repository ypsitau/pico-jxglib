//==============================================================================
// ShellCmd_Font.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/Shell.h"
#include "jxglib/Font.h"
#include "UF2Installer.h"


jxglib::USBDevice::MSCDrive& ShellCmd_Font_GetMSCDrive();

namespace jxglib::ShellCmd_Font {

const FontSet& InstallFontFromMSCDrive(Printable& terr, uint32_t* pAddr, uint32_t* pBytes);
bool UninstallAllFonts(Printable& terr);
void PrintFontSet(Printable& tout, const FontSet& fontSet, uint32_t flashAddr, uint32_t flashBytes);
void PrintInstalledFonts(Printable& tout, Printable& terr);

//-----------------------------------------------------------------------------
// font
//-----------------------------------------------------------------------------
ShellCmd(font, "utility to manage fonts: install, uninstall, and list installed fonts")
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
		terr.Printf(" install        install font via MSC drive\n");
		terr.Printf(" uninstall-all  uninstall all installed fonts\n");
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
		} else if (::strcasecmp(subcmd, "uninstall-all") == 0) {
			UninstallAllFonts(terr);
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
	mscDrive.NotifyContentChanged();
	while (!uf2Installer.IsComplete()) {
		if (Tickable::TickSub()) {
			mscDrive.ClearHookHandler();
			mscDrive.NotifyContentChanged();
			return FontSet::None;
		}
	}
	Tickable::Sleep(500); // ignore remaining write10 requests
	mscDrive.ClearHookHandler();
	mscDrive.NotifyContentChanged();
	if (pAddr) *pAddr = uf2Installer.GetFlashAddrTop();
	if (pBytes) *pBytes = uf2Installer.GetFlashBytes();
	return *reinterpret_cast<const FontSet*>(uf2Installer.GetFlashAddrTop() + 8);
}

bool UninstallAllFonts(Printable& terr)
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
	terr.Printf("All fonts have been uninstalled.\n");
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
