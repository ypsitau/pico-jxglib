#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "jxglib/USBDevice/Video.h"

#define FRAME_WIDTH   128
#define FRAME_HEIGHT  96
#define FRAME_RATE    10

using namespace jxglib;

uint8_t frame_buffer[FRAME_WIDTH * FRAME_HEIGHT * 16 / 8];

class VideoTest : public USBDevice::Video {
private:
	int startPos_;
	volatile bool xferBusyFlag_;
public:
	static const uint8_t ctl_idx = 0;
	static const uint8_t stm_idx = 0;
public:
	VideoTest(USBDevice::Controller& deviceController) :
		USBDevice::Video(deviceController, "UVC Control", "UVC Streaming", 0x81, FRAME_WIDTH, FRAME_HEIGHT, FRAME_RATE),
		startPos_{0}, xferBusyFlag_{false} {}
public:
	virtual void On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) override;
	virtual int On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters) override;
public:
	virtual void OnTick() override;
};

#if 0
void VideoTest::video_send_frame(void)
{
	uint8_t ctl_idx = 0;
	uint8_t stm_idx = 0;
	static unsigned start_ms = 0;
	static unsigned already_sent = 0;

	if (!::tud_video_n_streaming(ctl_idx, stm_idx)) {
		already_sent = 0;
		startPos_ = 0;
		return;
	}
	if (!already_sent) {
		already_sent = 1;
		xferBusyFlag_ = true;
		start_ms = board_millis();
		fill_color_bar(frame_buffer, startPos_);
		::tud_video_n_frame_xfer(ctl_idx, stm_idx, frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
	}
	unsigned cur = board_millis();
	if (cur - start_ms < interval_ms) return; // not enough time
	if (xferBusyFlag_) return;
	start_ms += interval_ms;
	xferBusyFlag_ = 1;

	fill_color_bar(frame_buffer, startPos_);
	::tud_video_n_frame_xfer(ctl_idx, stm_idx, frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
}
#endif

void VideoTest::OnTick()
{
	static const uint8_t bar_color[8][4] = {
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
	if (!::tud_video_n_streaming(ctl_idx, stm_idx) || xferBusyFlag_) return;
	uint8_t* end = &frame_buffer[FRAME_WIDTH * 2];
	unsigned idx = (FRAME_WIDTH / 2 - 1) - (startPos_ % (FRAME_WIDTH / 2));
	uint8_t* p = &frame_buffer[idx * 4];
	for (unsigned i = 0; i < 8; ++i) {
		for (int j = 0; j < FRAME_WIDTH / (2 * 8); ++j) {
			memcpy(p, &bar_color[i], 4);
			p += 4;
			if (end <= p) {
				p = frame_buffer;
			}
		}
	}
	p = &frame_buffer[FRAME_WIDTH * 2];
	for (unsigned i = 1; i < FRAME_HEIGHT; ++i) {
		memcpy(p, frame_buffer, FRAME_WIDTH * 2);
		p += FRAME_WIDTH * 2;
	}
	startPos_++;
	xferBusyFlag_ = true;
	::tud_video_n_frame_xfer(ctl_idx, stm_idx, frame_buffer, FRAME_WIDTH * FRAME_HEIGHT * 16 / 8);
}

void VideoTest::On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
{
	xferBusyFlag_ = false;
}

int VideoTest::On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters)
{
	return VIDEO_ERROR_NONE;
}

//--------------------------------------------------------------------+
// Main
//--------------------------------------------------------------------+
int main(void)
{
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
	VideoTest video(deviceController);
	deviceController.Initialize();
	for (;;) Tickable::Tick();
}
