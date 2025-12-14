//==============================================================================
// jxglib/USBDevice/VideoTransmitter.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_VIDEOTRANSMITTER_H
#define PICO_JXGLIB_USBDEVICE_VIDEOTRANSMITTER_H
#include "jxglib/VideoTransmitter.h"
#include "jxglib/USBDevice/Video.h"

#if CFG_TUD_VIDEO > 0

namespace jxglib::USBDevice {

//------------------------------------------------------------------------------
// VideoTransmitter
//------------------------------------------------------------------------------
class VideoTransmitter : public Video, public jxglib::VideoTransmitter {
private:
	volatile bool transmitBusyFlag_;
public:
	static const uint8_t ctl_idx = 0;
	static const uint8_t stm_idx = 0;
public:
	VideoTransmitter(Controller& deviceController, const char* strControl, const char* strStreaming,
							uint8_t endp, const Size& size, int frameRate) :
		Video(deviceController, strControl, strStreaming, endp, size, frameRate), transmitBusyFlag_{false} {}
	~VideoTransmitter();
public:
	void Initialize();
public:
	virtual const Size& GetSize() const override { return size_; }
	virtual bool CanTransmit() override;
	virtual void Transmit(const void* buffFrame) override;
public:
	virtual void On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) override;
	virtual int On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters) override;
};

}

#endif

#endif
