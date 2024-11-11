//==============================================================================
// Buton.cpp
//==============================================================================
#include <stdio.h>
#include "jxglib/Button.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Button
//------------------------------------------------------------------------------
size_t Button::nButtons_ = 0;
Button* Button::pButtonTbl_[32];
struct repeating_timer Button::repeatingTimer_;
Button::EventBuff Button::eventBuff_;
bool Button::enableEventBuff_ = false;
const Button Button::None;

void Button::Initialize(int32_t msecPolling, bool enableEventBuff)
{
	enableEventBuff_ = enableEventBuff;
	for (size_t i = 0; i < nButtons_; i++) {
		Button* pButton =  pButtonTbl_[i];
		GPIO gpio = pButton->GetGPIO();
		::gpio_init(gpio);
		::gpio_set_dir(gpio, GPIO_IN);
		::gpio_pull_up(gpio);
	}
	::add_repeating_timer_ms(msecPolling, Callback, nullptr, &repeatingTimer_);
}

Button::Button() : gpio_{0}, name_{"none"}, status_{Status::None}
{
}

Button::Button(GPIO gpio, const char* name) : gpio_{gpio}, name_{name}, status_{Status::Released}
{
	pButtonTbl_[nButtons_++] = this;
}

bool Button::Callback(struct repeating_timer* pRepeatingTimer)
{
	for (size_t i = 0; i < nButtons_; i++) {
		Button* pButton =  pButtonTbl_[i];
		Status status = ::gpio_get(pButton->gpio_)? Status::Released : Status::Pressed;
		if (pButton->status_ != status) {
			pButton->status_ = status;
			if (enableEventBuff_ && !eventBuff_.IsFull()) {
				eventBuff_.WriteData(Event(status, pButton));
			}
		}
	}
    return true;
}

Button::Event Button::ReadEvent()
{
	return eventBuff_.HasData()? eventBuff_.ReadData() : Event::None;
}

Button::Event Button::WaitEvent()
{
	while (!eventBuff_.HasData()) tight_loop_contents();
	return eventBuff_.ReadData();
}

//------------------------------------------------------------------------------
// Button::Event
//------------------------------------------------------------------------------
const Button::Event Button::Event::None {Status::None, &Button::None};

}
