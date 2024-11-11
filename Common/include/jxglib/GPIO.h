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
public:
	uint pin;
public:
	GPIO(uint pin) : pin{pin} {}
	GPIO(const GPIO& gpio) : pin{gpio.pin} {}
	operator uint() const { return pin; }
	bool IsValid() const { return pin != static_cast<uint>(-1); }
	bool IsInvalid() const { return pin == static_cast<uint>(-1); }
public:
	const GPIO& set_function_XIP() const			{ ::gpio_set_function(pin, GPIO_FUNC_XIP); return *this; }
	const GPIO& set_function_SPI() const			{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART() const			{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C() const			{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_SIO() const			{ ::gpio_set_function(pin, GPIO_FUNC_SIO); return *this; }
	const GPIO& set_function_PIO0() const			{ ::gpio_set_function(pin, GPIO_FUNC_PIO0); return *this; }
	const GPIO& set_function_PIO1() const			{ ::gpio_set_function(pin, GPIO_FUNC_PIO1); return *this; }
	const GPIO& set_function_USB() const			{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
public:
	const GPIO& pull_up() const						{ ::gpio_pull_up(pin); return *this; }
};

class GPIO_Invalid_T : public GPIO {
public:
	GPIO_Invalid_T() : GPIO(static_cast<uint>(-1)) {}
};

class GPIO0_T : public GPIO {
public:
	GPIO0_T() : GPIO(0) {}
	const GPIO& set_function_SPI0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM0_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO1_T : public GPIO {
public:
	GPIO1_T() : GPIO(1) {}
	const GPIO& set_function_SPI0_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM0_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO2_T : public GPIO {
public:
	GPIO2_T() : GPIO(2) {}
	const GPIO& set_function_SPI0_SCK() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_CTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM1_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO3_T : public GPIO {
public:
	GPIO3_T() : GPIO(3) {}
	const GPIO& set_function_SPI0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_RTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM1_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO4_T : public GPIO {
public:
	GPIO4_T() : GPIO(4) {}
	const GPIO& set_function_SPI0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM2_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO5_T : public GPIO {
public:
	GPIO5_T() : GPIO(5) {}
	const GPIO& set_function_SPI0_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM2_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO6_T : public GPIO {
public:
	GPIO6_T() : GPIO(6) {}
	const GPIO& set_function_SPI0_SCK() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_CTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM3_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO7_T : public GPIO {
public:
	GPIO7_T() : GPIO(7) {}
	const GPIO& set_function_SPI0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM3_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO8_T : public GPIO {
public:
	GPIO8_T() : GPIO(8) {}
	const GPIO& set_function_SPI1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM4_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO9_T : public GPIO {
public:
	GPIO9_T() : GPIO(9) {}
	const GPIO& set_function_SPI1_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM4_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO10_T : public GPIO {
public:
	GPIO10_T() : GPIO(10) {}
	const GPIO& set_function_SPI1_SCK() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_CTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM5_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO11_T : public GPIO {
public:
	GPIO11_T() : GPIO(11) {}
	const GPIO& set_function_SPI1_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM5_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO12_T : public GPIO {
public:
	GPIO12_T() : GPIO(12) {}
	const GPIO& set_function_SPI1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM6_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO13_T : public GPIO {
public:
	GPIO13_T() : GPIO(13) {}
	const GPIO& set_function_SPI1_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM6_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO14_T : public GPIO {
public:
	GPIO14_T() : GPIO(14) {}
	const GPIO& set_function_SPI1_SCK() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_CTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM7_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO15_T : public GPIO {
public:
	GPIO15_T() : GPIO(15) {}
	const GPIO& set_function_SPI1_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_RTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM7_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO16_T : public GPIO {
public:
	GPIO16_T() : GPIO(16) {}
	const GPIO& set_function_SPI0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM0_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO17_T : public GPIO {
public:
	GPIO17_T() : GPIO(17) {}
	const GPIO& set_function_SPI0_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM0_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO18_T : public GPIO {
public:
	GPIO18_T() : GPIO(18) {}
	const GPIO& set_function_SPI0_SCK() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_CTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM1_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO19_T : public GPIO {
public:
	GPIO19_T() : GPIO(19) {}
	const GPIO& set_function_SPI0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_RTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM1_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO20_T : public GPIO {
public:
	GPIO20_T() : GPIO(20) {}
	const GPIO& set_function_SPI0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM2_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_CLOCK_GPIN0() const	{ ::gpio_set_function(pin, GPIO_FUNC_GPCK); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO21_T : public GPIO {
public:
	GPIO21_T() : GPIO(21) {}
	const GPIO& set_function_SPI0_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM2_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_CLOCK_GPOUT0() const	{ ::gpio_set_function(pin, GPIO_FUNC_GPCK); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO22_T : public GPIO {
public:
	GPIO22_T() : GPIO(22) {}
	const GPIO& set_function_SPI0_SCK() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_CTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM3_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_CLOCK_GPIN1() const	{ ::gpio_set_function(pin, GPIO_FUNC_GPCK); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO23_T : public GPIO {
public:
	GPIO23_T() : GPIO(23) {}
	const GPIO& set_function_SPI0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM3_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_CLOCK_GPOUT1() const	{ ::gpio_set_function(pin, GPIO_FUNC_GPCK); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO24_T : public GPIO {
public:
	GPIO24_T() : GPIO(24) {}
	const GPIO& set_function_SPI1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM4_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_CLOCK_GPOUT2() const	{ ::gpio_set_function(pin, GPIO_FUNC_GPCK); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO25_T : public GPIO {
public:
	GPIO25_T() : GPIO(25) {}
	const GPIO& set_function_SPI1_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM4_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_CLOCK_GPOUT3() const	{ ::gpio_set_function(pin, GPIO_FUNC_GPCK); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO26_T : public GPIO {
public:
	GPIO26_T() : GPIO(26) {}
	const GPIO& set_function_SPI1_SCK() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_CTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM5_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO27_T : public GPIO {
public:
	GPIO27_T() : GPIO(27) {}
	const GPIO& set_function_SPI1_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART1_RTS() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C1_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM5_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_OVCUR_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO28_T : public GPIO {
public:
	GPIO28_T() : GPIO(28) {}
	const GPIO& set_function_SPI1_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_TX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SDA() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM6_A() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_DET() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

class GPIO29_T : public GPIO {
public:
	GPIO29_T() : GPIO(29) {}
	const GPIO& set_function_SPI1_CSn() const		{ ::gpio_set_function(pin, GPIO_FUNC_SPI); return *this; }
	const GPIO& set_function_UART0_RX() const		{ ::gpio_set_function(pin, GPIO_FUNC_UART); return *this; }
	const GPIO& set_function_I2C0_SCL() const		{ ::gpio_set_function(pin, GPIO_FUNC_I2C); return *this; }
	const GPIO& set_function_PWM6_B() const			{ ::gpio_set_function(pin, GPIO_FUNC_PWM); return *this; }
	const GPIO& set_function_USB_VBUS_EN() const	{ ::gpio_set_function(pin, GPIO_FUNC_USB); return *this; }
};

extern const GPIO_Invalid_T GPIO_Invalid;
extern const GPIO0_T GPIO0;
extern const GPIO1_T GPIO1;
extern const GPIO2_T GPIO2;
extern const GPIO3_T GPIO3;
extern const GPIO4_T GPIO4;
extern const GPIO5_T GPIO5;
extern const GPIO6_T GPIO6;
extern const GPIO7_T GPIO7;
extern const GPIO8_T GPIO8;
extern const GPIO9_T GPIO9;
extern const GPIO10_T GPIO10;
extern const GPIO11_T GPIO11;
extern const GPIO12_T GPIO12;
extern const GPIO13_T GPIO13;
extern const GPIO14_T GPIO14;
extern const GPIO15_T GPIO15;
extern const GPIO16_T GPIO16;
extern const GPIO17_T GPIO17;
extern const GPIO18_T GPIO18;
extern const GPIO19_T GPIO19;
extern const GPIO20_T GPIO20;
extern const GPIO21_T GPIO21;
extern const GPIO22_T GPIO22;
extern const GPIO23_T GPIO23;
extern const GPIO24_T GPIO24;
extern const GPIO25_T GPIO25;
extern const GPIO26_T GPIO26;
extern const GPIO27_T GPIO27;
extern const GPIO28_T GPIO28;
extern const GPIO29_T GPIO29;

}

#endif
