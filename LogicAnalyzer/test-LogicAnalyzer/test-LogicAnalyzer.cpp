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
	laboPlatform.GetLogicAnalyzer().AttachTelePlot(laboPlatform.GetTelePlot());
	//Stream& streamApplication = laboPlatform.GetStreamApplication();
	//LogicAnalyzer::SUMPAdapter sumpProtocol(laboPlatform.GetLogicAnalyzer(), streamApplication);
	for (;;) Tickable::Tick();
}
