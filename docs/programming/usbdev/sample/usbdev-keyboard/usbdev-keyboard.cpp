#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBDevice/HID.h"

using namespace jxglib;

auto& GPIO_ARROW_LEFT	= GPIO16;
auto& GPIO_ARROW_UP		= GPIO17;
auto& GPIO_ARROW_DOWN	= GPIO18;
auto& GPIO_ARROW_RIGHT	= GPIO19;

class MyKeyboard : public USBDevice::Keyboard {
private:
    int nKeycodePrev_;
public:
    MyKeyboard(USBDevice::Controller& deviceController);
public:
    virtual void OnTick() override;
};

int main(void)
{
    ::stdio_init_all(); 
    USBDevice::Controller deviceController({
            bcdUSB:				0x0200,
            bDeviceClass:		0x00,
            bDeviceSubClass:	0x00,
            bDeviceProtocol:	0x00,
            bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
            idVendor:			0xcafe,
            idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
            bcdDevice:			0x0100,
        },
        0x0409,                             // Language ID (English - United States)
        "pico-jxglib sample",               // Manufacturer
        "My Keyboard",                      // Product
        "0123456789ABCDEF",                 // Serial Number
        TUSB_DESC_CONFIG_ATT_REMOTE_WAKEUP);// Configuration Attributes
    MyKeyboard myKeyboard(deviceController);
    deviceController.Initialize();
    myKeyboard.Initialize();
    GPIO_ARROW_LEFT		.init().set_dir_IN().pull_up();
    GPIO_ARROW_UP		.init().set_dir_IN().pull_up();
    GPIO_ARROW_DOWN		.init().set_dir_IN().pull_up();
    GPIO_ARROW_RIGHT	.init().set_dir_IN().pull_up();
    for (;;) {
        Tickable::Tick();
    }
}

MyKeyboard::MyKeyboard(USBDevice::Controller& deviceController) :
    USBDevice::Keyboard(deviceController, "RaspberryPi Pico Keyboard Interface", 0x81),
    nKeycodePrev_{0}
{}

void MyKeyboard::OnTick()
{
    uint8_t reportId = 0;
    uint8_t modifier  = 0;
    uint8_t usageIdTbl[6] = { 0 };
    int nUsageId = 0;
    if (!GPIO_ARROW_LEFT.get())		usageIdTbl[nUsageId++] = HID_KEY_ARROW_LEFT;
    if (!GPIO_ARROW_UP.get())		usageIdTbl[nUsageId++] = HID_KEY_ARROW_UP;
    if (!GPIO_ARROW_DOWN.get())		usageIdTbl[nUsageId++] = HID_KEY_ARROW_DOWN;
    if (!GPIO_ARROW_RIGHT.get())	usageIdTbl[nUsageId++] = HID_KEY_ARROW_RIGHT;
    if (::tud_suspended()) {
        // Wake up host if we are in suspend mode and REMOTE_WAKEUP feature is enabled by host
        if (nUsageId > 0) ::tud_remote_wakeup();
    } else if (!hid_ready()) {
        // do nothing
    } else if (nUsageId > 0) {
        // Send key report
        hid_keyboard_report(reportId, modifier, usageIdTbl);
    } else if (nKeycodePrev_ > 0) {
        // Send empty key report if previously we sent a key report with keycode, to indicate key release
        hid_keyboard_report(reportId, modifier, nullptr);
    }
    nKeycodePrev_ = nUsageId;
}
