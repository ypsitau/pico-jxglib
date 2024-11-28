//==============================================================================
// jxglib/Util.h
//==============================================================================
#ifndef PICO_JXGLIB_UTIL_H
#define PICO_JXGLIB_UTIL_H
#include "pico/stdlib.h"

namespace jxglib {

template<typename T> T ChooseMin(T a, T b) { return (a < b)? a : b; }
template<typename T> T ChooseMax(T a, T b) { return (a > b)? a : b; }

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
	T& value = *pValue, &range = *pRange;
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
	return true;
}

template<typename T>
bool AdjustRange(T* pValue, T* pRange, T valueMin, T valueExceed, T* pValueSkip)
{
	T& value = *pValue, &range = *pRange, &valueSkip = *pValueSkip;
	valueSkip = 0;
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
	return true;
}

}

#endif
