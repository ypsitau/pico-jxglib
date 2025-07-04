//==============================================================================
// Shell.cpp
//==============================================================================
#include <stdlib.h>
#include <ctype.h>
#include "jxglib/Shell.h"

namespace jxglib {

//------------------------------------------------------------------------------
// Shell
//------------------------------------------------------------------------------
Shell Shell::Instance;
const char* Shell::StartupScriptName = "/autoexec.sh";
const char* Shell::specialTokens_[] = { ">>", "||", ">", "|", ";", "{", "}", "(", ")", "[", "]" };

Shell::Shell() : stat_{Stat::Startup}, pTerminal_{&TerminalDumb::Instance},
	pCmdRunning_{nullptr}, tokenizer_(specialTokens_, count_of(specialTokens_)), interactiveFlag_{false}
{
	SetPrompt_("%d%w>");
}

bool Shell::RunCmd(Readable& tin, Printable& tout, Printable& terr, char* line, int bytesLine)
{
	char* tokenTbl[128];
	int nToken = count_of(tokenTbl);
	const char* errMsg;
	Readable* ptin = &tin;
	Printable* ptout = &tout;
	Printable* pterr = &terr;
	if (FS::IsLegalDriveName(line)) {
		if (FS::SetDriveCur(line)) return true;
		pterr->Printf("failed to change drive to %s\n", line);
		return false;
	}
	const char* errorMsg;
	if (!tokenizer_.Tokenize(line, bytesLine, tokenTbl, &nToken, &errorMsg)) {
		pterr->Println(errorMsg);
		return false;
	}
	if (nToken == 0) return false;
	int nTokensCmd = nToken;
	bool appendFlag = false;
	const char* fileNameOut = nullptr;
	for (int iToken = 0; iToken < nToken; iToken++) {
		const char* token = tokenTbl[iToken];
		if (!Tokenizer::IsQuoted(line, token) &&
				((appendFlag = (::strcmp(token, ">>") == 0)) || ::strcmp(token, ">") == 0)) {
			nTokensCmd = iToken;
			tokenTbl[iToken] = nullptr;
			if (iToken + 1 >= nToken) {
				pterr->Println("missing file name");
				return false;
			}
			fileNameOut = tokenTbl[iToken + 1];
			break;
		}
	}
	std::unique_ptr<FS::File> pFileOut;
	if (fileNameOut) {
		pFileOut.reset(FS::OpenFile(fileNameOut, appendFlag? "a" : "w"));
		if (!pFileOut) {
			pterr->Printf("failed to open %s\n", fileNameOut);
			return false;
		}
		ptout = pFileOut.get();
	}
	int argc = 0;
	char** argv = tokenTbl;
	for (int iToken = 0; iToken < nTokensCmd; iToken++) {
		const char* token = tokenTbl[iToken];
		if (!Tokenizer::IsQuoted(line, token) && ::strcmp(token, ";") == 0) {
			tokenTbl[iToken] = nullptr;
			if (!RunSingleCmd(*ptin, *ptout, *pterr, argc, argv)) return false;
			argc = 0;
			argv = tokenTbl + iToken + 1;
		} else {
			argc++;
		}
	}
	return RunSingleCmd(*ptin, *ptout, *pterr, argc, argv);	
}

bool Shell::RunSingleCmd(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[])
{
	if (argc == 0) return true; // no command to run
	for (Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetNext()) {
		if (::strcmp(argv[0], pCmd->GetName()) == 0) {
			pCmdRunning_ = pCmd;
			bool enableHistoryFlag = GetTerminal().IsHistoryEnabled();
			GetTerminal().EnableHistory(false); // disable history while running a command
			pCmd->Run(tin, tout, terr, argc, argv);
			GetTerminal().EnableHistory(enableHistoryFlag);
			return true;
		}
	}
	terr.Printf("%s: command not found\n", argv[0]);
	return false;
}

bool Shell::RunScript(Readable& tin, Printable& tout, Printable& terr, Readable& script)
{
	char line[256];
	while (script.ReadLine(line, sizeof(line)) > 0) {
		if (!RunCmd(tin, tout, terr, line, sizeof(line))) return false;
	}
	return true;
}

void Shell::OnTick()
{
	switch (stat_) {
	case Stat::Startup: {
		FS::Drive* pDrive = FS::GetDriveCur();
		if (pDrive && pDrive->IsPrimary()) {
			std::unique_ptr<FS::File> pFileScript(FS::OpenFile(StartupScriptName, "r"));
			if (pFileScript) {
				Terminal::ReadableKeyboard tin(GetTerminal());
				Printable& tout = GetTerminal();
				Printable& terr = GetTerminal();
				RunScript(tin, tout, terr, *pFileScript);
			}
		}
		stat_ = Stat::Begin;
		break;
	}
	case Stat::Begin: {
		char prompt[256];
		GetTerminal().ReadLine_Begin(EvalPrompt(prompt, sizeof(prompt)));
		stat_ = Stat::Prompt;
		break;
	}
	case Stat::Prompt: {
		char* line = GetTerminal().ReadLine_Process();
		if (line) {
			stat_ = Stat::Running;
			Terminal::ReadableKeyboard tin(GetTerminal());
			Printable& tout = GetTerminal();
			Printable& terr = GetTerminal();
			RunCmd(tin, tout, terr, line, Terminal::EditBuffSize);
			stat_ = Stat::Begin;
		}
		break;
	}
	case Stat::Running: {
		if (!interactiveFlag_) {
			KeyData keyData;
			GetTerminal().GetKeyDataNB(&keyData);
			if (keyData.GetChar() == 'C' - '@') Tickable::SetSignal();
		}
		break;
	}
	}
}

const char* Shell::EvalPrompt(char* prompt, int lenMax)
{
	enum class Stat { Normal, Variable, };
	Stat stat = Stat::Normal;
	bool validDTFlag = false;
	DateTime dt;
	int i = 0;
	for (const char* p = GetPrompt(); *p && lenMax > 0; p++, lenMax--) {
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
			switch (ch) {
			case 'd': {
				FS::Drive* pDrive = FS::GetDriveCur();
				if (pDrive) {
					i += ::snprintf(prompt + i, lenMax - i, "%s", pDrive->GetDriveName());
					if (i < lenMax) prompt[i++] = ':';
				}
				break;
			}
			case 'w': {
				FS::Drive* pDrive = FS::GetDriveCur();
				if (!pDrive) {
					// nothing to do
				} else if (!pDrive->Mount()) {
					if (i < lenMax) prompt[i++] = '?'; // indicate unmounted drive
				} else {
					int len = ::snprintf(prompt + i, lenMax - i, "%s", pDrive->GetDirNameCur());
					if (len > 1 && prompt[i + len - 1] == '/') {
						prompt[i + len - 1] = '\0'; // remove trailing slash
						len--;
					}
					i += len;
				}
				break;
			}
			case 'p': {
				i += ::snprintf(prompt + i, lenMax - i, "%s", GetPlatformName());
				break;
			}
			case 'y': case 'Y': case 'M': case 'D':
			case 'h': case 'H': case 'A': case 'm': case 's': {
				if (!validDTFlag) {
					RTC::Get(&dt);
					validDTFlag = true;
				}
				switch (ch) {
				case 'Y': {
					i += ::snprintf(prompt + i, lenMax - i, "%04d", dt.year);
					break;
				}
				case 'y': {
					i += ::snprintf(prompt + i, lenMax - i, "%02d", dt.year % 100);
					break;
				}
				case 'M': {
					i += ::snprintf(prompt + i, lenMax - i, "%02d", dt.month);
					break;
				}
				case 'D': {
					i += ::snprintf(prompt + i, lenMax - i, "%02d", dt.day);
					break;
				}
				case 'h': {
					i += ::snprintf(prompt + i, lenMax - i, "%02d", dt.hour);
					break;
				}
				case 'H': {
					i += ::snprintf(prompt + i, lenMax - i, "%02d", dt.GetHour12());
					break;
				}
				case 'A': {
					i += ::snprintf(prompt + i, lenMax - i, "%s", dt.IsAM() ? "AM" : "PM");
					break;
				}
				case 'm': {
					i += ::snprintf(prompt + i, lenMax - i, "%02d", dt.min);
					break;
				}
				case 's': {
					i += ::snprintf(prompt + i, lenMax - i, "%02d", dt.sec);
					break;
				}
				default:
					break;
				}
				break;
			}
			default:
				break;
			}
			stat = Stat::Normal;
			break;
		}
		default:
			break;
		}
	}
	if (i >= lenMax) ::panic("Shell::EvalPrompt: prompt buffer overflow");
	prompt[i] = '\0';
	return prompt;
}

