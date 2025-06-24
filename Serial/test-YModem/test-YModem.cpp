#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/UART.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/RTC/DS323x.h"

using namespace jxglib;

class YModem {
private:
	struct CtrlChar {
		static const uint8_t SOH = 0x01;  // Start of 128-byte block
		static const uint8_t STX = 0x02;  // Start of 1024-byte block
		static const uint8_t EOT = 0x04;  // End of transmission
		static const uint8_t ACK = 0x06;  // Acknowledge
		static const uint8_t NAK = 0x15;  // Negative acknowledge
		static const uint8_t CAN = 0x18;  // Cancel
		static const uint8_t C   = 0x43;  // 'C' for CRC mode
	};
	
	Stream& stream_;
	
	static const int TIMEOUT_MS = 10000;  // 10 seconds timeout
	static const int MAX_RETRIES = 10;
	
	// Helper methods
	uint16_t CalcCRC16(const uint8_t* data, size_t len);
	bool WaitForChar(uint8_t expectedChar, int timeoutMs = TIMEOUT_MS);
	bool SendBlock(uint8_t blockNum, const uint8_t* data, size_t dataLen, bool use1K = false);
	
public:
	YModem(Stream& stream) : stream_(stream) {}
public:
	bool SendFile(const char* fileName);
	bool RecvFile(const char* fileName);
};

uint16_t YModem::CalcCRC16(const uint8_t* data, size_t len)
{
	uint16_t crc = 0;
	while (len--) {
		crc ^= *data++ << 8;
		for (int i = 0; i < 8; i++) {
			if (crc & 0x8000) {
				crc = (crc << 1) ^ 0x1021;
			} else {
				crc <<= 1;
			}
		}
	}
	return crc;
}

bool YModem::WaitForChar(uint8_t expectedChar, int timeoutMs)
{
	uint32_t startTime = time_us_32() / 1000;
	while ((time_us_32() / 1000 - startTime) < timeoutMs) {
		uint8_t ch;
		if (stream_.Read(&ch, 1) == 1) {
			if (ch == expectedChar) {
				return true;
			}
		}
		sleep_ms(1);
	}
	return false;
}

bool YModem::SendBlock(uint8_t blockNum, const uint8_t* data, size_t dataLen, bool use1K)
{
	int blockSize = use1K ? 1024 : 128;
	uint8_t* block = new uint8_t[blockSize + 5]; // header + data + CRC
		// Build block
	block[0] = use1K ? CtrlChar::STX : CtrlChar::SOH;
	block[1] = blockNum;
	block[2] = ~blockNum;
	
	// Copy data and pad with zeros if necessary
	memcpy(&block[3], data, dataLen);
	if (dataLen < blockSize) {
		memset(&block[3 + dataLen], 0, blockSize - dataLen);
	}
	
	// Calculate CRC
	uint16_t crc = CalcCRC16(&block[3], blockSize);
	block[blockSize + 3] = (crc >> 8) & 0xff;
	block[blockSize + 4] = crc & 0xff;
	
	// Send block with retries
	for (int retry = 0; retry < MAX_RETRIES; retry++) {
		stream_.Write(block, blockSize + 5);
				if (WaitForChar(CtrlChar::ACK, TIMEOUT_MS)) {
			delete[] block;
			return true;
		}
		// Handle NAK or timeout - resend
		uint8_t response;
		if (stream_.Read(&response, 1) == 1) {
			if (response == CtrlChar::CAN) {
				delete[] block;
				return false;
			}
		}
	}
	
	delete[] block;
	return false;
}

bool YModem::SendFile(const char* fileName)
{
	// Wait for receiver to send 'C' (CRC mode request)
	if (!WaitForChar(CtrlChar::C, TIMEOUT_MS)) {
		return false;
	}
	// Try to open file
	FILE* file = fopen(fileName, "rb");
	if (!file) {
		// Send cancel
		uint8_t cancel = CtrlChar::CAN;
		stream_.Write(&cancel, 1);
		return false;
	}
	
	// Get file size
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);
	
	// Send header block (block #0) with filename and size
	char headerData[128];
	memset(headerData, 0, sizeof(headerData));
	
	// Extract filename from path
	const char* baseName = strrchr(fileName, '/');
	if (!baseName) baseName = strrchr(fileName, '\\');
	if (!baseName) baseName = fileName;
	else baseName++; // skip the separator
	
	// Format: filename + NUL + size + NUL + modification_time + NUL
	int pos = 0;
	strncpy(&headerData[pos], baseName, 127);
	pos += strlen(baseName) + 1;
	
	// Add file size
	pos += snprintf(&headerData[pos], 127 - pos, "%ld", fileSize);
	pos++;
	
	if (!SendBlock(0, (uint8_t*)headerData, 128)) {
		fclose(file);
		return false;
	}
		// Wait for ACK after header
	if (!WaitForChar(CtrlChar::C, TIMEOUT_MS)) {
		fclose(file);
		return false;
	}
	
	// Send file data blocks
	uint8_t blockNum = 1;
	uint8_t buffer[1024];
	size_t bytesRead;
	
	while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
		bool use1K = (bytesRead > 128);
		if (!SendBlock(blockNum, buffer, bytesRead, use1K)) {
			fclose(file);
			return false;
		}
		blockNum++;
	}
	
	fclose(file);		// Send EOT
	for (int retry = 0; retry < MAX_RETRIES; retry++) {
		uint8_t eot = CtrlChar::EOT;
		stream_.Write(&eot, 1);
		if (WaitForChar(CtrlChar::ACK, TIMEOUT_MS)) {
			break;
		}
	}
	
	// Send final empty block to end YModem session
	if (!WaitForChar(CtrlChar::C, TIMEOUT_MS)) {
		return false;
	}
	
	char emptyBlock[128];
	memset(emptyBlock, 0, sizeof(emptyBlock));
	SendBlock(0, (uint8_t*)emptyBlock, 128);
	
	return true;
}

bool YModem::RecvFile(const char* fileName)
{
	return false;
}

ShellCmd(ysend, "Send file via YModem protocol")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return 1;
	if (argc < 2 || arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... FILE...\n", GetName());
		arg.PrintHelp(terr);
		return 1;	}
	
	Stream& stream = UART0;
	YModem ymodem(stream);
	for (Arg::Each each(argv[1], argv[argc]); const char* fileName = each.Next(); ) {
		ymodem.SendFile(fileName);
		tout.Printf("Sending file: %s\n", fileName);
	}
	return 0;
}

int main()
{
	::stdio_init_all();
	LFS::Flash driveA("A:",  0x1010'0000, 0x0004'0000); // Flash address and size 256kB
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
	::i2c_init(i2c0, 400'000);
	GPIO16.set_function_I2C0_SDA().pull_up();
	GPIO17.set_function_I2C0_SCL().pull_up();
	RTC::DS323x rtc(i2c0);
	for (;;) {
		// :
		// any other jobs
		// :
		Tickable::Tick();
	}
}
