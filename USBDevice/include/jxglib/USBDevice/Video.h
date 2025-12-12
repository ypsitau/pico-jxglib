//==============================================================================
// jxglib/USBDevice/Video.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_VIDEO_H
#define PICO_JXGLIB_USBDEVICE_VIDEO_H
#include "jxglib/USBDevice.h"

#if CFG_TUD_VIDEO > 0

namespace jxglib::USBDevice {

//------------------------------------------------------------------------------
// Video
//------------------------------------------------------------------------------
class Video : public Interface {
public:
	struct TU_ATTR_PACKED uvc_control_desc_t {
		tusb_desc_interface_t itf;
		tusb_desc_video_control_header_1itf_t header;
		tusb_desc_video_control_camera_terminal_t camera_terminal;
		tusb_desc_video_control_output_terminal_t output_terminal;
	};
	struct TU_ATTR_PACKED uvc_streaming_desc_t {
		tusb_desc_interface_t itf;
		tusb_desc_video_streaming_input_header_1byte_t header;
		tusb_desc_video_format_uncompressed_t format;
		tusb_desc_video_frame_uncompressed_continuous_t frame;
		tusb_desc_video_streaming_color_matching_t color;
#if !CFG_TUD_VIDEO_STREAMING_BULK
		// For ISO streaming, USB spec requires to alternate interface
		tusb_desc_interface_t itf_alt;
#endif
		tusb_desc_endpoint_t ep;
	};
	struct TU_ATTR_PACKED ConfigDesc {
		tusb_desc_interface_assoc_t iad;
		uvc_control_desc_t video_control;
		uvc_streaming_desc_t video_streaming;
	};
protected:
	Size size_;
	int frameRate_;
public:
	Video(Controller& deviceController, const char* strControl, const char* strStreaming,
								uint8_t endp, const Size& size, int frameRate);
public:
	const Size& GetSize() const { return size_; }
	int GetWidth() const { return size_.width; }
	int GetHeight() const { return size_.height; }
public:
	virtual void On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) {}
	virtual int On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters) {
		return VIDEO_ERROR_NONE;
	}
};

//------------------------------------------------------------------------------
// VideoSimple
//------------------------------------------------------------------------------
class VideoSimple : public Video {
private:
	volatile bool xferBusyFlag_;
public:
	static const uint8_t ctl_idx = 0;
	static const uint8_t stm_idx = 0;
public:
	VideoSimple(Controller& deviceController, const char* strControl, const char* strStreaming,
							uint8_t endp, const Size& size, int frameRate) :
		Video(deviceController, strControl, strStreaming, endp, size, frameRate), xferBusyFlag_{false} {}
	~VideoSimple();
public:
	void Initialize();
	bool CanTransferFrame() const;
	void TransferFrame(const void* frameBuffer);
public:
	virtual void On_frame_xfer_complete(uint_fast8_t ctl_idx, uint_fast8_t stm_idx) override;
	virtual int On_commit(uint_fast8_t ctl_idx, uint_fast8_t stm_idx, const video_probe_and_commit_control_t* parameters) override;
};

}

#endif

#endif
