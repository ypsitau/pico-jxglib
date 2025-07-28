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
		static const uint8_t Reset				= 0x00;
		static const uint8_t Run				= 0x01;
		static const uint8_t ID					= 0x02;
		static const uint8_t GetMetadata		= 0x04;
		static const uint8_t SetDivider			= 0x80;
		static const uint8_t SetReadDelayCount	= 0x81;
		static const uint8_t SetFlags			= 0x82;
		static const uint8_t SetTriggerMask		= 0xC0;
		static const uint8_t SetTriggerValues	= 0xC1;
		static const uint8_t SetTriggerConfig	= 0xC2;
	};
	// SUMP Metadata tokens
	struct TokenKey {
		static const uint8_t DeviceName			= 0x01;
		static const uint8_t FPGAVersion		= 0x02;
		static const uint8_t NumberProbes		= 0x20;
		static const uint8_t SampleMemory		= 0x21;
		static const uint8_t DynamicMemory		= 0x22;
		static const uint8_t MaxSampleRate		= 0x23;
		static const uint8_t ProtocolVersion	= 0x24;
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
	SUMPProtocol(LogicAnalyzer& logicAnalyzer, Stream& stream);
	void ProcessCommand(uint8_t cmd);
	void SendMetadata();
	void SendID();
	void RunCapture();
	void SendSamples();
private:
	void SendMetadata(uint8_t tokenKey);
	void SendMetadata(uint8_t tokenKey, const char* str);
	void SendMetadata(uint8_t tokenKey, uint32_t value);
};

SUMPProtocol::SUMPProtocol(LogicAnalyzer& logicAnalyzer, Stream& stream) : 
	logicAnalyzer_(logicAnalyzer), stream_(stream), sampleRate_(1000000), readCount_(1024), delayCount_(0), flags_(0)
{}

void SUMPProtocol::ProcessCommand(uint8_t cmd)
{
	switch (cmd) {
	case Command::Reset: {
		//Run->Disable();
		break;
	}
	case Command::Run: {
		RunCapture();
		break;
	}
	case Command::ID: {
		stream_.Print("1ALS").Flush();
		break;
	}
	case Command::GetMetadata:
		SendMetadata(TokenKey::DeviceName,		"pico-jxgLABO Logic Analyzer");
		SendMetadata(TokenKey::ProtocolVersion,	2);
		SendMetadata(TokenKey::SampleMemory,	8192);
		SendMetadata(TokenKey::MaxSampleRate,	100000000);
		SendMetadata(TokenKey::NumberProbes,	8);
		SendMetadata(TokenKey::END);
		stream_.Flush();
		break;
	case Command::SetDivider: {
		// Read 4 bytes for divider value
		uint8_t data[4];
		for (int i = 0; i < 4; i++) {
			data[i] = getchar();
		}
		uint32_t divider = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
		sampleRate_ = 100000000 / (divider + 1); // Assuming 100MHz base clock
		break;
	}
	case Command::SetReadDelayCount: {
		// Read 4 bytes for read and delay count
		uint8_t data[4];
		for (int i = 0; i < 4; i++) {
			data[i] = getchar();
		}
		readCount_ = (data[0] << 8) | data[1];
		delayCount_ = (data[2] << 8) | data[3];
		break;
	}
	case Command::SetFlags: {
		// Read 4 bytes for flags
		uint8_t data[4];
		for (int i = 0; i < 4; i++) {
			data[i] = getchar();
		}
		flags_ = (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];
		break;
	}
	default:
		break;
	}
}

void SUMPProtocol::SendMetadata(uint8_t tokenKey)
{
	stream_.PutCharRaw(tokenKey);
}

void SUMPProtocol::SendMetadata(uint8_t tokenKey, const char* str)
{
	stream_.PutCharRaw(tokenKey);
	stream_.PrintRaw(str);
	stream_.PutCharRaw(0);
}

void SUMPProtocol::SendMetadata(uint8_t tokenKey, uint32_t value)
{
	stream_.PutCharRaw(tokenKey);
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
