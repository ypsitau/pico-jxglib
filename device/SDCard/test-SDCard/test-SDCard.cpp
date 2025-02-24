// original: https://github.com/micropython/micropython-lib
#include <stdio.h>
#include <memory.h>
#include "pico/stdlib.h"
#include "jxglib/Stream.h"
#include "ff.h"
#include "diskio.h"
#include "hardware/spi.h"
#include "jxglib/PackedNumber.h"

using namespace jxglib;


class SDCard {
public:
	struct Partition {
		uint8_t BootID;
		uint8_t Start_Cylinder;
		uint8_t Start_Header;
		uint8_t Start_Sector;
		uint8_t System;
		uint8_t End_Cylinder;
		uint8_t End_Header;
		uint8_t End_Sector;
		Packed_uint32(LbaOfs);
		Packed_uint32(LbaSize);
	};
private:
	static const int _CMD_TIMEOUT					= 100;
	static const uint8_t _R1_IDLE_STATE				= 1 << 0;
	static const uint8_t _R1_ERASE_RESET			= 1 << 1;
	static const uint8_t _R1_ILLEGAL_COMMAND		= 1 << 2;
	static const uint8_t _R1_COM_CRC_ERROR			= 1 << 3;
	static const uint8_t _R1_ERASE_SEQUENCE_ERROR	= 1 << 4;
	static const uint8_t _R1_ADDRESS_ERROR			= 1 << 5;
	static const uint8_t _R1_PARAMETER_ERROR		= 1 << 6;
	static const uint8_t _TOKEN_CMD25				= 0xfc;
	static const uint8_t _TOKEN_STOP_TRAN			= 0xfd;
	static const uint8_t _TOKEN_DATA				= 0xfe;
public:
	static const uint baudrateSlow = 100 * 1000;	// 100kHz
private:
	spi_inst_t* spi_;
	const GPIO& cs_;
	uint baudrate_;
	uint8_t cmdbuf_[6];
	uint8_t tokenbuf_[1];
	int cdv_;
public:
	SDCard(spi_inst_t* spi, const GPIO& cs, uint baudrate);
	bool Initialize();
	bool ReadBlock(int lba, uint8_t* buf, int nBlocks);
	bool WriteBlock(int lba, const uint8_t* buf, int nBlocks);
public:
	static void PrintMBR(const uint8_t* bufSector);
private:
	int cmd(uint8_t cmd, uint32_t arg, uint8_t crc, int final = 0, bool release = true, bool skip1 = false);
	bool readinto(uint8_t* buf, int bytes);
private:
	bool write(uint8_t token, const uint8_t* buf, int bytes);
	bool write_token(uint8_t token);
	void SPIRead(uint8_t* data, int bytes, uint8_t repeated_tx_data) { spi_read_blocking(spi_, repeated_tx_data, data, bytes); }
	void SPIWrite(const uint8_t* data, int bytes) { spi_write_blocking(spi_, data, bytes); }
	void SPIWriteByte(uint8_t data) { spi_write_blocking(spi_, &data, 1); }
};

SDCard::SDCard(spi_inst_t* spi, const GPIO& cs, uint baudrate) : spi_{spi}, cs_{cs}, baudrate_{baudrate}
{
}

