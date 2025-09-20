//==============================================================================
// Telnet.cpp
//==============================================================================
#include "jxglib/Net/Telnet.h"

namespace jxglib::Net::Telnet {


//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
Server::Server(uint16_t port) : tcpServer_(port), pHandler_(&handlerDummy_), stat_{Stat::Neutral}, code_{Code::NoOperation}
{}

Server::~Server()
{
}

bool Server::Start(Handler* handler)
{
	return tcpServer_.Start();
}

void Server::Stop()
{
}

bool Server::IsRunning() const
{
	//return tcpServer_.IsRunning();
	return false;
}

bool Server::Send(const uint8_t* data, size_t len)
{
	return tcpServer_.Send(data, len);
}

bool Server::Send(const char* str)
{
	return Send(reinterpret_cast<const uint8_t*>(str), strlen(str));
}

void Server::OnSent(size_t len)
{
	pHandler_->OnSent(len);
}

void Server::OnRecv(const uint8_t* data, size_t len)
{
	const uint8_t* p = data;
	const uint8_t* pMark = data;
	for (size_t i = 0; i < len; i++, p++) {
		uint8_t ch = *p;
		switch (stat_) {
		case Stat::Neutral: {
			if (ch == Code::InterpretAsCommand) {
				if (pMark != p) pHandler_->OnRecv(pMark, p - pMark);
				stat_ = Stat::Command;
			}
			break;
		}
		case Stat::Command: {
			switch (ch) {
			case Code::SubnegotiationBegin:
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
			uint8_t codeResp =
				(code_ == Code::DO)? Code::WILL :
				(code_ == Code::DONT)? Code::WONT :
				(code_ == Code::WILL)? Code::DO :
				(code_ == Code::WONT)? Code::DONT :
				Code::NoOperation;
			uint8_t resp[] = { Code::InterpretAsCommand, codeResp, ch };
			tcpServer_.Send(resp, sizeof(resp));
			stat_ = Stat::Neutral;
			pMark = p + 1;
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
	if (pMark != p) pHandler_->OnRecv(pMark, p - pMark);
}

void Server::OnConnect()
{
	pHandler_->OnConnect();
}

void Server::OnDisconnect()
{
	pHandler_->OnDisconnect();
}

}
