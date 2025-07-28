#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/LogicAnalyzer.h"

using namespace jxglib;

//------------------------------------------------------------------------------
// SUMP Protocol Handler
// https://firmware.buspirate.com/binmode-reference/protocol-sump
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
		static const uint8_t SetTriggerMask		= 0xc0;
		static const uint8_t SetTriggerValues	= 0xc1;
		static const uint8_t SetTriggerConfig	= 0xc2;
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
	void ProcessCommand(uint8_t cmd, uint32_t arg);
	void RunCapture();
private:
	void SendValue(uint32_t value);
	void SendMetadata(uint8_t tokenKey);
	void SendMetadata(uint8_t tokenKey, const char* str);
	void SendMetadata(uint8_t tokenKey, uint32_t value);
	void Flush() { stream_.Flush(); }
};

SUMPProtocol::SUMPProtocol(LogicAnalyzer& logicAnalyzer, Stream& stream) : 
	logicAnalyzer_(logicAnalyzer), stream_(stream), sampleRate_(1000000), readCount_(1024), delayCount_(0), flags_(0)
{}

void SUMPProtocol::ProcessCommand(uint8_t cmd, uint32_t arg)
{
	uint8_t iStage = (cmd & 0x0c) >> 2;
	switch (cmd & 0xf3) {
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
	case Command::GetMetadata: {
		SendMetadata(TokenKey::DeviceName,		"pico-jxgLABO Logic Analyzer");
		SendMetadata(TokenKey::ProtocolVersion,	2);
		SendMetadata(TokenKey::SampleMemory,	8192);
		SendMetadata(TokenKey::MaxSampleRate,	100000000);
		SendMetadata(TokenKey::NumberProbes,	8);
		SendMetadata(TokenKey::END);
		Flush();
		break;
	}
	case Command::SetTriggerMask: {
		::printf("triggerMask: %u\n", arg);
		break;
	}
	case Command::SetTriggerValues: {
		::printf("triggerValues: %u\n", arg);
		break;
	}
	case Command::SetTriggerConfig: {
		::printf("triggerConfig: %u\n", arg);
		break;
	}
	case Command::SetDivider: {
		::printf("divider: %u\n", arg);
		break;
	}
	case Command::SetReadDelayCount: {
		delayCount_ = arg >> 16;
		readCount_ = arg & 0xffff;
		::printf("Read count set to %u, delay count set to %u\n", readCount_, delayCount_);
		break;
	}
	case Command::SetFlags: {
		flags_ = arg;
		::printf("Flags set to 0x%08X\n", flags_);
		break;
	}
	default:
		break;
	}
}

void SUMPProtocol::SendValue(uint32_t value)
{
	stream_.PutCharRaw((value >> 24) & 0xff);
	stream_.PutCharRaw((value >> 16) & 0xff);
	stream_.PutCharRaw((value >> 8) & 0xff);
	stream_.PutCharRaw(value & 0xff);
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
	SendValue(value);
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
	for (int j = 0; j < 50; ++j) {
		SendValue(0xff);
		SendValue(0x00);
	}
	Flush();
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
	enum class Stat { Cmd, Arg };
	Stat stat = Stat::Cmd;
	uint8_t cmd = 0;
	uint32_t arg = 0;
	int byteArg = 0;
	for (;;) {
		int c = cdcApplication.ReadChar();
		if (c < 0) {
			// nothing to do
		} else if (stat == Stat::Cmd) {
			cmd = static_cast<uint8_t>(c);
			if (cmd & 0x80) {
				byteArg = 0;
				stat = Stat::Arg;
			} else {
				sumpProtocol.ProcessCommand(cmd, 0);
			}
		} else if (stat == Stat::Arg) {
			arg = arg | static_cast<uint32_t>(static_cast<uint8_t>(c)) << (byteArg * 8);
			byteArg++;
			if (byteArg == 4) {
				sumpProtocol.ProcessCommand(cmd, arg);
				stat = Stat::Cmd;
			}
		}
		Tickable::Tick();
	}
}
