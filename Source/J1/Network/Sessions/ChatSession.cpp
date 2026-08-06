#include "ChatSession.h"
#include "NetworkWorker.h"
#include "Handlers/ChatPktHandler.h"
#include "../../J1GameInstance.h"

ChatSession::ChatSession(asio::io_context* io_context, UJ1GameInstance* gameInstance)
	: PacketSession(io_context, gameInstance)
{
	ChatPktHandler::Init();
}

ChatSession::~ChatSession()
{
}

void ChatSession::RequestDisconnect()
{
	// Leave Room Pkt 발송
	Chat::REQ_LEAVE_ROOM pkt;
	pkt.set_player_id(_player_id);
	
	SEND_PACKET(GetGameInstance(), ESessionType::Chat, Chat::PacketType::PKT_REQ_LEAVE_ROOM, pkt);
}

void ChatSession::OnConnect(const boost::system::error_code& err)
{
	if (!err)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Connection Success~")));
		// Enter Room Pkt 발송
		Chat::REQ_ENTER_ROOM pkt;
		pkt.set_name("admin");
		
		SEND_PACKET(GetGameInstance(), ESessionType::Chat, Chat::PacketType::PKT_REQ_ENTER_ROOM, pkt);
		
		AsyncRead();
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Connect Failed")));
	}
}

void ChatSession::DispatchPacket(SessionPtr session, PacketHeader& header, char* ptr, size_t size)
{
	ChatPktHandler::HandlePacket(session, header, ptr, size);
}
