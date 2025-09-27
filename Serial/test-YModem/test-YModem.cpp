#include <stdio.h>
#include <memory>
#include <cstdlib>
#include "pico/stdlib.h"
#include "jxglib/Serial.h"
#include "jxglib/Shell.h"
#include "jxglib/UART.h"
#include "jxglib/LFS/Flash.h"
#include "jxglib/RTC/DS323x.h"
#include "jxglib/FS.h"
#include "jxglib/ST7789.h"
//#include "jxglib/USBHost/HID.h"
#include "jxglib/Font/shinonome16.h"

using namespace jxglib;

Display::Terminal terminal;

class YModem {
private:
	struct CtrlCode {
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
	void SendCode(uint8_t code) { stream_.Write(&code, 1); }
	bool WaitForCode(uint8_t codeExpected, int msecTimeout = TIMEOUT_MS);
	int RecvBuff(uint8_t* buffer, size_t size, int msecTimeout = TIMEOUT_MS);
	bool SendBlock(uint8_t blockNum, const uint8_t* data, size_t dataLen, bool use1K = false);
	bool RecvBlock(uint8_t expectedBlockNum, uint8_t* data, size_t& dataLen, int msecTimeout = TIMEOUT_MS);
public:
	YModem(Stream& stream) : stream_(stream) {}
public:
	bool SendSingleFile(const char* fileName, bool finalFlag = true);
	bool BeginSendSession();
	bool EndSendSession();
public:
	bool RecvSingleFile(const char* dirNameDst, char* fileNameOut, size_t fileNameMaxLen);
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

bool YModem::WaitForCode(uint8_t codeExpected, int msecTimeout)
{
	uint32_t startTime = ::time_us_32() / 1000;
	while ((::time_us_32() / 1000 - startTime) < msecTimeout) {
		uint8_t ch;
		int result = RecvBuff(&ch, 1, 100);
		if (result == -1) {
			// User interruption
			return false;
		}
		if (result == 1) {
			if (ch == codeExpected) {
				return true;
			}
		}
	}
	return false;
}

int YModem::RecvBuff(uint8_t* buffer, size_t size, int msecTimeout)
{
	size_t totalRead = 0;
	uint32_t startTime = ::time_us_32() / 1000;
	while (totalRead < size) {
		// Check timeout
		if ((::time_us_32() / 1000 - startTime) >= msecTimeout) break;
		// Try to read available data
		int bytesRead = stream_.Read(buffer + totalRead, size - totalRead);
		if (bytesRead > 0) totalRead += bytesRead;
		// Allow other tasks to run and check for user interruption
		if (Tickable::TickSub()) return -1;  // User interruption
	}
	
	return totalRead;
}

bool YModem::SendBlock(uint8_t blockNum, const uint8_t* data, size_t dataLen, bool use1K)
{
	int blockSize = use1K ? 1024 : 128;
	uint8_t* block = new uint8_t[blockSize + 5]; // header + data + CRC
		// Build block
	block[0] = use1K ? CtrlCode::STX : CtrlCode::SOH;
	block[1] = blockNum;
	block[2] = ~blockNum;
		// Copy data and pad with zeros if necessary
	::memcpy(&block[3], data, dataLen);
	if (dataLen < blockSize) {
		::memset(&block[3 + dataLen], 0, blockSize - dataLen);
	}
	
	// Calculate CRC
	uint16_t crc = CalcCRC16(&block[3], blockSize);
	block[blockSize + 3] = (crc >> 8) & 0xff;
	block[blockSize + 4] = crc & 0xff;
		// Send block with retries
	for (int retry = 0; retry < MAX_RETRIES; retry++) {
		stream_.Write(block, blockSize + 5);
		if (WaitForCode(CtrlCode::ACK)) {
			delete[] block;
			return true;
		}		// Handle NAK or timeout - resend
		uint8_t response;
		int result = RecvBuff(&response, 1, 100);
		if (result == -1) {
			// User interruption
			delete[] block;
			return false;
		}
		if (result == 1) {
			if (response == CtrlCode::CAN) {
				delete[] block;
				return false;
			}
		}
	}
	
	delete[] block;
	return false;
}

bool YModem::SendSingleFile(const char* fileName, bool finalFlag)
{
	// Try to open file using FS::File
	std::unique_ptr<FS::File> file(FS::OpenFile(fileName, "r"));
	if (!file) {
		// Send cancel
		SendCode(CtrlCode::CAN);
		return false;
	}
	
	// Get file size
	int fileSize = file->Size();
	
	// Send header block (block #0) with filename and size
	char headerData[128];
	::memset(headerData, 0, sizeof(headerData));
	
	// Extract filename from path
	const char* baseName = FS::ExtractFileName(fileName);
	
	// Format: filename + NUL + size + NUL + modification_time + NUL
	int pos = 0;
	::strncpy(&headerData[pos], baseName, 127);
	pos += ::strlen(baseName) + 1;
	
	// Add file size
	pos += ::snprintf(&headerData[pos], 127 - pos, "%d", fileSize);
	pos++;
	
	if (!SendBlock(0, (uint8_t*)headerData, 128)) {
		return false;
	}
	
	// Wait for ACK after header
	if (!WaitForCode(CtrlCode::C)) {
		return false;
	}
	
	// Send file data blocks
	uint8_t blockNum = 1;
	uint8_t buffer[1024];
	int bytesRead;
	
	while ((bytesRead = file->Read(buffer, sizeof(buffer))) > 0) {
		bool use1K = (bytesRead > 128);
		if (!SendBlock(blockNum, buffer, bytesRead, use1K)) {
			return false;
		}
		blockNum++;
	}
	
	// Send EOT
	for (int retry = 0; retry < MAX_RETRIES; retry++) {
		SendCode(CtrlCode::EOT);
		if (WaitForCode(CtrlCode::ACK)) {
			break;
		}
	}
	
	// If not final file, wait for 'C' for next file transfer
	if (!finalFlag) {
		if (!WaitForCode(CtrlCode::C)) {
			return false;
		}
	}
	
	return true;
}

bool YModem::BeginSendSession()
{
	// Wait for receiver to send 'C' (CRC mode request)
	return WaitForCode(CtrlCode::C);
}

bool YModem::EndSendSession()
{
	// Wait for receiver to request next file
	if (!WaitForCode(CtrlCode::C)) {
		return false;
	}
	
	// Send final empty block to end YModem session
	char emptyBlock[128];
	::memset(emptyBlock, 0, sizeof(emptyBlock));
	return SendBlock(0, (uint8_t*)emptyBlock, 128);
}

bool YModem::RecvBlock(uint8_t expectedBlockNum, uint8_t* data, size_t& dataLen, int msecTimeout)
{
	uint8_t header[3];
	// Wait for block start (SOH or STX)
	bool foundFlag = false;
	uint32_t startTime = ::time_us_32() / 1000;
	while (!foundFlag && (::time_us_32() / 1000 - startTime) < msecTimeout) {
		SendCode(CtrlCode::C);
		int result = RecvBuff(&header[0], 1, 500);
		if (result == -1) {
			// User interruption
			return false;
		}
		if (result == 1) {
			terminal.Printf("Received header: 0x%02X '%c'\n", header[0], header[0]);
			if (header[0] == CtrlCode::SOH || header[0] == CtrlCode::STX) {
				foundFlag = true;
				break;
			} else if (header[0] == CtrlCode::EOT) {
				// End of transmission
				SendCode(CtrlCode::ACK);
				dataLen = 0;
				return true;
			} else if (header[0] == CtrlCode::CAN || header[0] == 0x03) {
				// Cancel
				return false;
			}
		}
	}
	
	if (!foundFlag) return false;

	// Determine block size
	int blockSize = (header[0] == CtrlCode::STX) ? 1024 : 128;	// Read block number and complement
	int result = RecvBuff(&header[1], 2, msecTimeout);
	if (result == -1) return false; // User interruption
	if (result != 2) return false;
	
	// Verify block number
	if (header[1] != expectedBlockNum || header[2] != (uint8_t)~expectedBlockNum) {
		// NAK - incorrect block number
		SendCode(CtrlCode::NAK);
		return false;
	}
	
	// Read data block
	result = RecvBuff(data, blockSize, msecTimeout);
	if (result == -1) return false; // User interruption
	if (result != blockSize) {
		SendCode(CtrlCode::NAK);
		return false;
	}
	
	// Read CRC
	uint8_t crcBytes[2];
	result = RecvBuff(crcBytes, 2, msecTimeout);
	if (result == -1) return false; // User interruption
	if (result != 2) {
		SendCode(CtrlCode::NAK);
		return false;
	}
	
	// Verify CRC
	uint16_t receivedCRC = (crcBytes[0] << 8) | crcBytes[1];
	uint16_t calculatedCRC = CalcCRC16(data, blockSize);
	
	if (receivedCRC != calculatedCRC) {
		// NAK - CRC error
		SendCode(CtrlCode::NAK);
		return false;
	}
	
	// ACK - block received successfully
	SendCode(CtrlCode::ACK);
	dataLen = blockSize;
	return true;
}

bool YModem::RecvSingleFile(const char* dirNameDst, char* fileNameOut, size_t fileNameMaxLen)
{
	int cntRetries = 30;
	uint8_t blockData[1024];
	size_t dataLen;
	int msecTimeoutFirst = 30000; // 30 seconds for first block

	if (!RecvBlock(0, blockData, dataLen, msecTimeoutFirst)) return false;
	// Parse header block
	const char* fileName = (const char*)blockData;
	if (::strlen(fileName) == 0) {
		// Empty filename means end of batch
		::strncpy(fileNameOut, "", fileNameMaxLen);
		return true;
	}
	
	// Extract file size from header
	const char* sizeStr = fileName + ::strlen(fileName) + 1;
	int fileSize = ::atoi(sizeStr);
	
	// Create full path for destination file
	char fullPath[256];
	::snprintf(fullPath, sizeof(fullPath), "%s/%s", dirNameDst, fileName);
	
	// Copy filename to output
	::strncpy(fileNameOut, fileName, fileNameMaxLen);
	fileNameOut[fileNameMaxLen - 1] = '\0';
	
	// Open file for writing
	std::unique_ptr<FS::File> pFile(FS::OpenFile(fullPath, "w"));
	if (!pFile) {
		// Send cancel
		SendCode(CtrlCode::CAN);
		return false;
	}
	
	// Receive data blocks
	uint8_t blockNum = 1;
	int totalBytesReceived = 0;
	
	while (totalBytesReceived < fileSize) {
		if (!RecvBlock(blockNum, blockData, dataLen)) {
			return false;
		}
		
		if (dataLen == 0) {
			// EOT received
			break;
		}
		
		// Write data to file (only the needed bytes)
		int bytesToWrite = (totalBytesReceived + dataLen > fileSize)? (fileSize - totalBytesReceived) : dataLen;
		
		if (pFile->Write(blockData, bytesToWrite) != bytesToWrite) {
			// Write error - send cancel
			SendCode(CtrlCode::CAN);
			return false;
		}
		
		totalBytesReceived += bytesToWrite;
		blockNum++;
	}
	
	return true;
}

ShellCmd(ysend, "Send file via YModem protocol")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help") || argc < 2) {
		terr.Printf("Usage: %s [OPTION]... FILE...\n", GetName());
		arg.PrintHelp(terr);
		return arg.GetBool("help")? Result::Success : Result::Error;
	}
	Stream& stream = UART0;
	YModem ymodem(stream);
	
