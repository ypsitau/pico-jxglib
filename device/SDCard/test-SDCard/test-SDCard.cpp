// original: https://github.com/micropython/micropython-lib
#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "ff.h"
#include "diskio.h"
#include "hardware/spi.h"

using namespace jxglib;

class SDCard {
	static const int _CMD_TIMEOUT = 100;
	static const uint8_t _R1_IDLE_STATE = 1 << 0;
	//static const uint8_t _R1_ERASE_RESET = 1 << 1;
	static const uint8_t _R1_ILLEGAL_COMMAND = 1 << 2;
	//static const uint8_t _R1_COM_CRC_ERROR = 1 << 3;
	//static const uint8_t _R1_ERASE_SEQUENCE_ERROR = 1 << 4;
	//static const uint8_t _R1_ADDRESS_ERROR = 1 << 5;
	//static const uint8_t _R1_PARAMETER_ERROR = 1 << 6;
	static const uint8_t _TOKEN_CMD25 = 0xFC;
	static const uint8_t _TOKEN_STOP_TRAN = 0xFD;
	static const uint8_t _TOKEN_DATA = 0xFE;
private:
	spi_inst_t* spi_;
	GPIO& cs_;
	uint baudrate_;
	uint8_t cmdbuf_[6];
	uint8_t dummybuf_[512];
	uint8_t tokenbuf_[1];
public:
	SDCard(spi_inst_t* spi, GPIO& cs, uint baudrate);
	void init_spi(uint baudrate);
	bool init_card(uint baudrate);
	void init_card_v1();
	void init_card_v2();
	int cmd(uint8_t cmd, uint32_t arg, uint8_t crc, int final = 0, bool release = true, bool skip1 = false);
	bool readinto(uint8_t* buf, int bytes);
	void write(uint8_t token, uint8_t* buf, int bytes);
private:
	void spi_write(uint8_t data) { spi_write_blocking(spi_, &data, 1); }
	void spi_write(const uint8_t* data, int bytes) { spi_write_blocking(spi_, data, bytes); }
	void spi_readinto(uint8_t* data, int bytes, uint8_t repeated_tx_data) { spi_read_blocking(spi_, repeated_tx_data, data, bytes); }
};

SDCard::SDCard(spi_inst_t* spi, GPIO& cs, uint baudrate) : spi_{spi}, cs_{cs}, baudrate_{baudrate}
{
	for (int i = 0; i < sizeof(dummybuf_); i++) dummybuf_[i] = 0xff;
}

void SDCard::init_spi(uint baudrate)
{
	::spi_init(spi_, baudrate);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
}

bool SDCard::init_card(uint baudrate)
{
	// init CS pin
	cs_.set_dir_OUT();
	cs_.put(1);

	// init SPI bus; use low data rate for initialisation
	init_spi(100000);
	// clock card at least 100 cycles with cs high
	for (int i = 0; i < 16; i++) spi_write(0xff);

	// CMD0: init card; should return _R1_IDLE_STATE (allow 5 attempts)
	bool successFlag = false;
	for (int i = 0; i < 5; i++) {
		if (cmd(0, 0, 0x95) == _R1_IDLE_STATE) {
			successFlag = true;
			break;
		}
	}
	if (!successFlag) return false; // no SD card

#if 0
	// CMD8: determine card version
	r = cmd(8, 0x01AA, 0x87, 4)
	if r == _R1_IDLE_STATE:
		init_card_v2()
	elif r == (_R1_IDLE_STATE | _R1_ILLEGAL_COMMAND):
		init_card_v1()
	else:
		raise OSError("couldn't determine SD card version")

	// get the number of sectors
	// CMD9: response R2 (R1 byte + 16-byte block read)
	if cmd(9, 0, 0, 0, False) != 0:
		raise OSError("no response from SD card")
	csd = bytearray(16)
	readinto(csd)
	if csd[0] & 0xC0 == 0x40:  // CSD version 2.0
		sectors = ((csd[8] << 8 | csd[9]) + 1) * 1024
	elif csd[0] & 0xC0 == 0x00:  // CSD version 1.0 (old, <=2GB)
		c_size = (csd[6] & 0b11) << 10 | csd[7] << 2 | csd[8] >> 6
		c_size_mult = (csd[9] & 0b11) << 1 | csd[10] >> 7
		read_bl_len = csd[5] & 0b1111
		capacity = (c_size + 1) * (2 ** (c_size_mult + 2)) * (2**read_bl_len)
		sectors = capacity // 512
	else:
		raise OSError("SD card CSD format not supported")
	// print('sectors', sectors)

	// CMD16: set block length to 512 bytes
	if cmd(16, 512, 0) != 0:
		raise OSError("can't set 512 block size")

#endif
	// set to high data rate now that it's initialised
	init_spi(baudrate);
	return true;
}

