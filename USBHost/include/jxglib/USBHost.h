//==============================================================================
// jxglib/USBHost.h
//==============================================================================
#ifndef PICO_JXGLIB_USBHOST_H
#define PICO_JXGLIB_USBHOST_H
#include "pico/stdlib.h"
#include "tusb.h"
#include "jxglib/FIFOBuff.h"
#include "jxglib/Tickable.h"
#include "jxglib/Keyboard.h"

namespace jxglib {

//------------------------------------------------------------------------------
// USBHost
//------------------------------------------------------------------------------
class USBHost : public Tickable {
public:
	class Keyboard : public jxglib::Keyboard, public Tickable {
	private:
		uint8_t modifier_;
		uint8_t keycode_[6];
		int cntHold_;
		FIFOBuff<KeyData, 8> fifoKeyData_;
	public:
		static const uint8_t convTbl_101Keyboard[128][3];
	public:
		Keyboard();
	public:
		void OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report);
	public:
		// virtual function of jxglib::Keyboard
		virtual bool GetKeyData(KeyData& keyData) override;
	public:
		// virtual function of Tickable
		virtual void OnTick() override;
	public:
		static KeyData CreateKeyData(uint8_t keycode, uint8_t modifier);
	};
	class Mouse {
	public:
		Mouse();
	public:
		void OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report);
	};
public:
	static USBHost* Instance;
private:
	Keyboard keyboard_;
	Mouse mouse_;
public:
	USBHost();
public:
	void Initialize(uint8_t rhport = BOARD_TUH_RHPORT);
public:
	static Keyboard& GetKeyboard() { return Instance->keyboard_; }
	static Mouse& GetMouse() { return Instance->mouse_; }
public:
	// virtual function of Tickable
	virtual void OnTick() override;
public:
	virtual void OnMount(uint8_t devAddr) {}
	virtual void OnUmount(uint8_t devAddr) {}
};

}

#endif
