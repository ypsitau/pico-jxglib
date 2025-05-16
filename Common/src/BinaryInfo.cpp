//==============================================================================
// BinaryInfo.cpp
//==============================================================================
#include "jxglib/BinaryInfo.h"

namespace jxglib {

//-----------------------------------------------------------------------------
// BinaryInfo
//-----------------------------------------------------------------------------
void BinaryInfo::PrintProgramInformation(Printable& tout)
{
	int n = 0;
	const char* str;
	uint32_t num;
	tout.Printf("Program Information\n");
	if (GetString(BINARY_INFO_ID_RP_PROGRAM_NAME, &str)) 				{ tout.Printf(" name:              %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_PROGRAM_VERSION_STRING, &str))		{ tout.Printf(" version:           %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_PROGRAM_FEATURE, &str))				{ tout.Printf(" feature:           %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_PROGRAM_URL, &str))					{ tout.Printf(" URL                %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_PROGRAM_DESCRIPTION, &str))			{ tout.Printf(" description        %s\n", str); n++; }
	do																	{ tout.Printf(" binary start:      0x%08x\n", 0x10000000); n++; } while (0);
	if (GetInt(BINARY_INFO_ID_RP_BINARY_END, &num))						{ tout.Printf(" binary end:        0x%08x\n", num); n++; }
	if (n == 0) tout.Printf(" none\n");
}

void BinaryInfo::PrintBuildInformation(Printable& tout)
{
	int n = 0;
	const char* str;
	tout.Printf("Build Information\n");
	if (GetString(BINARY_INFO_ID_RP_SDK_VERSION, &str))					{ tout.Printf(" sdk version:       %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_PICO_BOARD, &str))					{ tout.Printf(" pico_board:        %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_BOOT2_NAME, &str))					{ tout.Printf(" boot2_name:        %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_PROGRAM_BUILD_DATE_STRING, &str))	{ tout.Printf(" build date:        %s\n", str); n++; }
	if (GetString(BINARY_INFO_ID_RP_PROGRAM_BUILD_ATTRIBUTE, &str))		{ tout.Printf(" build attributes:  %s\n", str); n++; }
	if (n == 0) tout.Printf(" none\n");
}

void BinaryInfo::PrintFixedPinInformation(Printable& tout)
{
	int n = 0;
	tout.Printf("Fixed Pin Information\n");
	for (auto ppInfo = GetInfoEnd(); ppInfo != GetInfoStart(); --ppInfo) {
		const binary_info_t* pInfo = *(ppInfo - 1);
		switch (pInfo->type) {
		case BINARY_INFO_TYPE_PINS_WITH_FUNC: {
			n++;
			auto& info = *reinterpret_cast<const binary_info_pins_with_func_t*>(pInfo);
			uint32_t pin_encoding = info.pin_encoding;
			uint32_t encodingFormat = pin_encoding & 0b111;
			pin_encoding >>= 3;
			uint32_t pinFunc = pin_encoding & 0b1111;
			pin_encoding >>= 4;
			switch (encodingFormat) {
			case BI_PINS_ENCODING_RANGE: {
				n++;
				uint32_t pinLow = pin_encoding & 0b11111;
				pin_encoding >>= 5;
				uint32_t pinHigh = pin_encoding & 0b11111;
				pin_encoding >>= 5;
				char buff[32];
				::snprintf(buff, sizeof(buff), "%d-%d:", pinLow, pinHigh);
				tout.Printf(" %-19s%s\n", buff, GetPinFuncName(pinFunc));
				break;
			}
			case BI_PINS_ENCODING_MULTI: {
				uint32_t pinPrev = -1;
				for (int i = 0; i < 5; ++i) {
					uint32_t pin = pin_encoding & 0b11111;
					pin_encoding >>= 5;
					if (pin == pinPrev) break;
					char buff[32];
					::snprintf(buff, sizeof(buff), "%d:", pin);
					tout.Printf(" %-19s%s\n", buff, GetPinFuncName(pinFunc, pin));
					pinPrev = pin;
				}
				break;
			}
			default:
				break;
			}
			break;
		}
		case BINARY_INFO_TYPE_PINS_WITH_NAME: {
			n++;
			auto& info = *reinterpret_cast<const binary_info_pins_with_name_t*>(pInfo);
			uint32_t pin_mask = info.pin_mask;
			const char* label = info.label;
			for (int pin = 0; pin_mask; pin++, pin_mask >>= 1) {
				if (pin_mask & 1) {
					char buff[32];
					::snprintf(buff, sizeof(buff), "%d:", pin);
					tout.Printf(" %-19s", buff, info.label);
					for ( ; *label; label++) if (*label == '|') { label++; break; } else { tout.PutChar(*label); }
					tout.Println();
				}
			}
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_FUNC: {
			n++;
			auto& info = *reinterpret_cast<const binary_info_pins64_with_func_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_NAME: {
			n++;
			auto& info = *reinterpret_cast<const binary_info_pins64_with_name_t*>(pInfo);
			break;
		}
		default:
			break;
		}
	}
	if (n == 0) tout.Printf(" none\n");
}

void BinaryInfo::PrintMemoryMap(Printable& tout)
{
	tout.Printf("Memory Map\n");
	tout.Printf(" flash binary:      0x%08x-0x%08x %7d\n", &__flash_binary_start, &__flash_binary_end, &__flash_binary_end - &__flash_binary_start);
#if defined(__arm__)
	tout.Printf(" ram vector table:  0x%08x-0x%08x %7d\n", &ram_vector_table, &ram_vector_table + PICO_RAM_VECTOR_TABLE_SIZE, PICO_RAM_VECTOR_TABLE_SIZE * sizeof(void*));
#endif
	tout.Printf(" data:              0x%08x-0x%08x %7d\n", &__data_start__, &__data_end__, &__data_end__ - &__data_start__);
	tout.Printf(" bss:               0x%08x-0x%08x %7d\n", &__bss_start__, &__bss_end__, &__bss_end__ - &__bss_start__);
	tout.Printf(" heap:              0x%08x-0x%08x %7d\n", &__heap_start, &__heap_end, &__heap_end - &__heap_start);
	tout.Printf(" stack:             0x%08x-0x%08x %7d\n", &__heap_end, &__stack, &__stack - &__heap_end);
}

void BinaryInfo::Print(Printable& tout)
{
	for (auto ppInfo = GetInfoStart(); ppInfo != GetInfoEnd(); ++ppInfo) {
		const binary_info_t* pInfo = *ppInfo;
		tout.Printf("%04x %04x\n", pInfo->tag, pInfo->type);
		switch (pInfo->type) {
		case BINARY_INFO_TYPE_RAW_DATA: {
			auto& info = *reinterpret_cast<const binary_info_raw_data_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_SIZED_DATA: {
			auto& info = *reinterpret_cast<const binary_info_sized_data_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_BINARY_INFO_LIST_ZERO_TERMINATED: {
			auto& info = *reinterpret_cast<const binary_info_list_zero_terminated_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_BSON: {
			auto& info = *reinterpret_cast<const binary_info_sized_data_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_ID_AND_INT: {
			auto& info = *reinterpret_cast<const binary_info_id_and_int_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_ID_AND_STRING: {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_BLOCK_DEVICE: {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_func_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_name_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_NAMED_GROUP: {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PTR_INT32_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_ptr_int32_with_name_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PTR_STRING_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_ptr_string_with_name_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_func_t*>(pInfo);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_name_t*>(pInfo);
			break;
		}
		default:
			break;
		}
	}
}

bool BinaryInfo::GetInt(uint32_t id, uint32_t* pValue)
{
	for (auto ppInfo = GetInfoStart(); ppInfo != GetInfoEnd(); ++ppInfo) {
		const binary_info_t* pInfo = *ppInfo;
		if (pInfo->type == BINARY_INFO_TYPE_ID_AND_INT) {
			auto& info = *reinterpret_cast<const binary_info_id_and_int_t*>(pInfo);
			if (info.id == id) {
				*pValue = info.value;
				return true;
			}
		}
	}
	return false;
}

bool BinaryInfo::GetString(uint32_t id, const char** pValue)
{
	for (auto ppInfo = GetInfoStart(); ppInfo != GetInfoEnd(); ++ppInfo) {
		const binary_info_t* pInfo = *ppInfo;
		if (pInfo->type == BINARY_INFO_TYPE_ID_AND_STRING) {
			auto& info = *reinterpret_cast<const binary_info_id_and_string_t*>(pInfo);
			if (info.id == id) {
				*pValue = info.value;
				return true;
			}
		}
	}
	return false;
}

const char* BinaryInfo::GetPinFuncName(int pinFunc)
{
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

const char* BinaryInfo::GetPinFuncName(int pinFunc, uint pin)
{
#if defined(PICO_RP2040)
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
		{ "XIP",	"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"CLOCK GPOUR3",	"USB VBUS DET",		}, // 25
		{ "XIP",	"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 26
		{ "XIP",	"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB OVCUR DET",	}, // 27
		{ "XIP",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS DET",		}, // 28
		{ "XIP",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"",				"USB VBUS EN",		}, // 29
	};
	return (pinFunc < count_of(funcNameTbl[0]) && pin < count_of(funcNameTbl))? funcNameTbl[pin][pinFunc] : "unknown";
#elif defined(PICO_RP2350)
	static const char* funcNameTbl[][12] = {
		{ "HSTX",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB OVCUR DET",	"",			}, // 0
		{ "HSTX",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACECLK",		"USB VBUS DET",		"",			}, // 1
		{ "HSTX",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA0",	"USB VBUS EN",		"UART0 TX",	}, // 2
		{ "HSTX",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA1",	"USB OVCUR DET",	"UART0 RX",	}, // 3
		{ "HSTX",	"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA2",	"USB VBUS DET",		"",			}, // 4
		{ "HSTX",	"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"TRACEDATA3",	"USB VBUS EN",		"",			}, // 5
		{ "HSTX",	"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART1 TX",	}, // 6
		{ "HSTX",	"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"UART1 RX",	}, // 7
		{ "HSTX",	"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB VBUS EN",		"",			}, // 8
		{ "HSTX",	"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"",			}, // 9
		{ "HSTX",	"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"UART1 TX",	}, // 10
		{ "HSTX",	"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"UART1 RX",	}, // 11
		{ "HSTX",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN0",	"USB OVCUR DET",	"",			}, // 12
		{ "HSTX",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT0",	"USB VBUS DET",		"",			}, // 13
		{ "HSTX",	"SPI1 SCK",	"UART0 CTS","I2C1 SDA",	"PWM7 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN1",	"USB VBUS EN",		"UART0 TX",	}, // 14
		{ "HSTX",	"SPI1 TX",	"UART0 RTS","I2C1 SCL",	"PWM7 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT1",	"USB OVCUR DET",	"UART0 RX",	}, // 15
		{ "HSTX",	"SPI0 RX",	"UART0 TX",	"I2C0 SDA",	"PWM0 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"",			}, // 16
		{ "HSTX",	"SPI0 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM0 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"",			}, // 17
		{ "HSTX",	"SPI0 SCK",	"UART0 CTS","I2C1 SDA",	"PWM1 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART0 TX",	}, // 18
		{ "HSTX",	"SPI0 TX",	"UART0 RTS","I2C1 SCL",	"PWM1 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"QMI CS1n",		"USB VBUS DET",		"UART0 RX",	}, // 19
		{ "HSTX",	"SPI0 RX",	"UART1 TX",	"I2C0 SDA",	"PWM2 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN0",	"USB VBUS EN",		"",			}, // 20
		{ "HSTX",	"SPI0 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM2 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT0",	"USB OVCUR DET",	"",			}, // 21
		{ "HSTX",	"SPI0 SCK",	"UART1 CTS","I2C1 SDA",	"PWM3 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPIN1",	"USB VBUS DET",		"UART1 TX",	}, // 22
		{ "HSTX",	"SPI0 TX",	"UART1 RTS","I2C1 SCL",	"PWM3 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT1",	"USB VBUS EN",		"UART1 RX",	}, // 23
		{ "HSTX",	"SPI1 RX",	"UART1 TX",	"I2C0 SDA",	"PWM4 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUT2",	"USB OVCUR DET",	"",			}, // 24
		{ "HSTX",	"SPI1 CSn",	"UART1 RX",	"I2C0 SCL",	"PWM4 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"CLOCK GPOUR3",	"USB VBUS DET",		"",			}, // 25
		{ "HSTX",	"SPI1 SCK",	"UART1 CTS","I2C1 SDA",	"PWM5 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"UART1 TX",	}, // 26
		{ "HSTX",	"SPI1 TX",	"UART1 RTS","I2C1 SCL",	"PWM5 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB OVCUR DET",	"UART1 RX",	}, // 27
		{ "HSTX",	"SPI1 RX",	"UART0 TX",	"I2C0 SDA",	"PWM6 A",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS DET",		"",			}, // 28
		{ "HSTX",	"SPI1 CSn",	"UART0 RX",	"I2C0 SCL",	"PWM6 B",	"SIO",	"PIO0",	"PIO1",	"PIO2",	"",				"USB VBUS EN",		"",			}, // 29
	};
	return (pinFunc < count_of(funcNameTbl[0]) && pin < count_of(funcNameTbl))? funcNameTbl[pin][pinFunc] : "unknown";
#else
	return "unknown"
#endif
}

}
