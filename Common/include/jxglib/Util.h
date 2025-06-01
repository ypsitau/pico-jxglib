//==============================================================================
// jxglib/Util.h
//==============================================================================
#ifndef PICO_JXGLIB_UTIL_H
#define PICO_JXGLIB_UTIL_H
#include "pico/stdlib.h"

namespace jxglib {

template<typename T1, typename T2> T1 ChooseMin(T1 v1, T2 v2) { return (v1 < static_cast<T1>(v2))? v1 : static_cast<T1>(v2); }
template<typename T1, typename T2> T1 ChooseMax(T1 v1, T2 v2) { return (v1 > static_cast<T1>(v2))? v1 : static_cast<T1>(v2); }

template<typename T> void Swap(T* a, T* b) { T tmp = *a; *a = *b, *b = tmp; }

template<typename T>
void SortPair(T value1, T value2, T* pValueMin, T* pValueMax)
{
	if (value1 < value2) {
		*pValueMin = value1, *pValueMax = value2;
	} else {
		*pValueMin = value2, *pValueMax = value1;
	}
}

template<typename T>
bool CheckRange(T value, T valueMin, T valueExceed) { return valueMin <= value && value < valueExceed; }

template<typename T>
bool AdjustRange(T* pValue, T* pRange, T valueMin, T valueExceed)
{
	T value = *pValue, range = *pRange;
	if (range == 0) return false;
	if (range < 0) {
		value += range + 1, range = -range;
	}
	if (value < valueMin) {
		T valueSkip = valueMin - value;
		range -= valueSkip, value = valueMin;
		if (range <= 0) return false;
	}
	if (value >= valueExceed) return false;
	if (value + range > valueExceed) {
		range = valueExceed - value;
	}
	*pValue = value, *pRange = range;
	return true;
}

template<typename T>
bool AdjustRange(T* pValue, T* pRange, T valueMin, T valueExceed, T* pValueSkip)
{
	T value = *pValue, range = *pRange;
	int valueSkip = 0;
	*pValueSkip = 0;
	if (range == 0) return false;
	if (range < 0) {
		value += range + 1, range = -range;
	}
	if (value < valueMin) {
		valueSkip = valueMin - value;
		range -= valueSkip, value = valueMin;
		if (range <= 0) return false;
	}
	if (value >= valueExceed) return false;
	if (value + range > valueExceed) {
		range = valueExceed - value;
	}
	*pValue = value, *pRange = range, *pValueSkip = valueSkip;
	return true;
}

template<typename T = int32_t> T SignExtend(uint32_t value, int nBits)
{
	int nBitsShift = 32 - nBits;
	return static_cast<T>(static_cast<int32_t>(value << nBitsShift) >> nBitsShift);
}

void WaitMSecSinceBoot(uint32_t msec);

bool StartsWith(const char* str, const char* prefix);
bool StartsWithICase(const char* str, const char* prefix);
bool EndsWith(const char* str, const char* suffix);
bool EndsWithICase(const char* str, const char* suffix);
bool DoesContainSpace(const char* str);

const char* GetPlatformName();
const char* GetCPUArchName();

}

#endif
