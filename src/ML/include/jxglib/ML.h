//==============================================================================
// jxglib/ML.h
//==============================================================================
#ifndef PICO_JXGLIB_ML_H
#define PICO_JXGLIB_ML_H
#include "pico/stdlib.h"
#include "jxglib/Common.h"

namespace jxglib::ML {

//------------------------------------------------------------------------------
// ML
//------------------------------------------------------------------------------
template<typename T> size_t ArgMax(const T* data, size_t size, T* pValueMax = nullptr)
{
	size_t indexMax = 0;
	T valueMax = data[0];
	for (size_t i = 1; i < size; i++) {
		if (data[i] > valueMax) {
			valueMax = data[i];
			indexMax = i;
		}
	}
	if (pValueMax) *pValueMax = valueMax;
	return indexMax;
}

}

#endif