/*
	def init_card_v1(self):
		for i in range(_CMD_TIMEOUT):
			time.sleep_ms(50)
			self.cmd(55, 0, 0)
			if self.cmd(41, 0, 0) == 0:
				// SDSC card, uses byte addressing in read/write/erase commands
				self.cdv = 512
				// print("[SDCard] v1 card")
				return
		raise OSError("timeout waiting for v1 card")

	def init_card_v2(self):
		for i in range(_CMD_TIMEOUT):
			time.sleep_ms(50)
			self.cmd(58, 0, 0, 4)
			self.cmd(55, 0, 0)
			if self.cmd(41, 0x40000000, 0) == 0:
				self.cmd(58, 0, 0, -4)  // 4-byte response, negative means keep the first byte
				ocr = self.tokenbuf[0]  // get first byte of response, which is OCR
				if not ocr & 0x40:
					// SDSC card, uses byte addressing in read/write/erase commands
					self.cdv = 512
				else:
					// SDHC/SDXC card, uses block addressing in read/write/erase commands
					self.cdv = 1
				// print("[SDCard] v2 card")
				return
		raise OSError("timeout waiting for v2 card")
*/

int SDCard::cmd(uint8_t cmd, uint32_t arg, uint8_t crc, int final, bool release, bool skip1)
{
	cs_.put(0);
	// create and send the command
	uint8_t* buf = cmdbuf_;
	buf[0] = 0x40 | cmd;
	buf[1] = static_cast<uint8_t>(arg >> 24);
	buf[2] = static_cast<uint8_t>(arg >> 16);
	buf[3] = static_cast<uint8_t>(arg >> 8);
	buf[4] = static_cast<uint8_t>(arg);
	buf[5] = crc;
	spi_write(buf, 6);
	if (skip1) spi_readinto(tokenbuf_, 1, 0xff);

	// wait for the response (response[7] == 0)
	for (int i = 0; i < _CMD_TIMEOUT; i++) {
		spi_readinto(tokenbuf_, 1, 0xff);
		uint8_t response = tokenbuf_[0];
		if (!(response & 0x80)) {
			// this could be a big-endian integer that we are getting here
			// if final<0 then store the first byte to tokenbuf and discard the rest
			if (final < 0) {
				spi_readinto(tokenbuf_, 1, 0xff);
				final = -1 - final;
			}
			for (int j = 0; j < final; j++) {
				spi_write(0xff);
			}
			if (release) {
				cs_.put(1);
				spi_write(0xff);
			}
			return response;
		}
	}
	// timeout
	cs_.put(1);
	spi_write(0xff);
	return -1;
}

bool SDCard::readinto(uint8_t* buf, int bytes)
{
	cs_.put(0);
	// read until start byte (0xff)
	bool successFlag = false;
	for (int i = 0; i < _CMD_TIMEOUT; i++) {
		spi_readinto(tokenbuf_, 1, 0xff);
		if (tokenbuf_[0] == _TOKEN_DATA) {
			successFlag = true;
			break;
		}
		::sleep_ms(1);
	}
	if (!successFlag) {
		cs_.put(1);
		return false; // timeout waiting for response
	}
	// read data
	//mv = self.dummybuf_memoryview
	//if len(buf) != len(mv):
	//	mv = mv[: len(buf)]
	spi_readinto(buf, bytes, 0xff);
	// read checksum
	spi_write(0xff);
	spi_write(0xff);
	cs_.put(1);
	spi_write(0xff);
	return true;
}

