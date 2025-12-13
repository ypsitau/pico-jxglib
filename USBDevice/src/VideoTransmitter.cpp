//==============================================================================
// VideoTransmitter.cpp
// Windows support YUY2 and NV12
// https://docs.microsoft.com/en-us/windows-hardware/drivers/stream/usb-video-class-driver-overview
//==============================================================================
#include <stdlib.h>
#include "jxglib/USBDevice/VideoTransmitter.h"

#if CFG_TUD_VIDEO > 0

namespace jxglib::USBDevice {

//-----------------------------------------------------------------------------
// USBDevice::VideoTransmitter
//-----------------------------------------------------------------------------
VideoTransmitter::~VideoTransmitter()
{
}

void VideoTransmitter::Initialize()
{
}

bool VideoTransmitter::CanTransmitFrame() const
{
	return ::tud_video_n_streaming(ctl_idx, stm_idx) && !xferBusyFlag_;
}

void VideoTransmitter::TransmitFrame(const void* frameBuffer)
{
	xferBusyFlag_ = true;
	::tud_video_n_frame_xfer(ctl_idx, stm_idx, const_cast<void*>(frameBuffer), size_.width * size_.height * 16 / 8);
}

void VideoTransmitter::On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx)
{
	xferBusyFlag_ = false;
}

int VideoTransmitter::On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters)
{
	return VIDEO_ERROR_NONE;
}

}

#endif
