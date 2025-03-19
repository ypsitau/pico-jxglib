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
	class Keyboard : public jxglib::Keyboard, public Tickable {
	public:
		struct ConvEntry {
			uint8_t keyCode;
			uint8_t charCode;
			uint8_t charCodeShift;
			uint8_t charCodeCtrl;
		};
		using Report = hid_keyboard_report_t;
	private:
		Report report_;
		FIFOBuff<KeyData, 8> fifoKeyData_;
		bool firstFlag_;
		uint32_t msecHold_;
		uint32_t msecRepeat_;
	public:
		static const ConvEntry convEntryTbl_101Keyboard[256];
		static const ConvEntry convEntryTbl_106Keyboard[256];
	public:
		Keyboard();
	public:
		void OnReport(uint8_t devAddr, uint8_t iInstance, const hid_keyboard_report_t& report);
	public:
		// virtual function of jxglib::Keyboard
		virtual bool GetKeyData(KeyData* pKeyData) override;
		virtual int SenseKeyData(KeyData keyDataTbl[], int nKeysMax) override;
	public:
		// virtual function of Tickable
		virtual void OnTick() override;
	public:
		static KeyData CreateKeyData(uint8_t keycode, uint8_t modifier);
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
		Mouse& SetStage(const Rect& rcStage);
		Mouse& SetSensibility(float sensibility);
		void UpdateStage();
		Point CalcPoint() const;
	public:
		void OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report);
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
	// virtual function of Tickable
	virtual void OnTick() override;
};

}

#endif
