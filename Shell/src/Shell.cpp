//==============================================================================
// Shell.cpp
//==============================================================================
#include "jxglib/Shell.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Shell
//------------------------------------------------------------------------------
Shell Shell::Instance;

Shell::Shell() : stat_{Stat::Begin}, pTerminal_{&TerminalDumb::Instance}, pCmdRunning_{nullptr}
{
	::strcpy(prompt_, ">");
}

bool Shell::RunCmd(char* line)
{
	char* argv[16];
	int argc = count_of(argv);
	const char* errMsg;
	if (!Tokenizer().Tokenize(line, &argc, argv, &errMsg)) {
		GetTerminal().Println(errMsg);
		return false;
	}
	if (argc == 0) return false;
	for (Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetCmdNext()) {
		if (::strcmp(argv[0], pCmd->GetName()) == 0) {
			pCmdRunning_ = pCmd;
			pCmd->Run(GetTerminal(), argc, argv);
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
			RunCmd(line);
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
	for (const Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetCmdNext()) {
		lenMax = ChooseMax(lenMax, static_cast<int>(::strlen(pCmd->GetName())));
	}
	for (const Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetCmdNext()) {
		printable.Printf("%-*s  %s\n", lenMax, pCmd->GetName(), pCmd->GetHelp());
	}
}

//------------------------------------------------------------------------------
// Shell::Cmd
//------------------------------------------------------------------------------
Shell::Cmd* Shell::Cmd::pCmdHead_ = nullptr;

Shell::Cmd::Cmd(const char* name, const char* help) : name_{name}, help_{help}, pCmdNext_{nullptr}
{
	Cmd* pCmdPrev = nullptr;
	for (Cmd* pCmd = pCmdHead_; pCmd; pCmd = pCmd->GetCmdNext()) {
		if (::strcmp(name, pCmd->GetName()) <= 0) break;
		pCmdPrev = pCmd;
	}
	if (pCmdPrev) {
		SetCmdNext(pCmdPrev->GetCmdNext());
		pCmdPrev->SetCmdNext(this);
	} else {
		SetCmdNext(pCmdHead_);
		pCmdHead_ = this;
	}
}

}
