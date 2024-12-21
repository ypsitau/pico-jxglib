//==============================================================================
// TSC2046.cpp
//==============================================================================
#include "jxglib/TSC2046.h"

namespace jxglib {

//------------------------------------------------------------------------------
// TSC2046
//------------------------------------------------------------------------------
TSC2046::TSC2046(spi_inst_t* spi, const GPIO& gpio_CS, const GPIO& gpio_IRQ) :
		spi_{spi}, gpio_CS_{gpio_CS}, gpio_IRQ_{gpio_IRQ} {}

void TSC2046::Initialize()
{
	gpio_CS_.init().set_dir_OUT().put(1);
	gpio_IRQ_.init().set_dir_IN();
	SendCmd(ComposeCmd(0, 0, 0, 0));
}

bool TSC2046::IsTouched()
{
	return !gpio_IRQ_.get();
}

void TSC2046::SendCmd(uint8_t cmd)
{
	uint8_t buffSend[3] = { cmd, 0x00, 0x00 };
	uint8_t buffRecv[3];
	gpio_CS_.put(0);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	::spi_write_read_blocking(spi_, buffSend, buffRecv, sizeof(buffSend));
	gpio_CS_.put(1);
}

uint16_t TSC2046::ReadADC(uint8_t adc)
{
	uint8_t buffSend[3] = {
		ComposeCmd(
			adc,	// ADC .. 0b001:Y, 0b011:Z1, 0b100:Z2, 0b101:X
			0b0,	// Mode .. 12 bit
			0b1,	// Reference .. Single-end
			0b01), 	// Power Down Mode .. Reference Off, ADC On
		0x00, 0x00,		// dummy data to squeeze receiving data
	};
	uint8_t buffRecv[3];
	::spi_write_read_blocking(spi_, buffSend, buffRecv, sizeof(buffSend));
	return (static_cast<uint16_t>(buffRecv[1]) << 4) | (buffRecv[2] >> 4);
}

bool TSC2046::ReadPosition(uint16_t* px, uint16_t* py)
{
	uint16_t z1, z2;
	gpio_CS_.put(0);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
	*px = ReadADC(0b101);
	*py = ReadADC(0b001);
	z1 = ReadADC(0b011);
	z2 = ReadADC(0b100);
	gpio_CS_.put(1);
	return z1 > 0 && z2 > 0;
}

}
