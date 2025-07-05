//==============================================================================
// jxglib/USBDevice/Serial.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_SERIAL_H
#define PICO_JXGLIB_USBDEVICE_SERIAL_H
#include "jxglib/USBDevice/CDC.h"
#include "jxglib/Serial.h"

#if CFG_TUD_CDC > 0

namespace jxglib::USBDevice {

//-----------------------------------------------------------------------------
// Serial
//-----------------------------------------------------------------------------
class Serial : public USBDevice::CDC, public Stream {
private:
	char chPrev_;
	bool addCrFlag_;
	FIFOBuff<uint8_t, 64> buffRead_;
	FIFOBuff<uint8_t, 64> buffWrite_;
	VT100::Keyboard keyboard_;
public:
	Serial(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn);
public:
	// virtual functions of USBDevice::CDC
	virtual void OnTick() override;
public:
	Serial& AddCr(bool addCrFlag) { addCrFlag_ = addCrFlag; return* this; }
    Keyboard& GetKeyboard() { return keyboard_; }
public:
	// virtual functions of Stream
	virtual int Read(void* buff, int bytesBuff) override;
	virtual int Write(const void* buff, int bytesBuff) override;
	Printable& PutChar(char ch);
};

}

#endif

#endif
