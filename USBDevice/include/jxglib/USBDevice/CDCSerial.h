//==============================================================================
// jxglib/USBDevice/CDCSerial.h
//==============================================================================
#ifndef PICO_JXGLIB_USBDEVICE_CDCSERIAL_H
#define PICO_JXGLIB_USBDEVICE_CDCSERIAL_H
#include "jxglib/USBDevice/CDC.h"
#include "jxglib/Serial.h"

#if CFG_TUD_CDC > 0

namespace jxglib::USBDevice {

//-----------------------------------------------------------------------------
// CDCSerial
//-----------------------------------------------------------------------------
class CDCSerial : public USBDevice::CDC, public Stream {
private:
	char chPrev_;
	bool addCrFlag_;
	VT100::Keyboard keyboard_;
public:
	CDCSerial(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn);
public:
	CDCSerial& AddCr(bool addCrFlag) { addCrFlag_ = addCrFlag; return* this; }
    Keyboard& GetKeyboard() { return keyboard_; }
public:
	// virtual functions of Stream
	virtual int Read(void* buff, int bytesBuff) override;
	virtual int Write(const void* buff, int bytesBuff) override;
	virtual bool Flush() override;
	virtual Printable& PutChar(char ch) override;
	virtual Printable& PutCharRaw(char ch) override;
};

}

#endif

#endif
