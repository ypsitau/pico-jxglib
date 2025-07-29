#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/LogicAnalyzer.h"

using namespace jxglib;

//------------------------------------------------------------------------------
// SUMP Protocol Adapter
// https://firmware.buspirate.com/binmode-reference/protocol-sump
//------------------------------------------------------------------------------
class SUMPAdapter : public Tickable {
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
		static const uint8_t DeviceName				= 0x01;
		static const uint8_t FirmwareVersion		= 0x02;
		static const uint8_t NumberOfProbes			= 0x20;
		static const uint8_t SampleMemory			= 0x21;
		static const uint8_t DynamicMemory			= 0x22;
		static const uint8_t SampleRate				= 0x23;
		static const uint8_t ProtocolVersion		= 0x24;
		static const uint8_t NumberOfProbes_Short	= 0x40;
		static const uint8_t ProtocolVersion_Short	= 0x41;
		static const uint8_t EndOfMetadata			= 0x00;
	};
	struct Config {
		uint32_t triggerMask	= 0;
		uint32_t triggerValues	= 0;
		uint32_t triggerConfig	= 0;
		uint32_t divider		= 1;
		uint16_t delayCount		= 0;
		uint16_t readCount		= 0;
		uint32_t flags			= 0;
	};
	enum class Stat { Cmd, Arg };
private:
	LogicAnalyzer& logicAnalyzer_;
	Stream& stream_;
	struct {
		Stat stat = Stat::Cmd;
		uint8_t cmd = 0;
		uint32_t arg = 0;
		int byteArg = 0;
	} comm_;
	Config cfg_;
public:
	SUMPAdapter(LogicAnalyzer& logicAnalyzer, Stream& stream);
	void ProcessCommand(uint8_t cmd, uint32_t arg);
	void RunCapture();
private:
	void SendValue(uint32_t value);
	void SendMeta(uint8_t tokenKey);
	void SendMeta_String(uint8_t tokenKey, const char* str);
	void SendMeta_32bit(uint8_t tokenKey, uint32_t value);
	void Flush() { stream_.Flush(); }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "SUMPAdapter"; }
	virtual void OnTick() override;
};

SUMPAdapter::SUMPAdapter(LogicAnalyzer& logicAnalyzer, Stream& stream) :
	logicAnalyzer_(logicAnalyzer), stream_(stream)
{}

void SUMPAdapter::ProcessCommand(uint8_t cmd, uint32_t arg)
{
	if (cmd == Command::Reset) {
		//Run->Disable();
	} else if (cmd == Command::Run) {
		RunCapture();
	} else if (cmd == Command::ID) {
		stream_.Print("1ALS").Flush();
	} else if (cmd == Command::GetMetadata) {
		SendMeta_String(TokenKey::DeviceName,			"jxgLABO Logic Analyzer");
		SendMeta_String(TokenKey::FirmwareVersion,		"0.01");
		SendMeta_32bit(TokenKey::SampleMemory,			8192);
		SendMeta_32bit(TokenKey::SampleRate,			10'000'000);
		SendMeta_32bit(TokenKey::ProtocolVersion,		2);
		SendMeta_32bit(TokenKey::NumberOfProbes,		8);
		SendMeta(TokenKey::EndOfMetadata);
		Flush();
	} else if ((cmd & 0xf3) == Command::SetTriggerMask) {
		cfg_.triggerMask = arg;
	} else if ((cmd & 0xf3) == Command::SetTriggerValues) {
		cfg_.triggerValues = arg;
	} else if ((cmd & 0xf3) == Command::SetTriggerConfig) {
		cfg_.triggerConfig = arg;
	} else if (cmd == Command::SetDivider) {
		cfg_.divider = arg;
	} else if (cmd == Command::SetReadDelayCount) {
		cfg_.delayCount = arg >> 16;
		cfg_.readCount = arg & 0xffff;
	} else if (cmd == Command::SetFlags) {
		cfg_.flags = arg;
	}
}

void SUMPAdapter::SendValue(uint32_t value)
{
	stream_.PutByte((value >> 24) & 0xff);
	stream_.PutByte((value >> 16) & 0xff);
	stream_.PutByte((value >> 8) & 0xff);
	stream_.PutByte(value & 0xff);
}

void SUMPAdapter::SendMeta(uint8_t tokenKey)
{
	stream_.PutByte(tokenKey);
}

void SUMPAdapter::SendMeta_String(uint8_t tokenKey, const char* str)
{
	stream_.PutByte(tokenKey);
	stream_.PrintRaw(str);
	stream_.PutByte(0);
}

void SUMPAdapter::SendMeta_32bit(uint8_t tokenKey, uint32_t value)
{
	stream_.PutByte(tokenKey);
	SendValue(value);
}

void SUMPAdapter::RunCapture()
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

void SUMPAdapter::OnTick()
{
	int c = stream_.ReadChar();
	if (c < 0) {
		// nothing to do
	} else if (comm_.stat == Stat::Cmd) {
		comm_.cmd = static_cast<uint8_t>(c);
		if (comm_.cmd & 0x80) {
			comm_.byteArg = 0;
			comm_.stat = Stat::Arg;
		} else {
			ProcessCommand(comm_.cmd, 0);
		}
	} else if (comm_.stat == Stat::Arg) {
		comm_.arg = comm_.arg | static_cast<uint32_t>(static_cast<uint8_t>(c)) << (comm_.byteArg * 8);
		comm_.byteArg++;
		if (comm_.byteArg == 4) {
			ProcessCommand(comm_.cmd, comm_.arg);
			comm_.stat = Stat::Cmd;
		}
	}
}

int main()
{
	::stdio_init_all();
	LABOPlatform& laboPlatform = LABOPlatform::Instance;
	laboPlatform.AttachStdio().Initialize();
	Stream& cdcApplication = laboPlatform.GetCDCApplication();
	SUMPAdapter sumpProtocol(laboPlatform.GetLogicAnalyzer(), cdcApplication);
	for (;;) Tickable::Tick();
}
