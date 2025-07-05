#include "pico/stdlib.h"
#include "jxglib/USBDevice/CDC.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"

using namespace jxglib;

//-----------------------------------------------------------------------------
// CDCStream
//-----------------------------------------------------------------------------
class CDCStream : public USBDevice::CDC, public Stream {
private:
	char chPrev_;
	bool addCrFlag_;
	FIFOBuff<uint8_t, 64> buffRead_;
	FIFOBuff<uint8_t, 64> buffWrite_;
public:
	CDCStream(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn);
public:
	// virtual functions of USBDevice::CDC
	virtual void OnTick() override;
public:
	CDCStream& AddCr(bool addCrFlag) { addCrFlag_ = addCrFlag; return* this; }
public:
	// virtual functions of Stream
	virtual int Read(void* buff, int bytesBuff) override;
	virtual int Write(const void* buff, int bytesBuff) override;
	Printable& PutChar(char ch);
};

CDCStream::CDCStream(USBDevice::Controller& deviceController, const char* name, uint8_t endpNotif, uint8_t endpBulkOut, uint8_t endpBulkIn) :
		USBDevice::CDC(deviceController, name, endpNotif, 8, endpBulkOut, endpBulkIn, 64, 10), chPrev_{'\0'}, addCrFlag_{true} {}

void CDCStream::OnTick()
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

int CDCStream::Read(void* buff, int bytesBuff)
{
	return buffRead_.ReadBuff(static_cast<uint8_t*>(buff), bytesBuff);
}

int CDCStream::Write(const void* buff, int bytesBuff)
{
	::printf("CDCStream::Write: %d bytes %d\n", bytesBuff, Tickable::GetTickCalledDepth());
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		bytesWritten += buffWrite_.WriteBuff(static_cast<const uint8_t*>(buff) + bytesWritten, bytesBuff - bytesWritten);
		Tickable::Tick();
	}
	return bytesWritten;
}

Printable& CDCStream::PutChar(char ch)
{
	if (addCrFlag_ && chPrev_ != '\r' && ch == '\n') PutCharRaw('\r');
	PutCharRaw(ch);
	chPrev_ = ch;
	return *this;
}

//-----------------------------------------------------------------------------
// main
//-----------------------------------------------------------------------------
int main(void)
{
	::stdio_init_all(); 
	USBDevice::Controller deviceController({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			USBDevice::GenerateSpecificProductId(0x4000),
		bcdDevice:			0x0100,
	}, 0x0409, "CDC Stream", "CDC Stream Product", "0123456");
	CDCStream cdcStream(deviceController, "CDC Stream", 0x81, 0x02, 0x82);
	deviceController.Initialize();
	VT100::Keyboard keyboard(cdcStream);
	cdcStream.Initialize();
	Serial::Terminal terminal;
	terminal.AttachKeyboard(keyboard).AttachPrintable(cdcStream);
	//terminal.AttachKeyboard(keyboard);
	Shell::AttachTerminal(terminal.Initialize());
	for (;;) {
		//char buff[64];
		//int bytesRead = cdcStream.Read(buff, sizeof(buff) - 1);
		//if (bytesRead > 0) {
		//	buff[bytesRead] = '\0'; // Null-terminate the string
		//	cdcStream.Print(buff);
		//	//::printf("%s", buff);
		//}
		Tickable::Tick();
	}
}
