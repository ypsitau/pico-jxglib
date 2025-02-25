//==============================================================================
// jxglib/SDCard.h
// original: https://github.com/micropython/micropython-lib
//==============================================================================
#ifndef PICO_JXGLIB_SDCARD_H
#define PICO_JXGLIB_SDCARD_H
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "jxglib/Common.h"
#include "jxglib/PackedNumber.h"

namespace jxglib {

//------------------------------------------------------------------------------
// SDCard
//------------------------------------------------------------------------------
class SDCard {
public:
	struct PinAssign {
		const GPIO& CS;
	};
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
	static const int TrialCountForCmd				= 100;
	static const int TrialCountForRead				= 10;
	static const int TrialCountForWrite				= 10;
	static const uint8_t _R1_SUCCESS				= 0x00;
	static const uint8_t _R1_IDLE_STATE				= 1 << 0;
	static const uint8_t _R1_ERASE_RESET			= 1 << 1;
	static const uint8_t _R1_ILLEGAL_COMMAND		= 1 << 2;
	static const uint8_t _R1_COM_CRC_ERROR			= 1 << 3;
	static const uint8_t _R1_ERASE_SEQUENCE_ERROR	= 1 << 4;
	static const uint8_t _R1_ADDRESS_ERROR			= 1 << 5;
	static const uint8_t _R1_PARAMETER_ERROR		= 1 << 6;
	static const uint8_t Token_CMD24				= 0xfe;
	static const uint8_t Token_CMD25				= 0xfc;
	static const uint8_t Token_CMD25_STOP_TRAN		= 0xfd;
	static const uint8_t Token_ReadData				= 0xfe;
public:
	static const uint baudrateSlow = 100 * 1000;	// 100kHz
private:
	spi_inst_t* spi_;
	const GPIO& gpio_CS_;
	uint baudrate_;
	int cdv_;
	int nSectors_;
public:
	SDCard(spi_inst_t* spi, uint baudrate, const PinAssign& pinAssign);
	bool Initialize(bool debugFlag = false);
	bool ReadBlock(int lba, uint8_t* buf, int nBlocks);
	bool WriteBlock(int lba, const uint8_t* buf, int nBlocks);
public:
	int GetSectors() const { return nSectors_; }
	static void PrintMBR(const uint8_t* bufSector);
private:
	int WriteCommandFrame(uint8_t cmd, uint32_t arg, uint8_t crc,
				uint8_t* status = nullptr, int bytesStatus = 0, bool release = true);
	bool ReadDataPacket(uint8_t* buf, int bytes);
	bool WriteDataPacket(uint8_t token, const uint8_t* buf, int bytes);
	bool WriteToken(uint8_t token);
private:
	void SPIRead(uint8_t* data, int bytes, uint8_t dataToSend) { spi_read_blocking(spi_, dataToSend, data, bytes); }
	uint8_t SPIReadByte(uint8_t dataToSend = 0xff) {
		uint8_t data; spi_read_blocking(spi_, dataToSend, &data, 1); return data;
	}
	void SPIWrite(const uint8_t* data, int bytes) { spi_write_blocking(spi_, data, bytes); }
	void SPIWriteByte(uint8_t data) { spi_write_blocking(spi_, &data, 1); }
};

}

#endif