void Shell::AttachTerminal_(Terminal& terminal)
{
	pTerminal_ = &terminal;
	pTerminal_->SetCompletionProvider(completionProvider_);
	pTerminal_->SetTokenizer(tokenizer_);
}

Tokenizer Shell::CreateTokenizer()
{
	return Tokenizer(specialTokens_, count_of(specialTokens_));
}

void Shell::PrintHelp(Printable& printable, bool simpleFlag)
{
	if (simpleFlag) {
		for (const Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetNext()) {
			printable.Printf("%s\n", pCmd->GetName());
		}
	} else {
		int lenMax = 1;
		for (const Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetNext()) {
			lenMax = ChooseMax(lenMax, static_cast<int>(::strlen(pCmd->GetName())));
		}
		for (const Cmd* pCmd = Cmd::GetCmdHead(); pCmd; pCmd = pCmd->GetNext()) {
			printable.Printf("%-*s  %s\n", lenMax, pCmd->GetName(), pCmd->GetHelp());
		}
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
						pOptFound = &opt;
						break;
					}
				}
				if (!pOptFound) {
					terr.Printf("unknown option: --%s\n", longName);
					return false;
				} else if (pOptFound->DoesRequireValue() && !value) {
					terr.Printf("missing value for option: --%s\n", longName);
					return false;
				}
			} else {
				// short option
				const char* p = argv[iArg] + 1;
				for ( ; *p != '\0'; ++p) {
					char shortName = *p;
					pOptFound = nullptr;
					for (int iOpt = 0; iOpt < nOpts_; iOpt++) {
						const Opt& opt = optTbl_[iOpt];
						if (opt.CheckShortName(shortName)) {
							pOptFound = &opt;
							break;
						}
					}
					if (!pOptFound) {
						terr.Printf("unknown option: -%c\n", shortName);
						return false;
					} else if (pOptFound->DoesRequireValue()) {
						if (*(p + 1) != '\0') {
							terr.Printf("short-name option with a value must be at the last in a series: -%c\n", shortName);
							return false;
						} else if (iArg + 1 >= argc) {
							terr.Printf("missing value for option: -%c\n", shortName);
							return false;
						}
						value = argv[iArg + 1];
						nArgsToRemove = 2;
					} else if (*(p + 1) != '\0') {
						AddOptValue(pOptFound, nullptr);
					}
				}
			}
			if (pOptFound->GetType() == Opt::Type::Int) {
				const char* pEnd = value;
				::strtol(value, const_cast<char**>(&pEnd), 0);
				if (*pEnd != '\0') {
					terr.Printf("invalid value for option: %s\n", argv[iArg]);
					return false;
				}
			} else if (pOptFound->GetType() == Opt::Type::Float) {
				const char* pEnd = value;
				::strtof(value, const_cast<char**>(&pEnd));
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

void Shell::Arg::PrintHelp(Printable& tout, const char* strCaption, const char* lineHead) const
{
	if (strCaption) tout.Printf("%s", strCaption);
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
		tout.Printf("%s%-*s %s\n", lineHead, lenMax, str, opt.GetHelp());
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

bool Shell::Arg::GetFloat(const char* longName, float* pValue) const
{
	const OptValue* pOptValue = FindOptValue(longName);
	if (pOptValue) {
		*pValue = ::strtof(pOptValue->GetValue(), nullptr);
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

bool Shell::Arg::GetAssigned(const char* str, const char* name, const char** pValue)
{
	*pValue = nullptr;
	int len = ::strlen(name);
	if (::strncmp(str, name, len) != 0) return false;
	const char* value = str + len;
	if (*value == '=' || *value == ':') {
		*pValue = value + 1;
		return true;
	}
	return ::isspace(*value) || *value == '\0';
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

bool Shell::Arg::Opt::CheckShortName(char shortName) const
{
	return shortName == shortName_;
}

void Shell::Arg::Opt::MakeHelp(char* str, int len) const
{
	const char* strSepNoShort = " ";
	const char* strSepShort = " ";
	if (type_ == Type::Bool) {
		if (shortName_ == '\0') {
			::snprintf(str, len, "  %s--%s", strSepNoShort, longName_);
		} else {
			::snprintf(str, len, "-%c%s--%s", shortName_, strSepShort, longName_);
		}
	} else {
		if (shortName_ == '\0') {
			::snprintf(str, len, "  %s--%s=%s", strSepNoShort, longName_, helpValue_);
		} else {
			::snprintf(str, len, "-%c%s--%s=%s", shortName_, strSepShort, longName_, helpValue_);
		}
	}
}

//------------------------------------------------------------------------------
// Shell::Arg::Each
//------------------------------------------------------------------------------
Shell::Arg::Each::Each(const char*& argvBegin, const char*& argvEnd) : EachBase(argvBegin, argvEnd) {}

const char* Shell::Arg::Each::Next()
{
	return (argvCur_ == argvEnd_)? nullptr : *argvCur_++;
}

//------------------------------------------------------------------------------
// Shell::Arg::EachNum
//------------------------------------------------------------------------------
Shell::Arg::EachNum::EachNum(const char* str) :
	EachBase(const_cast<const char*&>(argvInternal_[0]), const_cast<const char*&>(argvInternal_[1])),
	mode_{Mode::None}, p_{""}, rangeLimit_{0}, hasRangeLimit_{false}, range_{0, 0, 1}, repeat_{0, 0, 0}, 
	chQuote_{'\0'}, errorMsg_{""}
{
	argvInternal_[0] = str;
	argvInternal_[1] = nullptr;
}

Shell::Arg::EachNum::EachNum(const char*& argvBegin, const char*& argvEnd) :
	EachBase(argvBegin, argvEnd),
	mode_{Mode::None}, p_{""}, rangeLimit_{0}, hasRangeLimit_{false}, range_{0, 0, 1}, repeat_{0, 0, 0},
	chQuote_{'\0'}, errorMsg_{""}
{}

bool Shell::Arg::EachNum::Next(int* pValue)
{
	if (mode_ == Mode::Repeat) {
		*pValue = repeat_.value;
		repeat_.cur++;
		if (repeat_.cur >= repeat_.count) {
			mode_ = Mode::None;
		}
		return true;
	}
	if (mode_ == Mode::Range) {
		*pValue = range_.cur;
		if ((range_.step > 0 && range_.cur > range_.end) || (range_.step < 0 && range_.cur < range_.end)) {
			mode_ = Mode::None;
		} else {
			range_.cur += range_.step;
			return true;
		}
	}
	for (;;) {
		if (mode_ == Mode::File) {
			if (pFile_) {
				unsigned char byte;
				if (pFile_->Read(&byte, 1) == 1) {
					*pValue = static_cast<int>(byte);
					return true;
				} else {
					// End of file reached
					pFile_.reset();
					mode_ = Mode::None;
					for ( ; ::isspace(*p_); ++p_) ;
					if (*p_ == ',') {
						++p_;
					} else if (*p_ == '\0') {
						// Continue to next argument
					} else {
						errorMsg_ = "unexpected character after file";
						return false;
					}
				}
			} else {
				mode_ = Mode::None;
			}
		} else if (mode_ == Mode::String) {
			if (*p_ == chQuote_) {
				++p_;
				mode_ = Mode::None;
				for ( ; ::isspace(*p_); ++p_) ;
				if (*p_ == ',') {
					++p_;
				} else if (*p_ == '\0') {
					// Continue to next argument
				} else {
					::printf("%02x\n", *p_);
					errorMsg_ = "unexpected character after string";
					return false;
				}
			} else if (*p_ == '\0') {
				errorMsg_ = "unexpected end of string";
				return false;
			} else {
				bool validFlag = false;
				char ch = ParseEscape(&p_, &validFlag);
				if (!validFlag) {
					errorMsg_ = "invalid escape sequence in string";
					return false;
				}
				*pValue = static_cast<unsigned char>(ch);
				return true;
			}
		}
		while (*p_ == '\0') {
			if (argvCur_ == argvEnd_) return false; // no more arguments
			p_ = *argvCur_++;
		}
		// Skip whitespace
		for ( ; ::isspace(*p_); ++p_) ;
		if (*p_ == '@') {
			++p_; // skip '@'
			const char* fileNameStart = p_;
			for ( ; *p_ != '\0' && !::isspace(*p_) && *p_ != ','; ++p_) ;
			if (p_ == fileNameStart) {
				errorMsg_ = "missing filename after @";
				return false;
			}
			char fileName[FS::MaxPath];
			int fileNameLen = p_ - fileNameStart;
			if (fileNameLen >= sizeof(fileName)) {
				errorMsg_ = "filename too long";
				return false;
			}
			::strncpy(fileName, fileNameStart, fileNameLen);
			fileName[fileNameLen] = '\0';
			pFile_.reset(FS::OpenFile(fileName, "rb"));
			if (!pFile_) {
				errorMsg_ = "cannot open file";
				return false;
			}
			mode_ = Mode::File;
		} else if (*p_ == '"' || *p_ == '\'') {
			chQuote_ = *p_;
			++p_; // skip opening quote
			mode_ = Mode::String;
		} else {
			// Parse number (existing logic continues...)
			char* endptr = nullptr;
			int n1 = ::strtol(p_, &endptr, 0);
			if (endptr == p_) {
				errorMsg_ = "not a number";
				return false;
			}
			p_ = endptr;
			// Check for range
			if (*p_ == '-') {
				++p_;
				int n2 = ::strtol(p_, &endptr, 0);
				if (endptr != p_) {
					// nothing to do
				} else if (hasRangeLimit_) {
					n2 = rangeLimit_;
				} else {
					errorMsg_ = "invalid range";
					return false;
				}
				p_ = endptr;
				range_.cur = n1;
				range_.end = n2;
				int n3 = 1;
				if (*p_ == ':') {
					++p_;
					n3 = ::strtol(p_, &endptr, 0);
					if (endptr == p_) n3 = 1;
					if (n3 == 0) {
						errorMsg_ = "zero step in range";
						return false;
					}
					if (n3 < 0) n3 = -n3;
					p_ = endptr;
				}
				range_.step = (n1 <= n2) ? n3 : -n3;
				// Output first value of range
				*pValue = range_.cur;
				if (range_.cur != range_.end) {
					mode_ = Mode::Range;
					range_.cur += range_.step;
				}
			} else if (*p_ == '*') {
				// Check for repeat format: value*count
				++p_;
				int count = ::strtol(p_, &endptr, 0);
				if (endptr == p_) {
					errorMsg_ = "missing count after *";
					return false;
				}
				if (count <= 0) {
					errorMsg_ = "repeat count must be positive";
					return false;
				}
				p_ = endptr;
				repeat_.value = n1;
				repeat_.count = count;
				repeat_.cur = 1; // First value is returned immediately
				*pValue = repeat_.value;
				if (repeat_.cur < repeat_.count) {
					mode_ = Mode::Repeat;
				}
			} else {
				*pValue = n1;
			}
			for ( ; ::isspace(*p_); ++p_) ;
			if (*p_ == ',') {
				++p_;
				return true;
			} else if (*p_ == '\0') {
				return true;
			} else {
				errorMsg_ = "unexpected character after number";
				return false;
			}
		}
	}
}

bool Shell::Arg::EachNum::CheckValidity(int* pCount)
{
	argvCur_ = argvBegin_;
	int value;
	int count = 0;
	while (Next(&value)) count++;
	bool rtn = IsSuccess();
	if (pCount) *pCount = count;
	argvCur_ = argvBegin_;
	return rtn;
}

//------------------------------------------------------------------------------
// Shell::Arg::EachGlob
//------------------------------------------------------------------------------
Shell::Arg::EachGlob::EachGlob(const char*& argvBegin, const char*& argvEnd) :  EachBase(argvBegin, argvEnd) {}

const char* Shell::Arg::EachGlob::Next()
{
	const char* pathName = nullptr;
	if (pGlob_) {
		pFileInfo_.reset(pGlob_->Read(&pathName));
		if (pFileInfo_) return pathName;
		pGlob_.reset();
	}
	while (argvCur_ != argvEnd_) {
		const char* arg = *argvCur_++;
		if (FS::DoesContainWildcard(arg)) {
			pGlob_.reset(FS::OpenGlob(arg));
			if (pGlob_) {
				pFileInfo_.reset(pGlob_->Read(&pathName));
				if (pFileInfo_) return pathName;
				pGlob_.reset();
			}
		} else {
			pGlob_.reset();
			pFileInfo_.reset(FS::GetFileInfo(arg));
			return arg;
		}
	}
	return nullptr;
}

//------------------------------------------------------------------------------
// Shell::Arg::EachSubcmd
//------------------------------------------------------------------------------
Shell::Arg::EachSubcmd::EachSubcmd(const char*& argvBegin, const char*& argvEnd) :
			EachBase(argvBegin, argvEnd), pSubcmdCur_{nullptr}, errorMsg_{""} {}

bool Shell::Arg::EachSubcmd::Initialize()
{
	SubcmdGroup* pSubcmdGroupStack[16];
	const char** argv = argvBegin_;
	Subcmd* pSubcmdPrev = nullptr;
	int iCmdGroupStack = 0;
	pSubcmdGroupStack[iCmdGroupStack] = &subcmdGroup_;
	for ( ; argv != argvEnd_; ++argv) {
		SubcmdGroup* pSubcmdGroupCur = pSubcmdGroupStack[iCmdGroupStack];
		const char* proc = *argv;
		const char* value = nullptr;
		if (::strcmp(proc, "{") == 0) {
			SubcmdGroup* pSubcmdGroup = new SubcmdGroup();
			((pSubcmdPrev && pSubcmdPrev->DoesRequireChild())? pSubcmdPrev : pSubcmdGroupCur)->AddChild(pSubcmdGroup);
			if (iCmdGroupStack >= count_of(pSubcmdGroupStack) - 1) {
				errorMsg_ = "too many nested command groups";
				return false;
			}
			iCmdGroupStack++;
			pSubcmdGroupStack[iCmdGroupStack] = pSubcmdGroup;
			pSubcmdPrev = nullptr;
			continue;
		} else if (::strcmp(proc, "}") == 0) {
			pSubcmdPrev = pSubcmdGroupCur;
			iCmdGroupStack--;
			if (iCmdGroupStack < 0) {
				errorMsg_ = "unmatched opening brace";			
				return false;
			}
			continue;
		}
		Subcmd* pSubcmd;
		if (GetAssigned(proc, "repeat", &value)) {
			int nRepeats = -1;
			if (value) {
				char* pEnd = nullptr;
				nRepeats = ::strtol(value, &pEnd, 0);
				if (*pEnd != '\0' || nRepeats < 0) {
					errorMsg_ = "invalid repeat value";
					return false;
				}
			}
			pSubcmd = new SubcmdRepeat(nRepeats);
		} else {
			pSubcmd = new SubcmdProc(proc);
		}
		((pSubcmdPrev && pSubcmdPrev->DoesRequireChild())? pSubcmdPrev : pSubcmdGroupCur)->AddChild(pSubcmd);
		pSubcmdPrev = pSubcmd;
	}
	pSubcmdCur_ = subcmdGroup_.GetChild();
	for ( ; pSubcmdCur_ && pSubcmdCur_->IsEmpty(); pSubcmdCur_ = pSubcmdCur_->GetNext()) ;
	return true;
}

const char* Shell::Arg::EachSubcmd::Next()
{
	while (pSubcmdCur_) {
		const char* proc = pSubcmdCur_->GetProc();
		pSubcmdCur_ = pSubcmdCur_->Advance();
		if (proc) return proc;
	}
	return nullptr;
}

//------------------------------------------------------------------------------
// Shell::Arg::Subcmd
//------------------------------------------------------------------------------
Shell::Arg::Subcmd* Shell::Arg::Subcmd::GetLast()
{
	Subcmd* pSubcmd = this;
	for ( ; pSubcmd->GetNext(); pSubcmd = pSubcmd->GetNext()) ;
	return pSubcmd;
}

//------------------------------------------------------------------------------
// Shell::Arg::SubcmdProc
//------------------------------------------------------------------------------
Shell::Arg::SubcmdProc::SubcmdProc(const char* proc) : proc_{proc}
{}

Shell::Arg::Subcmd* Shell::Arg::SubcmdProc::Advance()
{
	return GetNext()? GetNext() : GetParent()? GetParent()->AdvanceAtEnd() : nullptr;
}

void Shell::Arg::SubcmdProc::Print(int indentLevel) const
{
	::printf("%*s%s\n", indentLevel * 2, "", proc_);
}

//------------------------------------------------------------------------------
// Shell::Arg::SubcmdGroup
//------------------------------------------------------------------------------
Shell::Arg::SubcmdGroup::SubcmdGroup()
{}

void Shell::Arg::SubcmdGroup::AddChild(Subcmd* pSubcmd)
{
	pSubcmd->SetParent(this);
	if (GetChild()) {
		GetChild()->GetLast()->SetNext(pSubcmd);
	} else {
		pSubcmdChild_.reset(pSubcmd);
	}
}

bool Shell::Arg::SubcmdGroup::IsEmpty() const
{
	for (Subcmd* pSubcmd = GetChild(); pSubcmd; pSubcmd = pSubcmd->GetNext()) {
		if (!pSubcmd->IsEmpty()) return false;
	}
	return true;
}

Shell::Arg::Subcmd* Shell::Arg::SubcmdGroup::AdvanceAtEnd()
{
	return GetNext()? GetNext() : GetParent()? GetParent()->AdvanceAtEnd() : nullptr;;
}

Shell::Arg::Subcmd* Shell::Arg::SubcmdGroup::Advance()
{
	//return GetChild()? GetChild() : AdvanceAtEnd();
	return IsEmpty()? AdvanceAtEnd() : GetChild();
}

void Shell::Arg::SubcmdGroup::Print(int indentLevel) const
{
	::printf("%*s{\n", indentLevel * 2, "");
	for (Subcmd* pSubcmd = GetChild(); pSubcmd; pSubcmd = pSubcmd->GetNext()) pSubcmd->Print(indentLevel + 1);
	::printf("%*s}\n", indentLevel * 2, "");
}

//------------------------------------------------------------------------------
// Shell::Arg::SubcmdRepeat
//------------------------------------------------------------------------------
Shell::Arg::SubcmdRepeat::SubcmdRepeat(int nRepeats) : nRepeats_{nRepeats}, nCur_{0}
{}

Shell::Arg::Subcmd* Shell::Arg::SubcmdRepeat::AdvanceAtEnd()
{
	nCur_++;
	if (!GetChild()) {
		// nothing to do
	} else if (nRepeats_ < 0 || nCur_ < nRepeats_) {
		return GetChild();
	}
	return SubcmdGroup::AdvanceAtEnd();
}

Shell::Arg::Subcmd* Shell::Arg::SubcmdRepeat::Advance()
{
	nCur_ = 0;
	return (IsEmpty() || nRepeats_ == 0)? SubcmdGroup::AdvanceAtEnd() : GetChild();
}

void Shell::Arg::SubcmdRepeat::Print(int indentLevel) const
{
	if (nRepeats_ < 0) {
		::printf("%*srepeat {\n", indentLevel * 2, "");
	} else {
		::printf("%*srepeat:%d {\n", indentLevel * 2, "", nRepeats_);
	}
	for (Subcmd* pSubcmd = GetChild(); pSubcmd; pSubcmd = pSubcmd->GetNext()) pSubcmd->Print(indentLevel + 1);
	::printf("%*s}\n", indentLevel * 2, "");
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
		std::unique_ptr<FS::Dir> pDir(FS::OpenDir(GetHint()));
		if (pDir) {	// if the hint is a directory name
			::snprintf(dirName_, sizeof(dirName_), "%s", GetHint());
		} else {
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
			if (GetIByte() == 0) {
				// the first field is a command name
				::snprintf(result_, sizeof(result_), "%s", itemName);
			} else {
				// following fields are file names
				//FS::JoinPathName(result_, sizeof(result_), dirName_, itemName);
				::snprintf(result_, sizeof(result_), "%s", dirName_);
				FS::AppendPathName(result_, sizeof(result_), itemName);
			}
			return result_;
		} else if (wrappedAroundFlag) {
			if (nItemsReturned_ == 0) return nullptr;
			nItemsReturned_ = 0;
		}
	}
	return nullptr;
}

}
