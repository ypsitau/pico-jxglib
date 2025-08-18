#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"

using namespace jxglib;

class SigrokPico {
public:
	enum class Stat {
		Initial, Recover,
		Identify,
		AnalogScaleAndOffset,
		SampleRate,
		SampleLimit,
		AnalogChannelEnable, AnalogChannelEnable_Channel,
		DigitalChannelEnable, DigitalChannelEnable_Channel,
	};
private:
	Stream& stream_;
	Stat stat_;
	int nAnalogChannels_;
	int nDigitalChannels_;
	static const int versionNumber_ = 2;
	int uvoltScale_, uvoltOffset_;
public:
	SigrokPico(Stream& stream);
public:
	bool FeedChar(char ch);
};

SigrokPico::SigrokPico(Stream& stream) : stream_{stream}, stat_{Stat::Initial}, nAnalogChannels_{0}, nDigitalChannels_{0}, uvoltScale_{0}, uvoltOffset_{0}
{
	nDigitalChannels_ = 8;
}

bool SigrokPico::FeedChar(char ch)
{
	switch (stat_) {
	case Stat::Initial: {
		if (ch == '*') {			// Reset
		} else if (ch == '+') {		// Abort
		} else if (ch == 'i') {		// Identify
			stat_ = Stat::Identify;
		} else if (ch == 'a') {		// Analog Scale and Offset
			stat_ = Stat::AnalogScaleAndOffset;
		} else if (ch == 'R') {		// Sample rate
			stat_ = Stat::SampleRate;
		} else if (ch == 'L') {		// Sample limit
			stat_ = Stat::SampleLimit;
		} else if (ch == 'A') {		// Analog Channel Enable
			stat_ = Stat::AnalogChannelEnable;
		} else if (ch == 'D') {		// Digital Channel Enable
			stat_ = Stat::DigitalChannelEnable;
		} else if (ch == 'F') {		// Fixed Sample Mode
		} else if (ch == 'C') {		// Continuous Sample Mode
		}
		break;
	}
	case Stat::Recover: {
		if (ch == '*') {
			stat_ = Stat::Initial;
		}
		break;
	}
	case Stat::Identify: {
		if (ch == '\n') {
			stream_.Printf("SRPICO,A%02d1D%02d,%02d", nAnalogChannels_, nDigitalChannels_, versionNumber_).Flush();
		}
		stat_ = Stat::Initial;
		break;
	}
	case Stat::AnalogScaleAndOffset: {
		int channel = ch - '0';
		if (channel >= 0 && channel < nAnalogChannels_) {
			stream_.Printf("%04dx%05d", uvoltScale_, uvoltOffset_);
		}
		stat_ = Stat::Initial;
		break;
	}
	case Stat::SampleRate: {
		if ('0' <= ch && ch <= '9') {

		} else if (ch == '\n') {
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::SampleLimit: {
		if ('0' <= ch && ch <= '9') {

		} else if (ch == '\n') {
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::AnalogChannelEnable: {
		if (ch == '0' || ch == '1') {

			stat_ = Stat::AnalogChannelEnable_Channel;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::AnalogChannelEnable_Channel: {
		if ('0' <= ch && ch <= '9') {

		} else if (ch == '\n') {
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::DigitalChannelEnable: {
		if (ch == '0' || ch == '1') {

			stat_ = Stat::DigitalChannelEnable_Channel;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	case Stat::DigitalChannelEnable_Channel: {
		if ('0' <= ch && ch <= '9') {

		} else if (ch == '\n') {
			stream_.Printf("*").Flush();
			stat_ = Stat::Initial;
		} else {
			stat_ = Stat::Recover;
		}
		break;
	}
	default:
		break;
	}
	return true;
}

int main()
{	
	SigrokPico sigrokPico(LABOPlatform::Instance.GetStreamApplication());
	::stdio_init_all();
	LABOPlatform& laboPlatform = LABOPlatform::Instance;
	laboPlatform.AttachStdio().Initialize();
	Stream& stream = laboPlatform.GetStreamApplication();
	for (;;) {
		char buff[128];
		int bytesRead = stream.Read(buff, sizeof(buff));
		if (bytesRead > 0) {
			Dump.Ascii()(buff, bytesRead);
			for (int i = 0; i < bytesRead; i++) {
				sigrokPico.FeedChar(buff[i]);
			}
		}
		Tickable::Tick();
	}
}
