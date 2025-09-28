//==============================================================================
// LABOPlatform.cpp
//==============================================================================
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include "jxglib/LABOPlatform.h"
#if defined(CYW43_WL_GPIO_LED_PIN)
#include "jxglib/Net/Telnet.h"
#endif

using namespace jxglib;

//------------------------------------------------------------------------------
// C functions
//------------------------------------------------------------------------------
void jxglib_labo_init(bool attachStdioFlag)
{
	LABOPlatform::Instance.AttachStdio(attachStdioFlag).Initialize();
}

void jxglib_tick()
{
	Tickable::Tick();
}

bool jxglib_ticksub()
{
	return Tickable::TickSub();
}

void jxglib_sleep(int msec)
{
	Tickable::Sleep(msec);
}

void jxglib_shellcmd(const char* name, const char* help, int (*func)(int argc, char* argv[]))
{
	new Shell::Cmd_CAdaptor(name, help, func);
}

void jxglib_enable_startup_script(bool enableFlag)
{
	Shell::EnableStartupScript(enableFlag);
}

//------------------------------------------------------------------------------
// File handlers
//------------------------------------------------------------------------------
#if 1
extern "C" {
FS::File* pFileTbl[8] = { nullptr };

int _open(const char* fn, int oflag, ...)
{
	//::printf("_open(%s, %08x)\n", fn, oflag);
	int idx;
	for (idx = 0; idx < count_of(pFileTbl) && pFileTbl[idx]; ++idx) ;
	if (idx >= count_of(pFileTbl)) return -1;
	const char* mode = (oflag & O_APPEND)? "a" : (oflag & O_WRONLY)? "w" : "r";
	FS::File* pFile = FS::OpenFile(fn, mode);
	if (!pFile) return -1;
	pFileTbl[idx] = pFile;
	return idx + 3; // file descriptor starts at 3
}

int _close(int fd)
{
	//::printf("_close(%d)\n", fd);
	int idx = fd - 3;
	if (idx < 0 || idx >= count_of(pFileTbl) || !pFileTbl[idx]) return -1;
	FS::File* pFile = pFileTbl[idx];
	pFileTbl[idx] = nullptr;
	pFile->Close();
	delete pFile;
	return 0;
}

int _read(int fd, char* buffer, int length)
{
	//::printf("_read(%d, %p, %d)\n", fd, buffer, length);
	int idx = fd - 3;
	if (idx < 0 || idx >= count_of(pFileTbl) || !pFileTbl[idx]) return -1;
	FS::File* pFile = pFileTbl[idx];
	return pFile->Read(buffer, length);
}

int _write(int fd, char* buffer, int length)
{
	//::printf("_write(%d, %p, %d)\n", fd, buffer, length);
	int idx = fd - 3;
	if (idx < 0 || idx >= count_of(pFileTbl) || !pFileTbl[idx]) return -1;
	FS::File* pFile = pFileTbl[idx];
	return pFile->Write(buffer, length);
}

off_t _lseek(int fd, off_t pos, int whence)
{
	//::printf("_lseek(%d, %ld, %d)\n", fd, pos, whence);
	int idx = fd - 3;
	if (idx < 0 || idx >= count_of(pFileTbl) || !pFileTbl[idx]) return -1;
	FS::File* pFile = pFileTbl[idx];
	FS::File::SeekStart seekStart;
	switch (whence) {
	case SEEK_SET: seekStart = FS::File::SeekStart::Begin; break;
	case SEEK_CUR: seekStart = FS::File::SeekStart::Current; break;
	case SEEK_END: seekStart = FS::File::SeekStart::End; break;
	default: return -1; // Invalid whence
	}
	return pFile->Seek(pos, seekStart);
}

int _fstat(int fd, struct stat* buf)
{
	//::printf("_fstat(%d, %p)\n", fd, buf);
	int idx = fd - 3;
	if (idx < 0 || idx >= count_of(pFileTbl) || !pFileTbl[idx]) return -1;
	return -1;
}

}
#endif

//------------------------------------------------------------------------------
// Callback function for the LogicAnalyzer
//------------------------------------------------------------------------------
LogicAnalyzer& ShellCmd_LogicAnalyzer_GetLogicAnalyzer()
{
	return LABOPlatform::Instance.GetLogicAnalyzer();
}

//------------------------------------------------------------------------------
// LABOPlatform
//------------------------------------------------------------------------------
const char* LABOPlatform::textREADME_ = R"(Welcome to pico-jxgLABO!
Type 'help' in the shell to see available commands.
)";

LABOPlatform LABOPlatform::Instance;

