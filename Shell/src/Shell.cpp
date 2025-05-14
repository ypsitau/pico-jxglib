//==============================================================================
// Shell.cpp
//==============================================================================
#include "jxglib/FS.h"
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
	char* tokenTbl[16];
	int nToken = count_of(tokenTbl);
	const char* errMsg;
	Readable* ptin = &ReadableDumb::Instance;
	Printable* ptout = &GetTerminal();
	Printable* pterr = &GetTerminal();
	if (!Tokenizer().Tokenize(line, &nToken, tokenTbl, &errMsg)) {
		pterr->Println(errMsg);
		return false;
	}
	if (nToken == 0) return false;
	int argc = nToken;
	char** argv = tokenTbl;
	bool appendFlag = false;
	const char* fileNameOut = nullptr;
	for (int iToken = 0; iToken < nToken; iToken++) {
		if ((appendFlag = (::strcmp(tokenTbl[iToken], ">>") == 0)) || ::strcmp(tokenTbl[iToken], ">") == 0) {
			argc = iToken;
			tokenTbl[iToken] = nullptr;
			if (iToken + 1 >= nToken) {
				pterr->Println("missing file name");
				return false;
			}
			fileNameOut = tokenTbl[iToken + 1];
			break;
		}
	}
	FS::Stream streamOut;
	if (fileNameOut) {
		RefPtr<FS::File> pFile(FS::OpenFile(fileNameOut, appendFlag? "a" : "w"));
		if (!pFile) {
			pterr->Printf("failed to open %s\n", fileNameOut);
			return false;
		}
		streamOut.SetFile(pFile.release());
		ptout = &streamOut;
	}
	for (Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetCmdNext()) {
		if (::strcmp(argv[0], pCmd->GetName()) == 0) {
			pCmdRunning_ = pCmd;
			pCmd->Run(*ptin, *ptout, *pterr, argc, argv);
			return true;
		}
	}
	pterr->Printf("%s: command not found\n", argv[0]);
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
		if (!line) {
			// nothing to do
		} else if (FS::IsLegalDriveName(line)) {
			FS::SetDriveCur(line);
			stat_ = Stat::Begin;
		} else {
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
