//==============================================================================
// CmdLine.cpp
//==============================================================================
#include <ctype.h>
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

bool CmdLine::RunEntry(char* line)
{
	enum class Stat {
		SeekHead, SeekQuotation, SeekSpace,
	} stat = Stat::SeekHead;
	int argc = 0;
	char* argv[16];
	bool contFlag = true;
	for (char* p = line; contFlag; p++) {
		switch (stat) {
		case Stat::SeekHead: {
			if (!*p) {
				contFlag = false;
			} else if (isspace(*p)) {
				// nothing to do
			} else if (*p == '"') {
				if (argc < count_of(argv) - 1) argv[argc++] = p + 1;
				stat = Stat::SeekQuotation;
			} else {
				if (argc < count_of(argv) - 1) argv[argc++] = p;
				stat = Stat::SeekSpace;
			}
			break;
		}
		case Stat::SeekQuotation: {
			if (!*p) {
				GetTerminal().Printf("unmatched double quotation\n");
				return false;
			} else if (*p == '"') {
				*p = '\0';
				stat = Stat::SeekHead;
			} else {
				// nothing to do
			}
			break;
		}
		case Stat::SeekSpace: {
			if (!*p) {
				contFlag = false;
			} else if (isspace(*p)) {
				*p = '\0';
				stat = Stat::SeekHead;
			} else {
				// nothing to do
			}
			break;
		}
		}
	}
	argv[argc] = nullptr;
	if (argc == 0) return false;
	for (Entry* pEntry = Entry::GetEntryHead(); pEntry; pEntry = pEntry->GetEntryNext()) {
		if (::strcmp(argv[0], pEntry->GetName()) == 0) {
			pEntryRunning_ = pEntry;
			pEntry->Run(GetTerminal(), argc, argv);
			return true;
		}
	}
	GetTerminal().Printf("%s: command not found\n", argv[0]);
	return false;
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
			stat_ = Stat::Begin;
		}
		break;
	}
	case Stat::Running: {
		// nothing to do
		break;
	}
	}
}

void CmdLine::SetPrompt_(const char* prompt)
{
	size_t len = ChooseMin(sizeof(prompt_) - 1, ::strlen(prompt));
	::memcpy(prompt_, prompt, len);
	prompt_[len] = '\0';
}

void CmdLine::PrintList(Printable& printable)
{
	for (Entry* pEntry = Entry::GetEntryHead(); pEntry; pEntry = pEntry->GetEntryNext()) {
		printable.Printf("%s\n", pEntry->GetName());
	}
}

//------------------------------------------------------------------------------
// CmdLine::Entry
//------------------------------------------------------------------------------
CmdLine::Entry* CmdLine::Entry::pEntryHead_ = nullptr;

CmdLine::Entry::Entry(const char* name) : name_{name}, pEntryNext_{nullptr}
{
	Entry* pEntryPrev = nullptr;
	for (Entry* pEntry = pEntryHead_; pEntry; pEntry = pEntry->GetEntryNext()) {
		if (::strcmp(name, pEntry->GetName()) <= 0) break;
		pEntryPrev = pEntry;
	}
	if (pEntryPrev) {
		SetEntryNext(pEntryPrev->GetEntryNext());
		pEntryPrev->SetEntryNext(this);
	} else {
		SetEntryNext(pEntryHead_);
		pEntryHead_ = this;
	}
}

}
