//==============================================================================
// jxglib/Dir.h
//==============================================================================
#ifndef PICO_JXGLIB_DIR_H
#define PICO_JXGLIB_DIR_H
#include "pico/stdlib.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Dir
//------------------------------------------------------------------------------
enum class Dir {
	None	= 0,
	Up		= (1 << 0),
	Down	= (1 << 1),
	Left	= (1 << 2),
	Right	= (1 << 3),
};

//------------------------------------------------------------------------------
// DirVert
//------------------------------------------------------------------------------
enum class DirVert {
	None	= 0,
	Up		= (1 << 0),
	Down	= (1 << 1),
};

//------------------------------------------------------------------------------
// DirHorz
//------------------------------------------------------------------------------
enum class DirHorz {
	None	= 0,
	Left	= (1 << 2),
	Right	= (1 << 3),
};

}

#endif
