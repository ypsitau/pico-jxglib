//==============================================================================
// SDCard.cpp
// original: https://github.com/micropython/micropython-lib
//==============================================================================
#include <memory.h>
#include "jxglib/SDCard.h"

namespace jxglib {

//------------------------------------------------------------------------------
// SDCard
//------------------------------------------------------------------------------
SDCard::SDCard(spi_inst_t* spi, const GPIO& gpio_CS, uint baudrate) :
		spi_{spi}, gpio_CS_{gpio_CS}, baudrate_{baudrate}, cdv_{0}, nSectors_{0}
{
}

bool SDCard::Initialize()
{
	gpio_CS_.init().set_dir_OUT();
	gpio_CS_.put(1);
	::spi_init(spi_, baudrateSlow);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
	// clock card at least 100 cycles with cs high
	for (int i = 0; i < 16; i++) SPIWriteByte(0xff);
	// CMD0: init card; should return _R1_IDLE_STATE (allow 5 attempts)
	bool successFlag = false;
	for (int i = 0; i < 5; i++) {
		if (WriteCommandFrame(0, 0, 0x95) == _R1_IDLE_STATE) {
			successFlag = true;
			break;
		}
	}
	if (!successFlag) {
		return false; // no SD card
	}
	// CMD8: determine card version
	int r = WriteCommandFrame(8, 0x01aa, 0x87, 4);
	if (r == _R1_IDLE_STATE) {
		// v2 card
		successFlag = false;
		for (int iTrial = 0; iTrial < TrialCountOfCmd; iTrial++) {
			sleep_ms(50);
			WriteCommandFrame(58, 0, 0, 4);
			WriteCommandFrame(55, 0, 0);
			if (WriteCommandFrame(41, 0x40000000, 0) == 0) {
				WriteCommandFrame(58, 0, 0, -4); // 4-byte response, negative means keep the first byte
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
		if (!successFlag) {
			return false;
		}
	} else if (r == (_R1_IDLE_STATE | _R1_ILLEGAL_COMMAND)) {
		// v1 card
		successFlag = false;
		for (int iTrial = 0; iTrial < TrialCountOfCmd; iTrial++) {
			sleep_ms(50);
			WriteCommandFrame(55, 0, 0);
			if (WriteCommandFrame(41, 0, 0) == 0) {
				// SDSC card, uses byte addressing in read/write/erase commands
				cdv_ = 512;
				// print("[SDCard] v1 card")
				successFlag = true;
				break;
			}
		}
		if (!successFlag) {
			return false;
		}
	} else {
		return false; // couldn't determine SD card version
	}
	// get the number of sectors
	// CMD9: response R2 (R1 byte + 16-byte block read)
	if (WriteCommandFrame(9, 0, 0, 0, false) != 0) {
		return false; // no response from SD card
	}
	uint8_t csd[16];
	::memset(csd, 0x00, sizeof(csd));
	ReadDataPacket(csd, sizeof(csd));
	int c_size, c_size_mult, capacity, read_bl_len;
	if ((csd[0] & 0xc0) == 0x40) {  // CSD version 2.0
		nSectors_ = ((csd[8] << 8 | csd[9]) + 1) * 1024;
	} else if ((csd[0] & 0xc0) == 0x00) { // CSD version 1.0 (old, <=2GB)
		c_size = (csd[6] & 0b11) << 10 | csd[7] << 2 | csd[8] >> 6;
		c_size_mult = (csd[9] & 0b11) << 1 | csd[10] >> 7;
		read_bl_len = csd[5] & 0b1111;
		capacity = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << read_bl_len);
		nSectors_ = capacity / 512;
	} else {
		return false; // SD card CSD format not supported
	}
	// CMD16: set block length to 512 bytes
	if (WriteCommandFrame(16, 512, 0) != 0) {
		return false; // can't set 512 block size
	}
	// set to high data rate now that it's initialised
	::spi_init(spi_, baudrate_);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
	return true;
}

int SDCard::WriteCommandFrame(uint8_t cmd, uint32_t arg, uint8_t crc, int final, bool release, bool skip1)
{
	gpio_CS_.put(0);
	// create and send the command
	uint8_t commandFrame[6];
	commandFrame[0] = 0x40 | cmd;
	commandFrame[1] = static_cast<uint8_t>(arg >> 24);
	commandFrame[2] = static_cast<uint8_t>(arg >> 16);
	commandFrame[3] = static_cast<uint8_t>(arg >> 8);
	commandFrame[4] = static_cast<uint8_t>(arg);
	commandFrame[5] = crc;
	SPIWrite(commandFrame, sizeof(commandFrame));
	if (skip1) SPIReadByte();
	// wait for the response (response[7] == 0)
	for (int iTrial = 0; iTrial < TrialCountOfCmd; iTrial++) {
		uint8_t response = SPIReadByte();
		if (!(response & 0x80)) {
			// this could be a big-endian integer that we are getting here
			// if final<0 then store the first byte to tokenbuf and discard the rest
			if (final < 0) {
				SPIRead(tokenbuf_, 1, 0xff);
				final = -1 - final;
			}
			for (int j = 0; j < final; j++) SPIWriteByte(0xff);
			if (release) {
				gpio_CS_.put(1);
				SPIWriteByte(0xff);
			}
			return response;
		}
	}
	gpio_CS_.put(1);
	SPIWriteByte(0xff);
	return -1;
}

bool SDCard::ReadDataPacket(uint8_t* buf, int bytes)
{
	gpio_CS_.put(0);
	bool successFlag = false;
	// Wait for Data Token
	for (int iTrial = 0; iTrial < TrialCountOfCmd; iTrial++) {
		if (SPIReadByte() == Token_ReadData) {
			successFlag = true;
			break;
		}
		::sleep_ms(1);
	}
	if (successFlag) {
		uint8_t crc[2];
		SPIRead(buf, bytes, 0xff);			// Data Block
		SPIRead(crc, sizeof(crc), 0xff);	// CRC
	}
	gpio_CS_.put(1);
	SPIWriteByte(0xff);
	return successFlag;
}

bool SDCard::WriteDataPacket(uint8_t token, const uint8_t* buf, int bytes)
{
	gpio_CS_.put(0);
	uint8_t crc[2];
	crc[0] = crc[1] = 0xff;
	SPIWriteByte(token);		// Data Token
	SPIWrite(buf, bytes);		// Data Block
	SPIWrite(crc, sizeof(crc));	// CRC
	uint8_t rtn;
	SPIRead(&rtn, 1, 0xff);
	if ((rtn & 0x1f) != 0x05) {
		gpio_CS_.put(1);
		SPIWriteByte(0xff);
		return false;
	}
	while (SPIReadByte() == 0x00) ;
	gpio_CS_.put(1);
	SPIWriteByte(0xff);
	return true;
}

bool SDCard::WriteToken(uint8_t token)
{
	gpio_CS_.put(0);
	SPIWriteByte(token);
	SPIWriteByte(0xff);
	while (SPIReadByte() == 0x00) ;
	gpio_CS_.put(1);
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
		if (WriteCommandFrame(17, lba * cdv_, 0, 0, false) != 0) {
			// release the card
			gpio_CS_.put(1);
			return false; // EIO
		}
		// receive the data and release card
		ReadDataPacket(buf, nBlocks * 512);
	} else {
		// CMD18: set read address for multiple blocks
		if (WriteCommandFrame(18, lba * cdv_, 0, 0, false) != 0) {
			// release the card
			gpio_CS_.put(1);
			return false; // EIO
		}
		int offset = 0;
		for (int i = 0; i < nBlocks; i++) {
			// receive the data and release card
			ReadDataPacket(buf + offset, 512);
			offset += 512;
		}
		if (WriteCommandFrame(12, 0, 0xff, 0, true, true) != 0) {
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
		if (WriteCommandFrame(24, lba * cdv_, 0) != 0) {
			return false; // EIO
		}
		// send the data
		WriteDataPacket(Token_CMD24, buf, nBlocks * 512);
	} else {
		// CMD25: set write address for first block
		if (WriteCommandFrame(25, lba * cdv_, 0) != 0) {
			return false; // EIO
		}
		// send the data
		int offset = 0;
		for (int i = 0; i < nBlocks; i++) {
			WriteDataPacket(Token_CMD25, buf + offset, 512);
			offset += 512;
		}
		WriteToken(Token_CMD25_STOP_TRAN);
	}
	return true;
}

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

}
