//==============================================================================
// CmdLine.cpp
//==============================================================================
#include "jxglib/CmdLine.h"

namespace jxglib {

//------------------------------------------------------------------------------
// CmdLine
//------------------------------------------------------------------------------
CmdLine CmdLine::Instance;

CmdLine::CmdLine() : stat_{Stat::Begin}, pTerminal_{&TerminalDumb::Instance}, pEntryRunning_{nullptr}
{
	::strcpy(prompt_, ">");
}

void CmdLine::RunEntry(char* line)
{
	int argc = 1;
	char* argv[16];
	argv[0] = line;
	Entry* pEntry = Entry::GetEntryHead();
	pEntryRunning_ = pEntry;
	pEntry->Run(GetTerminal(), argc, argv);
	stat_ = Stat::Begin;
}

void CmdLine::OnTick()
{
	switch (stat_) {
	case Stat::Begin: {
		GetTerminal().ReadLine_Begin(prompt_);
		stat_ = Stat::Prompt;
		break;
	}
	case Stat::Prompt: {
		char* line = GetTerminal().ReadLine_Process();
		if (line) {
			stat_ = Stat::Running;
			RunEntry(line);
		}
		break;
	}
	case Stat::Running: {
		// nothing to do
		break;
	}
	}
}

//------------------------------------------------------------------------------
// CmdLine::Entry
//------------------------------------------------------------------------------
CmdLine::Entry* CmdLine::Entry::pEntryHead_ = nullptr;

CmdLine::Entry::Entry(const char* name) : name_{name}
{
	pEntryNext_ = pEntryHead_;
	pEntryHead_ = this;
}

}
