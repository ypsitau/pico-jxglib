//==============================================================================
// jxglib/Shell.h
//==============================================================================
#ifndef PICO_JXGLIB_SHELL_H
#define PICO_JXGLIB_SHELL_H
#include <stdlib.h>
#include <memory>
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"
#include "jxglib/FS.h"

#define ShellCmd_Named(symbol, strSymbol, help) \
class ShellCmd_##symbol : public Shell::Cmd { \
public: \
	ShellCmd_##symbol(const char* name_ = strSymbol) : Shell::Cmd(name_, help) {} \
public: \
	virtual int Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]) override; \
}; \
ShellCmd_##symbol ShellCmd_##symbol##_Instance; \
int ShellCmd_##symbol::Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[])

#define ShellCmdAlias_Named(symbolAlias, strSymbolAlias, symbolOrg) \
using ShellCmd_##symbolAlias = ShellCmd_##symbolOrg; \
ShellCmd_##symbolAlias ShellCmd_##symbolAlias##_Instance(strSymbolAlias);

#define ShellCmd(symbol, help) ShellCmd_Named(symbol, #symbol, help)
#define ShellCmdAlias(symbolAlias, symbolOrg) ShellCmdAlias_Named(symbolAlias, #symbolAlias, symbolOrg)

namespace jxglib {

//------------------------------------------------------------------------------
// Shell
//------------------------------------------------------------------------------
class Shell : public Tickable {
public:
	static const char* StartupScriptName;
public:
	class BreakDetector : public Tickable {
	private:
		Shell& shell_;
	public:
		BreakDetector(Shell& shell) : Tickable(100, Priority::Lowest), shell_{shell} {}
		virtual const char* GetTickableName() const override { return "Shell::BreakDetector"; }
		virtual void OnTick() override;
	};
	class Arg {
	public:
		class Opt {
		public:
			enum class Type { Bool, String, Int, Float };
		private:
			Type type_;
			const char* longName_;
			char shortName_;
			const char* strHelp_;
			const char* helpValue_;
		public:
			constexpr Opt(Type type, const char* longName, char shortName, const char* strHelp, const char* helpValue) :
				type_{type}, longName_{longName}, shortName_{shortName}, strHelp_{strHelp}, helpValue_{helpValue} {}
		public:
			void MakeHelp(char* str, int len) const;
			constexpr Type GetType() const { return type_; }
			constexpr const char* GetLongName() const { return longName_; }
			constexpr char GetShortName() const { return shortName_; }
			constexpr const char* GetHelp() const { return strHelp_; }
			constexpr const char* GetHelpValue() const { return helpValue_; }
			constexpr bool DoesRequireValue() const { return type_ != Type::Bool; }
		public:
			bool CheckLongName(const char* longName, const char** pValue) const;
			bool CheckLongName(const char* longName) const { return CheckLongName(longName, nullptr); }
			bool CheckShortName(char shortName) const;
		};
		class OptValue {
		private:
			const Opt* pOpt_;
			const char* value_;
			std::unique_ptr<OptValue> pOptValueNext_;
		public:
			OptValue(const Opt* pOpt, const char* value) : pOpt_{pOpt}, value_{value} {}
		public:
			const Opt& GetOpt() const { return *pOpt_; }
			const char* GetValue() const { return value_; }
			void SetNext(OptValue* pOptValueNext) { pOptValueNext_.reset(pOptValueNext); }
			OptValue* GetNext() const { return pOptValueNext_.get(); }
		};	
		class EachBase {
		protected:
			const char** argvCur_;
			const char** argvBegin_;
			const char** argvEnd_;
		public:
			EachBase(const char*& argvBegin, const char*& argvEnd) :
				argvCur_{&argvBegin}, argvBegin_{&argvBegin}, argvEnd_{&argvEnd} {}
		};
		class Each : public EachBase {
		public:
			Each(const char*& argvBegin, const char*& argvEnd);
			Each(char*& argvBegin, char*& argvEnd) : Each(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd)) {}
			const char* Next();
			void Rewind();
		};
		class EachNum : public EachBase {
		private:
			enum class Mode { None, String, Range, File, Repeat };
		private:
			Mode mode_;
			const char* argvInternal_[2];
			const char* p_;
			int rangeLimit_;
			bool hasRangeLimit_;
			struct {
				int cur, end, step;
			} range_;
			struct {
				int value, count, cur;
			} repeat_;
			struct {
				int value;
				bool enableFlag_;
			} blank_;
			char chQuote_;
			const char* errorMsg_;
			std::unique_ptr<FS::File> pFile_;
		public:
			EachNum(const char* str);
			EachNum(const char* str, int rangeLimit) : EachNum(str) { rangeLimit_ = rangeLimit; hasRangeLimit_ = true; }
			EachNum(const char*& argvBegin, const char*& argvEnd);
			EachNum(const char*& argvBegin, const char*& argvEnd, int rangeLimit) : EachNum(argvBegin, argvEnd) {
				rangeLimit_ = rangeLimit; hasRangeLimit_ = true;
			}
			EachNum(char*& argvBegin, char*& argvEnd) : EachNum(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd)) {}
			EachNum(char*& argvBegin, char*& argvEnd, int rangeLimit) : EachNum(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd), rangeLimit) {}
		public:
			void SetLimit(int rangeLimit) { rangeLimit_ = rangeLimit; hasRangeLimit_ = true; }
			bool Next(int* pValue);
			bool IsSuccess() const { return errorMsg_[0] == '\0'; }
			const char* GetErrorMsg() const { return errorMsg_; }
			bool CheckValidity(int* pCount = nullptr);
			void SetBlankValue(int value, bool enableFlag = true) { blank_.value = value; blank_.enableFlag_ = enableFlag; }
			void Rewind();
			int GetAll(int valueTbl[], int cntMax);
		};
		class EachGlob : public EachBase {
		private:
			std::unique_ptr<FS::Glob> pGlob_;
			std::unique_ptr<FS::FileInfo> pFileInfo_;
		public:
			EachGlob(const char*& argvBegin, const char*& argvEnd);
			EachGlob(char*& argvBegin, char*& argvEnd) : EachGlob(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd)) {}
			const char* Next();
			const FS::FileInfo& GetFileInfo() const { return *pFileInfo_; }
			void Rewind();
		};
		class Subcmd {
		protected:
			Subcmd* pSubcmdGroupParent_;
			Subcmd* pSubcmdNext_;
			std::unique_ptr<Subcmd> pSubcmdChild_;
		public:
			Subcmd();
		public:
			void SetParent(Subcmd* pSubcmdGroupParent) { pSubcmdGroupParent_ = pSubcmdGroupParent; }
			Subcmd* GetParent() const { return pSubcmdGroupParent_; }
			Subcmd* GetChild() const { return pSubcmdChild_.get(); }
			void SetNext(Subcmd* pNext) { pSubcmdNext_ = pNext; }
			Subcmd* GetNext() const { return pSubcmdNext_; }
			Subcmd* GetLast();
		public:
			virtual void AddChild(Subcmd* pSubcmdChild);
			virtual bool IsEmpty() const;
			virtual const char* GetProc() const { return nullptr; }
			virtual Subcmd* Advance();
			virtual void Print(int indentLevel = 0) const;
		public:
			virtual Subcmd* AdvanceAtEnd();
		};
		class SubcmdCustom : public Subcmd {
		protected:
			const char* proc_;
		public:
			SubcmdCustom(const char* proc);
		public:
			virtual bool IsEmpty() const override { return false; }
			virtual const char* GetProc() const override { return proc_; }
			virtual Subcmd* Advance() override;
			virtual void Print(int indentLevel = 0) const override;
		};
		class SubcmdRepeat : public Subcmd {
		private:
			int nRepeats_;
			int nCur_;
		public:
			SubcmdRepeat(int nRepeats);
		public:
			virtual Subcmd* AdvanceAtEnd();
		public:
			virtual Subcmd* Advance() override;
			virtual void Print(int indentLevel = 0) const override;
		};
		class EachSubcmd : public EachBase {
		private:
		private:
			Subcmd subcmdGroup_;
			Subcmd* pSubcmdCur_;
			const char* errorMsg_;
		public:
			EachSubcmd(const char*& argvBegin, const char*& argvEnd);
			EachSubcmd(char*& argvBegin, char*& argvEnd) : EachSubcmd(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd)) {}
		public:
			bool Initialize();
			const char* GetErrorMsg() const { return errorMsg_; }
			const char* Next();
			const Subcmd* NextSubcmd();
			void Rewind();
		public:
			void Print(int indentLevel = 0) const { subcmdGroup_.Print(indentLevel); }
		};
	private:
		const Opt* optTbl_;
		int nOpts_;
		std::unique_ptr<OptValue> pOptValueHead_;
	public:
		static const Arg None;
	public:
		Arg(const Opt* optTbl, int nOpts) : optTbl_{optTbl}, nOpts_{nOpts} {}
	public:
		bool Parse(Printable& terr, int& argc, const char* argv[]);
		bool Parse(Printable& terr, int& argc, char* argv[]) { return Parse(terr, argc, const_cast<const char**>(argv)); }
		void PrintHelp(Printable& tout, const char* strCaption = "Options:\n", const char* lineHead = " ") const;
	public:
		bool GetBool(const char* longName) const;
		bool GetString(const char* longName, const char** pValue) const;
		bool GetInt(const char* longName, int* pValue) const;
		bool GetFloat(const char* longName, float* pValue) const;
	private:
		void AddOptValue(const Opt* pOpt, const char* value);
		const OptValue* FindOptValue(const char* longName) const;
	public:
		constexpr static Opt OptBool(const char* longName, char shortName, const char* strHelp = "", const char* helpValue = "") {
			return Opt(Opt::Type::Bool, longName, shortName, strHelp, helpValue);
		}
		constexpr static Opt OptString(const char* longName, char shortName, const char* strHelp = "", const char* helpValue = "") {
			return Opt(Opt::Type::String, longName, shortName, strHelp, helpValue);
		}
		constexpr static Opt OptInt(const char* longName, char shortName, const char* strHelp = "", const char* helpValue = "") {
			return Opt(Opt::Type::Int, longName, shortName, strHelp, helpValue);
		}
		constexpr static Opt OptFloat(const char* longName, char shortName, const char* strHelp = "", const char* helpValue = "") {
			return Opt(Opt::Type::Float, longName, shortName, strHelp, helpValue);
		}
	public:
		static bool GetAssigned(const char* str, const char* name, const char** pValue);
		static Dict::Entry* CreateDictEntry(const char* str);
	};
	struct Result {
		static const int Success	= 0;
		static const int Error		= -1;
		static const int SignalExit	= -2;
	};
	class Cmd {
	public:
		using Arg = Shell::Arg;
		using Result = Shell::Result;
	private:
		const char* name_;
		const char* help_;
		Cmd* pCmdNext_;
	private:
		static Cmd* pCmdHead_;
	public:
		Cmd(const char* name, const char* help);
	public:
		const char* GetName() const { return name_; }
		const char* GetHelp() const { return help_; }
		void SetNext(Cmd* pCmd) { pCmdNext_ = pCmd; }
		Cmd* GetNext() { return pCmdNext_; }
		const Cmd* GetNext() const { return pCmdNext_; }
	public:
		static Cmd* GetCmdHead() { return pCmdHead_; }
	public:
		virtual int Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]) = 0;
	};
	class Cmd_CAdaptor : Cmd {
	public:
		using Func = int (*)(int argc, char* argv[]);
	private:
		Func func_;
	public:
		Cmd_CAdaptor(const char* name, const char* help, Func func) : Cmd(name, help), func_{func} {}
	public:
		virtual int Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]) override { return func_(argc, argv); }
	};
	class CandidateProvider {
	public:
		virtual const char* NextItemName(bool* pWrappedAroundFlag) = 0;
	};
	class CandidateProvider_Cmd : public CandidateProvider {
	private:
		const Cmd* pCmdCur_;
	public:
		CandidateProvider_Cmd() : pCmdCur_{Cmd::GetCmdHead()} {}
	public:
		virtual const char* NextItemName(bool* pWrappedAroundFlag) override;
	};
	class CandidateProvider_FileInfo : public CandidateProvider {
	private:
		std::unique_ptr<FS::FileInfo> pFileInfoHead_;
		FS::FileInfo* pFileInfoCur_;
	public:
		CandidateProvider_FileInfo(FS::FileInfo* pFileInfoHead) :
				pFileInfoHead_{pFileInfoHead}, pFileInfoCur_{pFileInfoHead_.get()} {}
	public:
		virtual const char* NextItemName(bool* pWrappedAroundFlag) override;
	};
	class CompletionProvider : public Terminal::CompletionProvider {
	private:
		std::unique_ptr<CandidateProvider> pCandidateProvider_;
		char dirName_[FS::MaxPath];
		char result_[FS::MaxPath];
		int nItemsReturned_;
		const char* prefix_;
	public:
		CompletionProvider() : nItemsReturned_{0}, prefix_{""} {}
	public:
		// virtual functions of Terminal::CompletionProvider
		virtual void StartCompletion() override;
		virtual void EndCompletion() override;
		virtual const char* NextCompletion() override;
	};
	enum class Stat { Startup, Begin, Prompt, Running, };
