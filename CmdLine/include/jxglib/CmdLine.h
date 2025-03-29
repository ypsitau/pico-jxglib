//==============================================================================
// jxglib/CmdLine.h
//==============================================================================
#ifndef PICO_JXGLIB_CMDLINE_H
#define PICO_JXGLIB_CMDLINE_H
#include "pico/stdlib.h"
#include "jxglib/Terminal.h"

#define CmdLineEntry(name) \
class CmdLineEntry_##name : public CmdLine::Entry { \
public: \
	static CmdLineEntry_##name Instance; \
public: \
	CmdLineEntry_##name() : CmdLine::Entry(#name) {} \
public: \
	virtual void Run(Terminal& terminal, int argc, char* argv[]) override; \
}; \
CmdLineEntry_##name CmdLineEntry_##name::Instance; \
void CmdLineEntry_##name::Run(Terminal& terminal, int argc, char* argv[])

namespace jxglib {

//------------------------------------------------------------------------------
// CmdLine
//------------------------------------------------------------------------------
class CmdLine : public Tickable {
public:
	class Entry {
	private:
		const char* name_;
		Entry* pEntryNext_;
	private:
		static Entry* pEntryHead_;
	public:
		Entry(const char* name);
	public:
		const char* GetName() const { return name_; }
		void SetEntryNext(Entry* pEntry) { pEntryNext_ = pEntry; }
		Entry* GetEntryNext() { return pEntryNext_; }
	public:
		static Entry* GetEntryHead() { return pEntryHead_; }
	public:
		virtual void Run(Terminal& terminal, int argc, char* argv[]) = 0;
	};
	enum class Stat { Begin, Prompt, Running, };
private:
	Stat stat_;
	char prompt_[64];
	Terminal* pTerminal_;
	Entry* pEntryRunning_;
public:
	static CmdLine Instance;
public:
	CmdLine();
public:
	bool RunEntry(char* line);
	Terminal& GetTerminal() { return *pTerminal_; }
public:
	static void AttachTerminal(Terminal& terminal) { Instance.AttachTerminal_(terminal); }
private:
	void AttachTerminal_(Terminal& terminal) { pTerminal_ = &terminal; }
public:
	// virtual functions of Tickable
	virtual const char* GetTickableName() const override { return "CmdLine"; }
	virtual void OnTick() override;
public:
	static void PrintList(Printable& printable);
};

}

#endif
