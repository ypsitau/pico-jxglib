//==============================================================================
// jxglib/Interp.h
//==============================================================================
#ifndef PICO_JXGLIB_SIZE_H
#define PICO_JXGLIB_SIZE_H
#include "pico/stdlib.h"
#include "hardware/interp.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Interp
//------------------------------------------------------------------------------
class Interp {
public:
	class Config {
	private:
		interp_config config_;
	public:
		Config(const interp_config& config) : config_{config} {}
	}
private:
	interp_hw_t* interp_;
public:
	Interp(interp_hw_t* interp) : interp_{interp} {}
public:

};

}

#endif
