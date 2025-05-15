//==============================================================================
// SDCard.cpp
// original: https://github.com/micropython/micropython-lib
//==============================================================================
#include <memory.h>
#include "jxglib/SDCard.h"

#define PRINTF(args...) if (debugFlag) ::printf(args)

namespace jxglib {

//------------------------------------------------------------------------------
// SDCard
//------------------------------------------------------------------------------
SDCard::SDCard(spi_inst_t* spi, uint baudrate, const PinAssign& pinAssign) : initializedFlag_{false},
		spi_{spi}, gpio_CS_{pinAssign.CS}, baudrate_{baudrate}, cdv_{0}, nSectors_{0}
{
}

bool SDCard::Initialize(bool debugFlag)
{
	initializedFlag_ = false;
	const uint8_t crc_zero = 0x00;
	gpio_CS_.init().set_dir_OUT();
	gpio_CS_.put(1);
	PRINTF("set SPI baudrate to %dHz\n", baudrateSlow);
	::spi_init(spi_, baudrateSlow);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
	PRINTF("send 128 clocks to SCLKs (at least requires 74 clocks) while DI and CS set to H\n");
	for (int i = 0; i < 16; i++) SPIWriteByte(0xff);
	do {
		bool successFlag = false;
		PRINTF("CMD0: GO_IDLE_STATE ... returns to IDLE_STATE\n");
		for (int iTrial = 0; iTrial < 5; iTrial++) {
			// https://crccalc.com/?crc=4000000000&method=CRC-8/LTE&datatype=hex&outtype=hex
			if (WriteCommandFrame(0, 0, 0x94) == _R1_IDLE_STATE) {
				successFlag = true;
				break;
			}
		}
		if (!successFlag) {
			PRINTF("Error: failed to go to idle state\n");
			return false;
		}
	} while (0);
	PRINTF("CMD8: SEND_IF_COND ... determines card version\n");
	uint8_t status[4];
	// https://crccalc.com/?crc=48000001aa&method=CRC-8/LTE&datatype=hex&outtype=hex
	int r1 = WriteCommandFrame(8, 0x01aa, 0x86, status, 4);
	if (r1 == _R1_IDLE_STATE) {
		PRINTF("card version is v2\n");
		bool successFlag = false;
		for (int iTrial = 0; iTrial < TrialCountForCmd; iTrial++) {
			sleep_ms(50);
			PRINTF("CMD58: READ_OCR\n");
			WriteCommandFrame(58, 0, crc_zero, status, 4);
			PRINTF("ACMD41: APP_SEND_OP_COND\n");
			WriteCommandFrame(55, 0, crc_zero);
			if (WriteCommandFrame(41, 0x40000000, crc_zero) == _R1_SUCCESS) {
				PRINTF("CMD58: READ_OCR\n");
				WriteCommandFrame(58, 0, crc_zero, status, 4);
				if (status[0] & 0x40) {
					PRINTF("SDHC/SDXC card, uses block addressing in read/write/erase commands\n");
					cdv_ = 1;
				} else {
					PRINTF("SDSC card, uses byte addressing in read/write/erase commands\n");
					cdv_ = 512;
				}
				successFlag = true;
				break;
			}
		}
		if (!successFlag) {
			PRINTF("Error: failed to read OCR\n");
			return false;
		}
	} else if (r1 == (_R1_IDLE_STATE | _R1_ILLEGAL_COMMAND)) {
		PRINTF("card version is v1\n");
		bool successFlag = false;
		for (int iTrial = 0; iTrial < TrialCountForCmd; iTrial++) {
			sleep_ms(50);
			PRINTF("ACMD41: APP_SEND_OP_COND\n");
			WriteCommandFrame(55, 0, crc_zero);
			if (WriteCommandFrame(41, 0, crc_zero) == _R1_SUCCESS) {
				PRINTF("SDSC card, uses byte addressing in read/write/erase commands\n");
				cdv_ = 512;
				successFlag = true;
				break;
			}
		}
		if (!successFlag) {
			PRINTF("Error: failed to read\n");
			return false;
		}
	} else {
		PRINTF("Error: unknown response 0x%02x\n", r1);
		return false;
	}
	// get the number of sectors
	PRINTF("CMD9: SEND_CSD\n");
	if (WriteCommandFrame(9, 0, crc_zero, nullptr, 0, false) != _R1_SUCCESS) {
		PRINTF("Error: no response from SD card\n");
		return false;
	}
	PRINTF("receives CSD (16-byte data packet)\n");
	uint8_t csd[16];
	ReadDataPacket(csd, sizeof(csd));
	if ((csd[0] & 0xc0) == 0x40) {
		PRINTF("CSD version 2.0\n");
		nSectors_ = ((csd[8] << 8 | csd[9]) + 1) * 1024;
	} else if ((csd[0] & 0xc0) == 0x00) {
		PRINTF("CSD version 1.0 (old, <=2GB)\n");
		int c_size = ((csd[6] & 0b11) << 10) | (csd[7] << 2) | (csd[8] >> 6);
		int c_size_mult = ((csd[9] & 0b11) << 1) | (csd[10] >> 7);
		int read_bl_len = csd[5] & 0b1111;
		int capacity = (c_size + 1) * (1 << (c_size_mult + 2)) * (1 << read_bl_len);
		nSectors_ = capacity / 512;
	} else {
		PRINTF("Error: invalid CSD format\n");
		return false;
	}
	PRINTF("CMD16: SET_BLOCKLEN\n");
	if (WriteCommandFrame(16, 512, crc_zero) != _R1_SUCCESS) {
		PRINTF("Error: no response from SD card\n");
		return false;
	}
	PRINTF("set SPI baudrate to %dHz\n", baudrate_);
	::spi_init(spi_, baudrate_);
	::spi_set_format(spi_, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST); 
	initializedFlag_ = true;
	return true;
}

int SDCard::WriteCommandFrame(uint8_t cmd, uint32_t arg, uint8_t crc, uint8_t* status, int bytesStatus, bool release)
{
	gpio_CS_.put(0);
	uint8_t commandFrame[6];
	commandFrame[0] = 0x40 | cmd;
	commandFrame[1] = static_cast<uint8_t>(arg >> 24);
	commandFrame[2] = static_cast<uint8_t>(arg >> 16);
	commandFrame[3] = static_cast<uint8_t>(arg >> 8);
	commandFrame[4] = static_cast<uint8_t>(arg);
	commandFrame[5] = crc | 0x01;
	SPIWrite(commandFrame, sizeof(commandFrame));
	if (cmd == 12) SPIReadByte(); // CMD12 has a reponse format of R1b
	int rtn = -1;
	for (int iTrial = 0; iTrial < TrialCountForCmd; iTrial++) {
		uint8_t resp = SPIReadByte();
		if (!(resp & 0x80)) {
			if (bytesStatus > 0) SPIRead(status, bytesStatus, 0xff);
			rtn = resp;
			break;
		}
	}
	if (release) {
		gpio_CS_.put(1);
		SPIWriteByte(0xff);
	}
	return rtn;
}

bool SDCard::ReadDataPacket(uint8_t* buf, int bytes)
{
	bool successFlag = false;
	gpio_CS_.put(0);
	for (int iTrial = 0; iTrial < TrialCountForRead; iTrial++) {
		if (SPIReadByte() == Token_ReadData) {
			uint8_t crc[2];
			SPIRead(buf, bytes, 0xff);			// Data Block
			SPIRead(crc, sizeof(crc), 0xff);	// CRC
			successFlag = true;
			break;
		}
		::sleep_ms(1);
	}
	gpio_CS_.put(1);
	SPIWriteByte(0xff);
	return successFlag;
}

bool SDCard::WriteDataPacket(uint8_t token, const uint8_t* buf, int bytes)
{
	bool successFlag = false;
	gpio_CS_.put(0);
	uint8_t crc[2];
	crc[0] = crc[1] = 0xff;
	SPIWriteByte(token);		// Data Token
	SPIWrite(buf, bytes);		// Data Block
	SPIWrite(crc, sizeof(crc));	// CRC
	uint8_t resp = SPIReadByte() & 0x1f;
	if (resp == 0x05) { // Data accepted
		while (SPIReadByte() == 0x00) ;
		successFlag = true;
	}
	gpio_CS_.put(1);
	SPIWriteByte(0xff);
	return successFlag;
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

bool SDCard::ReadBlock(int lba, void* buf, int nBlocks)
{
	const uint8_t crc_zero = 0x00;
	const uint8_t crc_ff = 0xff;
	uint8_t* bufCast = reinterpret_cast<uint8_t*>(buf);
	// workaround for shared bus, required for (at least) some Kingston devices,
	// ensure MOSI is high before starting transaction
	SPIWriteByte(0xff);
	if (nBlocks == 1) {
		// CMD17: set read address for single block
		if (WriteCommandFrame(17, lba * cdv_, crc_zero, nullptr, 0, false) != _R1_SUCCESS) {
			gpio_CS_.put(1);
			return false; // EIO
		}
		ReadDataPacket(bufCast, nBlocks * 512);
	} else {
		// CMD18: set read address for multiple blocks
		if (WriteCommandFrame(18, lba * cdv_, crc_zero, nullptr, 0, false) != _R1_SUCCESS) {
			gpio_CS_.put(1);
			return false; // EIO
		}
		int offset = 0;
		for (int i = 0; i < nBlocks; i++) {
			ReadDataPacket(bufCast + offset, 512);
			offset += 512;
		}
		if (WriteCommandFrame(12, 0, crc_ff) != _R1_SUCCESS) {
			return false; // EIO
		}
	}
	return true;
}

bool SDCard::WriteBlock(int lba, const void* buf, int nBlocks)
{
	bool successFlag = true;
	const uint8_t crc_zero = 0x00;
	const uint8_t* bufCast = reinterpret_cast<const uint8_t*>(buf);
	// workaround for shared bus, required for (at least) some Kingston
	// devices, ensure MOSI is high before starting transaction
	SPIWriteByte(0xff);
	if (nBlocks == 1) {
		// CMD24: WRITE_BLOCK
		if (WriteCommandFrame(24, lba * cdv_, crc_zero) != _R1_SUCCESS) {
			return false;
		}
		successFlag = WriteDataPacket(Token_CMD24, bufCast, nBlocks * 512);
	} else {
		// CMD25: WRITE_MULTIPLE_BLOCK
		if (WriteCommandFrame(25, lba * cdv_, crc_zero) != _R1_SUCCESS) {
			return false;
		}
		int offset = 0;
		for (int i = 0; i < nBlocks; i++) {
			if (!WriteDataPacket(Token_CMD25, bufCast + offset, 512)) {
				successFlag = false;
				break;
			}
			offset += 512;
		}
		WriteToken(Token_CMD25_STOP_TRAN);
	}
	return successFlag;
}

void SDCard::PrintMBR(Printable& printable, const uint8_t* bufSector)
{
	if (bufSector[510] != 0x55 || bufSector[511] != 0xaa) {
		printable.Printf("invalid MBR\n");
		return;
	}
	int offset = 446;
	for (int iPartition = 0; iPartition < 4; iPartition++, offset += 16) {
		const Partition& partition = *reinterpret_cast<const Partition*>(bufSector + offset);
		printable.Printf("#%d: BootID=%02x System=%02x LbaOfs=%08x LbaSize=%08x\n", iPartition + 1,
			partition.BootID, partition.System, Unpack_uint32(partition.LbaOfs), Unpack_uint32(partition.LbaSize));
	}
}

}
