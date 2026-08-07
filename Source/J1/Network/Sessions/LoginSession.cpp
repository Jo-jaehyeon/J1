#include "LoginSession.h"
#include "NetworkWorker.h"
#include "Handlers/LoginPktHandler.h"
#include "../../J1GameInstance.h"

LoginSession::LoginSession(asio::io_context* io_context, UJ1GameInstance* gameInstance)
	: PacketSession(io_context, gameInstance)
{
	LoginPktHandler::Init();
}

LoginSession::~LoginSession()
{
}

void LoginSession::RequestDisconnect()
{
	PacketSession::Disconnect();
}

void LoginSession::OnConnect(const boost::system::error_code& err)
{
	if (!err)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connection Success~")));
		if (_connected.exchange(true) == true)		return;		// 이미 connect 처리됨
		
		AsyncRead();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connect Failed")));
	}
}

void LoginSession::DispatchPacket(SessionPtr session, PacketHeader& header, char* ptr, size_t size)
{
	LoginPktHandler::HandlePacket(session, header, ptr, size);
}
