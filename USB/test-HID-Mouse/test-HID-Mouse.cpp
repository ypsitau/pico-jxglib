#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "jxglib/GPIO.h"
#include "jxglib/USBD.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// Mouse
//-----------------------------------------------------------------------------
class Mouse : public USBD::Mouse {
public:
	Mouse(USBD::Device& device) : USBD::Mouse(device, "RaspberryPi Pico Mouse", 0x81) {}
public:
	virtual void OnTask() override;
};

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBD::Device device({
		bcdUSB:				0x0200,
		bDeviceClass:		0x00,
		bDeviceSubClass:	0x00,
		bDeviceProtocol:	0x00,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBD::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "RaspberryPi Pico HMI", "RaspberryPi Pico HMI Device", "0123456789ABCDEF");
	Mouse mouse(device);
	device.Initialize();
	GPIO16.init().set_dir_IN().pull_up();
	GPIO17.init().set_dir_IN().pull_up();
	GPIO18.init().set_dir_IN().pull_up();
	GPIO19.init().set_dir_IN().pull_up();
	GPIO20.init().set_dir_IN().pull_up();
	GPIO21.init().set_dir_IN().pull_up();
	for (;;) {
		device.Task();
	}
	return 0;
}

//-----------------------------------------------------------------------------
// Mouse
//-----------------------------------------------------------------------------
void Mouse::OnTask()
{
	if (!hid_ready()) return;
	bool btnB = !GPIO21.get();
	if (btnB) {
		uint8_t report_id = 0;
		uint8_t button_mask = 0;
		int8_t x = 5;
		int8_t y = 5;
		int8_t vertical = 0;
		int8_t horizontal = 0;
		hid_mouse_report(report_id, button_mask, x, y, vertical, horizontal);
	}
}
