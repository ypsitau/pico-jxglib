//==============================================================================
// SPI.cpp
//==============================================================================
#include "jxglib/SPI.h"

namespace jxglib {

SPI SPI0{spi0};
SPI SPI1{spi1}; 

//------------------------------------------------------------------------------
// SPI
//------------------------------------------------------------------------------
SPI::SPI(spi_inst_t* spi) : spi_{spi} {}

SPI& SPI::get_instance(uint num)
{
    return (num == 0)? SPI0 : SPI1;
}

int SPI::Read(uint8_t* dst, size_t len, uint8_t repeated_tx_data)
{
    return ::spi_read_blocking(spi_, repeated_tx_data, dst, len);
}

int SPI::Write(const uint8_t* src, size_t len)
{
    return ::spi_write_blocking(spi_, src, len);
}

int SPI::WriteRead(const uint8_t* src, uint8_t* dst, size_t len)
{
    return ::spi_write_read_blocking(spi_, src, dst, len);
}

}
