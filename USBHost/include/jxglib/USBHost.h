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
#include "jxglib/Mouse.h"

namespace jxglib {

//------------------------------------------------------------------------------
// USBHost
//------------------------------------------------------------------------------
class USBHost : public Tickable {
public:
	class EventHandler {
	public:
		virtual void OnMount(uint8_t devAddr) {}
		virtual void OnUmount(uint8_t devAddr) {}
	};
	class Keyboard : public KeyboardRepeatable {
	public:
		struct UsageIdToKeyCode {
			uint8_t keyCodeUS;
			uint8_t keyCodeNonUS;
		};
		struct Report {
			uint8_t modifier;
			uint8_t keyCodeTbl[6];	
		};
	private:
		bool capsLockAsCtrlFlag_;
		Report reportCaptured_;
	public:
		static const UsageIdToKeyCode usageIdToKeyCodeTbl[256];
	public:
		Keyboard();
	public:
		void OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report);
	public:
		// virtual function of jxglib::Keyboard
		virtual jxglib::Keyboard& SetCapsLockAsCtrl(bool capsLockAsCtrlFlag = true) override;
		virtual uint8_t GetModifier() override { return reportCaptured_.modifier; }
		virtual int SenseKeyCode(uint8_t keyCodeTbl[], int nKeysMax = 1) override;
	};
	class Mouse : public jxglib::Mouse {
	private:
		float sensibility_;
		Rect rcStage_;
		Rect rcStageRaw_;
		int xRaw_;
		int yRaw_;
	public:
		Mouse();
	public:
		void UpdateStage();
		Point CalcPoint() const;
	public:
		void OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report);
	public:
		// virtual function of jxglib::Mouse
		virtual jxglib::Mouse& SetSensibility(float sensibility) override;
		virtual jxglib::Mouse& SetStage(const Rect& rcStage) override;
	};
public:
	static USBHost Instance;
private:
	Keyboard keyboard_;
	Mouse mouse_;
	EventHandler* pEventHandler_;
public:
	USBHost();
public:
	static void Initialize(uint8_t rhport = BOARD_TUH_RHPORT, EventHandler* pEventHandler = nullptr);
public:
	static Keyboard& GetKeyboard() { return Instance.keyboard_; }
	static Mouse& GetMouse() { return Instance.mouse_; }
	static EventHandler* GetEventHandler() { return Instance.pEventHandler_; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "USBHost"; }
	virtual void OnTick() override;
};

}

#endif