/*
	def write(self, token, buf):
		self.cs(0)

		// send: start of block, data, checksum
		self.spi.read(1, token)
		self.spi.write(buf)
		self.spi.write(b"\xff")
		self.spi.write(b"\xff")

		// check the response
		if (self.spi.read(1, 0xFF)[0] & 0x1F) != 0x05:
			self.cs(1)
			self.spi.write(b"\xff")
			return

		// wait for write to finish
		while self.spi.read(1, 0xFF)[0] == 0:
			pass

		self.cs(1)
		self.spi.write(b"\xff")

	def write_token(self, token):
		self.cs(0)
		self.spi.read(1, token)
		self.spi.write(b"\xff")
		// wait for write to finish
		while self.spi.read(1, 0xFF)[0] == 0x00:
			pass

		self.cs(1)
		self.spi.write(b"\xff")

	def readblocks(self, block_num, buf):
		// workaround for shared bus, required for (at least) some Kingston
		// devices, ensure MOSI is high before starting transaction
		self.spi.write(b"\xff")

		nblocks = len(buf) // 512
		assert nblocks and not len(buf) % 512, "Buffer length is invalid"
		if nblocks == 1:
			// CMD17: set read address for single block
			if self.cmd(17, block_num * self.cdv, 0, release=False) != 0:
				// release the card
				self.cs(1)
				raise OSError(5)  // EIO
			// receive the data and release card
			self.readinto(buf)
		else:
			// CMD18: set read address for multiple blocks
			if self.cmd(18, block_num * self.cdv, 0, release=False) != 0:
				// release the card
				self.cs(1)
				raise OSError(5)  // EIO
			offset = 0
			mv = memoryview(buf)
			while nblocks:
				// receive the data and release card
				self.readinto(mv[offset : offset + 512])
				offset += 512
				nblocks -= 1
			if self.cmd(12, 0, 0xFF, skip1=True):
				raise OSError(5)  // EIO

	def writeblocks(self, block_num, buf):
		// workaround for shared bus, required for (at least) some Kingston
		// devices, ensure MOSI is high before starting transaction
		self.spi.write(b"\xff")

		nblocks, err = divmod(len(buf), 512)
		assert nblocks and not err, "Buffer length is invalid"
		if nblocks == 1:
			// CMD24: set write address for single block
			if self.cmd(24, block_num * self.cdv, 0) != 0:
				raise OSError(5)  // EIO

			// send the data
			self.write(_TOKEN_DATA, buf)
		else:
			// CMD25: set write address for first block
			if self.cmd(25, block_num * self.cdv, 0) != 0:
				raise OSError(5)  // EIO
			// send the data
			offset = 0
			mv = memoryview(buf)
			while nblocks:
				self.write(_TOKEN_CMD25, mv[offset : offset + 512])
				offset += 512
				nblocks -= 1
			self.write_token(_TOKEN_STOP_TRAN)

	def ioctl(self, op, arg):
		if op == 4:  // get number of blocks
			return self.sectors
		if op == 5:  // get block size in bytes
			return 512
*/

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

#if 1
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
#endif
	return ( res );            /* Return with the response value */
}

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

uint8_t SendCmd(spi_inst_t* spi, uint8_t cmd, uint32_t arg, uint8_t crc = 0x01)
{
	//CS_HIGH();
	//rcv_spi();
	//CS_LOW();
	//rcv_spi();
	uint8_t buff[6];
	buff[0] = cmd;
	buff[1] = static_cast<uint8_t>(arg >> 24);
	buff[2] = static_cast<uint8_t>(arg >> 16);
	buff[3] = static_cast<uint8_t>(arg >> 8);
	buff[4] = static_cast<uint8_t>(arg >> 0);
	buff[5] = crc;
	::spi_write_blocking(spi, buff, sizeof(buff));
#if 0
	SendByte(spi, cmd);
	SendByte(spi, static_cast<uint8_t>(arg >> 24));
	SendByte(spi, static_cast<uint8_t>(arg >> 16));
	SendByte(spi, static_cast<uint8_t>(arg >> 8));
	SendByte(spi, static_cast<uint8_t>(arg >> 0));
	SendByte(spi, crc);
#endif
	uint8_t rtn;
	for (int i = 0; i < 10; i++) if (!((rtn = RecvByte(spi)) & 0x80)) break;
	return rtn;
}

uint8_t SendCMD0(spi_inst_t* spi)
{
	// CRC: https://crccalc.com/?crc=4000000000&method=CRC-8/LTE&datatype=hex&outtype=hex
	return SendCmd(spi, 0x40 | 0, 0, 0x94 | 0x01);
}

uint8_t SendCMD1(spi_inst_t* spi)
{
	// CRC: https://crccalc.com/?crc=4100000000&method=CRC-8/LTE&datatype=hex&outtype=hex
	return SendCmd(spi, 0x40 | 1, 0, 0x71 | 0x01);
}

uint8_t SendCMD8(spi_inst_t* spi)
{
	// CRC: https://crccalc.com/?crc=48000001aa&method=CRC-8/LTE&datatype=hex&outtype=hex
	return SendCmd(spi, 0x40 | 8, 0x1aa, 0x86 | 0x01);
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
#if 0
	{
		BYTE cmd_res;

		/* CMD0を0x01が返ってくるまでトライ */
		for (int i = 0; i < 10000; i++ )
		{
			cmd_res = SendCMD0(spi);
			//cmd_res = RecvByte(spi);
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
#if 1
	::printf("----\n");
	for (;;) {
		uint8_t rtn = SendCMD0(spi);
		::printf("%02x\n", rtn);
		if (rtn == 0x01) break;
		::sleep_ms(100);
	}
	for (;;) {
		uint8_t rtn = SendCMD1(spi);
		::printf("%02x\n", rtn);
		if (rtn == 0x00) break;
		::sleep_ms(100);
	}
	do {
		uint8_t rtn = SendCMD8(spi);
		::printf("%02x\n", rtn);
	} while (0);
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
