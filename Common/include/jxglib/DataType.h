//==============================================================================
// jxglib/DataType.h
//==============================================================================
#ifndef PICO_JXGLIB_DATATYPE_H
#define PICO_JXGLIB_DATATYPE_H
#include "pico/stdlib.h"

namespace jxglib {

enum class DataType {
	int8,
	int16,
	int32,
	int64,
	uint8,
	uint16,
	uint32,
	uint64,
};

}

#endif
