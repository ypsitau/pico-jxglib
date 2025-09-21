//==============================================================================
// jxglib/Net/Telnet.h
//==============================================================================
#ifndef PICO_JXGLIB_NET_TELNET_H
#define PICO_JXGLIB_NET_TELNET_H
#include "pico/stdlib.h"
#include "jxglib/WiFi.h"
#include "jxglib/Net/TCP.h"

namespace jxglib::Net::Telnet {

class Server;

struct Code {
	static constexpr uint8_t SubnegotiationEnd		= 0xf0;
	static constexpr uint8_t NoOperation			= 0xf1;
	static constexpr uint8_t DataMark				= 0xf2;
	static constexpr uint8_t Break					= 0xf3;
	static constexpr uint8_t InterruptProcess		= 0xf4;
	static constexpr uint8_t AbortOutput			= 0xf5;
	static constexpr uint8_t AreYouThere			= 0xf6;
	static constexpr uint8_t EraseCharacter			= 0xf7;
	static constexpr uint8_t EraseLine				= 0xf8;
	static constexpr uint8_t GoAhead				= 0xf9;
	static constexpr uint8_t SubnegotiationBegin	= 0xfa;
	static constexpr uint8_t WILL					= 0xfb;		// Option code follows
	static constexpr uint8_t WONT					= 0xfc;		// Option code follows
	static constexpr uint8_t DO						= 0xfd;		// Option code follows
	static constexpr uint8_t DONT					= 0xfe;		// Option code follows
	static constexpr uint8_t InterpretAsCommand		= 0xff;
};

struct Option {
	static constexpr uint8_t TransmitBinary					= 0x00;		// RFC 856
	static constexpr uint8_t Echo							= 0x01;		// RFC 857
	static constexpr uint8_t Reconnection					= 0x02;		// RFC 858
	static constexpr uint8_t SuppressGoAhead				= 0x03;		// RFC 858
	static constexpr uint8_t ApproxMessageSizeNegot			= 0x04;		// RFC 858
	static constexpr uint8_t Status							= 0x05;		// RFC 859
	static constexpr uint8_t TimingMark						= 0x06;		// RFC 860
	static constexpr uint8_t RemoteControlledTransAndEcho	= 0x07;		// RFC 726
	static constexpr uint8_t OutputLineWidth				= 0x08;		// RFC 861
	static constexpr uint8_t OutputPageSize					= 0x09;		// RFC 861
	static constexpr uint8_t OutputCarriageReturnDisposition= 0x0a;		// RFC 861
	static constexpr uint8_t OutputHorizontalTabStops		= 0x0b;		// RFC 861
	static constexpr uint8_t OutputHorizontalTabDisposition	= 0x0c;		// RFC 861
	static constexpr uint8_t OutputFormfeedDisposition		= 0x0d;		// RFC 861
	static constexpr uint8_t OutputVerticalTabStops			= 0x0e;		// RFC 861
	static constexpr uint8_t OutputVerticalTabDisposition	= 0x0f;		// RFC 861
	static constexpr uint8_t OutputLinefeedDisposition		= 0x10;		// RFC 861
	static constexpr uint8_t ExtendedASCII					= 0x11;		// RFC 861
	static constexpr uint8_t Logout							= 0x12;		// RFC 862
	static constexpr uint8_t ByteMacro						= 0x13;		// RFC 863
	static constexpr uint8_t DataEntryTerminal				= 0x14;		// RFC 864
	static constexpr uint8_t SUPDUP							= 0x15;		// RFC 865
	static constexpr uint8_t SUPDUPOutput					= 0x16;		// RFC 865
	static constexpr uint8_t SendLocation					= 0x17;		// RFC 866
	static constexpr uint8_t TerminalType					= 0x18;		// RFC 109
	static constexpr uint8_t EndOfRecord					= 0x19;		// RFC 885
	static constexpr uint8_t TACACSUserIdentification		= 0x1a;		// RFC 927
	static constexpr uint8_t OutputMarking					= 0x1b;		// RFC 933
	static constexpr uint8_t TerminalLocationNumber			= 0x1c;		// RFC 946
	static constexpr uint8_t Telnet3270Regime				= 0x1d;		// RFC 1041
	static constexpr uint8_t X3PAD							= 0x1e;		// RFC 1053
	static constexpr uint8_t NegotiateAboutWindowSize		= 0x1f;		// RFC 1073
	static constexpr uint8_t TerminalSpeed					= 0x20;		// RFC 1079
	static constexpr uint8_t RemoteFlowControl				= 0x21;		// RFC 1372
	static constexpr uint8_t Linemode						= 0x22;		// RFC 1184
	static constexpr uint8_t XDisplayLocation				= 0x23;		// RFC 1096
	static constexpr uint8_t EnvironmentOption				= 0x24;		// RFC 1408
	static constexpr uint8_t Authentication					= 0x25;		// RFC 2941
	static constexpr uint8_t EncryptionOption				= 0x26;		// RFC 2946
	static constexpr uint8_t NewEnvironmentOption			= 0x27;		// RFC 1572
	static constexpr uint8_t TN3270E						= 0x28;		// RFC 2355
	static constexpr uint8_t XAuth							= 0x29;		// RFC 3726
	static constexpr uint8_t Charset						= 0x2a;		// RFC 2066
	static constexpr uint8_t RSP							= 0x2b;		// RFC 1184
	static constexpr uint8_t ComPortControlOption			= 0x2c;		// RFC 2217
	static constexpr uint8_t TelnetSuppressLocalEcho		= 0x2d;		// RFC 857
	static constexpr uint8_t TelnetStartTLS					= 0x2e;		// RFC 2712
	static constexpr uint8_t KERMIT							= 0x2f;		// RFC 2840
	static constexpr uint8_t SEND_URL						= 0x30;		// RFC 1568
	static constexpr uint8_t FORWARD_X						= 0x31;		// RFC 1096
	static constexpr uint8_t PRAGMA_LOGON					= 0x8a;		// RFC 2943
	static constexpr uint8_t SSPI_LOGON						= 0x8b;		// RFC 2943
	static constexpr uint8_t PRAGMA_HEARTBEAT				= 0x8c;		// RFC 2943
	static constexpr uint8_t EXOPL							= 0xff;
};

//------------------------------------------------------------------------------
// Handler
//------------------------------------------------------------------------------
class Handler {
public:
	virtual void OnSent(size_t len) {}
	//virtual void OnRecv(const uint8_t* data, size_t len) {}
	virtual void OnConnect(const ip_addr_t& addr, uint16_t port) {}
	virtual void OnDisconnect() {}
};

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
class Stream : public jxglib::Stream {
private:
	char chPrev_;
	bool addCrFlag_;
	Server& telnetServer_;
	VT100::Keyboard keyboard_;
public:
	Stream(Server& telnetServer);
public:
	Stream& AddCr(bool addCrFlag) { addCrFlag_ = addCrFlag; return* this; }
	VT100::Keyboard& GetKeyboard() { return keyboard_; }
public:
	// virtual functions of Stream
	virtual int Read(void* buff, int bytesBuff) override;
	virtual int Write(const void* buff, int bytesBuff) override;
	virtual bool Flush() override;
	virtual Printable& PutChar(char ch) override;
};


//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
class Server : public TCP::Handler {
public:
	enum class Stat {
		Neutral,
		Command,
		Option,
		Subnegotiation,
		Subnegotiation_Command,
	};
private:
	TCP::Server tcpServer_;
	Telnet::Handler* pHandler_;
	Telnet::Handler handlerDummy_;
	Stat stat_;
	uint8_t code_;
	FIFOBuff<uint8_t, 1024> buffRecv_;
public:
	Server(uint16_t port = 23);
	~Server();
public:
	void SetHandler(Telnet::Handler& handler) { pHandler_ = &handler; }
	Telnet::Handler& GetHandler() { return *pHandler_; }
public:
	bool Start();
	void Stop();
	bool IsRunning() const;
	bool Send(const uint8_t* data, size_t len);
	bool Send(const char* str);
public:
	int ReadFromRecvBuff(void* buff, int bytesBuff);
	int WriteToRecvBuff(const uint8_t* data, size_t len);
public:
	// virtual functions of TCP::Handler
	void OnSent(size_t len) override;
	void OnRecv(const uint8_t* data, size_t len) override;
	void OnConnect(const ip_addr_t& addr, uint16_t port) override;
	void OnDisconnect() override;
};

const char* CodeToString(uint8_t code);
const char* OptionToString(uint8_t option);

}

#endif
