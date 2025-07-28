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
	struct Command {
		static const uint8_t RESET				= 0x00;
		static const uint8_t RUN				= 0x01;
		static const uint8_t ID					= 0x02;
		static const uint8_t GET_METADATA		= 0x04;
		static const uint8_t SET_DIVIDER		= 0x80;
		static const uint8_t SET_READ_DELAY_COUNT	= 0x81;
		static const uint8_t SET_FLAGS			= 0x82;
		static const uint8_t SET_TRIGGER_MASK	= 0xC0;
		static const uint8_t SET_TRIGGER_VALUES	= 0xC1;
		static const uint8_t SET_TRIGGER_CONFIG	= 0xC2;
	};
	// SUMP Metadata tokens
	struct MetaToken {
		static const uint8_t DEVICE_NAME		= 0x01;
		static const uint8_t FPGA_VERSION		= 0x02;
		static const uint8_t PROTOCOL_VERSION	= 0x03;
		static const uint8_t SAMPLE_MEMORY		= 0x21;
		static const uint8_t SAMPLE_RATE		= 0x23;
		static const uint8_t NUMBER_PROBES		= 0x40;
		static const uint8_t END				= 0x00;
	};
private:
	LogicAnalyzer& logicAnalyzer_;
	Stream& stream_;
	uint32_t sampleRate_;
	uint32_t readCount_;
	uint32_t delayCount_;
	uint32_t flags_;
	
public:
	SUMPProtocol(LogicAnalyzer& logicAnalyzer, Stream& stream) : 
			logicAnalyzer_(logicAnalyzer), stream_(stream),
			sampleRate_(1000000), readCount_(1024), delayCount_(0), flags_(0) {}
	void ProcessCommand(uint8_t cmd);
	void SendMetadata();
	void SendID();
	void RunCapture();
	void SendSamples();
private:
	void SendMetaData(uint8_t token);
	void SendMetaData(uint8_t token, const char* str);
	void SendMetaData(uint8_t token, uint32_t value);
};

void SUMPProtocol::ProcessCommand(uint8_t cmd)
{
	switch (cmd) {
	case Command::RESET:
		//pLogicAnalyzer_->Disable();
		break;
	case Command::RUN:
		RunCapture();
		break;
	case Command::ID:
		stream_.Print("1ALS").Flush();
		break;
	case Command::GET_METADATA:
		SendMetaData(MetaToken::DEVICE_NAME,		"pico-jxgLABO Logic Analyzer");
		SendMetaData(MetaToken::PROTOCOL_VERSION,	2);
		SendMetaData(MetaToken::SAMPLE_MEMORY,		8192);
		SendMetaData(MetaToken::SAMPLE_RATE,		100000000);
		SendMetaData(MetaToken::NUMBER_PROBES,		8);
		SendMetaData(MetaToken::END);
		stream_.Flush();
		break;
	case Command::SET_DIVIDER: {
		// Read 4 bytes for divider value
		uint8_t data[4];
		for (int i = 0; i < 4; i++) {
			data[i] = getchar();
		}
		uint32_t divider = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
		sampleRate_ = 100000000 / (divider + 1); // Assuming 100MHz base clock
		break;
	}
	case Command::SET_READ_DELAY_COUNT: {
		// Read 4 bytes for read and delay count
		uint8_t data[4];
		for (int i = 0; i < 4; i++) {
			data[i] = getchar();
		}
		readCount_ = (data[0] << 8) | data[1];
		delayCount_ = (data[2] << 8) | data[3];
		break;
	}
	case Command::SET_FLAGS: {
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

void SUMPProtocol::SendMetaData(uint8_t token)
{
	stream_.PutCharRaw(token);
}

void SUMPProtocol::SendMetaData(uint8_t token, const char* str)
{
	stream_.PutCharRaw(token);
	stream_.PrintRaw(str);
	stream_.PutCharRaw(0);
}

void SUMPProtocol::SendMetaData(uint8_t token, uint32_t value)
{
	stream_.PutCharRaw(token);
	stream_.PutCharRaw((value >> 24) & 0xFF);
	stream_.PutCharRaw((value >> 16) & 0xFF);
	stream_.PutCharRaw((value >> 8) & 0xFF);
	stream_.PutCharRaw(value & 0xFF);
}

void SUMPProtocol::RunCapture()
{
	// Configure and start capture
	//int pins[] = {0, 1, 2, 3, 4, 5, 6, 7}; // Default 8 pins
	//pLogicAnalyzer_->SetPins(pins, 8);
	//pLogicAnalyzer_->UpdateSamplingInfo();
	
	//if (pLogicAnalyzer_->Enable()) {
	//	// Wait for capture to complete or timeout
	//	absolute_time_t timeout = make_timeout_time_ms(1000); // 1 second timeout
	//	while (!time_reached(timeout)) {
	//		if (pLogicAnalyzer_->GetRawEventCount() >= readCount_) {
	//			break;
	//		}
	//		tight_loop_contents();
	//	}
	//	pLogicAnalyzer_->Disable();
	//	SendSamples();
	//}
}

void SUMPProtocol::SendSamples()
{
	//int eventCount = pLogicAnalyzer_->GetRawEventCount();
	//
	//// Send samples in reverse order (SUMP requirement)  
	//for (int i = eventCount - 1; i >= 0; i--) {
	//	const auto& rawEvent = pLogicAnalyzer_->GetRawEvent(0, i);
	//	
	//	// Get pin bitmap using the correct method
	//	uint32_t sample = rawEvent.GetPinBitmap(8) & 0xFF; // Assuming 8-bit pin bitmap
	//	putchar((sample >> 24) & 0xFF);
	//	putchar((sample >> 16) & 0xFF);
	//	putchar((sample >> 8) & 0xFF);
	//	putchar(sample & 0xFF);
	//}
}

LABOPlatform laboPlatform;
LogicAnalyzer logicAnalyzer;

int main()
{
	::stdio_init_all();
	laboPlatform.AttachStdio().Initialize();
	Stream& cdcApplication = laboPlatform.GetCDCApplication();
	SUMPProtocol sumpProtocol(logicAnalyzer, cdcApplication);
	//printf("SUMP Logic Analyzer Ready\n");
	for (;;) {
		int c = cdcApplication.ReadChar();
		if (c >= 0) sumpProtocol.ProcessCommand(static_cast<uint8_t>(c));
		Tickable::Tick();
	}
}
