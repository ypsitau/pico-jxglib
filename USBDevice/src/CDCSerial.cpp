//==============================================================================
// CDCSerial.cpp
//==============================================================================
#include "jxglib/USBDevice/CDCSerial.h"

namespace jxglib::USBDevice {

CDCSerial::CDCSerial(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn) :
	USBDevice::CDC(deviceController, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64), chPrev_{'\0'}, addCrFlag_{true}, keyboard_{*this} {}

int CDCSerial::Read(void* buff, int bytesBuff)
{
	int bytesRead = 0;
	while (bytesRead < bytesBuff) {
		int bytes = cdc_read(static_cast<uint8_t*>(buff) + bytesRead, bytesBuff - bytesRead);
		if (bytes <= 0) break; // no more data available
		bytesRead += bytes;
		Tickable::TickSub();
	}
	//if (bytesRead > 0) ::printf("CDCSerial::Read: %d bytes read 0x%02x\n", bytesRead, static_cast<const uint8_t*>(buff)[bytesRead - 1]);
	return bytesRead;
}

int CDCSerial::Write(const void* buff, int bytesBuff)
{
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		bytesWritten += cdc_write(static_cast<const uint8_t*>(buff) + bytesWritten, bytesBuff - bytesWritten);
		Tickable::TickSub();
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
