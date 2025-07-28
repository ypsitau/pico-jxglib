#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/LogicAnalyzer.h"

using namespace jxglib;

//------------------------------------------------------------------------------
// SUMP Protocol Handler
//------------------------------------------------------------------------------
class SUMPProtocol {
public:
	// SUMP Commands
	enum Command {
		CMD_RESET = 0x00,
		CMD_RUN = 0x01,
		CMD_ID = 0x02,
		CMD_GET_METADATA = 0x04,
		CMD_SET_DIVIDER = 0x80,
		CMD_SET_READ_DELAY_COUNT = 0x81,
		CMD_SET_FLAGS = 0x82,
		CMD_SET_TRIGGER_MASK = 0xC0,
		CMD_SET_TRIGGER_VALUES = 0xC1,
		CMD_SET_TRIGGER_CONFIG = 0xC2
	};
	// SUMP Metadata tokens
	enum MetadataToken {
		META_DEVICE_NAME = 0x01,
		META_FPGA_VERSION = 0x02,
		META_PROTOCOL_VERSION = 0x03,
		META_SAMPLE_MEMORY = 0x21,
		META_SAMPLE_RATE = 0x23,
		META_NUMBER_PROBES = 0x40,
		META_END = 0x00
	};
private:
	LogicAnalyzer* pLogicAnalyzer_;
	uint32_t sampleRate_;
	uint32_t readCount_;
	uint32_t delayCount_;
	uint32_t flags_;
	
public:
	SUMPProtocol(LogicAnalyzer* pLogicAnalyzer) : 
		pLogicAnalyzer_(pLogicAnalyzer),
		sampleRate_(1000000),  // 1MHz default
		readCount_(1024),
		delayCount_(0),
		flags_(0) {}
		
	void ProcessCommand(uint8_t cmd);
	void SendMetadata();
	void SendID();
	void RunCapture();
	void SendSamples();
};

void SUMPProtocol::ProcessCommand(uint8_t cmd) {
	switch (cmd) {
		case CMD_RESET:
			// Reset logic analyzer
			pLogicAnalyzer_->Disable();
			break;
			
		case CMD_RUN:
			RunCapture();
			break;
			
		case CMD_ID:
			SendID();
			break;
			
		case CMD_GET_METADATA:
			SendMetadata();
			break;
			
		case CMD_SET_DIVIDER: {
			// Read 4 bytes for divider value
			uint8_t data[4];
			for (int i = 0; i < 4; i++) {
				data[i] = getchar();
			}
			uint32_t divider = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
			sampleRate_ = 100000000 / (divider + 1); // Assuming 100MHz base clock
			break;
		}
		
		case CMD_SET_READ_DELAY_COUNT: {
			// Read 4 bytes for read and delay count
			uint8_t data[4];
			for (int i = 0; i < 4; i++) {
				data[i] = getchar();
			}
			readCount_ = (data[0] << 8) | data[1];
			delayCount_ = (data[2] << 8) | data[3];
			break;
		}
		
		case CMD_SET_FLAGS: {
			// Read 4 bytes for flags
			uint8_t data[4];
			for (int i = 0; i < 4; i++) {
				data[i] = getchar();
			}
			flags_ = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
			break;
		}
		
		default:
			// Unknown command, ignore
			break;
	}
}

void SUMPProtocol::SendID() {
	// Send SUMP ID: "1ALS"
	putchar('1');
	putchar('A');
	putchar('L');
	putchar('S');
}

void SUMPProtocol::SendMetadata() {
	// Device name
	putchar(META_DEVICE_NAME);
	const char* deviceName = "Pico Logic Analyzer\0";
	for (const char* p = deviceName; *p; p++) {
		putchar(*p);
	}
	putchar(0);
	
	// Protocol version
	putchar(META_PROTOCOL_VERSION);
	putchar(0x00);
	putchar(0x00);
	putchar(0x00);
	putchar(0x02); // Version 2
	
	// Sample memory (bytes)
	putchar(META_SAMPLE_MEMORY);
	putchar(0x00);
	putchar(0x00);
	putchar(0x20);
	putchar(0x00); // 8192 samples
	
	// Maximum sample rate
	putchar(META_SAMPLE_RATE);
	putchar(0x05);
	putchar(0xF5);
	putchar(0xE1);
	putchar(0x00); // 100MHz
	
	// Number of probes
	putchar(META_NUMBER_PROBES);
	putchar(0x00);
	putchar(0x00);
	putchar(0x00);
	putchar(0x08); // 8 channels
	
	// End of metadata
	putchar(META_END);
}

void SUMPProtocol::RunCapture()
{
	// Configure and start capture
	int pins[] = {0, 1, 2, 3, 4, 5, 6, 7}; // Default 8 pins
	pLogicAnalyzer_->SetPins(pins, 8);
	pLogicAnalyzer_->UpdateSamplingInfo();
	
	if (pLogicAnalyzer_->Enable()) {
		// Wait for capture to complete or timeout
		absolute_time_t timeout = make_timeout_time_ms(1000); // 1 second timeout
		while (!time_reached(timeout)) {
			if (pLogicAnalyzer_->GetRawEventCount() >= readCount_) {
				break;
			}
			tight_loop_contents();
		}
		pLogicAnalyzer_->Disable();
		SendSamples();
	}
}

void SUMPProtocol::SendSamples()
{
	int eventCount = pLogicAnalyzer_->GetRawEventCount();
	
	// Send samples in reverse order (SUMP requirement)  
	for (int i = eventCount - 1; i >= 0; i--) {
		const auto& rawEvent = pLogicAnalyzer_->GetRawEvent(0, i);
		
		// Get pin bitmap using the correct method
		uint32_t sample = rawEvent.GetPinBitmap(8) & 0xFF; // Assuming 8-bit pin bitmap
		putchar((sample >> 24) & 0xFF);
		putchar((sample >> 16) & 0xFF);
		putchar((sample >> 8) & 0xFF);
		putchar(sample & 0xFF);
	}
}

LABOPlatform laboPlatform;
LogicAnalyzer logicAnalyzer;
SUMPProtocol sumpProtocol(&logicAnalyzer);

int main()
{
	::stdio_init_all();
	laboPlatform.AttachStdio().Initialize();
	
	printf("SUMP Logic Analyzer Ready\n");
	
	for (;;) {
		int c = getchar_timeout_us(0);
		if (c != PICO_ERROR_TIMEOUT) {
			sumpProtocol.ProcessCommand(static_cast<uint8_t>(c));
		}
		Tickable::Tick();
	}
}
