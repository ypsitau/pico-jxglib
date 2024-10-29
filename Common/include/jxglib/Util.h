//==============================================================================
// jxglib/Util.h
//==============================================================================
#ifndef PICO_JXGLIB_UTIL_h
#define PICO_JXGLIB_UTIL_H
#include "pico/stdlib.h"

namespace jxglib {

template<typename T> T ChooseMin(T a, T b) { return (a < b)? a : b; }
template<typename T> T ChooseMax(T a, T b) { return (a > b)? a : b; }

template<typename T>
void SortPair(int v1, int v2, int* pMin, int* pMax)
{
	if (v1 < v2) {
		*pMin = v1, *pMax = v2;
	} else {
		*pMin = v2, *pMax = v1;
	}
}

}

#endif
