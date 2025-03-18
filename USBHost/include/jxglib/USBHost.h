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
		virtual bool GetKeyData(KeyData& keyData) override;
	public:
		// virtual function of Tickable
		virtual void OnTick() override;
	public:
		static KeyData CreateKeyData(uint8_t keycode, uint8_t modifier);
	};
	class Mouse {
	public:
		class Status {
		private:
			Point pt_;
			int deltaX_;
			int deltaY_;
			int deltaWheel_;
			int deltaPan_;
			uint8_t buttons_;
			uint8_t buttonsPrev_;
		public:
			Status() : deltaX_{0}, deltaY_{0}, deltaWheel_{0}, deltaPan_{0}, buttons_{0}, buttonsPrev_{0} {}
			Status(const Status& status) : pt_{status.pt_}, deltaX_{status.deltaX_}, deltaY_{status.deltaY_},
				deltaWheel_{status.deltaWheel_}, deltaPan_{status.deltaPan_},
				buttons_{status.buttons_}, buttonsPrev_{status.buttonsPrev_} {}
		public:
			void SetPoint(const Point& pt) { pt_ = pt; }
			void Update(const hid_mouse_report_t& report, const Point& pt);
			Status Capture();
			uint8_t GetButtons() const { return buttons_; }
			const Point& GetPoint() const { return pt_; }
			int GetDeltaX() const { return deltaX_; }
			int GetDeltaY() const { return deltaY_; }
			int GetDeltaWheel() const { return deltaWheel_; }
			uint8_t GetPan() const { return deltaPan_; }
			bool GetButtonLeft() const { return !!(buttons_ & MOUSE_BUTTON_LEFT); }
			bool GetButtonRight() const { return !!(buttons_ & MOUSE_BUTTON_RIGHT); }
			bool GetButtonMiddle() const { return !!(buttons_ & MOUSE_BUTTON_MIDDLE); }
			bool GetButtonBackward() const { return !!(buttons_ & MOUSE_BUTTON_BACKWARD); }
			bool GetButtonForward() const { return !!(buttons_ & MOUSE_BUTTON_FORWARD); }
			bool IsButtonLeftChanged() const { return !!((buttons_ ^ buttonsPrev_) & MOUSE_BUTTON_LEFT); }
			bool IsButtonRightChanged() const { return !!((buttons_ ^ buttonsPrev_) & MOUSE_BUTTON_RIGHT); }
			bool IsButtonMiddleChanged() const { return !!((buttons_ ^ buttonsPrev_) & MOUSE_BUTTON_MIDDLE); }
			bool IsButtonBackwardChanged() const { return !!((buttons_ ^ buttonsPrev_) & MOUSE_BUTTON_BACKWARD); }
			bool IsButtonForwardChanged() const { return !!((buttons_ ^ buttonsPrev_) & MOUSE_BUTTON_FORWARD); }
			bool IsButtonLeftPressed() const { return IsButtonLeftChanged() && GetButtonLeft(); }
			bool IsButtonRightPressed() const { return IsButtonRightChanged() && GetButtonRight(); }
			bool IsButtonMiddlePressed() const { return IsButtonMiddleChanged() && GetButtonMiddle(); }
			bool IsButtonBackwardPressed() const { return IsButtonBackwardChanged() && GetButtonBackward(); }
			bool IsButtonForwardPressed() const { return IsButtonForwardChanged() && GetButtonForward(); }
			bool IsButtonLeftReleased() const { return IsButtonLeftChanged() && !GetButtonLeft(); }
			bool IsButtonRightReleased() const { return IsButtonRightChanged() && !GetButtonRight(); }
			bool IsButtonMiddleReleased() const { return IsButtonMiddleChanged() && !GetButtonMiddle(); }
			bool IsButtonBackwardReleased() const { return IsButtonBackwardChanged() && !GetButtonBackward(); }
			bool IsButtonForwardReleased() const { return IsButtonForwardChanged() && !GetButtonForward(); }
		};
	private:
		float sensibility_;
		Rect rcStage_;
		Rect rcStageRaw_;
		int xRaw_;
		int yRaw_;
		Status status_;
	public:
		Mouse();
	public:
		void SetStage(const Rect& rcStage) { rcStage_ = rcStage; UpdateStage(); }
		void SetSensibility(float sensibility) { sensibility_ = sensibility; UpdateStage(); }
		void UpdateStage();
		Point CalcPoint() const;
	public:
		void OnReport(uint8_t devAddr, uint8_t iInstance, const hid_mouse_report_t& report);
	public:
		Status CaptureStatus() { return status_.Capture(); }
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
