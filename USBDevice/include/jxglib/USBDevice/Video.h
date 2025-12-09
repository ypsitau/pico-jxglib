//==============================================================================
// jxglib/USBDevice/Video.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_VIDEO_H
#define PICO_JXGLIB_USBDEVICE_VIDEO_H
#include "jxglib/USBDevice.h"

#if CFG_TUD_VIDEO > 0

namespace jxglib::USBDevice {

class Video : public Interface {
public:
	Video(Controller& deviceController, const char* strControl, const char* strStreaming, uint8_t endp, int width, int height, int frameRate);
};

}

#endif

#endif
