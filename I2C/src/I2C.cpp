//==============================================================================
// I2C.cpp
//==============================================================================
#include "jxglib/I2C.h"

namespace jxglib {

I2C I2C0{i2c0};
I2C I2C1{i2c1}; 

//------------------------------------------------------------------------------
// I2C
//------------------------------------------------------------------------------
I2C::I2C(i2c_inst_t* i2c) : i2c_{i2c} {}

I2C& I2C::get_instance(uint num)
{
    return (num == 0)? I2C0 : I2C1;
}

int I2C::Read(uint8_t addr, void* dst, size_t len, bool nostop, uint usecTimeout)
{
    return 0;
}

int I2C::Write(uint8_t addr, const void* src, size_t len, bool nostop)
{
    return 0;
}

}

