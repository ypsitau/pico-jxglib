//==============================================================================
// jxglib/Buton.h
//==============================================================================
#ifndef PICO_JXGLIB_BUTTON_H
#define PICO_JXGLIB_BUTTON_H
#include <stdio.h>
#include <memory>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "jxglib/GPIO.h"
#include "jxglib/FIFOBuff.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Button
//------------------------------------------------------------------------------
class Button {
public:
	enum class Status { None, Pressed, Released };
	class Event {
	private:
		const Button* pButton_;
	public:
		static const Event None;
	public:
		Event() : pButton_{nullptr} {}
		Event(const Event& event) : pButton_{event.pButton_} {}
		Event(Status eventType, const Button* pButton) : pButton_(pButton) {}
		void operator=(const Event& event) { pButton_ = event.pButton_; } 
	public:
		bool IsButton(const Button& button) const { return pButton_ == &button; }
		bool IsNone() const { return pButton_->IsNone(); }
		bool IsPressed() const { return pButton_->IsPressed(); }
		bool IsReleased() const { return pButton_->IsReleased(); }
		bool IsPressed(const Button& button) const { return IsPressed() && pButton_ == &button; }
		bool IsReleased(const Button& button) const { return IsReleased() && pButton_ == &button; }
		const char* GetName() const { return pButton_->GetName(); }
	};
	using EventBuff = FIFOBuff<Event, 64>;
private:
	GPIO gpio_;
	const char* name_;
	volatile Status status_;
private:
	static size_t nButtons_;
	static Button* pButtonTbl_[32];
	static struct repeating_timer repeatingTimer_;
	static EventBuff eventBuff_;
	static bool enableEventBuff_;
public:
	static void Initialize(int32_t msecPolling, bool enableEventBuff);
	static Event ReadEvent();
	static Event WaitEvent();
public:
	Button(GPIO gpio, const char* name = "");
public:
	GPIO GetGPIO() const { return gpio_; }
	const char* GetName() const { return name_; }
	bool IsNone() const { return status_ == Status::None; }
	bool IsPressed() const { return status_ == Status::Pressed; }
	bool IsReleased() const { return status_ == Status::Released; }
private:
	static bool Callback(struct repeating_timer* pRepeatingTimer);
};

}

#endif
