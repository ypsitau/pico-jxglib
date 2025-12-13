//==============================================================================
// jxglib/USBDevice/VideoTransmitter.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_VIDEOTRANSMITTER_H
#define PICO_JXGLIB_USBDEVICE_VIDEOTRANSMITTER_H
#include "jxglib/USBDevice/Video.h"

#if CFG_TUD_VIDEO > 0

namespace jxglib::USBDevice {

//------------------------------------------------------------------------------
// VideoTransmitter
//------------------------------------------------------------------------------
class VideoTransmitter : public Video {
private:
	volatile bool xferBusyFlag_;
public:
	static const uint8_t ctl_idx = 0;
	static const uint8_t stm_idx = 0;
public:
	VideoTransmitter(Controller& deviceController, const char* strControl, const char* strStreaming,
							uint8_t endp, const Size& size, int frameRate) :
		Video(deviceController, strControl, strStreaming, endp, size, frameRate), xferBusyFlag_{false} {}
	~VideoTransmitter();
public:
	void Initialize();
	bool CanTransmitFrame() const;
	void TransmitFrame(const void* frameBuffer);
public:
	virtual void On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) override;
	virtual int On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters) override;
};

}

#endif

#endif
