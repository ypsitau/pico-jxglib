#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBDevice/MSCDrive.h"
#include "jxglib/USBDevice/CDCSerial.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

class LABOPlatform {
private:
	USBDevice::Controller deviceController_;
	FAT::Flash fat_;
	USBDevice::MSCDrive mscDrive_;
	USBDevice::CDCSerial cdcSerial_;
	Serial::Terminal terminal_;
	bool attachStdioFlag_;
private:
	static const char* textREADME_;
public:
	LABOPlatform(int bytesFlash = PICO_FLASH_SIZE_BYTES - 0x010'0000);
public:
	void Initialize();
	USBDevice::Controller& GetDeviceController() { return deviceController_; }
	FAT::Flash& GetFAT() { return fat_; }
	LABOPlatform& AttachStdio() { attachStdioFlag_ = true; return *this; }
};

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

int main(void)
{
	::stdio_init_all();
	LABOPlatform laboPlatform;
	laboPlatform.AttachStdio().Initialize();
	::adc_init();
	::adc_set_temp_sensor_enabled(true);
	for (;;) Tickable::Tick();
}
