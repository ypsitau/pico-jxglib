//==============================================================================
// Serial.cpp
//==============================================================================
#include "jxglib/USBDevice/Serial.h"

namespace jxglib::USBDevice {

Serial::Serial(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn) :
	USBDevice::CDC(deviceController, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64, 10), chPrev_{'\0'}, addCrFlag_{true}, keyboard_{*this} {}

void Serial::OnTick()
{
	uint8_t buff[64];
	if (cdc_available()) {
		int bytesToRead = buffRead_.GetRoomLength();
		int bytesRead = cdc_read(buff, bytesToRead);
		if (bytesRead > 0) buffRead_.WriteBuff(buff, bytesRead);
	}
	int bytesToWrite = buffWrite_.ReadBuff(buff, sizeof(buff));
	bytesToWrite = ChooseMin(bytesToWrite, cdc_write_available());
	if (bytesToWrite > 0) {
		cdc_write(buff, bytesToWrite);
		cdc_write_flush();
	}
}

int Serial::Read(void* buff, int bytesBuff)
{
	return buffRead_.ReadBuff(static_cast<uint8_t*>(buff), bytesBuff);
}

int Serial::Write(const void* buff, int bytesBuff)
{
	::printf("Serial::Write: %d bytes %d\n", bytesBuff, Tickable::GetTickCalledDepth());
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		bytesWritten += buffWrite_.WriteBuff(static_cast<const uint8_t*>(buff) + bytesWritten, bytesBuff - bytesWritten);
		Tickable::Tick();
	}
	return bytesWritten;
}

Printable& Serial::PutChar(char ch)
{
	if (addCrFlag_ && chPrev_ != '\r' && ch == '\n') PutCharRaw('\r');
	PutCharRaw(ch);
	chPrev_ = ch;
	return *this;
}

}
