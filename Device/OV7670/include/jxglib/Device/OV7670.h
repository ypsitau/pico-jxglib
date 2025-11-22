//==============================================================================
// jxglib/Device/OV7670.h
//==============================================================================
#ifndef PICO_JXGLIB_DEVICE_OV7670_H
#define PICO_JXGLIB_DEVICE_OV7670_H
#include "pico/stdlib.h"
#include "jxglib/PIO.h"
#include "jxglib/DMA.h"
#include "jxglib/Image.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// OV7670
//------------------------------------------------------------------------------
class OV7670 {
private:
	PIO::StateMachine sm_;
	PIO::Program program_;
	PIO::Program programToReset_;
	DMA::Channel* pChannel_;
	DMA::ChannelConfig channelConfig_;
public:
	OV7670();
public:
	void Run(const GPIO& din);
	bool IsRunning() const { return sm_.IsValid(); }
public:
	OV7670& Capture(Image& image);
};

}

#endif
