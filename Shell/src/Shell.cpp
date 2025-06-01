//==============================================================================
// Shell.cpp
//==============================================================================
#include <stdlib.h>
#include "jxglib/Shell.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Shell
//------------------------------------------------------------------------------
Shell Shell::Instance;
const char* Shell::StartupScriptName = "/autoexec.sh";

Shell::Shell() : stat_{Stat::Startup}, pTerminal_{&TerminalDumb::Instance}, pCmdRunning_{nullptr}
{
	::strcpy(prompt_, "%d%w>");
}

bool Shell::RunCmd(char* line)
{
	char* tokenTbl[16];
	int nToken = count_of(tokenTbl);
	const char* errMsg;
	Terminal::ReadableKeyboard tin(GetTerminal());
	Readable* ptin = &tin;
	Printable* ptout = &GetTerminal();
	Printable* pterr = &GetTerminal();
	if (FS::IsLegalDriveName(line)) {
		if (FS::SetDriveCur(line)) return true;
		pterr->Printf("failed to change drive to %s\n", line);
		return false;
	}
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
	RefPtr<FS::File> pFileOut;
	if (fileNameOut) {
		pFileOut.reset(FS::OpenFile(fileNameOut, appendFlag? "a" : "w"));
		if (!pFileOut) {
			pterr->Printf("failed to open %s\n", fileNameOut);
			return false;
		}
		ptout = pFileOut.get();
	}
	for (Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetNext()) {
		if (::strcmp(argv[0], pCmd->GetName()) == 0) {
			pCmdRunning_ = pCmd;
			pCmd->Run(*ptin, *ptout, *pterr, argc, argv);
			return true;
		}
	}
	pterr->Printf("%s: command not found\n", argv[0]);
	return false;
}

bool Shell::RunScript(Readable& readable)
{
	char line[256];
	while (readable.ReadLine(line, sizeof(line)) > 0) {
		if (!RunCmd(line)) return false;
	}
	return true;
}

