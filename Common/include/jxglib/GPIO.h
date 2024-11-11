//==============================================================================
// jxglib/GPIO.h
//==============================================================================
#ifndef PICO_JXGLIB_GPIO_H
#define PICO_JXGLIB_GPIO_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// GPIO
//------------------------------------------------------------------------------
class GPIO {
protected:
	uint pin_;
	const char* name_;
public:
	GPIO(uint pin, const char* name) : pin_{pin}, name_{name} {}
	GPIO(const GPIO& gpio) : pin_{gpio.pin_} {}
	operator uint() const { return pin_; }
	const char* GetName() const { return name_; }
public:
	void SetFunc_SIO() { ::gpio_set_function(pin_, GPIO_FUNC_SIO); }
	void SetFunc_PIO0() { ::gpio_set_function(pin_, GPIO_FUNC_PIO0); }
	void SetFunc_PIO1() { ::gpio_set_function(pin_, GPIO_FUNC_PIO1); }
};

class GPIO0_T : public GPIO {
public:
	void SetFunc_SPI0_RX()		{ ::gpio_set_function(pin_, GPIO_FUNC_SPI); }
	void SetFunc_UART0_TX()		{ ::gpio_set_function(pin_, GPIO_FUNC_UART); }
	void SetFunc_I2C0_SDA()		{ ::gpio_set_function(pin_, GPIO_FUNC_I2C); }
	void SetFunc_PWM0_A()		{ ::gpio_set_function(pin_, GPIO_FUNC_PWM); }
	void SetFunc_USB_OVCUR_DET(){ ::gpio_set_function(pin_, GPIO_FUNC_USB); }
};

extern const GPIO GPIO0;
extern const GPIO GPIO1;
extern const GPIO GPIO2;
extern const GPIO GPIO3;
extern const GPIO GPIO4;
extern const GPIO GPIO5;
extern const GPIO GPIO6;
extern const GPIO GPIO7;
extern const GPIO GPIO8;
extern const GPIO GPIO9;
extern const GPIO GPIO10;
extern const GPIO GPIO11;
extern const GPIO GPIO12;
extern const GPIO GPIO13;
extern const GPIO GPIO14;
extern const GPIO GPIO15;
extern const GPIO GPIO16;
extern const GPIO GPIO17;
extern const GPIO GPIO18;
extern const GPIO GPIO19;
extern const GPIO GPIO20;
extern const GPIO GPIO21;
extern const GPIO GPIO22;
extern const GPIO GPIO23;
extern const GPIO GPIO24;
extern const GPIO GPIO25;
extern const GPIO GPIO26;
extern const GPIO GPIO27;
extern const GPIO GPIO28;
extern const GPIO GPIO29;

}

#endif
