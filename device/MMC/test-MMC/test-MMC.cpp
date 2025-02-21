#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "ff.h"
#include "diskio.h"
#include "hardware/spi.h"

using namespace jxglib;

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
	// CRC: https://crccalc.com/?crc=4000000000&method=CRC-8/LTE&datatype=hex&outtype=hex
	SendCmd(spi, 0x40 | 0, 0, 0x94 | 0x01);
}

void SendCMD8(spi_inst_t* spi)
{
	// CRC: https://crccalc.com/?crc=48000001aa&method=CRC-8/LTE&datatype=hex&outtype=hex
	SendCmd(spi, 0x40 | 8, 0x1aa, 0x86 | 0x01);
}

#define SPI_CH spi0
#define P_SPI_SCK 2
#define P_SPI_TX 3
#define P_SPI_RX 4
#define P_SPI_CS 5

/* Definitions for MMC/SDC command */
#define CMD0    (0x40+0)    /* GO_IDLE_STATE */
#define CMD1    (0x40+1)    /* SEND_OP_COND (MMC) */
#define CMD16    (0x40+16)    /* SET_BLOCKLEN */
#define CMD17    (0x40+17)    /* READ_SINGLE_BLOCK */

#define CS_HIGH() {gpio_put(P_SPI_CS, 1);}
#define CS_LOW() {gpio_put(P_SPI_CS, 0);}

static void xmit_spi( BYTE d )    /* Send a byte to the SDC/MMC */
{
    BYTE src[ 1 ];

    src[ 0 ] = d;
    spi_write_blocking( SPI_CH, src, 1 );
}

static BYTE rcv_spi( void )        /* Send a 0xFF to the SDC/MMC and get the received byte */
{
    BYTE dst[ 1 ];

    spi_read_blocking( SPI_CH, 0xFF, dst, 1 );

    return ( dst[ 0 ] );
}

static BYTE send_cmd(
    BYTE cmd,        /* 1st byte (Start + Index) */
    DWORD arg        /* Argument (32 bits) */
)
{
    BYTE n, res;

    /* Select the card */
    CS_HIGH();
    rcv_spi();
    CS_LOW();
    rcv_spi();

    /* Send a command packet */
    xmit_spi( cmd );                        /* Start + Command index */
    xmit_spi( (BYTE)( arg >> 24 ) );        /* Argument[31..24] */
    xmit_spi( (BYTE)( arg >> 16 ) );        /* Argument[23..16] */
    xmit_spi( (BYTE)( arg >>  8 ) );            /* Argument[15..8] */
    xmit_spi( (BYTE)arg );                /* Argument[7..0] */
    n = 0x01;                            /* Dummy CRC + Stop */
    if ( cmd == CMD0 )
    {
        n = 0x95;            /* Valid CRC for CMD0(0) */
    }
    xmit_spi( n );

    /* Receive a command response */
    n = 10;                                /* Wait for a valid response in timeout of 10 attempts */
    for ( n = 0; n < 10; n++ )
    {
        res = rcv_spi();
        if ( ( res & 0x80 ) == 0x00 )
        {
            break;
        }
    }

    return ( res );            /* Return with the response value */
}

int main()
{
	const auto& GPIO_SCK = GPIO2;
	const auto& GPIO_TX = GPIO3;
	const auto& GPIO_RX = GPIO4;
	const auto& GPIO_CS = GPIO5;
	::stdio_init_all();
	spi_inst_t* spi = spi0;
#if 0
    {
        spi_init( SPI_CH, 100 * 1000 );
        spi_set_format( SPI_CH, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST );
        gpio_set_function( P_SPI_RX, GPIO_FUNC_SPI );
        gpio_set_function( P_SPI_SCK, GPIO_FUNC_SPI );
        gpio_set_function( P_SPI_TX, GPIO_FUNC_SPI );
		gpio_pull_up(P_SPI_TX);
        gpio_init( P_SPI_CS );
        gpio_set_dir( P_SPI_CS, GPIO_OUT );
    }

    CS_HIGH();
	gpio_set_function(P_SPI_RX, GPIO_FUNC_SIO);
	gpio_set_dir(P_SPI_RX, GPIO_OUT);
	gpio_put(P_SPI_RX, 1);
    /* ダミークロックを80発 */
    {
        BYTE i;
        for ( i= 0; i < 10; i++ )
        {
            rcv_spi();
        }
    }
	gpio_put(P_SPI_RX, 0);
	gpio_set_function( P_SPI_RX, GPIO_FUNC_SPI );
	spi_init( SPI_CH, 10 * 1000 * 1000 );
#else
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
	::spi_init(spi, 10 * 1000 * 1000);
#endif
#if 1
	::printf("----\n");
    {
        BYTE cmd_res;

		/* CMD0を0x01が返ってくるまでトライ */
        for (int i = 0; i < 10000; i++ )
        {
			SendCMD0(spi);
			cmd_res = RecvByte(spi);
			//cmd_res = send_cmd( CMD0, 0 );
            printf("A:%02x\n", cmd_res);
			if ( cmd_res == 0x01 )
            {
                break;
            }
        }

        if ( cmd_res == 0x01 )/* Enter Idle state */
        {
            /* CMD1を0x00が返ってくるまでトライ */
            for (int i = 0; i < 10000; i++ )
            {
                cmd_res = send_cmd( CMD1, 0 );
				printf("B:%02x\n", cmd_res);
                if ( cmd_res == 0x00 )
                {
                    /* 初期化完了 */
                    //stat = 0;
                    break;
                }
                sleep_us( 100 );
            }

            /* CMD16でブロックサイズを512バイトに設定 */
            send_cmd( CMD16, 512 );
        }
    }
#endif
#if 0
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
#endif
}

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
