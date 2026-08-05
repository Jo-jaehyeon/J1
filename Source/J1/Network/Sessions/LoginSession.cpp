#include "LoginSession.h"
#include "NetworkWorker.h"
#include "Handlers/LoginPktHandler.h"
#include "../../J1GameInstance.h"

LoginSession::LoginSession(asio::io_context* io_context)
	: PacketSession(io_context)
{
	LoginPktHandler::Init();
}

LoginSession::~LoginSession()
{
}

void LoginSession::RequestDisconnect()
{
	// Leave Room Pkt 발송
	//Chat::REQ_LEAVE_ROOM pkt;
	//pkt.set_player_id(_player_id);
	//
	//SEND_PACKET(Chat::PacketType::PKT_REQ_LEAVE_ROOM, pkt);
}

void LoginSession::OnConnect(const boost::system::error_code& err)
{
	if (!err)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connection Success~")));
		// Enter Room Pkt 발송
		//Chat::REQ_ENTER_ROOM pkt;
		//pkt.set_name("admin");
		//
		//SEND_PACKET(Chat::PacketType::PKT_REQ_ENTER_ROOM, pkt);
		
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
