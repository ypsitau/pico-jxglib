#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "ff.h"
#include "diskio.h"
#include "hardware/spi.h"

using namespace jxglib;

#if 1

void SendByte(spi_inst_t* spi, uint8_t data)
{
	::spi_write_blocking(spi, &data, 1);
}

uint8_t RecvByte(spi_inst_t* spi)
{
	uint8_t rtn;
	::spi_read_blocking(spi, 0xff, &rtn, 1);
	return rtn;
}

void SendCmd(spi_inst_t* spi, uint8_t cmd, uint32_t arg, uint8_t crc = 0x01)
{
	//uint8_t buff[6];
	//buff[0] = cmd;
	//buff[1] = static_cast<uint8_t>(arg >> 24);
	//buff[2] = static_cast<uint8_t>(arg >> 16);
	//buff[3] = static_cast<uint8_t>(arg >> 8);
	//buff[4] = static_cast<uint8_t>(arg >> 0);
	//buff[5] = crc;
	//::spi_write_blocking(spi, buff, sizeof(buff));
	SendByte(spi, cmd);
	SendByte(spi, static_cast<uint8_t>(arg >> 24));
	SendByte(spi, static_cast<uint8_t>(arg >> 16));
	SendByte(spi, static_cast<uint8_t>(arg >> 8));
	SendByte(spi, static_cast<uint8_t>(arg >> 0));
	SendByte(spi, crc);
}

void SendCMD0(spi_inst_t* spi)
{
	// https://crccalc.com/?crc=4000000000&method=CRC-8/LTE&datatype=hex&outtype=hex
	SendCmd(spi, 0x40, 0, 0x94 | 0x01);
}

void SendCMD8(spi_inst_t* spi)
{
	// https://crccalc.com/?crc=48000001aa&method=CRC-8/LTE&datatype=hex&outtype=hex
	SendCmd(spi, 0x48, 0x1aa, 0x86 | 0x01);
}


int main()
{
	const auto& GPIO_SCK = GPIO2;
	const auto& GPIO_TX = GPIO3;
	const auto& GPIO_RX = GPIO4;
	const auto& GPIO_CS = GPIO5;
	::stdio_init_all();
	spi_inst_t* spi = spi0;
	GPIO_SCK.set_function_SPI0_SCK();
	GPIO_TX.set_function_SPI0_TX().pull_up();
	GPIO_RX.set_function_SPI0_RX();
	GPIO_CS.init().set_dir_OUT();
	::spi_init(spi, 100 * 1000);
	::spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
	GPIO_TX.init().set_dir_OUT();
	GPIO_TX.put(1);	// DI
	GPIO_CS.put(1);	// CS
	::sleep_ms(10);	// at least 1 msec
	for (int i = 0; i < 80; i++) RecvByte(spi);
	GPIO_TX.init().set_function_SPI0_TX();
	GPIO_CS.put(0);	// CS
	::printf("----\n");
	for (;;) {
		SendCMD0(spi);
		uint8_t rtn = RecvByte(spi);
		::printf("%02x\n", rtn);
		if (rtn == 0x01) break;
		::sleep_ms(1000);
	}
	uint8_t rtn;
	SendCMD8(spi);
	rtn = RecvByte(spi);
	::printf("%02xn", rtn);

}
#endif
#if 0
extern "C" DRESULT disk_readp (
    BYTE* buff,        /* Pointer to the destination object */
    DWORD sector,    /* Sector number (LBA) */
    UINT offset,    /* Offset in the sector */
    UINT count        /* Byte count (bit15:destination) */
);


int main()
{
	::stdio_init_all();
	BYTE buff[512];
	::memset(buff, 0x00, sizeof(buff));
	disk_initialize(0);

	::memset(buff, 0x00, sizeof(buff));
	disk_readp(buff, 0, 0, 1);
	Dump.Ascii()(buff, 512);
	//for (int i = 0; i < sizeof(buff); i++) {
	//	buff[i] = static_cast<BYTE>(i);
	//}
	//disk_write(0, buff, 0, 1);
	//::memset(buff, 0x00, sizeof(buff));
	//for (int i = 0; i < 2; i++) {
	//	disk_read(0, buff, i, 1);
	//	//disk_readp(buff, i, 0, 1);
	//	::printf("%d\n", i);
	//	Dump.Ascii()(buff, 512);
	//}
}
#endif
