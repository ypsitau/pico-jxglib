//==============================================================================
// LABOPlatform.cpp
//==============================================================================
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

//------------------------------------------------------------------------------
// C functions
//------------------------------------------------------------------------------
void jxglib_labo_init()
{
	LABOPlatform::Instance.Initialize();
}

void jxglib_tick()
{
	Tickable::Tick();
}

void jxglib_sleep(int msec)
{
	Tickable::Sleep(msec);
}

//------------------------------------------------------------------------------
// Callback function for the LogicAnalyzer
//------------------------------------------------------------------------------
LogicAnalyzer& ShellCmd_LogicAnalyzer_GetLogicAnalyzer()
{
	return LABOPlatform::Instance.GetLogicAnalyzer();
}

//------------------------------------------------------------------------------
// LABOPlatform
//------------------------------------------------------------------------------
const char* LABOPlatform::textREADME_ = R"(Welcome to pico-jxgLABO!
Type 'help' in the shell to see available commands.
)";

LABOPlatform LABOPlatform::Instance;

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
	fat_("*L:", bytesFlash),
	mscDrive_(deviceController_, 0x01, 0x81),
	streamTerminal_(deviceController_, "StreamSerial", 0x82, 0x03, 0x83),
	streamApplication_(deviceController_, "StreamApplication", 0x84, 0x05, 0x85),
	telePlot_(streamApplication_),
	attachStdioFlag_{false}
{}

void LABOPlatform::Initialize()
{
	deviceController_.Initialize();
	mscDrive_.Initialize(fat_);
	streamTerminal_.Initialize();
	streamApplication_.Initialize();
	if (!attachStdioFlag_) terminal_.AttachKeyboard(streamTerminal_.GetKeyboard()).AttachPrintable(streamTerminal_);
	Shell::AttachTerminal(terminal_.Initialize());
	if (!fat_.Mount()) {
		fat_.Format();
		std::unique_ptr<FS::File> pFile(fat_.OpenFile("/README.txt", "w"));
		if (pFile) pFile->Print(textREADME_);
	}
}
