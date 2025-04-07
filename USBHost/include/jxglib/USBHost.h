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
	struct ItemType {
		// 6.2.2.4 Main Items
		static const uint8_t Input				= 0x80;
		static const uint8_t Output				= 0x90;
		static const uint8_t Feature			= 0xb0;
		static const uint8_t Collection			= 0xa0;
		static const uint8_t EndCollection		= 0xc0;
		// 6.2.2.7 Global Items
		static const uint8_t UsagePage			= 0x04;
		static const uint8_t LogicalMinimum		= 0x14;
		static const uint8_t LogicalMaximum		= 0x24;
		static const uint8_t PhysicalMinimum	= 0x34;
		static const uint8_t PhysicalMaximum	= 0x44;
		static const uint8_t UnitExponent		= 0x54;
		static const uint8_t Unit				= 0x64;
		static const uint8_t ReportSize			= 0x74;
		static const uint8_t ReortID			= 0x84;
		static const uint8_t ReportCount		= 0x94;
		static const uint8_t Push				= 0xa4;
		static const uint8_t Pop				= 0xb4;
		// 6.2.2.8 Local Items
		static const uint8_t Usage				= 0x08;
		static const uint8_t UsageMinimum		= 0x18;
		static const uint8_t UsageMaximum		= 0x28;
		static const uint8_t DesignatorIndex	= 0x38;
		static const uint8_t DesignatorMinimum	= 0x48;
		static const uint8_t DesignatorMaximum	= 0x58;
		static const uint8_t StringIndex		= 0x78;
		static const uint8_t StringMaximum		= 0x88;
		static const uint8_t StringMinimum		= 0x98;
		static const uint8_t Delimeter			= 0xa8;
	};
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
public:
	static const char* GetItemTypeName(uint8_t itemType);
	static void PrintReportDescriptor(const uint8_t* descReport, uint16_t descLen);
};

}

#endif
