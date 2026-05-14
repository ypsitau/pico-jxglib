#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBDevice/MSCDrive.h"
#include "jxglib/FAT/Flash.h"

using namespace jxglib;

int main(void)
{
    ::stdio_init_all(); 
    // Prepare the shell
    Serial::Terminal terminal;
    Shell::AttachTerminal(terminal.Initialize());
    // Prepare the USB Device Descriptor and ID strings
    USBDevice::Controller deviceController({
            bcdUSB:				0x0200,
            bDeviceClass:		0x00,		// Use Interface Class (Composite Device)
            bDeviceSubClass:	0x00,		// Unused
            bDeviceProtocol:	0x00,		// Unused
            bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
            idVendor:			0xcafe,
            idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
            bcdDevice:			0x0100,
        },
        0x0409,					// Language: English (United States)
        "pico-jxglib sample",	// Manufacturer
        "RPi Flash Device",		// Product
        "3141592653"			// Serial Number (must be unique for each device)
    );
    // Prepare the FAT Drive
    FAT::Flash fat("*Flash:", 0x010'0000);
    // Prepare the MSC Drive with the addresses for the Bulk OUT and Bulk IN endpoints
    USBDevice::MSCDrive mscDrive(deviceController, 0x01, 0x81);
    // Initialize the USB Device and the MSC Drive
    deviceController.Initialize();
    mscDrive.Initialize(fat);
    for (;;) {
        Tickable::Tick();
    }
}