private:
	Stat stat_;
	std::unique_ptr<char[]> banner_;
	std::unique_ptr<char[]> prompt_;
	Terminal* pTerminal_;
	Cmd* pCmdRunning_;
	CompletionProvider completionProvider_;
	Tokenizer tokenizer_;
	bool interactiveFlag_;
	BreakDetector breakDetector_;
public:
	static Shell Instance;
private:
	static const char* specialTokens_[];
public:
	Shell();
public:
	bool RunCmd(Readable& tin, Printable& tout, Printable& terr, char* line, int bytesLine);
	bool RunSingleCmd(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]);
	bool RunScript(Readable& tin, Printable& tout, Printable& terr, Readable& script);
	Terminal& GetTerminal() { return *pTerminal_; }
public:
	static void SetBanner(const char* banner) { Instance.SetBanner_(banner); }
	static const char* GetBanner() { return Instance.GetBanner_(); }
	static void SetPrompt(const char* prompt) { Instance.SetPrompt_(prompt); }
	static const char* GetPrompt() { return Instance.GetPrompt_(); }
	static void AttachTerminal(Terminal& terminal) { Instance.AttachTerminal_(terminal); }
	static void BeginInteractive() { Instance.BeginInteractive_(); }
	static void EndInteractive() { Instance.EndInteractive_(); }
private:
	void SetBanner_(const char* banner) { banner_.reset(new char[::strlen(banner) + 1]); ::strcpy(banner_.get(), banner); }
	const char* GetBanner_() const { return banner_? banner_.get() : ""; }
	void SetPrompt_(const char* prompt) { prompt_.reset(new char[::strlen(prompt) + 1]); ::strcpy(prompt_.get(), prompt); }
	const char* GetPrompt_() const { return prompt_? prompt_.get() : ""; }
	const char* EvalPrompt(char* prompt, int lenMax);
	void AttachTerminal_(Terminal& terminal);
	void BeginInteractive_() { interactiveFlag_ = true; }
	void EndInteractive_() { interactiveFlag_ = false; }
	bool IsInteractive() const { return interactiveFlag_; }
	bool IsRunning() const { return stat_ == Stat::Running; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "Shell"; }
	virtual void OnTick() override;
public:
	static Tokenizer CreateTokenizer();
	static void PrintHistory(Printable& printable);
	static void PrintHelp(Printable& printable, bool simpleFlag = false);
};

}

#endif
