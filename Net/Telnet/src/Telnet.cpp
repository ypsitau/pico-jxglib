//==============================================================================
// Telnet.cpp
//==============================================================================
#include "jxglib/Net/Telnet.h"

namespace jxglib::Net::Telnet {


//------------------------------------------------------------------------------
// Server
//------------------------------------------------------------------------------
Server::Server(uint16_t port) : tcpServer_(port), pHandler_(&handlerDummy_)
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
