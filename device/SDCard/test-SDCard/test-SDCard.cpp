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
	const GPIO& cs_;
	uint baudrate_;
	uint8_t cmdbuf_[6];
	uint8_t dummybuf_[512];
	uint8_t tokenbuf_[1];
	int cdv_;
public:
	SDCard(spi_inst_t* spi, const GPIO& cs, uint baudrate);
	void init_spi(uint baudrate);
	bool init_card();
	bool init_card_v1();
	bool init_card_v2();
	int cmd(uint8_t cmd, uint32_t arg, uint8_t crc, int final = 0, bool release = true, bool skip1 = false);
	bool readinto(uint8_t* buf, int bytes);
	void write(uint8_t token, uint8_t* buf, int bytes);
private:
	void spi_write(uint8_t data) { spi_write_blocking(spi_, &data, 1); }
	void spi_write(const uint8_t* data, int bytes) { spi_write_blocking(spi_, data, bytes); }
	void spi_readinto(uint8_t* data, int bytes, uint8_t repeated_tx_data) { spi_read_blocking(spi_, repeated_tx_data, data, bytes); }
};

SDCard::SDCard(spi_inst_t* spi, const GPIO& cs, uint baudrate) : spi_{spi}, cs_{cs}, baudrate_{baudrate}
{
	for (int i = 0; i < sizeof(dummybuf_); i++) dummybuf_[i] = 0xff;
}

void SDCard::init_spi(uint baudrate)
{
	::spi_init(spi_, baudrate);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
}

bool SDCard::init_card()
{
	// init CS pin
	cs_.set_dir_OUT();
	cs_.put(1);

	// init SPI bus; use low data rate for initialisation
	init_spi(100 * 1000);
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

	// CMD8: determine card version
	int r = cmd(8, 0x01AA, 0x87, 4);
	if (r == _R1_IDLE_STATE) {
		init_card_v2();
	} else if (r == (_R1_IDLE_STATE | _R1_ILLEGAL_COMMAND)) {
		init_card_v1();
	} else {
		return false; // couldn't determine SD card version
	}
	// get the number of sectors
	// CMD9: response R2 (R1 byte + 16-byte block read)
	if (cmd(9, 0, 0, 0, false) != 0) {
		return false; // no response from SD card
	}
	uint8_t csd[16];
	readinto(csd, sizeof(csd));
	int sectors, c_size, c_size_mult, capacity, read_bl_len;
	if (csd[0] & 0xc0 == 0x40) {  // CSD version 2.0
		sectors = ((csd[8] << 8 | csd[9]) + 1) * 1024;
	} else if (csd[0] & 0xC0 == 0x00) { // CSD version 1.0 (old, <=2GB)
		c_size = (csd[6] & 0b11) << 10 | csd[7] << 2 | csd[8] >> 6;
		c_size_mult = (csd[9] & 0b11) << 1 | csd[10] >> 7;
		read_bl_len = csd[5] & 0b1111;
		capacity = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << read_bl_len);
		sectors = capacity / 512;
	} else {
		return false; // SD card CSD format not supported
	}
	// print('sectors', sectors)

	// CMD16: set block length to 512 bytes
	if (cmd(16, 512, 0) != 0) {
		return false; // can't set 512 block size
	}

	// set to high data rate now that it's initialised
	init_spi(baudrate_);
	return true;
}

bool SDCard::init_card_v1()
{
	for (int i = 0; i < _CMD_TIMEOUT; i++) {
		sleep_ms(50);
		cmd(55, 0, 0);
		if (cmd(41, 0, 0) == 0) {
			// SDSC card, uses byte addressing in read/write/erase commands
			cdv_ = 512;
			// print("[SDCard] v1 card")
			return true;
		}
	}
	return false; // timeout waiting for v1 card
}

bool SDCard::init_card_v2()
{
	for (int i = 0; i < _CMD_TIMEOUT; i++) {
		sleep_ms(50);
		cmd(58, 0, 0, 4);
		cmd(55, 0, 0);
		if (cmd(41, 0x40000000, 0) == 0) {
			cmd(58, 0, 0, -4); // 4-byte response, negative means keep the first byte
			uint8_t ocr = tokenbuf_[0];  // get first byte of response, which is OCR
			if (!(ocr & 0x40)) {
				// SDSC card, uses byte addressing in read/write/erase commands
				cdv_ = 512;
			} else {
				// SDHC/SDXC card, uses block addressing in read/write/erase commands
				cdv_ = 1;
			}
			// print("[SDCard] v2 card")
			return true;
		}
	}
	return false; // timeout waiting for v2 card
}

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

int main()
{
	const auto& GPIO_SCK = GPIO2;
	const auto& GPIO_TX = GPIO3;
	const auto& GPIO_RX = GPIO4;
	const auto& GPIO_CS = GPIO5;
	::stdio_init_all();
	spi_inst_t* spi = spi0;
	GPIO_SCK.set_function_SPI0_SCK();
	GPIO_TX.set_function_SPI0_TX();
	GPIO_RX.set_function_SPI0_RX();
	GPIO_CS.init().set_dir_OUT();
	SDCard sdCard(spi, GPIO_CS, 10 * 1000 * 1000);
	sdCard.init_card();
}