bool SDCard::Initialize()
{
	// init CS pin
	cs_.put(1);

	// init SPI bus; use low data rate for initialisation
	::spi_init(spi_, baudrateSlow);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
	// clock card at least 100 cycles with cs high
	for (int i = 0; i < 16; i++) SPIWriteByte(0xff);

	::printf("check %d\n", __LINE__);
	// CMD0: init card; should return _R1_IDLE_STATE (allow 5 attempts)
	bool successFlag = false;
	for (int i = 0; i < 5; i++) {
		if (cmd(0, 0, 0x95) == _R1_IDLE_STATE) {
			successFlag = true;
			break;
		}
	}
	if (!successFlag) return false; // no SD card
	::printf("check %d\n", __LINE__);
	// CMD8: determine card version
	int r = cmd(8, 0x01aa, 0x87, 4);
	if (r == _R1_IDLE_STATE) {
		// v2 card
		successFlag = false;
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
				successFlag = true;
				break;
			}
		}
		if (!successFlag) return false;
	} else if (r == (_R1_IDLE_STATE | _R1_ILLEGAL_COMMAND)) {
		// v1 card
		successFlag = false;
		for (int i = 0; i < _CMD_TIMEOUT; i++) {
			sleep_ms(50);
			cmd(55, 0, 0);
			if (cmd(41, 0, 0) == 0) {
				// SDSC card, uses byte addressing in read/write/erase commands
				cdv_ = 512;
				// print("[SDCard] v1 card")
				successFlag = true;
				break;
			}
		}
		if (!successFlag) return false;
	} else {
		return false; // couldn't determine SD card version
	}
	::printf("check %d\n", __LINE__);
	// get the number of sectors
	// CMD9: response R2 (R1 byte + 16-byte block read)
	if (cmd(9, 0, 0, 0, false) != 0) {
		return false; // no response from SD card
	}
	::printf("check %d\n", __LINE__);
	uint8_t csd[16];
	::memset(csd, 0x00, sizeof(csd));
	readinto(csd, sizeof(csd));
	Dump.NoAddr()(csd, sizeof(csd));
	int sectors, c_size, c_size_mult, capacity, read_bl_len;
	if ((csd[0] & 0xc0) == 0x40) {  // CSD version 2.0
		sectors = ((csd[8] << 8 | csd[9]) + 1) * 1024;
	} else if ((csd[0] & 0xc0) == 0x00) { // CSD version 1.0 (old, <=2GB)
		c_size = (csd[6] & 0b11) << 10 | csd[7] << 2 | csd[8] >> 6;
		c_size_mult = (csd[9] & 0b11) << 1 | csd[10] >> 7;
		read_bl_len = csd[5] & 0b1111;
		capacity = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << read_bl_len);
		sectors = capacity / 512;
	} else {
		return false; // SD card CSD format not supported
	}
	printf("sectors: %d\n", sectors);
	::printf("check %d\n", __LINE__);

	// CMD16: set block length to 512 bytes
	if (cmd(16, 512, 0) != 0) {
		return false; // can't set 512 block size
	}

	::printf("check %d\n", __LINE__);
	// set to high data rate now that it's initialised
	::spi_init(spi_, baudrate_);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
	return true;
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
	SPIWrite(buf, 6);
	if (skip1) SPIRead(tokenbuf_, 1, 0xff);

	// wait for the response (response[7] == 0)
	for (int i = 0; i < _CMD_TIMEOUT; i++) {
		SPIRead(tokenbuf_, 1, 0xff);
		uint8_t response = tokenbuf_[0];
		if (!(response & 0x80)) {
			// this could be a big-endian integer that we are getting here
			// if final<0 then store the first byte to tokenbuf and discard the rest
			if (final < 0) {
				SPIRead(tokenbuf_, 1, 0xff);
				final = -1 - final;
			}
			for (int j = 0; j < final; j++) {
				SPIWriteByte(0xff);
			}
			if (release) {
				cs_.put(1);
				SPIWriteByte(0xff);
			}
			return response;
		}
	}
	// timeout
	cs_.put(1);
	SPIWriteByte(0xff);
	return -1;
}

