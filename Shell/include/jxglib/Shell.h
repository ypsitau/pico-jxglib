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
ShellCmd_##symbolOrg ShellCmd_##symbolAlias##_Instance(strSymbolAlias);

#define ShellCmd(symbol, help) ShellCmd_Named(symbol, #symbol, help)
#define ShellCmdAlias(symbolAlias, symbolOrg) ShellCmdAlias_Named(symbolAlias, #symbolAlias, symbolOrg)

namespace jxglib {

//------------------------------------------------------------------------------
// Shell
//------------------------------------------------------------------------------
class Shell : public Tickable {
public:
	class Arg {
	public:
		class Opt {
		public:
			enum class Type { Bool, String, Int };
		private:
			Type type_;
			const char* longName_;
			const char* shortName_;
			const char* strHelp_;
			const char* helpValue_;
		public:
			constexpr Opt(Type type, const char* longName, const char* shortName, const char* strHelp, const char* helpValue) :
				type_{type}, longName_{longName}, shortName_{shortName}, strHelp_{strHelp}, helpValue_{helpValue} {}
		public:
			void MakeHelp(char* str, int len) const;
			constexpr Type GetType() const { return type_; }
			constexpr const char* GetLongName() const { return longName_; }
			constexpr const char* GetShortName() const { return shortName_; }
			constexpr const char* GetHelp() const { return strHelp_; }
			constexpr const char* GetHelpValue() const { return helpValue_; }
			constexpr bool DoesRequireValue() const { return type_ != Type::Bool; }
		public:
			bool CheckLongName(const char* longName, const char** pValue) const;
			bool CheckLongName(const char* longName) const { return CheckLongName(longName, nullptr); }
			bool CheckShortName(const char* shortName) const;
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
		void PrintHelp(Printable& tout) const;
	public:
		bool GetBool(const char* longName) const;
		bool GetString(const char* longName, const char** pValue) const;
		bool GetInt(const char* longName, int* pValue) const;
	private:
		void AddOptValue(const Opt* pOpt, const char* value);
		const OptValue* FindOptValue(const char* longName) const;
	public:
		constexpr static Opt OptBool(const char* longName, const char* shortName, const char* strHelp = "", const char* helpValue = "") {
			return Opt(Opt::Type::Bool, longName, shortName, strHelp, helpValue);
		}
		constexpr static Opt OptString(const char* longName, const char* shortName, const char* strHelp = "", const char* helpValue = "") {
			return Opt(Opt::Type::String, longName, shortName, strHelp, helpValue);
		}
		constexpr static Opt OptInt(const char* longName, const char* shortName, const char* strHelp = "", const char* helpValue = "") {
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
		void SetCmdNext(Cmd* pCmd) { pCmdNext_ = pCmd; }
		Cmd* GetCmdNext() { return pCmdNext_; }
		const Cmd* GetCmdNext() const { return pCmdNext_; }
	public:
		static Cmd* GetCmdHead() { return pCmdHead_; }
	public:
		virtual int Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]) = 0;
	};
	class ItemProvider {
	public:
		virtual const char* NextItemName() = 0;
	};
	class ItemProvider_Cmd : public ItemProvider {
	private:
		const Cmd* pCmd_;
	public:
		ItemProvider_Cmd() : pCmd_{Cmd::GetCmdHead()} {}
	public:
		virtual const char* NextItemName() override;
	};
	class ItemProvider_Dir : public ItemProvider {
	private:
		RefPtr<FS::Dir> pDir_;
	public:
		ItemProvider_Dir(FS::Dir* pDir) : pDir_{pDir} {}	
	public:
		virtual const char* NextItemName() override;
	};
	class ComplementProvider : public Terminal::ComplementProvider {
	private:
		//RefPtr<FS::Dir> pDir_;
		std::unique_ptr<ItemProvider> pItemProvider_;
		char itemNameFirst_[FS::MaxPath];
		int nItemsReturned_;
	public:
		ComplementProvider() : nItemsReturned_{0} {}
	public:
		// virtual functions of Terminal::ComplementProvider
		virtual void StartComplement() override;
		virtual void EndComplement() override;
		virtual const char* NextComplement() override;
	};
	enum class Stat { Begin, Prompt, Running, };
private:
	Stat stat_;
	char prompt_[64];
	Terminal* pTerminal_;
	Cmd* pCmdRunning_;
	ComplementProvider complementProvider_;
public:
	static Shell Instance;
public:
	Shell();
public:
	bool RunCmd(char* line);
	Terminal& GetTerminal() { return *pTerminal_; }
public:
	static const char* GetPrompt() { return Instance.GetPrompt_(); }
	static void SetPrompt(const char* prompt) { Instance.SetPrompt_(prompt); }
	static void AttachTerminal(Terminal& terminal) { Instance.AttachTerminal_(terminal); }
private:
	const char* GetPrompt_() const { return prompt_; }
	void SetPrompt_(const char* prompt);
	const char* MakePrompt(char* prompt, int lenMax);
	void AttachTerminal_(Terminal& terminal);
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "Shell"; }
	virtual void OnTick() override;
public:
	static void PrintHelp(Printable& printable);
};

}

#endif
