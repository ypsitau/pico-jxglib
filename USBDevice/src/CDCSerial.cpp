//==============================================================================
// CDCSerial.cpp
//==============================================================================
#include "jxglib/USBDevice/CDCSerial.h"

namespace jxglib::USBDevice {

CDCSerial::CDCSerial(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn) :
	USBDevice::CDC(deviceController, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64, 10), chPrev_{'\0'}, addCrFlag_{true}, keyboard_{*this} {}

void CDCSerial::OnTick()
{
	uint8_t buff[64];
	if (cdc_available()) {
		int bytesToRead = buffRead_.GetRoomLength();
		int bytesRead = cdc_read(buff, bytesToRead);
		if (bytesRead > 0) buffRead_.WriteBuff(buff, bytesRead);
	}
}

int CDCSerial::Read(void* buff, int bytesBuff)
{
	return buffRead_.ReadBuff(static_cast<uint8_t*>(buff), bytesBuff);
}

int CDCSerial::Write(const void* buff, int bytesBuff)
{
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		bytesWritten += cdc_write(static_cast<const uint8_t*>(buff) + bytesWritten, bytesBuff - bytesWritten);
		Tickable::Tick();
	}
	return bytesWritten;
}

bool CDCSerial::Flush()
{
	cdc_write_flush();
	return true;
}

Printable& CDCSerial::PutChar(char ch)
{
	if (addCrFlag_ && chPrev_ != '\r' && ch == '\n') PutCharRaw('\r');
	PutCharRaw(ch);
	chPrev_ = ch;
	return *this;
}

}
