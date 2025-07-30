//==============================================================================
// jxglib/LABOPlatform.h
//==============================================================================
#ifndef PICO_JXGLIB_LABOPLATFORM_H
#define PICO_JXGLIB_LABOPLATFORM_H
#include "pico/stdlib.h"
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/TelePlot.h"
#include "jxglib/USBDevice/MSCDrive.h"
#include "jxglib/USBDevice/CDCSerial.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/LogicAnalyzer.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LABOPlatform
//------------------------------------------------------------------------------
class LABOPlatform {
private:
	USBDevice::Controller deviceController_;
	FAT::Flash fat_;
	USBDevice::MSCDrive mscDrive_;
	USBDevice::CDCSerial streamTerminal_;
	USBDevice::CDCSerial streamApplication_;
	TelePlot telePlot_;
	Serial::Terminal terminal_;
	bool attachStdioFlag_;
	LogicAnalyzer logicAnalyzer_;
private:
	static const char* textREADME_;
public:
	static LABOPlatform Instance;
public:
	LABOPlatform(int bytesFlash = PICO_FLASH_SIZE_BYTES - 0x010'0000);
public:
	void Initialize();
	LABOPlatform& AttachStdio() { attachStdioFlag_ = true; return *this; }
public:
	USBDevice::Controller& GetDeviceController() { return deviceController_; }
	FAT::Flash& GetFAT() { return fat_; }
	USBDevice::MSCDrive& GetMSCDrive() { return mscDrive_; }
	USBDevice::CDCSerial& GetStreamTerminal() { return streamTerminal_; }
	USBDevice::CDCSerial& GetStreamApplication() { return streamApplication_; }
	Serial::Terminal& GetTerminal() { return terminal_; }
	TelePlot& GetTelePlot() { return telePlot_; }
	LogicAnalyzer& GetLogicAnalyzer() { return logicAnalyzer_; }
};

}

#endif
