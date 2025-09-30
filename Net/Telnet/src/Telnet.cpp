//==============================================================================
// Telnet.cpp
//==============================================================================
#include "jxglib/Net/Telnet.h"

namespace jxglib::Net::Telnet {

//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
Server::Server(uint16_t port) : tcpServer_(port), pEventHandler_(&EventHandler::Dummy),
					stat_{Stat::Neutral}, code_{Code::NoOperation}
{
	tcpServer_.SetTransmitter(*this);
	tcpServer_.SetEventHandler(*this);
}

Server::~Server()
{
}

bool Server::Start()
{
	return WiFi::Instance.IsConnected() && tcpServer_.Start();
}

void Server::Stop()
{
	tcpServer_.Stop();
}

bool Server::IsRunning() const
{
	return tcpServer_.IsRunning();
}

bool Server::Send(const uint8_t* data, size_t len)
{
	return tcpServer_.Send(data, len, true);
}

bool Server::Send(const char* str)
{
	return Send(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

bool Server::Flush()
{
	return tcpServer_.Flush();
}

void Server::OnSent(size_t len)
{
	GetTransmitter().OnSent(len);
}

void Server::OnRecv(const uint8_t* data, size_t len)
{
	//::printf("Telnet::Server::OnRecv(len=%d)\n", len);
	//Stdio::Instance.Dump(data, len);
	const uint8_t* p = data;
	const uint8_t* pMark = data;
	for (size_t i = 0; i < len; i++, p++) {
		uint8_t ch = *p;
		switch (stat_) {
		case Stat::Neutral: {
			if (ch == Code::InterpretAsCommand) {
				if (p != pMark) GetTransmitter().OnRecv(pMark, p - pMark);
				stat_ = Stat::Command;
			}
			break;
		}
		case Stat::Command: {
			switch (ch) {
			case Code::SubnegotiationBegin:
				//::printf("SubnegotiationBegin\n");
				stat_ = Stat::Subnegotiation;
				break;
			case Code::DO:
			case Code::DONT:
			case Code::WILL:
			case Code::WONT:
				code_ = ch;
				stat_ = Stat::Option;
				break;
			default:
				// other command
				pMark = p + 1;
				stat_ = Stat::Neutral;
				break;
			}
			break;
		}
		case Stat::Option: {
			//::printf("Option: %s %s\n", CodeToString(code_), OptionToString(ch));
			pMark = p + 1;
			stat_ = Stat::Neutral;
			break;
		}
		case Stat::Subnegotiation: {
			switch (ch) {
			case Code::InterpretAsCommand: {
				stat_ = Stat::Subnegotiation_Command;
			}
			default: break;
			}
			break;
		}
		case Stat::Subnegotiation_Command: {
			switch (ch) {
			case Code::SubnegotiationEnd: {
				//::printf("SubnegotiationEnd\n");
				pMark = p + 1;
				stat_ = Stat::Neutral;
			}
			default: {
				stat_ = Stat::Subnegotiation;
				break;
			}
			}
			break;
		}
		default: break;
		}
	}
	if (p != pMark) GetTransmitter().OnRecv(pMark, p - pMark);
}

void Server::OnConnect(const ip_addr_t& addr, uint16_t port)
{
	static const uint8_t buffNegotiation[] = {
		Code::InterpretAsCommand, Code::WILL, Option::Echo,
		Code::InterpretAsCommand, Code::WILL, Option::SuppressGoAhead,
		Code::InterpretAsCommand, Code::DO,   Option::SuppressGoAhead,
		//Code::InterpretAsCommand, Code::DO,   Option::TerminalType,
	};
	tcpServer_.Send(buffNegotiation, sizeof(buffNegotiation));
	GetEventHandler().OnConnect(addr, port);
}

void Server::OnDisconnect()
{
	GetEventHandler().OnDisconnect();
	tcpServer_.Start();
}

//------------------------------------------------------------------------------
// Stream
//------------------------------------------------------------------------------
Stream::Stream(Server& telnetServer) : telnetServer_(telnetServer), chPrev_{'\0'}, addCrFlag_{true}, keyboard_{*this}
{
	telnetServer_.SetTransmitter(*this);
}

int Stream::Read(void* buff, int bytesBuff)
{
	int bytesRead = 0;
	while (bytesRead < bytesBuff) {
		int bytes = buffRecv_.ReadBuff(static_cast<uint8_t*>(buff) + bytesRead, bytesBuff - bytesRead);
		if (bytes <= 0) break; // no more data available
		bytesRead += bytes;
		Tickable::TickSub();
	}
	return bytesRead;
}

int Stream::Write(const void* buff, int bytesBuff)
{
	int bytesWritten = 0;
	while (bytesWritten < bytesBuff) {
		bytesWritten += telnetServer_.Send(static_cast<const uint8_t*>(buff) + bytesWritten, bytesBuff - bytesWritten);
		Tickable::TickSub();
	}
	return bytesWritten;
}

bool Stream::Flush()
{
	telnetServer_.Flush();
	return true;
}

Printable& Stream::PutChar(char ch)
{
	if (ch != '\r') {
		if (addCrFlag_ && ch == '\n') PutCharRaw('\r');
		PutCharRaw(ch);
	}
	chPrev_ = ch;
	return *this;
}

void Stream::OnSent(size_t len)
{
	// nothing to do
}

void Stream::OnRecv(const uint8_t* data, size_t len)
{
	buffRecv_.WriteBuff(data, len);
}

//------------------------------------------------------------------------------
// functions
//------------------------------------------------------------------------------
const char* CodeToString(uint8_t code)
{
	switch (code) {
	case Code::SubnegotiationEnd:		return "SubnegotiationEnd";
	case Code::NoOperation:				return "NoOperation";
	case Code::DataMark:				return "DataMark";
	case Code::Break:					return "Break";
	case Code::InterruptProcess:		return "InterruptProcess";
	case Code::AbortOutput:				return "AbortOutput";
	case Code::AreYouThere:				return "AreYouThere";
	case Code::EraseCharacter:			return "EraseCharacter";
		case Code::EraseLine:				return "EraseLine";
	case Code::GoAhead:					return "GoAhead";
	case Code::SubnegotiationBegin:		return "SubnegotiationBegin";
	case Code::WILL:					return "WILL";
	case Code::WONT:					return "WONT";
	case Code::DO:						return "DO";
	case Code::DONT:					return "DONT";
	case Code::InterpretAsCommand:		return "InterpretAsCommand";
	default:							return "???";
	}
}

const char* OptionToString(uint8_t option)
{
	switch (option) {
	case Option::TransmitBinary:					return "TransmitBinary";
	case Option::Echo:								return "Echo";
	case Option::Reconnection:						return "Reconnection";
	case Option::SuppressGoAhead:					return "SuppressGoAhead";
	case Option::ApproxMessageSizeNegot:			return "ApproxMessageSizeNegot";
	case Option::Status:							return "Status";
	case Option::TimingMark:						return "TimingMark";
	case Option::RemoteControlledTransAndEcho:		return "RemoteControlledTransAndEcho";
	case Option::OutputLineWidth:					return "OutputLineWidth";
	case Option::OutputPageSize:					return "OutputPageSize";
	case Option::OutputCarriageReturnDisposition:	return "OutputCarriageReturnDisposition";
	case Option::OutputHorizontalTabStops:			return "OutputHorizontalTabStops";
	case Option::OutputHorizontalTabDisposition:	return "OutputHorizontalTabDisposition";
	case Option::OutputFormfeedDisposition:			return "OutputFormfeedDisposition";
	case Option::OutputVerticalTabStops:			return "OutputVerticalTabStops";
	case Option::OutputVerticalTabDisposition:		return "OutputVerticalTabDisposition";
	case Option::OutputLinefeedDisposition:			return "OutputLinefeedDisposition";
	case Option::ExtendedASCII:						return "ExtendedASCII";
	case Option::Logout:							return "Logout";
	case Option::ByteMacro:							return "ByteMacro";
	case Option::DataEntryTerminal:					return "DataEntryTerminal";
	case Option::SUPDUP:							return "SUPDUP";
	case Option::SUPDUPOutput:						return "SUPDUPOutput";
	case Option::SendLocation:						return "SendLocation";
	case Option::TerminalType:						return "TerminalType";
	case Option::EndOfRecord:						return "EndOfRecord";
	case Option::TACACSUserIdentification:			return "TACACSUserIdentification";
	case Option::OutputMarking:						return "OutputMarking";
	case Option::TerminalLocationNumber:			return "TerminalLocationNumber";
	case Option::Telnet3270Regime:					return "Telnet3270Regime";
	case Option::X3PAD:								return "X3PAD";
	case Option::NegotiateAboutWindowSize:			return "NegotiateAboutWindowSize";
	case Option::TerminalSpeed:						return "TerminalSpeed";
	case Option::RemoteFlowControl:					return "RemoteFlowControl";
	case Option::Linemode:							return "Linemode";
	case Option::XDisplayLocation:					return "XDisplayLocation";
	case Option::EnvironmentOption:					return "EnvironmentOption";
	case Option::Authentication:					return "Authentication";
	case Option::EncryptionOption:					return "EncryptionOption";
	case Option::NewEnvironmentOption:				return "NewEnvironmentOption";
	case Option::TN3270E:							return "TN3270E";
	case Option::XAuth:								return "XAuth";
	case Option::Charset:							return "Charset";
	case Option::RSP:								return "RSP";
	case Option::ComPortControlOption:				return "ComPortControlOption";
	case Option::TelnetSuppressLocalEcho:			return "TelnetSuppressLocalEcho";
	case Option::TelnetStartTLS:					return "TelnetStartTLS";
	case Option::KERMIT:							return "KERMIT";
	case Option::SEND_URL:							return "SEND_URL";
	case Option::FORWARD_X:							return "FORWARD_X";
	case Option::PRAGMA_LOGON:						return "PRAGMA_LOGON";
	case Option::SSPI_LOGON:						return "SSPI_LOGON";
	case Option::PRAGMA_HEARTBEAT:					return "PRAGMA_HEARTBEAT";
	case Option::EXOPL:								return "EXOPL";
	default:										return "???";
	}
}

}