	// Begin YModem session
	if (!ymodem.BeginSendSession()) {
		terr.Printf("Failed to begin YModem session\n");
		return Result::Error;
	}
	
	// Send each file
	bool success = true;
	for (int iArg = 1; iArg < argc; iArg++) {
		const char* fileName = argv[iArg];
		bool finalFlag = (iArg == argc - 1);
		if (!ymodem.SendSingleFile(fileName, finalFlag)) {
			terr.Printf("Failed to send file: %s\n", fileName);
			success = false;
			break;
		} else {
			tout.Printf("Sent: %s\n", fileName);
		}
	}
	
	// End YModem session
	if (success) {
		if (!ymodem.EndSendSession()) {
			terr.Printf("Failed to end YModem session\n");
			success = false;
		}
	}
	return success? Result::Success : Result::Error;
}

ShellCmd(yrecv, "Receive file(s) via YModem protocol")
{
	static const Arg::Opt optTbl[] = {
		Arg::OptBool("help",		'h',	"prints this help"),
	};
	Arg arg(optTbl, count_of(optTbl));
	if (!arg.Parse(terr, argc, argv)) return Result::Error;
	if (arg.GetBool("help")) {
		terr.Printf("Usage: %s [OPTION]... DESTDIR\n", GetName());
		terr.Printf("Receive file(s) via YModem protocol into the specified directory.\n");
		arg.PrintHelp(terr);
		return Result::Error;
	}
	const char* dirNameDst = (argc < 2)? "." : argv[1];
	Stream& stream = Stdio::Instance;
	YModem ymodem(stream);
	char fileName[FS::MaxPath];
	int nFilesMax = 100;
	tout.Printf("Receiving files to directory: %s\n", dirNameDst);
	for (int nFiles = 0; nFiles < nFilesMax; nFiles++) {
		if (!ymodem.RecvSingleFile(dirNameDst, fileName, sizeof(fileName))) return Result::Error;
		// Check if we received an empty filename (end of batch)
		if (::strlen(fileName) == 0) break;
		// Printf feedback about received file
		tout.Printf("Received file: %s\n", fileName);
	}
	return Result::Success;
}

int main()
{
	::stdio_init_all();
	LFS::Flash driveA("A:",  0x1010'0000, 0x0004'0000); // Flash address and size 256kB
#if 0
	::spi_init(spi1, 125'000'000);
	GPIO14.set_function_SPI1_SCK();
	GPIO15.set_function_SPI1_TX();
	ST7789 display(spi1, 240, 320, {RST: GPIO10, DC: GPIO11, CS: GPIO12, BL: GPIO13});
	terminal.Initialize().AttachDisplay(display.Initialize(Display::Dir::Rotate90)).SetFont(Font::shinonome16);
	USBHost::Initialize();
	USBHost::Keyboard keyboard;
	terminal.AttachKeyboard(keyboard.SetCapsLockAsCtrl());
#endif
#if 1
	Serial::Terminal terminal;
	Shell::AttachTerminal(terminal.Initialize());
#endif
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
