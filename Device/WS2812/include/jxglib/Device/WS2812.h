//==============================================================================
// jxglib/Device/WS2812.h
//==============================================================================
#ifndef PICO_JXGLIB_DEVICE_WS2812_H
#define PICO_JXGLIB_DEVICE_WS2812_H
#include "pico/stdlib.h"
#include "jxglib/PIO.h"

namespace jxglib::Device {

//------------------------------------------------------------------------------
// Device::WS2812
//------------------------------------------------------------------------------
class WS2812 {
private:
	PIO::StateMachine sm_;
	PIO::Program program_;
public:
	static const uint32_t Freq = 800000;
public:
	WS2812() {}
public:
	void Run(const GPIO& gpio);
	bool IsRunning() const { return sm_.IsValid(); }
public:
	WS2812& Put(uint8_t r, uint8_t g, uint8_t b);
	WS2812& Put(const Color& c) { return Put(c.r, c.g, c.b); }

};

}

#endif
