//==============================================================================
// Shell.cpp
//==============================================================================
#include "jxglib/Shell.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Shell
//------------------------------------------------------------------------------
Shell Shell::Instance;

Shell::Shell() : stat_{Stat::Begin}, pTerminal_{&TerminalDumb::Instance}, pEntryRunning_{nullptr}
{
	::strcpy(prompt_, ">");
}

bool Shell::RunEntry(char* line)
{
	char* argv[16];
	int argc = count_of(argv);
	const char* errMsg;
	if (!Tokenizer().Tokenize(line, &argc, argv, &errMsg)) {
		GetTerminal().Println(errMsg);
		return false;
	}
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

void Shell::OnTick()
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

void Shell::SetPrompt_(const char* prompt)
{
	size_t len = ChooseMin(sizeof(prompt_) - 1, ::strlen(prompt));
	::memcpy(prompt_, prompt, len);
	prompt_[len] = '\0';
}

void Shell::PrintHelp(Printable& printable)
{
	int lenMax = 1;
	for (const Entry* pEntry = Entry::GetEntryHead(); pEntry; pEntry = pEntry->GetEntryNext()) {
		lenMax = ChooseMax(lenMax, static_cast<int>(::strlen(pEntry->GetName())));
	}
	for (const Entry* pEntry = Entry::GetEntryHead(); pEntry; pEntry = pEntry->GetEntryNext()) {
		printable.Printf("%-*s  %s\n", lenMax, pEntry->GetName(), pEntry->GetHelp());
	}
}

//------------------------------------------------------------------------------
// Shell::Entry
//------------------------------------------------------------------------------
Shell::Entry* Shell::Entry::pEntryHead_ = nullptr;

Shell::Entry::Entry(const char* name, const char* help) : name_{name}, help_{help}, pEntryNext_{nullptr}
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
