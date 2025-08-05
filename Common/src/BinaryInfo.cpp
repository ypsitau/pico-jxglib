//==============================================================================
// BinaryInfo.cpp
//==============================================================================
#include "jxglib/BinaryInfo.h"

namespace jxglib::BinaryInfo {

static bool GetInt(uint32_t id, uint32_t *pValue);
static bool GetString(uint32_t id, const char** pValue);
static const binary_info_t** GetInfoStart() { return reinterpret_cast<const binary_info_t**>(&__binary_info_start); }
static const binary_info_t** GetInfoEnd() { return reinterpret_cast<const binary_info_t**>(&__binary_info_end); }

void PrintProgramInformation(Printable& tout)
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

void PrintBuildInformation(Printable& tout)
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

void PrintFixedPinInformation(Printable& tout)
{
	const char* labelTbl[30 + 1] = { nullptr };	// 30 GPIO pins + 1 for null terminator
	auto isLabelMatched = [](const char* label1, const char* label2) {
		const char* p1 = label1;
		const char* p2 = label2;
		if (!p1 || !p2) return false;
		for ( ; ; p1++, p2++) {
			char ch1 = (*p1 == '|')? '\0' : *p1;
			char ch2 = (*p2 == '|')? '\0' : *p2;
			if (ch1 != ch2) return false;
			if (ch1 == '\0') break;
		}
		return true;
	};
	auto printPinFunc = [&tout](int pinStart, int pinEnd, const char* label) {
		char buff[32];
		if (pinStart == pinEnd) {
			::snprintf(buff, sizeof(buff), "%d:", pinStart);
		} else {
			::snprintf(buff, sizeof(buff), "%d-%d:", pinStart, pinEnd);
		}
		tout.Printf(" %-19s", buff);
		for (const char* p = label; *p && *p != '|'; p++) tout.PutChar(*p);
		tout.Println();
	};	
	auto parsePinsWithFunc = [&labelTbl](uint64_t pin_encoding, int nBitsPinFunc, int nBitsPin, int nPins) {
	    // 32 bits
		// p4_5 : p3_5 : p2_5 : p1_5 : p0_5 : func_4 : 010_3 //individual pins p0,p1,p2,p3,p4 ... if fewer than 5 then duplicate p
    	//                    phi_5 : plo_5 : func_4 : 001_3 // pin range plo-phi inclusive
	    // 64 bits
	    // p6_8 : p5_8 : p4_8 : p3_8 : p2_8 : p1_8 : p0_8 : func_5 : 010_3 //individual pins p0,p1,p2 ... if fewer than 7 then duplicate p
    	//                    phi_8 : plo_8 : func_5 : 001_3 // pin range plo-phi inclusive
		int nBitsEncodingFormat = 3;
		uint32_t encodingFormat = static_cast<uint32_t>(pin_encoding & ((1u << nBitsEncodingFormat) - 1));
		pin_encoding >>= nBitsEncodingFormat;
		uint32_t pinFunc = static_cast<uint32_t>(pin_encoding & ((1u << nBitsPinFunc) - 1));
		pin_encoding >>= nBitsPinFunc;
		uint32_t maskPin = (1u << nBitsPin) - 1;
		switch (encodingFormat) {
		case BI_PINS_ENCODING_RANGE: {
			uint32_t pinLow = static_cast<uint32_t>(pin_encoding & maskPin);
			pin_encoding >>= nBitsPin;
			uint32_t pinHigh = static_cast<uint32_t>(pin_encoding & maskPin);
			for (uint32_t pin = pinLow; pin <= pinHigh; ++pin) {
				if (!labelTbl[pin]) labelTbl[pin] = GPIOInfo::GetFuncName(pinFunc, pin);
			}
			break;
		}
		case BI_PINS_ENCODING_MULTI: {
			uint32_t pinPrev = -1;
			for (int i = 0; i < nPins; ++i) {
				uint32_t pin = static_cast<uint32_t>(pin_encoding & maskPin);
				pin_encoding >>= nBitsPin;
				if (pin == pinPrev) break;
				if (!labelTbl[pin]) labelTbl[pin] = GPIOInfo::GetFuncName(pinFunc, pin);
				pinPrev = pin;
			}
			break;
		}
		default:
			break;
		}
	};
	auto parsePinsWithName = [&labelTbl](uint64_t pin_mask, const char* label) {
		for (int pin = 0; pin_mask; pin++, pin_mask >>= 1) {
			if (pin_mask & 1) {
				if (!labelTbl[pin]) labelTbl[pin] = label;
				label = ::strchr(label, '|');
				label = label? label + 1 : nullptr;
			}
		}
	};
	tout.Printf("Fixed Pin Information\n");
	for (auto ppInfo = GetInfoStart(); ppInfo != GetInfoEnd(); ++ppInfo) {
		const binary_info_t* pInfo = *ppInfo;
		switch (pInfo->type) {
		case BINARY_INFO_TYPE_PINS_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_func_t*>(pInfo);
			parsePinsWithFunc(info.pin_encoding, 4, 5, 5);
			break;
		}
		case BINARY_INFO_TYPE_PINS_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins_with_name_t*>(pInfo);
			parsePinsWithName(info.pin_mask, info.label);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_FUNC: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_func_t*>(pInfo);
			parsePinsWithFunc(info.pin_encoding, 5, 8, 7);
			break;
		}
		case BINARY_INFO_TYPE_PINS64_WITH_NAME: {
			auto& info = *reinterpret_cast<const binary_info_pins64_with_name_t*>(pInfo);
			parsePinsWithName(info.pin_mask, info.label);
			break;
		}
		default:
			break;
		}
	}
	int n = 0;
	const char* labelPrev = nullptr;
	int pinStart = -1;
	for (int pin = 0; pin < count_of(labelTbl); ++pin) {
		const char* label = labelTbl[pin];
		if (label) {
			n++;
			if (!labelPrev) {
				pinStart = pin;
			} else if (!isLabelMatched(label, labelPrev)) {
				printPinFunc(pinStart, pin - 1, labelPrev);
				pinStart = pin;
			}
		} else if (labelPrev) {
			printPinFunc(pinStart, pin - 1, labelPrev);
			pinStart = -1;
		}
		labelPrev = label;
	}
	if (n == 0) tout.Printf(" none\n");
}

void PrintMemoryMap(Printable& tout)
{
	tout.Printf("Memory Map\n");
	tout.Printf(" flash binary:      0x%08x-0x%08x %7d\n", &__flash_binary_start, &__flash_binary_end, &__flash_binary_end - &__flash_binary_start);
#if 0
	tout.Printf(" ram vector table:  0x%08x-0x%08x %7d\n", &ram_vector_table, &ram_vector_table + PICO_RAM_VECTOR_TABLE_SIZE, PICO_RAM_VECTOR_TABLE_SIZE * sizeof(void*));
#endif
	tout.Printf(" data:              0x%08x-0x%08x %7d\n", &__data_start__, &__data_end__, &__data_end__ - &__data_start__);
	tout.Printf(" bss:               0x%08x-0x%08x %7d\n", &__bss_start__, &__bss_end__, &__bss_end__ - &__bss_start__);
	tout.Printf(" heap:              0x%08x-0x%08x %7d\n", &__heap_start, &__heap_end, &__heap_end - &__heap_start);
	tout.Printf(" stack:             0x%08x-0x%08x %7d\n", &__heap_end, &__stack, &__stack - &__heap_end);
}

void Print(Printable& tout)
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

bool GetInt(uint32_t id, uint32_t* pValue)
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

bool GetString(uint32_t id, const char** pValue)
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

}
