#include "J1PacketSession.h"

PacketSession::PacketSession(asio::io_context* io_context)
	: _socket(*io_context)
	, _io_context(io_context)
{
}

PacketSession::~PacketSession()
{
}

void PacketSession::Run()
{

}

void PacketSession::Connect(std::string host, int port)
{
	const boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::make_address(host), port);
	_socket.async_connect(endpoint, 
		boost::bind(
			&PacketSession::OnConnect, 
			this, 
			boost::asio::placeholders::error
		)
	);
}

void PacketSession::Disconnect()
{

}

void PacketSession::OnConnect(const boost::system::error_code& err)
{
	if (!err)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connection Success~")));
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connect Failed")));
	}
}