#include "jxglib/GPIOInfo.h"

namespace jxglib::GPIOInfo {


const char* GetHelp_AvailableFunc()
{
#if defined(PICO_RP2040)
	return "spi, uart, i2c, pwm, sio, pio0, pio1, clock, usb, xip, null";
#elif defined(PICO_RP2350)
	return "spi, uart, uart-aux, i2c, pwm, sio, pio0, pio1, pio2, clock, usb, hstx, xip-cs1, coresight-trace, null";
#else
	return "(none)";
#endif
}


gpio_function_t StringToFunc(const char* str, bool* pValidFlag)
{
	*pValidFlag = true;
	if (::strcasecmp(str, "spi") == 0) {
		return GPIO_FUNC_SPI;
	} else if (::strcasecmp(str, "uart") == 0) {
		return GPIO_FUNC_UART;
	} else if (::strcasecmp(str, "i2c") == 0) {
		return GPIO_FUNC_I2C;
	} else if (::strcasecmp(str, "pwm") == 0) {
		return GPIO_FUNC_PWM;
	} else if (::strcasecmp(str, "sio") == 0) {
		return GPIO_FUNC_SIO;
	} else if (::strcasecmp(str, "pio0") == 0) {
		return GPIO_FUNC_PIO0;
	} else if (::strcasecmp(str, "pio1") == 0) {
		return GPIO_FUNC_PIO1;
	} else if (::strcasecmp(str, "clock") == 0) {
		return GPIO_FUNC_GPCK;
	} else if (::strcasecmp(str, "usb") == 0) {
		return GPIO_FUNC_USB;
	} else if (::strcasecmp(str, "null") == 0) {
		return GPIO_FUNC_NULL;
#if defined(PICO_RP2040)
	} else if (::strcasecmp(str, "xip") == 0) {
		return GPIO_FUNC_XIP;
#elif defined(PICO_RP2350)
	} else if (::strcasecmp(str, "hstx") == 0) {
		return GPIO_FUNC_HSTX;
	} else if (::strcasecmp(str, "pio2") == 0) {
		return GPIO_FUNC_PIO2;
	} else if (::strcasecmp(str, "xip-cs1") == 0) {
		return GPIO_FUNC_XIP_CS1;
	} else if (::strcasecmp(str, "coresight-trace") == 0) {
		return GPIO_FUNC_CORESIGHT_TRACE;
	} else if (::strcasecmp(str, "uart-aux") == 0) {
		return GPIO_FUNC_UART_AUX;
#endif
	}
	*pValidFlag = false;	
	return GPIO_FUNC_NULL;
}


const char* GetFuncName(int pinFunc, const char* funcNameNull)
{
	if (pinFunc == GPIO_FUNC_NULL) return funcNameNull;
#if defined(PICO_RP2040)
	static const char* funcNameTbl[] = {
		"XIP",		"SPI",		"UART",		"I2C",		"PWM",		"SIO",		"PIO0",		"PIO1",		"GPCK",		"USB",
	};
	return (pinFunc < count_of(funcNameTbl))? funcNameTbl[pinFunc] : "unknown";
#elif defined(PICO_RP2350)
	static const char* funcNameTbl[] = {
		"HSTX",		"SPI",		"UART",		"I2C",		"PWM",		"SIO",		"PIO0",		"PIO1",		"PIO2",		"GPCK",		"USB",		"UART_AUX",
	};
	return (pinFunc < count_of(funcNameTbl))? funcNameTbl[pinFunc] : "unknown";
#else
	return "unknown";
#endif
}

const char* GetFuncName(int pinFunc, uint pin, const char* funcNameNull)
{
	if (pinFunc == GPIO_FUNC_NULL) return funcNameNull;
#if defined(PICO_RP2040)
	// RP2040 Datasheet (rp2040-datasheet.pdf) 1.4.3 GPIO Functions
	static const char* funcNameTbl[][10] = {
		{ "XIP",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 0
		{ "XIP",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 1
		{ "XIP",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 2
		{ "XIP",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 3
		{ "XIP",	"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 4
		{ "XIP",	"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 5
		{ "XIP",	"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 6
		{ "XIP",	"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 7
		{ "XIP",	"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 8
		{ "XIP",	"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 9
		{ "XIP",	"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 10
		{ "XIP",	"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 11
		{ "XIP",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 12
		{ "XIP",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 13
		{ "XIP",	"SPI1 SCK",	"UART0 CTS","I2C1 SDA",	"PWM7 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 14
		{ "XIP",	"SPI1 TX",	"UART0 RTS","I2C1 SCL",	"PWM7 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 15
		{ "XIP",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 16
		{ "XIP",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 17
		{ "XIP",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 18
		{ "XIP",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 19
		{ "XIP",	"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPIN0",	"USB VBUS EN",		}, // 20
		{ "XIP",	"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT0",	"USB OVCUR DET",	}, // 21
		{ "XIP",	"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPIN1",	"USB VBUS DET",		}, // 22
		{ "XIP",	"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT1",	"USB VBUS EN",		}, // 23
		{ "XIP",	"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT2",	"USB OVCUR DET",	}, // 24
		{ "XIP",	"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUT3",	"USB VBUS DET",		}, // 25
		{ "XIP",	"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 26
		{ "XIP",	"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 27
		{ "XIP",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 28
		{ "XIP",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 29
	};
	return (pinFunc < count_of(funcNameTbl[0]) && pin < count_of(funcNameTbl))? funcNameTbl[pin][pinFunc] : "unknown";
#elif defined(PICO_RP2350)
	// RP2350 Datasheet (rp2350-datasheet.pdf) 1.2.3 GPIO Functions (Bank 0)
	static const char* funcNameTbl[][12] = {
		{ "",		"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB OVCUR DET",	"",			}, // 0
		{ "",		"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACECLK",		"USB VBUS DET",		"",			}, // 1
		{ "",		"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA0",	"USB VBUS EN",		"UART0 TX",	}, // 2
		{ "",		"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA1",	"USB OVCUR DET",	"UART0 RX",	}, // 3
		{ "",		"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA2",	"USB VBUS DET",		"",			}, // 4
		{ "",		"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA3",	"USB VBUS EN",		"",			}, // 5
		{ "",		"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART1 TX",	}, // 6
		{ "",		"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"UART1 RX",	}, // 7
		{ "",		"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB VBUS EN",		"",			}, // 8
		{ "",		"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"",			}, // 9
		{ "",		"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"UART1 TX",	}, // 10
		{ "",		"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"UART1 RX",	}, // 11
		{ "HSTX",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN0",	"USB OVCUR DET",	"",			}, // 12
		{ "HSTX",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT0",	"USB VBUS DET",		"",			}, // 13
		{ "HSTX",	"SPI1 SCK",	"UART0 CTS","I2C1 SDA",	"PWM7 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN1",	"USB VBUS EN",		"UART0 TX",	}, // 14
		{ "HSTX",	"SPI1 TX",	"UART0 RTS","I2C1 SCL",	"PWM7 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT1",	"USB OVCUR DET",	"UART0 RX",	}, // 15
		{ "HSTX",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"",			}, // 16
		{ "HSTX",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"",			}, // 17
		{ "HSTX",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART0 TX",	}, // 18
		{ "HSTX",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB VBUS DET",		"UART0 RX",	}, // 19
		{ "",		"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN0",	"USB VBUS EN",		"",			}, // 20
		{ "",		"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT0",	"USB OVCUR DET",	"",			}, // 21
		{ "",		"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN1",	"USB VBUS DET",		"UART1 TX",	}, // 22
		{ "",		"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT1",	"USB VBUS EN",		"UART1 RX",	}, // 23
		{ "",		"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT2",	"USB OVCUR DET",	"",			}, // 24
		{ "",		"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT3",	"USB VBUS DET",		"",			}, // 25
		{ "",		"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"UART1 TX",	}, // 26
		{ "",		"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART1 RX",	}, // 27
		{ "",		"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"",			}, // 28
		{ "",		"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"",			}, // 29
	};
	return (pinFunc < count_of(funcNameTbl[0]) && pin < count_of(funcNameTbl))? funcNameTbl[pin][pinFunc] : "unknown";
#else
	return "unknown";
#endif
}

}
