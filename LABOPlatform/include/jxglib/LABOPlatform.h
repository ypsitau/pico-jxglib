//==============================================================================
// jxglib/LABOPlatform.h
//==============================================================================
#ifndef PICO_JXGLIB_LABOPLATFORM_H
#define PICO_JXGLIB_LABOPLATFORM_H
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/USBDevice/MSCDrive.h"
#include "jxglib/USBDevice/CDCSerial.h"
#include "jxglib/FAT/Flash.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LABOPlatform
//------------------------------------------------------------------------------
class LABOPlatform {
private:
	USBDevice::Controller deviceController_;
	FAT::Flash fat_;
	USBDevice::MSCDrive mscDrive_;
	USBDevice::CDCSerial cdcTerminal_;
	USBDevice::CDCSerial cdcApplication_;
	Serial::Terminal terminal_;
	bool attachStdioFlag_;
private:
	static const char* textREADME_;
public:
	LABOPlatform(int bytesFlash = PICO_FLASH_SIZE_BYTES - 0x010'0000);
public:
	void Initialize();
	LABOPlatform& AttachStdio() { attachStdioFlag_ = true; return *this; }
public:
	USBDevice::Controller& GetDeviceController() { return deviceController_; }
	FAT::Flash& GetFAT() { return fat_; }
	USBDevice::MSCDrive& GetMSCDrive() { return mscDrive_; }
	USBDevice::CDCSerial& GetCDCSerial() { return cdcTerminal_; }
	USBDevice::CDCSerial& GetCDCApplication() { return cdcApplication_; }
	Serial::Terminal& GetTerminal() { return terminal_; }
};

}

#endif