bool SDCard::readinto(uint8_t* buf, int bytes)
{
	cs_.put(0);
	// read until start byte (0xff)
	bool successFlag = false;
	for (int i = 0; i < _CMD_TIMEOUT; i++) {
		SPIRead(tokenbuf_, 1, 0xff);
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
	SPIRead(buf, bytes, 0xff);
	// read checksum
	SPIWriteByte(0xff);
	SPIWriteByte(0xff);
	cs_.put(1);
	SPIWriteByte(0xff);
	return true;
}

bool SDCard::write(uint8_t token, const uint8_t* buf, int bytes)
{
	cs_.put(0);
	// send: start of block, data, checksum
	uint8_t rtn;
	SPIRead(&rtn, 1, token);
	SPIWrite(buf, bytes);
	SPIWriteByte(0xff);
	SPIWriteByte(0xff);
	// check the response
	SPIRead(&rtn, 1, 0xff);
	if ((rtn & 0x1f) != 0x05) {
		cs_.put(1);
		SPIWriteByte(0xff);
		return false;
	}
	// wait for write to finish
	for (;;) {
		SPIRead(&rtn, 1, 0xff);
		if (rtn != 0x00) break;
	}
	cs_.put(1);
	SPIWriteByte(0xff);
	return true;
}

bool SDCard::write_token(uint8_t token)
{
	cs_.put(0);
	uint8_t rtn;
	SPIRead(&rtn, 1, token);
	SPIWriteByte(0xff);
	// wait for write to finish
	for (;;) {
		SPIRead(&rtn, 1, 0xff);
		if (rtn != 0x00) break;
	}
	cs_.put(1);
	SPIWriteByte(0xff);
	return true;
}

bool SDCard::ReadBlock(int lba, uint8_t* buf, int nBlocks)
{
	// workaround for shared bus, required for (at least) some Kingston
	// devices, ensure MOSI is high before starting transaction
	SPIWriteByte(0xff);
	if (nBlocks == 1) {
		// CMD17: set read address for single block
		if (cmd(17, lba * cdv_, 0, 0, false) != 0) {
			// release the card
			cs_.put(1);
			return false; // EIO
		}
		// receive the data and release card
		readinto(buf, nBlocks * 512);
	} else {
		// CMD18: set read address for multiple blocks
		if (cmd(18, lba * cdv_, 0, 0, false) != 0) {
			// release the card
			cs_.put(1);
			return false; // EIO
		}
		int offset = 0;
		for (int i = 0; i < nBlocks; i++) {
			// receive the data and release card
			readinto(buf + offset, 512);
			offset += 512;
		}
		if (cmd(12, 0, 0xff, 0, true, true) != 0) {
			return false; // EIO
		}
	}
	return true;
}

bool SDCard::WriteBlock(int lba, const uint8_t* buf, int nBlocks)
{
	// workaround for shared bus, required for (at least) some Kingston
	// devices, ensure MOSI is high before starting transaction
	SPIWriteByte(0xff);
	if (nBlocks == 1) {
		// CMD24: set write address for single block
		if (cmd(24, lba * cdv_, 0) != 0) {
			return false; // EIO
		}
		// send the data
		write(_TOKEN_DATA, buf, nBlocks * 512);
	} else {
		// CMD25: set write address for first block
		if (cmd(25, lba * cdv_, 0) != 0) {
			return false; // EIO
		}
		// send the data
		int offset = 0;
		for (int i = 0; i < nBlocks; i++) {
			write(_TOKEN_CMD25, buf + offset, 512);
			offset += 512;
		}
		write_token(_TOKEN_STOP_TRAN);
	}
	return true;
}

/*
	def ioctl(self, op, arg):
		if op == 4:  // get number of blocks
			return self.sectors
		if op == 5:  // get block size in bytes
			return 512
*/

void SDCard::PrintMBR(const uint8_t* bufSector)
{
	if (bufSector[510] != 0x55 || bufSector[511] != 0xaa) {
		::printf("invalid MBR\n");
		return;
	}
	int offset = 446;
	for (int iPartition = 0; iPartition < 4; iPartition++, offset += 16) {
		const Partition& partition = *reinterpret_cast<const Partition*>(bufSector + offset);
		::printf("#%d: BootID=%02x System=%02x LbaOfs=%08x LbaSize=%08x\n", iPartition + 1,
			partition.BootID, partition.System, Unpack_uint32(partition.LbaOfs), Unpack_uint32(partition.LbaSize));
	}
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
	GPIO_TX.set_function_SPI0_TX();
	GPIO_RX.set_function_SPI0_RX();
	GPIO_CS.init().set_dir_OUT();
	SDCard sdCard(spi, GPIO_CS, 10 * 1000 * 1000);
	sdCard.Initialize();
	uint8_t buf[512];
	sdCard.ReadBlock(0, buf, 1);
	SDCard::PrintMBR(buf);
	for (int i = 0; i < 512; i++) buf[i] = static_cast<uint8_t>(i);
	//::memset(buf, 0x00, sizeof(buf));
	sdCard.WriteBlock(1, buf, 1);
	sdCard.WriteBlock(2, buf, 1);
	::memset(buf, 0x00, sizeof(buf));
	for (int i = 0; i < 4; i++) {
		::printf("sector#%d\n", i);
		sdCard.ReadBlock(i, buf, 1);
		Dump(buf, 512);
	}
}
