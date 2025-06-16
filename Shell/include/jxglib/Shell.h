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
	class Arg {
	public:
		class Opt {
		public:
			enum class Type { Bool, String, Int };
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
		class Iterator {
		protected:
			const char** argv_;
			const char** argvBegin_;
			const char** argvEnd_;
		public:
			Iterator(const char*& argvBegin, const char*& argvEnd) :
				argv_{&argvBegin}, argvBegin_{&argvBegin}, argvEnd_{&argvEnd} {}
		};
		class Each : public Iterator {
		public:
			Each(const char*& argvBegin, const char*& argvEnd);
			Each(char*& argvBegin, char*& argvEnd) : Each(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd)) {}
			const char* Next();
		};
		class EachNum : public Iterator {
		private:
			const char* p_;
			bool rangeActiveFlag_;
			int rangeCur_, rangeEnd_, rangeStep_;
		public:
			EachNum(const char*& argvBegin, const char*& argvEnd);
			EachNum(char*& argvBegin, char*& argvEnd) : EachNum(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd)) {}
			bool Next(int* pValue);
			bool IsValid();
		};
		class Glob : public Iterator {
		private:
			std::unique_ptr<FS::Glob> pGlob_;
			std::unique_ptr<FS::FileInfo> pFileInfo_;
		public:
			Glob(const char*& argvBegin, const char*& argvEnd);
			Glob(char*& argvBegin, char*& argvEnd) : Glob(const_cast<const char*&>(argvBegin), const_cast<const char*&>(argvEnd)) {}
			const char* Next();
			const FS::FileInfo& GetFileInfo() const { return *pFileInfo_; }
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
	};
	class Cmd {
	public:
		using Arg = Shell::Arg;
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
	char prompt_[64];
	Terminal* pTerminal_;
	Cmd* pCmdRunning_;
	CompletionProvider completionProvider_;
	Tokenizer tokenizer_;
	bool interactiveFlag_;
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
	static const char* GetPrompt() { return Instance.GetPrompt_(); }
	static void SetPrompt(const char* prompt) { Instance.SetPrompt_(prompt); }
	static void AttachTerminal(Terminal& terminal) { Instance.AttachTerminal_(terminal); }
	static void BeginInteractive() { Instance.BeginInteractive_(); }
	static void EndInteractive() { Instance.EndInteractive_(); }
private:
	const char* GetPrompt_() const { return prompt_; }
	void SetPrompt_(const char* prompt);
	const char* MakePrompt(char* prompt, int lenMax);
	void AttachTerminal_(Terminal& terminal);
	void BeginInteractive_() { interactiveFlag_ = true; }
	void EndInteractive_() { interactiveFlag_ = false; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "Shell"; }
	virtual void OnTick() override;
public:
	static void PrintHelp(Printable& printable, bool simpleFlag = false);
};

}

#endif