LABOPlatform::LABOPlatform(int bytesFlash) :
	deviceController_({
		bcdUSB:				0x0200,
		bDeviceClass:		TUSB_CLASS_MISC,
		bDeviceSubClass:	MISC_SUBCLASS_COMMON,
		bDeviceProtocol:	MISC_PROTOCOL_IAD,
		bMaxPacketSize0:	CFG_TUD_ENDPOINT0_SIZE,
		idVendor:			0xcafe,
		idProduct:			0x1ab0,
		bcdDevice:			0x0100,
	}, 0x0409, "jxglib", "pico-jxgLABO", "000000000000"),
	fat_("*L:", bytesFlash),
	mscDrive_(deviceController_, 0x01, 0x81),
	streamCDC_Terminal_(deviceController_, "StreamSerial", 0x82, 0x03, 0x83),
	streamCDC_Application_(deviceController_, "StreamApplication", 0x84, 0x05, 0x85),
	telePlot_(streamCDC_Application_),
	attachStdioFlag_{false},
	sigrokAdapter_(logicAnalyzer_, streamCDC_Terminal_, streamCDC_Application_),
	stdio_driver_ {
		out_chars:						func_out_chars,
		out_flush:						func_out_flush,
		in_chars:						func_in_chars,
		set_chars_available_callback:	func_set_chars_available_callback,
		next:							nullptr,
#if PICO_STDIO_ENABLE_CRLF_SUPPORT
		last_ended_with_cr:				false,
		crlf_enabled:					true
#endif
	}
{}

void LABOPlatform::Initialize()
{
	deviceController_.Initialize();
	mscDrive_.Initialize(fat_);
	streamCDC_Terminal_.Initialize();
	streamCDC_Application_.Initialize();
	if (!attachStdioFlag_) {
		terminal_.AttachKeyboard(streamCDC_Terminal_.GetKeyboard()).AttachPrintable(streamCDC_Terminal_);
		::stdio_set_driver_enabled(&stdio_driver_, true);
	}
	Shell::AttachTerminal(terminal_.Initialize());
	if (!fat_.Mount()) {
		fat_.Format();
		std::unique_ptr<FS::File> pFile(fat_.OpenFile("/README.txt", "w"));
		if (pFile) pFile->Print(textREADME_);
	}
	Shell::Instance.Startup();
}

LABOPlatform& LABOPlatform::AttachStdio(bool attachStdioFlag)
{
	attachStdioFlag_ = attachStdioFlag;
	if (attachStdioFlag_) {
		sigrokAdapter_.SetPrintableErr(Stdio::Instance);
	} else {
		sigrokAdapter_.SetPrintableErr(streamCDC_Terminal_);
	}
	return *this;
}

void LABOPlatform::SetTerminalInterface(Printable& printable, Keyboard& keyboard)
{
	terminal_.AttachPrintable(printable).AttachKeyboard(keyboard);
	sigrokAdapter_.SetPrintableErr(printable);
}

void LABOPlatform::RestoreTerminalInterface()
{
	if (attachStdioFlag_) {
		terminal_.AttachKeyboard(Stdio::GetKeyboard()).AttachPrintable(Stdio::Instance);
		sigrokAdapter_.SetPrintableErr(Stdio::Instance);
	} else {
		terminal_.AttachKeyboard(streamCDC_Terminal_.GetKeyboard()).AttachPrintable(streamCDC_Terminal_);
		sigrokAdapter_.SetPrintableErr(streamCDC_Terminal_);
	}
}

void LABOPlatform::func_out_chars(const char* buf, int len)
{
	Printable& printable = Instance.GetTerminal().GetPrintable();
	for (int i = 0; i < len; ++i, ++buf) printable.PutChar(*buf);
}

void LABOPlatform::func_out_flush(void)
{
	Instance.GetTerminal().GetPrintable().Flush();
}

int LABOPlatform::func_in_chars(char* buf, int len)
{
	return 0;
}

void LABOPlatform::func_set_chars_available_callback(void (*fn)(void*), void* param)
{
}

//------------------------------------------------------------------------------
// Telnet Support
//------------------------------------------------------------------------------
#if defined(CYW43_WL_GPIO_LED_PIN)

class TelnetHandler : public Net::EventHandler {
private:
	LABOPlatform& laboPlatform_;
	Net::Telnet::Server telnetServer_;
	Net::Telnet::Stream telnetStream_;
public:
	TelnetHandler(LABOPlatform& laboPlatform);
public:
	Net::Telnet::Server& GetTelnetServer() { return telnetServer_; }
	Net::Telnet::Stream& GetTelnetStream() { return telnetStream_; }
public:
	virtual void OnConnect(const ip_addr_t& addr, uint16_t port) override;
	virtual void OnDisconnect() override;
};

TelnetHandler telnetHandler(LABOPlatform::Instance);

Net::Telnet::Server& ShellCmd_Net_Telnet_GetTelnetServer() { return telnetHandler.GetTelnetServer(); }

TelnetHandler::TelnetHandler(LABOPlatform& laboPlatform) :
					laboPlatform_{laboPlatform}, telnetStream_{telnetServer_}
{
	telnetServer_.SetEventHandler(*this);
}

void TelnetHandler::OnConnect(const ip_addr_t& addr, uint16_t port)
{
	Printable& tout = laboPlatform_.GetTerminal();
	tout.Printf("Telnet client connected: %s:%u\n", ::ipaddr_ntoa(&addr), port);
	Shell::Logout();
	laboPlatform_.SetTerminalInterface(telnetStream_, telnetStream_.GetKeyboard());
}

void TelnetHandler::OnDisconnect()
{
	laboPlatform_.RestoreTerminalInterface();
	Printable& tout = laboPlatform_.GetTerminal();
	tout.Printf("Telnet client disconnected\n");
}

#endif
