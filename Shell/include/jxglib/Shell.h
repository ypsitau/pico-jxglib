//==============================================================================
// jxglib/Shell.h
//==============================================================================
#ifndef PICO_JXGLIB_SHELL_H
#define PICO_JXGLIB_SHELL_H
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"

#define ShellCmd_Named(symbol, name, help) \
class ShellCmd_##symbol : public Shell::Cmd { \
public: \
	static ShellCmd_##symbol Instance; \
public: \
	ShellCmd_##symbol() : Shell::Cmd(name, help) {} \
public: \
	virtual int Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[]) override; \
}; \
ShellCmd_##symbol ShellCmd_##symbol::Instance; \
int ShellCmd_##symbol::Run(Readable& tin, Printable& tout, Printable& terr, int argc, char* argv[])

#define ShellCmd(name, help) ShellCmd_Named(name, #name, help)

namespace jxglib {

//------------------------------------------------------------------------------
// Shell
//------------------------------------------------------------------------------
class Shell : public Tickable {
public:
	class Cmd {
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
	enum class Stat { Begin, Prompt, Running, };
private:
	Stat stat_;
	char prompt_[64];
	Terminal* pTerminal_;
	Cmd* pCmdRunning_;
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
	void AttachTerminal_(Terminal& terminal) { pTerminal_ = &terminal; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "Shell"; }
	virtual void OnTick() override;
public:
	static void PrintHelp(Printable& printable);
};

}

#endif
