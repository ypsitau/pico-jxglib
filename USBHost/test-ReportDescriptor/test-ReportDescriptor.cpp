#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Stdio.h"
#include "jxglib/USBHost.h"

using namespace jxglib;
/*
C 言語で、ジョイスティックの USB report descriptor と、それをテストする report データのサンプルを 10 個つくってください。
- reort descritor の変数名は descReort, report データサンプルの変数名は reportTbl とすること。それぞれ static 宣言すること
- コメントを英語で表記すること
*/

USBHost::HIDDriver* PrepareHIDDriver(const uint8_t* descReport, int descLen);

#if 1
void test_Keyboard()
{
	// USB HID Report Descriptor for a keyboard
	static const uint8_t descReport[] = {
		0x05, 0x01, // Usage Page (Generic Desktop)
		0x09, 0x06, // Usage (Keyboard)
		0xA1, 0x01, // Collection (Application)
		0x05, 0x07, // Usage Page (Key Codes)
		0x19, 0xE0, // Usage Minimum (224)
		0x29, 0xE7, // Usage Maximum (231)
		0x15, 0x00, // Logical Minimum (0)
		0x25, 0x01, // Logical Maximum (1)
		0x75, 0x01, // Report Size (1)
		0x95, 0x08, // Report Count (8)
		0x81, 0x02, // Input (Data, Variable, Absolute) - Modifier keys
		0x95, 0x01, // Report Count (1)
		0x75, 0x08, // Report Size (8)
		0x81, 0x03, // Input (Constant) - Reserved byte
		0x95, 0x06, // Report Count (6)
		0x75, 0x08, // Report Size (8)
		0x15, 0x00, // Logical Minimum (0)
		0x25, 0x65, // Logical Maximum (101)
		0x05, 0x07, // Usage Page (Key Codes)
		0x19, 0x00, // Usage Minimum (0)
		0x29, 0x65, // Usage Maximum (101)
		0x81, 0x00, // Input (Data, Array) - Key array
		0xC0        // End Collection
	};
	// Sample report data for testing (10 samples)
	static const uint8_t reportTbl[10][8] = {
		{0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'a'
		{0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'b'
		{0x02, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'c'
		{0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'd'
		{0x00, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'e'
		{0x02, 0x00, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'f'
		{0x00, 0x00, 0x0A, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'g'
		{0x00, 0x00, 0x0B, 0x00, 0x00, 0x00, 0x00, 0x00}, // 'h'
		{0x02, 0x00, 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00}, // Left Shift + 'i'
		{0x00, 0x00, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00}  // 'j'
	};
	::printf("\n" "test_Keyboard\n");
	std::unique_ptr<USBHost::HIDDriver> pHIDDriver(PrepareHIDDriver(descReport, sizeof(descReport)));
	for (int i = 0; i < count_of(reportTbl); i++) {
		pHIDDriver->GetHID().OnReport(reportTbl[i], sizeof(reportTbl[0])); 
		::printf("Modifier:%d%d%d%d%d%d%d%d %02x %02x %02x %02x %02x %02x\n",
			pHIDDriver->GetVariable(0x0007'00e0),
			pHIDDriver->GetVariable(0x0007'00e1),
			pHIDDriver->GetVariable(0x0007'00e2),
			pHIDDriver->GetVariable(0x0007'00e3),
			pHIDDriver->GetVariable(0x0007'00e4),
			pHIDDriver->GetVariable(0x0007'00e5),
			pHIDDriver->GetVariable(0x0007'00e6),
			pHIDDriver->GetVariable(0x0007'00e7),
			pHIDDriver->GetArrayItem(0), pHIDDriver->GetArrayItem(1), pHIDDriver->GetArrayItem(2),
			pHIDDriver->GetArrayItem(3), pHIDDriver->GetArrayItem(4), pHIDDriver->GetArrayItem(5));
	}
}

void test_Mouse()
{
	// USB Mouse Report Descriptor
	static uint8_t descReport[] = {
		0x05, 0x01, // Usage Page (Generic Desktop)
		0x09, 0x02, // Usage (Mouse)
		0xA1, 0x01, // Collection (Application)
		0x09, 0x01, //   Usage (Pointer)
		0xA1, 0x00, //   Collection (Physical)
		0x05, 0x09, //     Usage Page (Button)
		0x19, 0x01, //     Usage Minimum (Button 1)
		0x29, 0x03, //     Usage Maximum (Button 3)
		0x15, 0x00, //     Logical Minimum (0)
		0x25, 0x01, //     Logical Maximum (1)
		0x95, 0x03, //     Report Count (3)
		0x75, 0x01, //     Report Size (1)
		0x81, 0x02, //     Input (Data, Variable, Absolute)
		0x95, 0x01, //     Report Count (1)
		0x75, 0x05, //     Report Size (5)
		0x81, 0x03, //     Input (Constant, Variable, Absolute)
		0x05, 0x01, //     Usage Page (Generic Desktop)
		0x09, 0x30, //     Usage (X)
		0x09, 0x31, //     Usage (Y)
		0x15, 0x81, //     Logical Minimum (-127)
		0x25, 0x7F, //     Logical Maximum (127)
		0x75, 0x08, //     Report Size (8)
		0x95, 0x02, //     Report Count (2)
		0x81, 0x06, //     Input (Data, Variable, Relative)
		0xC0,       //   End Collection
		0xC0        // End Collection
	};
	// Sample report data for testing
	static uint8_t reportTbl[10][3] = {
		{0x00, 0x00, 0x00}, // No buttons pressed, no movement
		{0x01, 0x10, 0x10}, // Button 1 pressed, move diagonally
		{0x02, 0xFF, 0x00}, // Button 2 pressed, move left
		{0x04, 0x00, 0xFF}, // Button 3 pressed, move down
		{0x03, 0x7F, 0x7F}, // Button 1 and 2 pressed, move to bottom-right
		{0x00, 0x81, 0x81}, // No buttons pressed, move to top-left
		{0x01, 0x00, 0x7F}, // Button 1 pressed, move up
		{0x02, 0x7F, 0x00}, // Button 2 pressed, move right
		{0x04, 0x7F, 0x81}, // Button 3 pressed, move diagonally
		{0x00, 0x00, 0x00}  // No buttons pressed, no movement
	};
	::printf("\n" "test_Mouse\n");
	std::unique_ptr<USBHost::HIDDriver> pHIDDriver(PrepareHIDDriver(descReport, sizeof(descReport)));
	for (int i = 0; i < count_of(reportTbl); i++) {
		pHIDDriver->GetHID().OnReport(reportTbl[i], sizeof(reportTbl[0])); 
		::printf("Button1:%d Button2:%d Button3:%d X:%d Y:%d\n",
			pHIDDriver->GetVariable(0x0001'0001, 0x0009'0001),
			pHIDDriver->GetVariable(0x0001'0001, 0x0009'0002),
			pHIDDriver->GetVariable(0x0001'0001, 0x0009'0003),
			pHIDDriver->GetVariable(0x0001'0001, 0x0001'0030),
			pHIDDriver->GetVariable(0x0001'0001, 0x0001'0031));
	}
}

void test_GamePad1()
{
	// USB report descriptor for a generic gamepad
	static const uint8_t descReport[] = {
		0x05, 0x01,        // Usage Page (Generic Desktop)
		0x09, 0x05,        // Usage (Gamepad)
		0xA1, 0x01,        // Collection (Application)
		0x05, 0x09,        // Usage Page (Button)
		0x19, 0x01,        // Usage Minimum (Button 1)
		0x29, 0x10,        // Usage Maximum (Button 16)
		0x15, 0x00,        // Logical Minimum (0)
		0x25, 0x01,        // Logical Maximum (1)
		0x95, 0x10,        // Report Count (16)
		0x75, 0x01,        // Report Size (1)
		0x81, 0x02,        // Input (Data, Variable, Absolute)
		0x05, 0x01,        // Usage Page (Generic Desktop)
		0x09, 0x30,        // Usage (X)
		0x09, 0x31,        // Usage (Y)
		0x09, 0x32,        // Usage (Z)
		0x09, 0x35,        // Usage (Rz)
		0x15, 0x81,        // Logical Minimum (-127)
		0x25, 0x7F,        // Logical Maximum (127)
		0x75, 0x08,        // Report Size (8)
		0x95, 0x04,        // Report Count (4)
		0x81, 0x02,        // Input (Data, Variable, Absolute)
		0xC0               // End Collection
	};
	// Sample report data for the gamepad
	static const uint8_t reportTbl[10][6] = {
		{0x00, 0x00, 0x00, 0x00, 0x00, 0x00}, // All buttons released, all axes centered
		{0x01, 0x00, 0x7F, 0x7F, 0x7F, 0x7F}, // Button 1 pressed, all axes maxed
		{0x02, 0x00, 0x81, 0x81, 0x81, 0x81}, // Button 2 pressed, all axes minned
		{0x04, 0x00, 0x7F, 0x00, 0x7F, 0x00}, // Button 3 pressed, alternating axis values
		{0x08, 0x00, 0x00, 0x7F, 0x00, 0x7F}, // Button 4 pressed, alternating axis values
		{0x10, 0x00, 0x40, 0x40, 0x40, 0x40}, // Button 5 pressed, all axes at mid-low
		{0x20, 0x00, 0xC0, 0xC0, 0xC0, 0xC0}, // Button 6 pressed, all axes at mid-high
		{0x40, 0x00, 0x81, 0x7F, 0x81, 0x7F}, // Button 7 pressed, alternating max/min
		{0x80, 0x00, 0x7F, 0x81, 0x7F, 0x81}, // Button 8 pressed, alternating min/max
		{0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00}  // All buttons pressed, all axes centered
	};
	::printf("\n" "test_GamePad1\n");
	std::unique_ptr<USBHost::HIDDriver> pHIDDriver(PrepareHIDDriver(descReport, sizeof(descReport)));
	for (int i = 0; i < count_of(reportTbl); i++) {
		pHIDDriver->GetHID().OnReport(reportTbl[i], sizeof(reportTbl[0])); 
		::printf("Button:%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d%d LStick:%4d:%4d RStick:%4d:%4d\n",
			pHIDDriver->GetVariable(0x0009'0001),
			pHIDDriver->GetVariable(0x0009'0002),
			pHIDDriver->GetVariable(0x0009'0003),
			pHIDDriver->GetVariable(0x0009'0004),
			pHIDDriver->GetVariable(0x0009'0005),
			pHIDDriver->GetVariable(0x0009'0006),
			pHIDDriver->GetVariable(0x0009'0007),
			pHIDDriver->GetVariable(0x0009'0008),
			pHIDDriver->GetVariable(0x0009'0009),
			pHIDDriver->GetVariable(0x0009'000a),
			pHIDDriver->GetVariable(0x0009'000b),
			pHIDDriver->GetVariable(0x0009'000c),
			pHIDDriver->GetVariable(0x0009'000d),
			pHIDDriver->GetVariable(0x0009'000e),
			pHIDDriver->GetVariable(0x0009'000f),
			pHIDDriver->GetVariable(0x0009'0010),
			pHIDDriver->GetVariable(0x0001'0030),
			pHIDDriver->GetVariable(0x0001'0031),
			pHIDDriver->GetVariable(0x0001'0032),
			pHIDDriver->GetVariable(0x0001'0035));
	}
}

void test_GamePad2()
{
	// USB report descriptor for a gamepad
	static const uint8_t descReport[] = {
		0x05, 0x01,        // Usage Page (Generic Desktop)
		0x09, 0x05,        // Usage (Gamepad)
		0xA1, 0x01,        // Collection (Application)
		0x05, 0x01,        //   Usage Page (Generic Desktop)
		0x09, 0x30,        //   Usage (X)
		0x09, 0x31,        //   Usage (Y)
		0x09, 0x32,        //   Usage (Z)
		0x09, 0x33,        //   Usage (Rx)
		0x15, 0x00,        //   Logical Minimum (0)
		0x26, 0xFF, 0x0F,  //   Logical Maximum (4095)
		0x75, 0x0C,        //   Report Size (12)
		0x95, 0x04,        //   Report Count (4)
		0x81, 0x02,        //   Input (Data, Var, Abs)
		0x05, 0x09,        //   Usage Page (Button)
		0x19, 0x01,        //   Usage Minimum (Button 1)
		0x29, 0x08,        //   Usage Maximum (Button 8)
		0x15, 0x00,        //   Logical Minimum (0)
		0x25, 0x01,        //   Logical Maximum (1)
		0x75, 0x01,        //   Report Size (1)
		0x95, 0x08,        //   Report Count (8)
		0x81, 0x02,        //   Input (Data, Var, Abs)
		0xC0               // End Collection
	};
	// Sample report data (10 entries)
	// Each report contains 4 axes (12 bits each) and 8 buttons (1 bit each)
	static const uint8_t reportTbl[10][8] = {
		// Axes: [X, Y, Z, Rx], Buttons: [B1-B8]
		{0x00, 0x10, 0x00, 0x20, 0x00, 0x30, 0x00, 0x01}, // Example 1
		{0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x0F, 0xFF, 0x00}, // Example 2
		{0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0xFF}, // Example 3
		{0x40, 0x08, 0x40, 0x08, 0x40, 0x08, 0x40, 0x55}, // Example 4
		{0x20, 0x04, 0x20, 0x04, 0x20, 0x04, 0x20, 0xAA}, // Example 5
		{0x10, 0x02, 0x10, 0x02, 0x10, 0x02, 0x10, 0xF0}, // Example 6
		{0x08, 0x01, 0x08, 0x01, 0x08, 0x01, 0x08, 0x0F}, // Example 7
		{0x04, 0x00, 0x04, 0x00, 0x04, 0x00, 0x04, 0x3C}, // Example 8
		{0x02, 0x00, 0x02, 0x00, 0x02, 0x00, 0x02, 0xC3}, // Example 9
		{0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x5A}  // Example 10
	};
	::printf("\n" "test_GamePad2\n");
	std::unique_ptr<USBHost::HIDDriver> pHIDDriver(PrepareHIDDriver(descReport, sizeof(descReport)));
	for (int i = 0; i < count_of(reportTbl); i++) {
		pHIDDriver->GetHID().OnReport(reportTbl[i], sizeof(reportTbl[0])); 
		::printf("Button:%d%d%d%d%d%d%d%d LStick:%5d:%5d RStick:%5d:%5d\n",
			pHIDDriver->GetVariable(0x0009'0001),
			pHIDDriver->GetVariable(0x0009'0002),
			pHIDDriver->GetVariable(0x0009'0003),
			pHIDDriver->GetVariable(0x0009'0004),
			pHIDDriver->GetVariable(0x0009'0005),
			pHIDDriver->GetVariable(0x0009'0006),
			pHIDDriver->GetVariable(0x0009'0007),
			pHIDDriver->GetVariable(0x0009'0008),
			pHIDDriver->GetVariable(0x0001'0030),
			pHIDDriver->GetVariable(0x0001'0031),
			pHIDDriver->GetVariable(0x0001'0032),
			pHIDDriver->GetVariable(0x0001'0033));
	}
}

void test_Joystick()
{
	// USB report descriptor for a joystick
	static const uint8_t descReport[] = {
		0x05, 0x01,        // Usage Page (Generic Desktop)
		0x09, 0x04,        // Usage (Joystick)
		0xA1, 0x01,        // Collection (Application)
		0x05, 0x02,        //   Usage Page (Simulation Controls)
		0x09, 0xBB,        //   Usage (Throttle)
		0x15, 0x00,        //   Logical Minimum (0)
		0x26, 0xFF, 0x00,  //   Logical Maximum (255)
		0x75, 0x08,        //   Report Size (8)
		0x95, 0x01,        //   Report Count (1)
		0x81, 0x02,        //   Input (Data, Var, Abs)
		0x05, 0x01,        //   Usage Page (Generic Desktop)
		0x09, 0x01,        //   Usage (Pointer)
		0xA1, 0x00,        //   Collection (Physical)
		0x09, 0x30,        //     Usage (X)
		0x09, 0x31,        //     Usage (Y)
		0x15, 0x81,        //     Logical Minimum (-127)
		0x25, 0x7F,        //     Logical Maximum (127)
		0x75, 0x08,        //     Report Size (8)
		0x95, 0x02,        //     Report Count (2)
		0x81, 0x02,        //     Input (Data, Var, Abs)
		0xC0,              //   End Collection
		0x05, 0x09,        //   Usage Page (Button)
		0x19, 0x01,        //   Usage Minimum (Button 1)
		0x29, 0x08,        //   Usage Maximum (Button 8)
		0x15, 0x00,        //   Logical Minimum (0)
		0x25, 0x01,        //   Logical Maximum (1)
		0x75, 0x01,        //   Report Size (1)
		0x95, 0x08,        //   Report Count (8)
		0x81, 0x02,        //   Input (Data, Var, Abs)
		0xC0               // End Collection
	};
	// Sample report data for testing the joystick
	static const uint8_t reportTbl[10][4] = {
		{0x10, 0x00, 0x00, 0x00}, // Neutral position, no buttons pressed
		{0x20, 0x7F, 0x00, 0x01}, // Max X, neutral Y, Button 1 pressed
		{0x30, 0x00, 0x7F, 0x02}, // Neutral X, max Y, Button 2 pressed
		{0x40, 0x81, 0x00, 0x04}, // Min X, neutral Y, Button 3 pressed
		{0x50, 0x00, 0x81, 0x08}, // Neutral X, min Y, Button 4 pressed
		{0x60, 0x7F, 0x7F, 0x10}, // Max X, max Y, Button 5 pressed
		{0x70, 0x81, 0x81, 0x20}, // Min X, min Y, Button 6 pressed
		{0x80, 0x40, 0x40, 0x40}, // Mid X, mid Y, Button 7 pressed
		{0x90, 0x00, 0x00, 0x80}, // Neutral X, neutral Y, Button 8 pressed
		{0xa0, 0x7F, 0x81, 0x00}  // Max X, min Y, no buttons pressed
	};
	::printf("\n" "test_Joystick\n");
	std::unique_ptr<USBHost::HIDDriver> pHIDDriver(PrepareHIDDriver(descReport, sizeof(descReport)));
	for (int i = 0; i < count_of(reportTbl); i++) {
		pHIDDriver->GetHID().OnReport(reportTbl[i], sizeof(reportTbl[0])); 
		::printf("Button:%d%d%d%d%d%d%d%d Throttle:%3d X:%3d Y:%3d\n",
			pHIDDriver->GetVariable(0x0009'0001),
			pHIDDriver->GetVariable(0x0009'0002),
			pHIDDriver->GetVariable(0x0009'0003),
			pHIDDriver->GetVariable(0x0009'0004),
			pHIDDriver->GetVariable(0x0009'0005),
			pHIDDriver->GetVariable(0x0009'0006),
			pHIDDriver->GetVariable(0x0009'0007),
			pHIDDriver->GetVariable(0x0009'0008),
			pHIDDriver->GetVariable(0x0002'00bb),
			pHIDDriver->GetVariable(0x0001'0001, 0x0001'0030),
			pHIDDriver->GetVariable(0x0001'0001, 0x0001'0031));
	}
}
#endif

USBHost::HIDDriver* PrepareHIDDriver(const uint8_t* descReport, int descLen)
{
	auto pApplication = USBHost::ReportDescriptor().Parse(descReport, descLen);
	if (!pApplication) {
		::panic("error while parsing report descriptor\n");
	}
	pApplication->Print(Stdio::Instance);
	auto pHIDDriver = new USBHost::HIDDriver(0);
	pHIDDriver->AttachHID(new USBHost::HID(0, 0, pApplication), pApplication); 
	return pHIDDriver;
}

int main()
{
	::stdio_init_all();
	test_Keyboard();
	//test_Mouse();
	test_GamePad1();
	//test_GamePad2();
	//test_Joystick();
	for (;;) ::tight_loop_contents();
}
