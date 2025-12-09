/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
//#include "jxglib/USBDevice.h"

//using namespace jxglib;

#define FRAME_WIDTH   128
#define FRAME_HEIGHT  96
#define FRAME_RATE    10

int board_millis()
{
		return to_ms_since_boot(get_absolute_time());
}

void led_blinking_task(void* param);
void usb_device_task(void *param);
void video_send_frame(void);

#if CFG_TUSB_OS == OPT_OS_FREERTOS
void freertos_init_task(void);
#endif

extern "C" const uint8_t* GetConfigDesc(int* pBytes);

//--------------------------------------------------------------------+
// Main
//--------------------------------------------------------------------+
int main(void)
{
#if 0
	USBDevice::Controller deviceController({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "Video Test", "Video Test Product", "0123456");
	int bytes;
	deviceController.RegisterConfigDesc(GetConfigDesc(&bytes), bytes);
	deviceController.Initialize();
#endif

	// If using FreeRTOS: create blinky, tinyusb device, video task
	// init device stack on configured roothub port
	tusb_rhport_init_t dev_init = {
		.role = TUSB_ROLE_DEVICE,
		.speed = TUSB_SPEED_AUTO
	};
	tusb_init(BOARD_TUD_RHPORT, &dev_init);

	while (1) {
		tud_task(); // tinyusb device task
		video_send_frame();
	}
}

//--------------------------------------------------------------------+
// USB Video
//--------------------------------------------------------------------+
static unsigned frame_num = 0;
static unsigned tx_busy = 0;
static unsigned interval_ms = 1000 / FRAME_RATE;

// YUY2 frame buffer
static uint8_t frame_buffer[FRAME_WIDTH * FRAME_HEIGHT * 16 / 8];

static void fill_color_bar(uint8_t* buffer, unsigned start_position)
{
	/* EBU color bars: https://stackoverflow.com/questions/6939422 */
	static uint8_t const bar_color[8][4] = {
			/*  Y,   U,   Y,   V */
			{ 235, 128, 235, 128}, /* 100% White */
			{ 219,  16, 219, 138}, /* Yellow */
			{ 188, 154, 188,  16}, /* Cyan */
			{ 173,  42, 173,  26}, /* Green */
			{  78, 214,  78, 230}, /* Magenta */
			{  63, 102,  63, 240}, /* Red */
			{  32, 240,  32, 118}, /* Blue */
			{  16, 128,  16, 128}, /* Black */
	};
	uint8_t* p;

	/* Generate the 1st line */
	uint8_t* end = &buffer[FRAME_WIDTH * 2];
	unsigned idx = (FRAME_WIDTH / 2 - 1) - (start_position % (FRAME_WIDTH / 2));
	p = &buffer[idx * 4];
	for (unsigned i = 0; i < 8; ++i) {
		for (int j = 0; j < FRAME_WIDTH / (2 * 8); ++j) {
			memcpy(p, &bar_color[i], 4);
			p += 4;
			if (end <= p) {
				p = buffer;
			}
		}
	}

	/* Duplicate the 1st line to the others */
	p = &buffer[FRAME_WIDTH * 2];
	for (unsigned i = 1; i < FRAME_HEIGHT; ++i) {
		memcpy(p, buffer, FRAME_WIDTH * 2);
		p += FRAME_WIDTH * 2;
	}
}

void video_send_frame(void)
{
	static unsigned start_ms = 0;
	static unsigned already_sent = 0;

	if (!tud_video_n_streaming(0, 0)) {
		already_sent = 0;
		frame_num = 0;
		return;
	}

	if (!already_sent) {
		already_sent = 1;
		tx_busy = 1;
		start_ms = board_millis();
		fill_color_bar(frame_buffer, frame_num);
		tud_video_n_frame_xfer(0, 0, (void*) frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
	}

	unsigned cur = board_millis();
	if (cur - start_ms < interval_ms) return; // not enough time
	if (tx_busy) return;
	start_ms += interval_ms;
	tx_busy = 1;

	fill_color_bar(frame_buffer, frame_num);
	tud_video_n_frame_xfer(0, 0, (void*) frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
}

void tud_video_frame_xfer_complete_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
{
	tx_busy = 0;
	/* flip buffer */
	++frame_num;
}

int tud_video_commit_cb(uint_fast8_t ctl_idx, uint_fast8_t stm_idx,
												video_probe_and_commit_control_t const* parameters)
{
	/* convert unit to ms from 100 ns */
	interval_ms = parameters->dwFrameInterval / 10000;
	return VIDEO_ERROR_NONE;
}
