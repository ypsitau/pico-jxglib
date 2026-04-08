//==============================================================================
// jxglib/LABOPlatform.h
//==============================================================================
#ifndef PICO_JXGLIB_LABOPLATFORM_H
#define PICO_JXGLIB_LABOPLATFORM_H
#include "pico/stdlib.h"
#include "pico/stdio/driver.h"
#if defined (__cplusplus)
#include "jxglib/Shell.h"
#include "jxglib/Serial.h"
#include "jxglib/TelePlot.h"
#include "jxglib/USBDevice/MSCDrive.h"
#include "jxglib/USBDevice/CDCSerial.h"
#include "jxglib/USBDevice/VideoTransmitter.h"
#include "jxglib/FAT/Flash.h"
#include "jxglib/FAT/SDCard.h"
#include "jxglib/LogicAnalyzer.h"
#include "jxglib/Device/WS2812.h"
#include "jxglib/Camera.h"
#include "jxglib/Font.h"

namespace jxglib {

//------------------------------------------------------------------------------
// LABOPlatform
//------------------------------------------------------------------------------
class LABOPlatform {
private:
	USBDevice::Controller deviceController_;
	FAT::Flash fat_;
	USBDevice::MSCDrive mscDrive_;
	USBDevice::CDCSerial streamCDC_Terminal_;
	USBDevice::CDCSerial streamCDC_Application_;
	std::unique_ptr<USBDevice::VideoTransmitter> pVideoTransmitter_;
	TelePlot telePlot_;
	Serial::Terminal terminal_;
	bool attachStdioFlag_;
	LogicAnalyzer logicAnalyzer_;
	LogicAnalyzer::SigrokAdapter sigrokAdapter_;
	Device::WS2812 ws2812Device_;
	stdio_driver_t stdio_driver_;
private:
	static const char* textREADME_;
public:
	static LABOPlatform Instance;
	static const int bytesProgramMax = 0x0018'0000;
public:
	LABOPlatform(int bytesDrive = PICO_FLASH_SIZE_BYTES - bytesProgramMax);
public:
	void Initialize();
	LABOPlatform& AttachStdio(bool attachStdioFlag = true);
	void SetTerminalInterface(Printable& printable, Keyboard& keyboard);
	void RestoreTerminalInterface();
	void SetSigrokInterface(Stream& stream);
	void RestoreSigrokInterface();
public:
	USBDevice::Controller& GetDeviceController() { return deviceController_; }
	FAT::Flash& GetFAT() { return fat_; }
	USBDevice::MSCDrive& GetMSCDrive() { return mscDrive_; }
	Serial::Terminal& GetTerminal() { return terminal_; }
	Printable& GetPrintable() { return terminal_.GetPrintable(); }
	TelePlot& GetTelePlot() { return telePlot_; }
	LogicAnalyzer& GetLogicAnalyzer() { return logicAnalyzer_; }
	Device::WS2812& GetWS2812Device() { return ws2812Device_; }
private:
	static void func_out_chars(const char* buf, int len);
	static void func_out_flush(void);
	static int func_in_chars(char* buf, int len);
	static void func_set_chars_available_callback(void (*fn)(void*), void *param);
};

}

#endif

#if defined (__cplusplus)
extern "C"{
#endif

void jxglib_labo_init(bool attachStdioFlag);
void jxglib_tick();
bool jxglib_ticksub();
void jxglib_sleep(int msec);
void jxglib_shellcmd(const char* name, const char* help, int (*func)(int argc, char* argv[]));
void jxglib_enable_startup_script(bool enableFlag);

#if defined (__cplusplus)
}
#endif

#endif
