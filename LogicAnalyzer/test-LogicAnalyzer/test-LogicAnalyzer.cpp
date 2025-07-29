#include <stdio.h>
#include "pico/stdlib.h"
#include "jxglib/LABOPlatform.h"
#include "jxglib/LogicAnalyzer.h"

using namespace jxglib;

int main()
{
	::stdio_init_all();
	LABOPlatform& laboPlatform = LABOPlatform::Instance;
	laboPlatform.AttachStdio().Initialize();
	Stream& cdcApplication = laboPlatform.GetCDCApplication();
	LogicAnalyzer::SUMPAdapter sumpProtocol(laboPlatform.GetLogicAnalyzer(), cdcApplication);
	for (;;) Tickable::Tick();
}
