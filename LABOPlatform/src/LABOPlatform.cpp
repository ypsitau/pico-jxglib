//==============================================================================
// LABOPlatform.cpp
//==============================================================================
#include "jxglib/LABOPlatform.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LABOPlatform
//------------------------------------------------------------------------------
const char* LABOPlatform::textREADME_ = R"(Welcome to pico-jxgLABO!
Type 'help' in the shell to see available commands.
)";

LABOPlatform::LABOPlatform(int bytesFlash) :
	deviceController_({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			0x1ab0,
		bcdDevice:			0x0100,
	}, 0x0409, "jxglib", "pico-jxgLABO", "000000000000"),
	fat_("*G:", bytesFlash),
	mscDrive_(deviceController_, 0x01, 0x81),
	cdcSerial_(deviceController_, "CDCSerial", 0x82, 0x03, 0x83),
	attachStdioFlag_{false}
{}

void LABOPlatform::Initialize()
{
	deviceController_.Initialize();
	mscDrive_.Initialize(fat_);
	cdcSerial_.Initialize();
	if (!attachStdioFlag_) terminal_.AttachKeyboard(cdcSerial_.GetKeyboard()).AttachPrintable(cdcSerial_);
	Shell::AttachTerminal(terminal_.Initialize());
	if (!fat_.Mount()) {
		fat_.Format();
		std::unique_ptr<FS::File> pFile(fat_.OpenFile("/README.txt", "w"));
		if (pFile) pFile->Print(textREADME_);
	}
}

}
