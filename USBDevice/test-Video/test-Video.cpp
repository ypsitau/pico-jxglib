#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "jxglib/USBDevice/Video.h"

using namespace jxglib;

class Video_ColorBar : public USBDevice::Video {
private:
	int startPos_;
	volatile bool xferBusyFlag_;
	uint8_t* frameBuffer_;
public:
	static const uint8_t ctl_idx = 0;
	static const uint8_t stm_idx = 0;
public:
	Video_ColorBar(USBDevice::Controller& deviceController, uint8_t endp, int width, int height, int frameRate) :
		USBDevice::Video(deviceController, "UVC Control", "UVC Streaming", endp, width, height, frameRate),
		startPos_{0}, xferBusyFlag_{false}, frameBuffer_{nullptr} {}
	~Video_ColorBar() { ::free(frameBuffer_); }
public:
	void Initialize();
public:
	virtual void On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) override;
	virtual int On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters) override;
public:
	virtual void OnTick() override;
};

void Video_ColorBar::Initialize()
{
	frameBuffer_ = reinterpret_cast<uint8_t*>(::malloc(width_ * height_ * 16 / 8));
}

void Video_ColorBar::OnTick()
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
	uint8_t* end = &frameBuffer_[width_ * 2];
	unsigned idx = (width_ / 2 - 1) - (startPos_ % (width_ / 2));
	uint8_t* p = &frameBuffer_[idx * 4];
	for (unsigned i = 0; i < 8; ++i) {
		for (int j = 0; j < width_ / (2 * 8); ++j) {
			memcpy(p, &bar_color[i], 4);
			p += 4;
			if (end <= p) {
				p = frameBuffer_;
			}
		}
	}
	p = &frameBuffer_[width_ * 2];
	for (unsigned i = 1; i < height_; ++i) {
		memcpy(p, frameBuffer_, width_ * 2);
		p += width_ * 2;
	}
	startPos_++;
	xferBusyFlag_ = true;
	::tud_video_n_frame_xfer(ctl_idx, stm_idx, frameBuffer_, width_ * height_ * 16 / 8);
}

void Video_ColorBar::On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
{
	xferBusyFlag_ = false;
}

int Video_ColorBar::On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters)
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
	Video_ColorBar video(deviceController, 0x81, 80, 60, 10);
	deviceController.Initialize();
	video.Initialize();
	for (;;) Tickable::Tick();
}
