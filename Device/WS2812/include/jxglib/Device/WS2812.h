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
	WS2812() {}
public:
	void Initialize(const GPIO& gpio, uint32_t freq = 800000);
	const WS2812& Put(uint8_t r, uint8_t g, uint8_t b) const;
	const WS2812& Put(const Color& c) const { return Put(c.r, c.g, c.b); }

};

}

#endif