void Shell::OnTick()
{
	switch (stat_) {
	case Stat::Startup: {
		FS::Drive* pDrive = FS::GetDriveCur();
		if (pDrive && pDrive->IsPrimary()) {
			RefPtr<FS::File> pFileScript(FS::OpenFile(StartupScriptName, "r"));
			if (pFileScript) RunScript(*pFileScript);
		}
		stat_ = Stat::Begin;
		break;
	}
	case Stat::Begin: {
		char prompt[256];
		GetTerminal().ReadLine_Begin(MakePrompt(prompt, sizeof(prompt)));
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

const char* Shell::MakePrompt(char* prompt, int lenMax)
{
	enum class Stat { Normal, Variable, };
	Stat stat = Stat::Normal;
	int i = 0;
	for (const char* p = prompt_; *p && lenMax > 0; p++, lenMax--) {
		char ch = *p;
		switch (stat) {
		case Stat::Normal: {
			if (ch == '%') {
				stat = Stat::Variable;
			} else {
				if (i < lenMax) prompt[i++] = ch;
			}
			break;
		}
		case Stat::Variable: {
			if (ch == 'd') {
				FS::Drive* pDrive = FS::GetDriveCur();
				if (pDrive) {
					int len = ChooseMin(::strlen(pDrive->GetDriveName()), lenMax - i);
					::memcpy(prompt + i, pDrive->GetDriveName(), len);
					i += len;
					if (i < lenMax) prompt[i++] = ':';
				}
			} else if (ch == 'w') {
				FS::Drive* pDrive = FS::GetDriveCur();
				if (pDrive) {
					int len = ChooseMin(::strlen(pDrive->GetDirNameCur()), lenMax - i);
					::memcpy(prompt + i, pDrive->GetDirNameCur(), len);
					if (len > 1 && prompt[i + len - 1] == '/') {
						prompt[i + len - 1] = '\0'; // remove trailing slash
						len--;
					}
					i += len;
				}
			} else {
				// nothing to do, just ignore the variable
			}
			stat = Stat::Normal;
			break;
		}
		default:
			break;
		}
	}
	if (i >= lenMax) ::panic("Shell::MakePrompt: prompt buffer overflow");
	prompt[i++] = '\0';
	return prompt;
}

void Shell::AttachTerminal_(Terminal& terminal)
{
	pTerminal_ = &terminal;
	pTerminal_->SetCompletionProvider(completionProvider_);
}

void Shell::PrintHelp(Printable& printable)
{
	int lenMax = 1;
	for (const Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetNext()) {
		lenMax = ChooseMax(lenMax, static_cast<int>(::strlen(pCmd->GetName())));
	}
	for (const Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetNext()) {
		printable.Printf("%-*s  %s\n", lenMax, pCmd->GetName(), pCmd->GetHelp());
	}
}

//-----------------------------------------------------------------------------
// Shell::Arg
//-----------------------------------------------------------------------------
const Shell::Arg Shell::Arg::None(nullptr, 0);

bool Shell::Arg::Parse(Printable& terr, int& argc, const char* argv[])
{
	for (int iArg = 1; iArg < argc; ) {
		if (argv[iArg][0] == '-') {
			int nArgsToRemove = 1;
			const Opt* pOptFound = nullptr;
			const char* value = nullptr;
			if (argv[iArg][1] == '-') {
				// long option
				const char* longName = argv[iArg] + 2;
				for (int iOpt = 0; iOpt < nOpts_; iOpt++) {
					const Opt& opt = optTbl_[iOpt];
					if (opt.CheckLongName(longName, &value)) {
						if (opt.DoesRequireValue() && !value) {
							terr.Printf("missing value for option: %s\n", argv[iArg]);
							return false;
						}
						pOptFound = &opt;
						break;
					}
				}
			} else {
				// short option
				const char* shortName = argv[iArg] + 1;
				for (int iOpt = 0; iOpt < nOpts_; iOpt++) {
					const Opt& opt = optTbl_[iOpt];
					if (opt.CheckShortName(shortName)) {
						if (opt.DoesRequireValue()) {
							if (iArg + 1 >= argc) {
								terr.Printf("missing value for option: %s\n", argv[iArg]);
								return false;
							}
							value = argv[iArg + 1];
							nArgsToRemove = 2;
						} 
						pOptFound = &opt;
						break;
					}
				}
			}
			if (!pOptFound) {
				terr.Printf("unknown option: %s\n", argv[iArg]);
				return false;
			}
			if (pOptFound->GetType() == Opt::Type::Int) {
				const char* pEnd = value;
				::strtol(value, const_cast<char**>(&pEnd), 0);
				if (*pEnd != '\0') {
					terr.Printf("invalid value for option: %s\n", argv[iArg]);
					return false;
				}
			}
			AddOptValue(pOptFound, value);
			for (int iArgIter = iArg; iArgIter + nArgsToRemove <= argc; iArgIter++) argv[iArgIter] = argv[iArgIter + nArgsToRemove];
			argc -= nArgsToRemove;
		} else {
			++iArg;
		}
	}
	return true;	
}

void Shell::Arg::PrintHelp(Printable& tout) const
{
	int lenMax = 0;
	char str[80];
	for (int iOpt = 0; iOpt < nOpts_; iOpt++) {
		const Opt& opt = optTbl_[iOpt];
		opt.MakeHelp(str, sizeof(str));
		lenMax = ChooseMax(::strlen(str), lenMax);;
	}
	for (int iOpt = 0; iOpt < nOpts_; iOpt++) {
		const Opt& opt = optTbl_[iOpt];
		opt.MakeHelp(str, sizeof(str));
		tout.Printf("%-*s %s\n", lenMax, str, opt.GetHelp());
		//tout.Printf("%s %s\n", str, opt.GetHelp());
	}
}

bool Shell::Arg::GetBool(const char* longName) const
{
	return !!FindOptValue(longName);
}

bool Shell::Arg::GetString(const char* longName, const char** pValue) const
{
	const OptValue* pOptValue = FindOptValue(longName);
	if (pOptValue) {
		*pValue = pOptValue->GetValue();
		return true;
	}
	return false;
}

bool Shell::Arg::GetInt(const char* longName, int* pValue) const
{
	const OptValue* pOptValue = FindOptValue(longName);
	if (pOptValue) {
		*pValue = ::strtol(pOptValue->GetValue(), nullptr, 0);
		return true;
	}
	return false;
}

void Shell::Arg::AddOptValue(const Opt* pOpt, const char* value)
{
	if (!value) value = "";
	if (pOptValueHead_) {
		OptValue* pOptValue = pOptValueHead_.get();
		for ( ; pOptValue->GetNext(); pOptValue = pOptValue->GetNext()) ;
		pOptValue->SetNext(new OptValue(pOpt, value));
	} else {
		pOptValueHead_.reset(new OptValue(pOpt, value));
	}
}

const Shell::Arg::OptValue* Shell::Arg::FindOptValue(const char* longName) const
{
	for (const OptValue* pOptValue = pOptValueHead_.get(); pOptValue; pOptValue = pOptValue->GetNext()) {
		if (pOptValue->GetOpt().CheckLongName(longName, nullptr)) return pOptValue;
	}
	return nullptr;
}

//-----------------------------------------------------------------------------
// Shell::Arg::Opt
//-----------------------------------------------------------------------------
bool Shell::Arg::Opt::CheckLongName(const char* longName, const char** pValue) const
{
	const char* p1 = longName;
	const char* p2 = longName_;
	for ( ; ; p1++, p2++) {
		char ch1 = (*p1 == '=')? '\0' : *p1;
		char ch2 = *p2;
		if (ch1 != ch2) return false;
		if (ch1 == '\0') break;
	}
	if (pValue && *p1 == '=') *pValue = p1 + 1;
	return true;
}

bool Shell::Arg::Opt::CheckShortName(const char* shortName) const
{
	return ::strcmp(shortName, shortName_) == 0;
}

void Shell::Arg::Opt::MakeHelp(char* str, int len) const
{
	if (type_ == Type::Bool) {
		if (shortName_[0] == '\0') {
			::snprintf(str, len, "--%s", longName_);
		} else {
			::snprintf(str, len, "--%s, -%s", longName_, shortName_);
		}
	} else {
		if (shortName_[0] == '\0') {
			::snprintf(str, len, "--%s=%s", longName_, helpValue_);
		} else {
			::snprintf(str, len, "--%s=%s, -%s %s", longName_, helpValue_, shortName_, helpValue_);
		}
	}
}

//------------------------------------------------------------------------------
// Shell::Cmd
//------------------------------------------------------------------------------
Shell::Cmd* Shell::Cmd::pCmdHead_ = nullptr;

Shell::Cmd::Cmd(const char* name, const char* help) : name_{name}, help_{help}, pCmdNext_{nullptr}
{
	Cmd* pCmdPrev = nullptr;
	for (Cmd* pCmd = pCmdHead_; pCmd; pCmd = pCmd->GetNext()) {
		if (::strcmp(name, pCmd->GetName()) <= 0) break;
		pCmdPrev = pCmd;
	}
	if (pCmdPrev) {
		SetNext(pCmdPrev->GetNext());
		pCmdPrev->SetNext(this);
	} else {
		SetNext(pCmdHead_);
		pCmdHead_ = this;
	}
}

//------------------------------------------------------------------------------
// Shell::CandidateProvider_Cmd
//------------------------------------------------------------------------------
const char* Shell::CandidateProvider_Cmd::NextItemName(bool* pWrappedAroundFlag)
{
	if (!pCmdCur_) return nullptr;
	const char* itemName = pCmdCur_->GetName();
	pCmdCur_ = pCmdCur_->GetNext();
	if (*pWrappedAroundFlag = !pCmdCur_) pCmdCur_ = Cmd::GetCmdHead(); // loop back to the head
	return itemName;
}

//------------------------------------------------------------------------------
// Shell::CandidateProvider_FileInfo
//------------------------------------------------------------------------------
const char* Shell::CandidateProvider_FileInfo::NextItemName(bool* pWrappedAroundFlag)
{
	if (!pFileInfoCur_) return nullptr;
	const char* itemName = pFileInfoCur_->GetName();
	pFileInfoCur_ = pFileInfoCur_->GetNext();
	if (*pWrappedAroundFlag = !pFileInfoCur_) pFileInfoCur_ = pFileInfoHead_.get();
	return itemName;
}

//------------------------------------------------------------------------------
// Shell::CompletionProvider
//------------------------------------------------------------------------------
void Shell::CompletionProvider::StartCompletion()
{
	dirName_[0] = '\0';
	prefix_ = "";
	nItemsReturned_ = 0;
	if (GetIByte() == 0) {
		// the first field is a command name
		pCandidateProvider_.reset(new CandidateProvider_Cmd());
		prefix_ = GetHint();
	} else {
		// following fields are file names
		RefPtr<FS::Dir> pDir(FS::OpenDir(GetHint()));
		::snprintf(dirName_, sizeof(dirName_), "%s", GetHint());
		if (!pDir) {
			FS::SplitDirName(GetHint(), dirName_, sizeof(dirName_), &prefix_);
			pDir.reset(FS::OpenDir(dirName_));
		}
		if (pDir) {
			std::unique_ptr<FS::FileInfo> pFileInfoHead(pDir->ReadAll(FS::FileInfo::CmpDefault));
			pCandidateProvider_.reset(new CandidateProvider_FileInfo(pFileInfoHead.release()));
		}
	}
}

void Shell::CompletionProvider::EndCompletion()
{
	pCandidateProvider_.reset();
}

const char* Shell::CompletionProvider::NextCompletion()
{
	if (!pCandidateProvider_) return nullptr;
	const char* itemName;
	bool wrappedAroundFlag = false;
	while (itemName = pCandidateProvider_->NextItemName(&wrappedAroundFlag)) {
		if (StartsWithICase(itemName, prefix_)) {
			nItemsReturned_++;
			FS::JoinPathName(result_, sizeof(result_), dirName_, itemName);
			return result_;
		} else if (wrappedAroundFlag) {
			if (nItemsReturned_ == 0) return nullptr;
			nItemsReturned_ = 0;
		}
	}
	return nullptr;
}

}
